#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "meu wifi";
const char* password = "minha senha";
const char* serverName = "http://192.168.0.8/dado"; // IP do gateway
 
String id = "Mottu Pop";
 
void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Conectado");
}
 
void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverName);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
 
    int rssi = WiFi.RSSI();
    String body = "id=" + id + "&rssi=" + String(rssi);
 
    int httpResponseCode = http.POST(body);
    Serial.println("Resposta: " + String(httpResponseCode));
    http.end();
  }
delay(5000);
}