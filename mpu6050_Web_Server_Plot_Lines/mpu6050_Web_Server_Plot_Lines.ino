#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <WebServer.h>

// ================= WIFI =================
const char* ssid = "Wifi_CasaNova";
const char* password = "casanova";

WebServer server(80);

// ================= MPU CONFIG =================
const uint8_t MPU_ADDR = 0x68;

#define SDA_PIN 21
#define SCL_PIN 22

#define SAMPLE_RATE_HZ 50
#define DT (1.0f / SAMPLE_RATE_HZ)

#define IMPACT_THRESHOLD 2.5f

// ================= DATA STRUCT =================
typedef struct {
  float ax, ay, az;
  float gx, gy, gz;
  float roll, pitch;
  int impact;
} MPUData;

MPUData data;

// ================= KALMAN =================
typedef struct {
  float angle;
  float bias;
  float rate;
  float P[2][2];
} KalmanFilter;

KalmanFilter kalmanRoll, kalmanPitch;

// ================= TASK =================
TaskHandle_t mpuTask;

// ================= KALMAN FUNCTIONS =================
void kalmanInit(KalmanFilter &k) {
  k.angle = 0;
  k.bias = 0;
  k.P[0][0] = 1;
  k.P[0][1] = 0;
  k.P[1][0] = 0;
  k.P[1][1] = 1;
}

float kalmanUpdate(KalmanFilter &k, float newAngle, float newRate) {
  const float Q_angle = 0.001f;
  const float Q_bias  = 0.003f;
  const float R_measure = 0.03f;

  k.rate = newRate - k.bias;
  k.angle += DT * k.rate;

  k.P[0][0] += DT * (DT*k.P[1][1] - k.P[0][1] - k.P[1][0] + Q_angle);
  k.P[0][1] -= DT * k.P[1][1];
  k.P[1][0] -= DT * k.P[1][1];
  k.P[1][1] += Q_bias * DT;

  float S = k.P[0][0] + R_measure;
  float K0 = k.P[0][0] / S;
  float K1 = k.P[1][0] / S;

  float y = newAngle - k.angle;
  k.angle += K0 * y;
  k.bias  += K1 * y;

  float P00 = k.P[0][0];
  float P01 = k.P[0][1];

  k.P[0][0] -= K0 * P00;
  k.P[0][1] -= K0 * P01;
  k.P[1][0] -= K1 * P00;
  k.P[1][1] -= K1 * P01;

  return k.angle;
}

// ================= MPU INIT =================
void initMPU() {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B);
  Wire.write(0x00);
  Wire.endTransmission();

  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x1C);
  Wire.write(0x00);
  Wire.endTransmission();

  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x1B);
  Wire.write(0x00);
  Wire.endTransmission();
}

// ================= MPU TASK =================
void mpuTaskFunction(void *pv) {
  while (true) {
    int16_t ax, ay, az, gx, gy, gz, temp;

    Wire.beginTransmission(MPU_ADDR);
    Wire.write(0x3B);
    Wire.endTransmission(false);

    if (Wire.requestFrom(MPU_ADDR, (uint8_t)14) != 14) {
      vTaskDelay(10 / portTICK_PERIOD_MS);
      continue;
    }

    ax = Wire.read() << 8 | Wire.read();
    ay = Wire.read() << 8 | Wire.read();
    az = Wire.read() << 8 | Wire.read();
    temp = Wire.read() << 8 | Wire.read();
    gx = Wire.read() << 8 | Wire.read();
    gy = Wire.read() << 8 | Wire.read();
    gz = Wire.read() << 8 | Wire.read();

    data.ax = ax / 16384.0f;
    data.ay = ay / 16384.0f;
    data.az = az / 16384.0f;

    data.gx = gx / 131.0f;
    data.gy = gy / 131.0f;
    data.gz = gz / 131.0f;

    float rollAcc  = atan2(data.ay, data.az) * 180.0f / PI;
    float pitchAcc = atan2(-data.ax,
                     sqrt(data.ay*data.ay + data.az*data.az))
                     * 180.0f / PI;

    data.roll  = kalmanUpdate(kalmanRoll, rollAcc, data.gx);
    data.pitch = kalmanUpdate(kalmanPitch, pitchAcc, data.gy);

    float mag = sqrt(
      data.ax*data.ax +
      data.ay*data.ay +
      data.az*data.az
    );

    data.impact = (mag > IMPACT_THRESHOLD) ? 1 : 0;

    vTaskDelay(1000 / SAMPLE_RATE_HZ / portTICK_PERIOD_MS);
  }
}

// ================= HTTP HANDLERS =================
void handleData() {
  String json = "{";
  json += "\"ax\":" + String(data.ax,4) + ",";
  json += "\"ay\":" + String(data.ay,4) + ",";
  json += "\"az\":" + String(data.az,4) + ",";
  json += "\"roll\":" + String(data.roll,2) + ",";
  json += "\"pitch\":" + String(data.pitch,2);
  json += "}";

  server.send(200, "application/json", json);
}

// ================= HTML PAGE =================
const char HTML_PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="utf-8">
<title>ESP32 MPU6050</title>
<script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
</head>
<body>
<h2>ESP32 MPU6050 - Tempo Real</h2>
<canvas id="chart"></canvas>

<script>
const ctx = document.getElementById('chart');
const chart = new Chart(ctx, {
  type: 'line',
  data: {
    labels: [],
    datasets: [
      {label:'ax', data:[]},
      {label:'ay', data:[]},
      {label:'az', data:[]},
      {label:'roll', data:[]},
      {label:'pitch', data:[]}
    ]
  },
  options: {
    animation:false,
    responsive:true
  }
});

setInterval(async () => {
  const r = await fetch('/data');
  const d = await r.json();
  const t = new Date().toLocaleTimeString();

  chart.data.labels.push(t);
  chart.data.datasets[0].data.push(d.ax);
  chart.data.datasets[1].data.push(d.ay);
  chart.data.datasets[2].data.push(d.az);
  chart.data.datasets[3].data.push(d.roll);
  chart.data.datasets[4].data.push(d.pitch);

  if (chart.data.labels.length > 100) {
    chart.data.labels.shift();
    chart.data.datasets.forEach(ds => ds.data.shift());
  }

  chart.update();
}, 50);
</script>
</body>
</html>
)rawliteral";

void handleRoot() {
  server.send_P(200, "text/html", HTML_PAGE);
}

// ================= SETUP / LOOP =================
void setup() {
  Serial.begin(115200);

  Wire.begin(SDA_PIN, SCL_PIN);
  initMPU();

  kalmanInit(kalmanRoll);
  kalmanInit(kalmanPitch);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.begin();

  xTaskCreatePinnedToCore(
    mpuTaskFunction,
    "MPU_TASK",
    8192,
    NULL,
    2,
    &mpuTask,
    0
  );
}

void loop() {
  server.handleClient();
}
