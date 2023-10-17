#include <WiFi.h>

const char* wifi_ssid = "Heitor-S20-FE";
const char* wifi_password = "heitor123";
int wifi_timeout = 200000;

int led = 2;

void conecteWiFi(){
  WiFi.mode(WIFI_STA); //"station mode": permite o ESP32 ser um cliente da rede WiFi
  WiFi.begin(wifi_ssid, wifi_password);
  Serial.print("Conectando à rede WiFi .. ");

  unsigned long startTime = millis();
  
  while(WiFi.status() != WL_CONNECTED && (millis() - startTime < wifi_timeout)){
    Serial.print(".");
    delay(100);
  }
  Serial.println();

  if(WiFi.status() != WL_CONNECTED){
    Serial.println("Falhou!");
  } else{
    Serial.print("Conectado com o IP: ");
    Serial.println(WiFi.localIP());
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(led, OUTPUT);
  conecteWiFi();
}

void loop() {
  digitalWrite(led, HIGH);
  delay(2000);
  digitalWrite(led, LOW);
  delay(2000);
}
