#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

const char* ssid = "WIFI";
const char* password = "PASSWORD";

AsyncWebServer server(80);

unsigned long lastScan = 0;
String scanResults = "[]";

String encryptionType(wifi_auth_mode_t type) {
  switch(type) {
    case WIFI_AUTH_OPEN: return "Open";
    case WIFI_AUTH_WEP: return "WEP";
    case WIFI_AUTH_WPA_PSK: return "WPA";
    case WIFI_AUTH_WPA2_PSK: return "WPA2";
    case WIFI_AUTH_WPA_WPA2_PSK: return "WPA/WPA2";
    case WIFI_AUTH_WPA2_ENTERPRISE: return "WPA2 Enterprise";
    case WIFI_AUTH_WPA3_PSK: return "WPA3";
    default: return "Unknown";
  }
}

void performScan() {
  int n = WiFi.scanNetworks();

  DynamicJsonDocument doc(8192);
  JsonArray arr = doc.to<JsonArray>();

  for(int i = 0; i < n; i++) {
    JsonObject obj = arr.createNestedObject();

    obj["ssid"] = WiFi.SSID(i);
    obj["rssi"] = WiFi.RSSI(i);
    obj["channel"] = WiFi.channel(i);
    obj["encryption"] = encryptionType(WiFi.encryptionType(i));
    obj["bssid"] = WiFi.BSSIDstr(i);
  }

  serializeJson(arr, scanResults);
}

void setup() {
  Serial.begin(115200);

  if(!LittleFS.begin(true)) {
    Serial.println("LittleFS Mount Failed");
    return;
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.print("Connecting");

  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected");
  Serial.println(WiFi.localIP());

  performScan();

  server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");

  server.on("/scan", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "application/json", scanResults);
  });

  server.begin();
}

void loop() {
  if(millis() - lastScan > 5000) {
    lastScan = millis();
    performScan();
  }
}