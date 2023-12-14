#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <TelegramBot.h>
#include <DHT.h>
#include <FS.h>
#include <SPIFFS.h>
#include <string>

#define DHTPIN 4 
#define DHTTYPE DHT11

WiFiClient wifi_client;
PubSubClient mqtt_client(wifi_client);

const int sensorVib = 34;
const int sensorTemp = 4;
const int led = 2;

DHT dht(sensorTemp, DHT11);

const char* wifi_ssid = "Familia Maia 2G";
const char* wifi_password = "nli9q7lc";
int wifi_timeout = 100000;

const char* mqtt_broker = "io.adafruit.com";
const int mqtt_port = 1883;
int mqtt_timeout = 10000;

const char* mqtt_usernameAdafruitIO = "KalvinMaia10";
const char* mqtt_keyAdafruitIO = "aio_sCEa94lWJD0Ai2FsW6KfYy6gQZr6";

const char BotToken[] = "6663659329:AAEN7nsgIkcnT9W5L0vhkNoj6Xb9jPHi7i8"; // Define token do bot.

WiFiClientSecure net_ssl;               // Cria um cliente SSL.
TelegramBot bot (BotToken, net_ssl);    // Adiciona funções da biblioteca

int valor = 0;
String  leitura = "";

void connectWiFi() {
  Serial.print("Conectando à rede WiFi .. ");

  unsigned long tempoInicial = millis();
  while (WiFi.status() != WL_CONNECTED && (millis() - tempoInicial < wifi_timeout)) {
    Serial.print(".");
    delay(100);
  }
  Serial.println();

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Conexão com WiFi falhou!");
  } else {
    Serial.print("Conectado com o IP: ");
    Serial.println(WiFi.localIP());
    digitalWrite(led, HIGH);
  }
}

void connectMQTT() {
  unsigned long tempoInicial = millis();
  while (!mqtt_client.connected() && (millis() - tempoInicial < mqtt_timeout)) {
    if (WiFi.status() != WL_CONNECTED) {
      connectWiFi();
    }
    Serial.print("Conectando ao MQTT Broker..");

    if (mqtt_client.connect("ESP32Client", mqtt_usernameAdafruitIO, mqtt_keyAdafruitIO)) {
      Serial.println();
      Serial.print("Conectado ao broker MQTT!");
    } else {
      Serial.println();
      Serial.print("Conexão com o broker MQTT falhou!");
      delay(500);
    }
  }
  Serial.println();
}

void writeFile(String state, String path) { //escreve conteúdo em um arquivo
  File rFile = SPIFFS.open(path, "a");//a para anexar
  if (!rFile) {
    Serial.println("Erro ao abrir arquivo!");
  }
  else {
    Serial.print("tamanho");
    Serial.println(rFile.size());
    rFile.println(state);
    Serial.print("Gravou: ");
    Serial.println(state);
  }
  rFile.close();
}

String readFile(String path) {
  Serial.println("Read file");
  File rFile = SPIFFS.open(path, "r");//r+ leitura e escrita
  if (!rFile) {
    Serial.println("Erro ao abrir arquivo!");
  }
  else {
    Serial.print("----------Lendo arquivo ");
    Serial.print(path);
    Serial.println("  ---------");
    while (rFile.position() < rFile.size())
    {
      s = rFile.readStringUntil('\n');
      s.trim();
      Serial.println(s);
    }
    rFile.close();
    return s;
  }
}

void formatFile() {
  Serial.println("Formantando SPIFFS");
  SPIFFS.format();
  Serial.println("Formatou SPIFFS com sucesso!");
}

void openFS(void) {
  if (!SPIFFS.begin()) {
    Serial.println("\nErro ao abrir o sistema de arquivos!");
  }
  else {
    Serial.println("\nSistema de arquivos aberto com sucesso!");
  }
}

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(1);
  WiFi.mode(WIFI_STA); //"station mode": permite o ESP32 ser um cliente da rede WiFi
  WiFi.begin(wifi_ssid, wifi_password);
  connectWiFi();
  mqtt_client.setServer(mqtt_broker, mqtt_port);

  bot.begin();  // Iniciar bot do Telegram.

  dht.begin();

  pinMode(sensorVib, INPUT);
  pinMode(led, OUTPUT);

}

void loop() {
  leitura = Serial.readString();

  if(leitura != "") Serial.println(leitura);
  
  int sensorValue = digitalRead(sensorVib);
  float temperatura = dht.readTemperature();
  int umidade = dht.readHumidity();
  
  Serial.println("Vibracao: "); // (mm/s) = sensorValue*1mV/3300mV
  Serial.println(sensorValue); // (mm/s) = sensorValue*1mV/3300mV
  Serial.println("Umidade: ");
  Serial.println(umidade);
  Serial.println("Temperatura: ");
  Serial.println(temperatura);
  
  
  if (!mqtt_client.connected()) { // Se MQTT não estiver conectado
    connectMQTT();
  }

  if (mqtt_client.connected()) {
    
     
    mqtt_client.publish("KalvinMaia10/feeds/imd0902", String(temperatura).c_str());
    Serial.println("Publicou o dado: " + String(temperatura));
    
    mqtt_client.loop();

    mqtt_client.publish("KalvinMaia10/feeds/imd0903", String(umidade).c_str());
    Serial.println("Publicou o dado: " + String(umidade));
    
    mqtt_client.loop();

    mqtt_client.publish("KalvinMaia10/feeds/vibracao", String(sensorValue).c_str());
    Serial.println("Publicou o dado: " + String(sensorValue));
    
    mqtt_client.loop();
    delay(10000);
  }
  
    message m = bot.getUpdates();     
    if ( m.chat_id != 0 ) {           
      Serial.println(m.text);
      bot.sendMessage(m.chat_id, m.text); 
    } 
    else {
      Serial.println("Nenhuma nova mensagem.");
    }


}
