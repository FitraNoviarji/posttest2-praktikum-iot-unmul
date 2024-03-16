#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// Define pins
#define BUZZER_PIN D3
#define LED_PIN  D1
#define TAMBAH_PIN  D5
#define KURANG_PIN  D6
#define NYALAKAN_MATIKAN_PIN  D7

const char* ssid = "metanoo";       // Nama SSID AP/Hotspot
const char* password = "masasihdek";    // Password Wifi

// Define variables
int brightness = 0;
int toneFrequency = 100;
bool enabled = false;

ESP8266WebServer server(80);
String webpage;

void setup() {
  Serial.begin(115200);
  delay(500);
  // Initialize pins
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(TAMBAH_PIN, INPUT_PULLUP);
  pinMode(KURANG_PIN, INPUT_PULLUP);
  pinMode(NYALAKAN_MATIKAN_PIN, INPUT_PULLUP);

  // Connect ke WiFi
  Serial.println();
  Serial.print("Configuring access point...");
  
  // Connect to WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  
  // Start web server
  server.on("/", HTTP_GET, handleRoot);
  server.begin();
  Serial.println("HTTP server started");

  // Print IP address
  Serial.print("Server IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  server.handleClient();
  
  // Check button states
  if (digitalRead(TAMBAH_PIN) == LOW && enabled) {
    brightness = min(brightness + 3, 249);
    analogWrite(LED_PIN, brightness);
    delay(200);
  }
  
  if (digitalRead(KURANG_PIN) == LOW && enabled) {
    brightness = max(brightness - 3, 0);
    analogWrite(LED_PIN, brightness);
    delay(200);
  }

  if (digitalRead(NYALAKAN_MATIKAN_PIN) == LOW) {
    enabled = !enabled;
    if (!enabled) {
      digitalWrite(LED_PIN, LOW);
      noTone(BUZZER_PIN);
    }
    delay(200);
  }

  if (enabled) {
    tone(BUZZER_PIN, toneFrequency);
    // Adjust tone frequency
    if (digitalRead(TAMBAH_PIN) == LOW) {
      toneFrequency = min(toneFrequency + 10, 2500);
      delay(200);
    }

    if (digitalRead(KURANG_PIN) == LOW) {
      toneFrequency = max(toneFrequency - 10, 100);
      delay(200);
    }
  } else {
    noTone(BUZZER_PIN);
  }
}

void handleRoot() {
  String page = "<!DOCTYPE html><html><head><title>Monitoring</title></head><body>";
  page += "<h1>Monitoring</h1>";
  page += "<p><strong>KECERAHAN:</strong> ";
  page += brightness;
  page += "</p><p><strong>FREKUENSI:</strong> ";
  page += toneFrequency;
  page += "</p><p><strong>Status:</strong> ";
  page += (enabled ? "NYALA" : "MATI");
  page += "</p></body></html>";

  server.send(200, "text/html", page);
}