/*
  AXON IoT - GSM/Cellular Module Integration Example
  
  This sketch demonstrates how to connect your ESP32 to the AXON Cloud 
  using a standard SIM800L or compatible GSM cellular module via the TinyGSM library.
  This allows deploying devices in remote fields where Wi-Fi is unavailable.
  
  Prerequisites:
  1. Define AXON_USE_CELLULAR before including headers.
  2. Install the "TinyGSM" library via the Arduino IDE Library Manager.
  3. Ensure your SIM card has an active GPRS data plan.
*/

#define AXON_TEMPLATE_ID "your-device-uuid-here"
#define AXON_AUTH_TOKEN "your-auth-token-here"

// Enable cellular client driver
#define AXON_USE_CELLULAR

// Select your cellular modem model (TinyGSM definitions)
#define TINY_GSM_MODEM_SIM800

#include <axon.h>
#include <AxonCellular.h>

// Cellular APN configurations (change according to your SIM network carrier)
const char apn[]      = "internet.telecom"; // APN string
const char gprsUser[] = "";                 // APN Username (usually empty)
const char gprsPass[] = "";                 // APN Password (usually empty)

// Hardware Serial pins mapping to GSM Module TX/RX pins
#define MODEM_TX             27
#define MODEM_RX             26
#define MODEM_PWR_KEY        4   // Transistor pin connected to SIM800 power pin

HardwareSerial SerialAT(1);
AxonCellular cellular(SerialAT, MODEM_PWR_KEY);

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("=================================================");
  Serial.println("        AXON IoT CELLULAR MODEM INITIALIZER      ");
  Serial.println("=================================================");

  // Connect to GPRS network over Cellular module
  Serial.println("Initializing modem and connecting to cellular APN...");
  if (cellular.begin(apn, gprsUser, gprsPass)) {
    Serial.println("[GSM] Cellular registration active.");
  } else {
    Serial.println("[GSM] Modem connection failed. Verify power supply & APN settings.");
    while (true); // halt
  }

  // Bind an actuator callback to V3 (Smart Switch)
  Axon.setWriteHandler("V3", [](const char* val) {
    Serial.print("[Actuator] V3 command received: ");
    Serial.println(val);
  });
}

void loop() {
  // Transmit telemetry periodically and receive incoming commands over cellular
  Axon.run();
  
  // Example: Write simulated soil metrics to V0 every 10 seconds
  static unsigned long lastTx = 0;
  if (millis() - lastTx > 10000) {
    lastTx = millis();
    float moistureReading = analogRead(34) * (100.0 / 4095.0); // simulated sensor
    Axon.virtualWrite("V0", moistureReading);
    Serial.print("[GSM Tx] Soil Moisture sent: ");
    Serial.print(moistureReading);
    Serial.println("%");
  }
}
