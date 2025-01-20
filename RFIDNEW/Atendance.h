#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <MFRC522.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char *mqtt_broker = "broker.mqtt.cool";
const char *topic = "test/topic-priyanshu";
const int mqtt_port = 1883;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

String _AuthACcard = "2188CE1D";

struct SharedState {
  String lastScannedUID = "";
  bool acTurnOnRequested = false;
  unsigned long lastScanTime = 0;
} globalState;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define RST_PIN D1  // Reset pin
#define SS_PIN D2   // Slave Select pin

MFRC522 mfrc522(SS_PIN, RST_PIN);


WiFiClient espClient;
PubSubClient client(espClient);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Attendance : public Task {
public:
  Attendance(bool _enabled = false, unsigned long _interval = 0) : Task(_enabled, _interval) {}

protected:

int StatusLED = D0;
int DataLED = D8;

  void setup() {
  pinMode(StatusLED, OUTPUT);
  pinMode(DataLED, OUTPUT);
    while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
    digitalWrite(StatusLED, LOW);  // Status LED off
  }


   Serial.println("Connected to the WiFi network");
  digitalWrite(StatusLED, HIGH);  // Status LED on

  // Connecting to the MQTT broker
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);

    while (!client.connected()) {
    String client_id = "esp8266-client-";
    client_id += String(WiFi.macAddress());

    Serial.printf("The client %s connects to the MQTT broker\n", client_id.c_str());

    if (client.connect(client_id.c_str())) {
      Serial.println("MQTT broker connected");
    } else {
      Serial.print("Failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }

   // Publish a message to the topic
  client.publish(topic, "Hello From ESP8266!");
  client.subscribe(topic);

  // Initialize SPI bus and MFRC522
  SPI.begin();
  mfrc522.PCD_Init();

}


static void callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message:");

  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }

  Serial.println();
  Serial.println(" - - - - - - - - - - - -");
}

  void loop() {
     client.loop();

  // Look for new RFID cards
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    Serial.print("UID tag: ");
    String uidStr = "";
    String data = "";
    data = printHex(mfrc522.uid.uidByte, mfrc522.uid.size);
    Serial.println(data);
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
      Serial.print(mfrc522.uid.uidByte[i], HEX);
      uidStr += String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
      uidStr += String(mfrc522.uid.uidByte[i], HEX);
    }
    Serial.println();

    // Turn on DataLED when UID is scanned
    digitalWrite(DataLED, HIGH);

    // Publish UID to MQTT topic
    client.publish(topic, data.c_str());
    Serial.println("UID published to MQTT");

    globalState.lastScannedUID = uidStr;
      
      if (uidStr.equalsIgnoreCase(_AuthACcard)) {
        globalState.acTurnOnRequested = true;
        globalState.lastScanTime = millis();
        Serial.println("Lab key");
        
      }

    // Turn off DataLED after a short delay
    delay(500);
    digitalWrite(DataLED, LOW);
    uidStr="";
    // Halt PICC
    mfrc522.PICC_HaltA();
  }
  }

  String printHex(byte *buffer, byte bufferSize) {
  String str = "";
  for (uint8_t i = 0; i < bufferSize; i++) {
    str += String(buffer[i]);
  }
  return "{\"UID\":\"" + str + "\"}";
}

};
