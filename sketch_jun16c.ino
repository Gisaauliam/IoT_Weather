#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>

#define DHTPIN 4
#define DHTTYPE DHT22
#define MQ2_PIN 34

const char* ssid = "KOS PUTRI";
const char* password = "kosputri7";

DHT dht(DHTPIN, DHTTYPE);
WebServer server(80);

float suhu = 0;
float kelembapan = 0;
int gas = 0;

void bacaSensor() {
  suhu = dht.readTemperature();
  kelembapan = dht.readHumidity();
  gas = analogRead(MQ2_PIN);

  Serial.println("====================");

  if (isnan(suhu) || isnan(kelembapan)) {
    Serial.println("Gagal membaca DHT22");
  } else {
    Serial.print("Suhu: ");
    Serial.print(suhu);
    Serial.println(" °C");

    Serial.print("Kelembapan: ");
    Serial.print(kelembapan);
    Serial.println(" %");
  }

  Serial.print("Gas MQ2: ");
  Serial.println(gas);
}

void handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Dashboard Monitoring Udara</title>
<style>
body{
  font-family: Arial;
  background:#f4f4f4;
  text-align:center;
  padding:20px;
}
.card{
  background:white;
  width:300px;
  margin:15px auto;
  padding:20px;
  border-radius:15px;
  box-shadow:0 2px 8px rgba(0,0,0,0.2);
}
.value{
  font-size:35px;
  font-weight:bold;
  color:#0077cc;
}
</style>
</head>
<body>

<h2>Dashboard Monitoring Udara ESP32</h2>

<div class="card">
  <h3>Suhu</h3>
  <div class="value" id="suhu">0 °C</div>
</div>

<div class="card">
  <h3>Kelembapan</h3>
  <div class="value" id="hum">0 %</div>
</div>

<div class="card">
  <h3>Gas MQ2</h3>
  <div class="value" id="gas">0</div>
</div>

<div class="card">
  <h3>Status Udara</h3>
  <div class="value" id="status">AMAN</div>
</div>

<script>
function loadData(){
fetch('/data')
.then(response => response.json())
.then(data => {
document.getElementById("suhu").innerHTML = data.suhu + " °C";
document.getElementById("hum").innerHTML = data.kelembapan + " %";
document.getElementById("gas").innerHTML = data.gas;

if(data.gas > 2000){
document.getElementById("status").innerHTML = "BAHAYA";
}
else if(data.gas > 1200){
document.getElementById("status").innerHTML = "WASPADA";
}
else{
document.getElementById("status").innerHTML = "AMAN";
}
});
}

setInterval(loadData, 2000);
loadData();
</script>

</body>
</html>
)rawliteral";

  server.send(200, "text/html", html);
}

void handleData() {
  bacaSensor();

  String json = "{";
  json += "\"suhu\":" + String(suhu,1) + ",";
  json += "\"kelembapan\":" + String(kelembapan,1) + ",";
  json += "\"gas\":" + String(gas);
  json += "}";

  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200);
  delay(2000);

  Serial.println("ESP32 START");

  dht.begin();

  Serial.println("Menghubungkan ke WiFi...");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi Connected");
  Serial.print("IP Address : ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/data", handleData);

  server.begin();

  Serial.println("Dashboard Aktif");
}

void loop() {
  server.handleClient();
}