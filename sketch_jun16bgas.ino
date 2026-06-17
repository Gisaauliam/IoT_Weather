#include <DHT.h>

#define DHTPIN 4
#define DHTTYPE DHT22
#define MQ2_PIN 34

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  delay(1000);

  dht.begin();
  Serial.println("ESP32 siap membaca DHT22 dan MQ2");
}

void loop() {
  float suhu = dht.readTemperature();
  float kelembapan = dht.readHumidity();
  int gas = analogRead(MQ2_PIN);

  Serial.println("====================");

  if (isnan(suhu) || isnan(kelembapan)) {
    Serial.println("Gagal membaca DHT22");
  } else {
    Serial.print("Suhu: ");
    Serial.print(suhu);
    Serial.println(" C");

    Serial.print("Kelembapan: ");
    Serial.print(kelembapan);
    Serial.println(" %");
  }

  Serial.print("Gas MQ2: ");
  Serial.println(gas);

  delay(2000);
}
