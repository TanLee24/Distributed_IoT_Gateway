#include "task_mqtt.h"

// --- Wi-Fi & ThingsBoard Credentials ---
#define WIFI_SSID       "BuiThiNgocKieu"
#define WIFI_PASS       "15051971"
#define TB_SERVER       "thingsboard.cloud"
#define TB_TOKEN        "q5ofhps74hs6eqdmg1m5"

WiFiClient espClient;
PubSubClient client(espClient);

// --- RPC Callback Function (Server-to-Device Commands) ---
void mqttCallback(char* topic, byte* payload, unsigned int length) 
{
    // 1. Convert the incoming payload byte array into a String
    String message = "";
    for (int i = 0; i < length; i++) { message += (char)payload[i]; }
    
    Serial.printf("[MQTT RPC] Command received from Web: %s\n", message.c_str());

    // 2. Parse the JSON RPC command from ThingsBoard (Format: {"method":"setRelay","params":true})
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, message);
    
    if (!error) 
    {
        String method = doc["method"] | "";
        bool params = doc["params"] | false;

        // Handle "setRelay4" method call
        if (method == "setRelay4") 
        {
            if (params == true) 
            {
                Serial1.print("4"); // Send command via UART to turn ON IN4 on STM32
                Serial.println(" -> Command sent to STM32: TURN ON IN4");
            } 
            else 
            {
                Serial1.print("5"); // Send command via UART to turn OFF IN4 on STM32
                Serial.println(" -> Command sent to STM32: TURN OFF IN4");
            }
        }
    }
}

// --- MQTT Connection Management ---
void connectToMQTT() 
{
    if (WiFi.status() != WL_CONNECTED) 
    {
        Serial.print("Connecting to WiFi");
        WiFi.begin(WIFI_SSID, WIFI_PASS);
        while (WiFi.status() != WL_CONNECTED) { vTaskDelay(500); Serial.print("."); }
        Serial.println("\nWiFi Connected!");
    }

    while (!client.connected()) 
    {
        Serial.print("Connecting to ThingsBoard...");
        // Connect using Access Token as the username (no password required)
        if (client.connect("ESP32_Client", TB_TOKEN, NULL)) 
        {
            Serial.println(" SUCCESS!");
            // Subscribe to the Server-Side RPC topic
            client.subscribe("v1/devices/me/rpc/request/+"); 
        } 
        else 
        {
            Serial.print(" Failed, rc=");
            Serial.println(client.state());
            vTaskDelay(5000); // Wait 5 seconds before retrying
        }
    }
}

// --- Main MQTT Task ---
void Task_MQTT_Process(void *pvParameters) 
{
    client.setServer(TB_SERVER, 1883);
    client.setCallback(mqttCallback); // Attach the callback function for incoming RPCs

    SensorData localData;
    unsigned long lastSendTime = 0;

    while (1) 
    {
        if (!client.connected()) { connectToMQTT(); }
        client.loop(); // Crucial loop call to maintain connection and process incoming messages

        // Publish telemetry data every 5 seconds
        if (millis() - lastSendTime > 5000) 
        {
            // Acquire Mutex to safely read data from the shared structure
            if (xSemaphoreTake(dataMutex, pdMS_TO_TICKS(100)) == pdTRUE) 
            {
                localData = sharedData; 
                xSemaphoreGive(dataMutex);
                
                // Serialize data into ThingsBoard JSON format
                char telemetryStr[128];
                sprintf(telemetryStr, "{\"temperature\":%.1f, \"humidity\":%.1f, \"light\":%d}", 
                        localData.temp, localData.humi, localData.light);
                
                // Publish telemetry to Cloud
                client.publish("v1/devices/me/telemetry", telemetryStr);
                Serial.printf("[MQTT] Telemetry published: %s\n", telemetryStr);
            }
            lastSendTime = millis();
        }
        
        vTaskDelay(pdMS_TO_TICKS(10)); // Small delay to yield CPU and prevent watchdog triggers
    }
}