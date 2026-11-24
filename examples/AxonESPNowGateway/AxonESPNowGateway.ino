/*
  AXON IoT - ESP-NOW Central Hub Gateway Example
  
  This sketch demonstrates how to configure an ESP32 as a Central Gateway Hub
  to receive data packets from multiple peripheral nodes via ESP-NOW, and
  transmit the collected sensor readings to the AXON IoT Cloud.
  
  Prerequisites:
  1. Define AXON_USE_ESPNOW before including the headers.
  2. Put your Wi-Fi credentials to connect the gateway to the cloud.
*/

#define AXON_TEMPLATE_ID "your-device-uuid-here"
#define AXON_AUTH_TOKEN "your-auth-token-here"

// Enable ESP-NOW feature in the SDK
#define AXON_USE_ESPNOW

#include <WiFi.h>
#include <axon.h>
#include <AxonESPNow.h>

// Wi-Fi network credentials for the Gateway Hub
const char* ssid = "Your-WiFi-SSID";
const char* password = "Your-WiFi-Password";

// Callback triggered whenever an ESP-NOW packet is received
void onDataReceived(const char* vpin, const char* value) {
  Serial.print("[Gateway] Received ESP-NOW telemetry: VPIN=");
  Serial.print(vpin);
  Serial.print(", Value=");
  Serial.println(value);

  // Directly forward the received pin value to the AXON IoT Cloud!
  Axon.virtualWrite(vpin, value);
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("=================================================");
  Serial.println("        AXON IoT ESP-NOW GATEWAY INITIALIZER     ");
  Serial.println("=================================================");

  // Connect the gateway to your local Wi-Fi router
  WiFi.begin(ssid, password);
  Serial.print("Connecting Gateway to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nGateway Wi-Fi connected!");

  // Initialize AXON Cloud connection
  Axon.begin(AXON_TEMPLATE_ID, AXON_AUTH_TOKEN);

  // Initialize ESP-NOW receiver callback
  // Incoming data of format "VPIN:VALUE" will trigger onDataReceived
  if (AxonESPNow::begin(onDataReceived)) {
    Serial.println("[Gateway] ESP-NOW listening pipeline online.");
    Serial.print("[Gateway] Broadcast MAC Address: ");
    Serial.println(WiFi.macAddress());
  } else {
    Serial.println("[Gateway] ESP-NOW initialization failed.");
  }
}

void loop() {
  // Keep cloud link active and route incoming telemetry packets
  Axon.run();
}
