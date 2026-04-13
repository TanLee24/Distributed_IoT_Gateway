#include "mqtt.h"

// --- THÔNG TIN WIFI & THINGSBOARD CỦA BẠN ---
#define WIFI_SSID       "BuiThiNgocKieu"
#define WIFI_PASS       "15051971"
#define TB_SERVER       "thingsboard.cloud" // Nếu bạn đăng ký bản Maker/Cloud
#define TB_TOKEN        "q5ofhps74hs6eqdmg1m5"

WiFiClient espClient;
PubSubClient client(espClient);

// --- HÀM BẮT LỆNH TỪ DASHBOARD GỬI XUỐNG (RPC) ---
void mqttCallback(char* topic, byte* payload, unsigned int length) {
    // 1. Chuyển cục data nhận được thành chuỗi String
    String message = "";
    for (int i = 0; i < length; i++) { message += (char)payload[i]; }
    
    Serial.printf("[MQTT RPC] Nhận lệnh từ Web: %s\n", message.c_str());

    // 2. Bóc tách JSON lệnh của ThingsBoard (Dạng: {"method":"setRelay","params":true})
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, message);
    
    if (!error) {
        String method = doc["method"] | "";
        bool params = doc["params"] | false;

        // Nếu Web gọi method là "setRelay4"
        if (method == "setRelay4") {
            if (params == true) {
                Serial1.print("4"); // Bắn lệnh qua UART cho STM32 BẬT IN4
                Serial.println(" -> Đã ra lệnh cho STM32: BẬT IN4");
            } else {
                Serial1.print("5"); // Bắn lệnh qua UART cho STM32 TẮT IN4
                Serial.println(" -> Đã ra lệnh cho STM32: TẮT IN4");
            }
        }
    }
}

// --- HÀM KẾT NỐI & GIỮ KẾT NỐI ---
void connectToMQTT() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.print("Connecting to WiFi");
        WiFi.begin(WIFI_SSID, WIFI_PASS);
        while (WiFi.status() != WL_CONNECTED) { vTaskDelay(500); Serial.print("."); }
        Serial.println("\nWiFi Connected!");
    }

    while (!client.connected()) {
        Serial.print("Connecting to ThingsBoard...");
        // Dùng Access Token làm Username, không cần Password
        if (client.connect("ESP32_Client", TB_TOKEN, NULL)) {
            Serial.println(" XONG!");
            // Đăng ký nghe lệnh RPC từ server
            client.subscribe("v1/devices/me/rpc/request/+"); 
        } else {
            Serial.print(" Thất bại, mã lỗi: ");
            Serial.println(client.state());
            vTaskDelay(5000); // Chờ 5s thử lại
        }
    }
}

// --- TASK CHÍNH ---
void Task_MQTT_Process(void *pvParameters) {
    client.setServer(TB_SERVER, 1883);
    client.setCallback(mqttCallback); // Gắn hàm đón lõng lệnh

    SensorData localData;
    unsigned long lastSendTime = 0;

    while (1) {
        if (!client.connected()) { connectToMQTT(); }
        client.loop(); // Lệnh cực kỳ quan trọng để duy trì kết nối và nhận tin

        // Cứ mỗi 5 giây thì gửi dữ liệu lên 1 lần
        if (millis() - lastSendTime > 5000) {
            
            // Xin khóa Mutex để copy dữ liệu từ UART đưa sang
            if (xSemaphoreTake(dataMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
                localData = sharedData; 
                xSemaphoreGive(dataMutex);
                
                // Đóng gói thành JSON chuẩn ThingsBoard
                char telemetryStr[128];
                sprintf(telemetryStr, "{\"temperature\":%.1f, \"humidity\":%.1f, \"light\":%d}", 
                        localData.temp, localData.humi, localData.light);
                
                // Bắn lên Cloud
                client.publish("v1/devices/me/telemetry", telemetryStr);
                Serial.printf("[MQTT] Đã đẩy lên Cloud: %s\n", telemetryStr);
            }
            lastSendTime = millis();
        }
        
        vTaskDelay(pdMS_TO_TICKS(10)); // Tránh treo Task
    }
}