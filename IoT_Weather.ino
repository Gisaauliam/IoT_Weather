#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

const char* ssid     = "HOTSPOT@UPNJATIM.AC.ID";
const char* password = "belanegara";

// DHT
#define DHTPIN 0
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Web Server
ESP8266WebServer server(80);

// ================= WEB (PINK IMUT) =================
void handleRoot() {
  String html = R"====(
<!DOCTYPE html>
<html>
<head>
  <title>Weather Mpruy</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body {
      font-family: 'Comic Sans MS', cursive;
      text-align: center;
      background: linear-gradient(to right, #ff9a9e, #fecfef);
      color: #fff;
      margin-top: 50px;
    }

    .card {
      background: rgba(255,255,255,0.3);
      padding: 25px;
      border-radius: 25px;
      display: inline-block;
      box-shadow: 0 10px 25px rgba(0,0,0,0.2);
      backdrop-filter: blur(10px);
    }

    .team {
      font-size:20px;
      font-weight:bold;
      background: rgba(255,255,255,0.3);
      display:inline-block;
      padding:8px 15px;
      border-radius:20px;
      margin-bottom:20px;
    }

    .value {
      font-size: 45px;
      font-weight: bold;
    }

    .label {
      font-size: 18px;
    }

    .emoji {
      font-size: 30px;
    }
  </style>
</head>
<body>

  <h1> Weather Mpruy </h1>
  <div class="team"> Kelompok 5 <3</div>

  <div class="card">
    <div class="label">Temperature</div>
    <div class="value" id="temp">--</div>

    <div class="label">Humidity</div>
    <div class="value" id="hum">--</div>
  </div>

<script>
function getData() {
  fetch('/json')
    .then(res => res.json())
    .then(data => {
      document.getElementById('temp').innerHTML = data.temperature + " °C";
      document.getElementById('hum').innerHTML = data.humidity + " %";
    });
}
setInterval(getData, 2000);
getData();
</script>

</body>
</html>
)====";

  server.send(200, "text/html", html);
}

// ================= JSON =================
void handleRootJSON() {
  float t = dht.readTemperature();
  float h = dht.readHumidity();

  if (isnan(t) || isnan(h)) {
    server.send(500, "application/json", "{\"error\":\"sensor\"}");
    return;
  }

  String json = "{";
  json += "\"temperature\": " + String(t) + ",";
  json += "\"humidity\": " + String(h);
  json += "}";

  server.send(200, "application/json", json);
}

// ================= SETUP =================
void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);

  dht.begin();

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextColor(WHITE);

  WiFi.begin(ssid, password);

  int retry = 0;
  while (WiFi.status() != WL_CONNECTED && retry < 20) {
    delay(500);
    retry++;
  }

  server.on("/", handleRoot);
  server.on("/json", handleRootJSON);
  server.begin();

  // tampilkan IP
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,0);
  display.println("IP Address:");
  display.setTextSize(2);
  display.setCursor(0,20);
  display.println(WiFi.localIP());
  display.display();
  delay(3000);
}

// ================= LOOP =================
void loop() {
  server.handleClient();

  float t = dht.readTemperature();
  float h = dht.readHumidity();

  display.clearDisplay();

  display.setTextSize(2);
  display.setCursor(0,0);
  display.print("T:");
  display.print(t);

  display.setCursor(0,20);
  display.print("H:");
  display.print(h);

  display.setTextSize(1);
  display.setCursor(0,50);
  display.print(WiFi.localIP());

  display.display();
  delay(2000);
}