#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include "esp_system.h"
#include "esp_heap_caps.h"
#include "esp_timer.h"

// ================= WIFI =================
const char* ssid = "Wifi_CasaNova";
const char* password = "Kp@120912";

WebServer server(80);

// ================= HTML =================
const char HTML_PAGE[] PROGMEM =
"<!DOCTYPE html>\n"
"<html>\n"
"<head>\n"
"<meta charset='utf-8'>\n"
"<title>ESP32 Dashboard</title>\n"
"<script src='https://cdn.jsdelivr.net/npm/chart.js'></script>\n"
"<style>\n"
"body{background:#111;color:#eee;font-family:Arial;text-align:center;}\n"
".card{display:inline-block;width:160px;margin:8px;padding:8px;background:#222;border-radius:8px;}\n"
"canvas{max-width:90%;margin:20px auto;}\n"
"</style>\n"
"</head>\n"
"<body>\n"
"<h2>ESP32 Internal Dashboard</h2>\n"

"<div class='card'><h4>RSSI</h4><p id='rssi'></p></div>\n"
"<div class='card'><h4>Heap</h4><p id='heap'></p></div>\n"
"<div class='card'><h4>Hall</h4><p id='hall'></p></div>\n"
"<div class='card'><h4>CPU</h4><p id='cpu'></p></div>\n"
"<div class='card'><h4>Uptime</h4><p id='uptime'></p></div>\n"

"<canvas id='chartRssi'></canvas>\n"
"<canvas id='chartHeap'></canvas>\n"
"<canvas id='chartHall'></canvas>\n"

"<script>\n"
"const MAX=60;\n"
"const labels=[];\n"

"function makeChart(ctx,label){\n"
" return new Chart(ctx,{type:'line',data:{labels:labels,datasets:[{label:label,data:[],borderWidth:2,fill:false}]},\n"
" options:{animation:false,responsive:true,scales:{x:{display:false}}}});\n"
"}\n"

"const cRssi=makeChart(document.getElementById('chartRssi'),'RSSI (dBm)');\n"
"const cHeap=makeChart(document.getElementById('chartHeap'),'Heap (bytes)');\n"
"const cHall=makeChart(document.getElementById('chartHall'),'Hall');\n"

"function push(chart,val){\n"
" chart.data.datasets[0].data.push(val);\n"
" if(chart.data.datasets[0].data.length>MAX)chart.data.datasets[0].data.shift();\n"
" chart.update();}\n"

"setInterval(async()=>{\n"
" const d=await(await fetch('/data')).json();\n"
" const t=new Date().toLocaleTimeString();\n"
" labels.push(t); if(labels.length>MAX)labels.shift();\n"
" rssi.innerText=d.rssi+' dBm';\n"
" heap.innerText=(d.heap/1024).toFixed(1)+' KB';\n"
" hall.innerText=d.hall;\n"
" cpu.innerText=d.cpu+' MHz';\n"
" uptime.innerText=d.uptime+' min';\n"
" push(cRssi,d.rssi);\n"
" push(cHeap,d.heap);\n"
" push(cHall,d.hall);\n"
"},1000);\n"
"</script>\n"
"</body>\n"
"</html>\n";

// ================= HANDLERS =================
void handleRoot() {
  server.send_P(200, "text/html", HTML_PAGE);
}

void handleData() {
  int rssi = WiFi.RSSI();
  uint32_t heap = esp_get_free_heap_size();
  int hall = hallRead();
  uint32_t cpu = getCpuFrequencyMhz();
  unsigned long uptime =
    (unsigned long)(esp_timer_get_time() / 1000000ULL / 60);

  String json = "{";
  json += "\"rssi\":" + String(rssi) + ",";
  json += "\"heap\":" + String(heap) + ",";
  json += "\"hall\":" + String(hall) + ",";
  json += "\"cpu\":" + String(cpu) + ",";
  json += "\"uptime\":" + String(uptime);
  json += "}";

  server.send(200, "application/json", json);
}

// ================= SETUP / LOOP =================
void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.begin();
}

void loop() {
  server.handleClient();
}
