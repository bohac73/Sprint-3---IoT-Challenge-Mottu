#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "meu wifi";
const char* password = "minha senha";

WebServer server(80);

struct Node {
  String id;
  int rssi;
};

Node nodes[3]; // até 3 emissores
int nodeCount = 0;

String pageTemplate() {
  String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'>";
  html += "<title>SGP</title>";
  html += "<style>";
  html += "body {margin:0; font-family: 'Segoe UI', Tahoma, sans-serif; text-align:center; background: linear-gradient(135deg, #0f2027, #203a43, #2c5364); color:#00e5ff;}";
  html += "h1 {color:#00e5ff; text-shadow:0 0 10px #00e5ff, 0 0 20px #00e5ff;}";
  html += "canvas {border:2px solid #00e5ff; border-radius:10px; background:rgba(0,0,0,0.6); box-shadow:0 0 15px #00e5ff;} ";
  html += "footer {margin-top:20px; font-size:12px; color:#aaa;}";
  html += "</style>";
  html += "</head><body>";
  html += "<h1>Sistema de Gestão de Pátios (RSSI)</h1>";
  html += "<canvas id='mapa' width='400' height='400'></canvas>";
  html += "<footer>Powered by TechLab&reg; - 2025</footer>";
  html += "<script>";
  html += "var ctx=document.getElementById('mapa').getContext('2d');";
  html += "ctx.clearRect(0,0,400,400);";

  for(int i=0;i<nodeCount;i++){
    int rssi = nodes[i].rssi;
    float distancia = pow(10, ((-40 - rssi)/(10*2.0))); // RSSI0=-40, n=2
    int posX = 200 + (i*100-100); // espaçamento dos nós
    int posY = 200;
    int raio = (int)(distancia*1); // escala gráfica

    // Ajuste das cores
    html += "ctx.strokeStyle = '#00e5ff';"; // borda neon azul
    html += "ctx.fillStyle = '#FFFFFF';";   // texto branco
    html += "ctx.font = '10px Segoe UI';";  // fonte clean

    // Desenha círculo e texto
    html += "ctx.beginPath();ctx.arc(" + String(posX) + "," + String(posY) + "," + String(raio) + ",0,2*Math.PI);";
    html += "ctx.stroke();ctx.fillText('" + nodes[i].id + " (" + String(rssi) + " dBm)'," + String(posX-40) + "," + String(posY-10) + ");";
  }

  html += "</script></body></html>";
  return html;
}

void handleRoot() {
  server.send(200, "text/html", pageTemplate());
}

void handleDado() {
  if (server.hasArg("id") && server.hasArg("rssi")) {
    String id = server.arg("id");
    int rssi = server.arg("rssi").toInt();
    bool found = false;

    for (int i=0; i<nodeCount; i++) {
      if (nodes[i].id == id) {
        nodes[i].rssi = rssi;
        found = true;
        break;
      }
    }

    if (!found && nodeCount < 3) {
      nodes[nodeCount].id = id;
      nodes[nodeCount].rssi = rssi;
      nodeCount++;
    }
    server.send(200, "text/plain", "OK");
  } else {
    server.send(400, "text/plain", "Faltam argumentos");
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Conectado no WiFi");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/dado", HTTP_POST, handleDado);
  server.begin();
}

void loop() {
  server.handleClient();
}
