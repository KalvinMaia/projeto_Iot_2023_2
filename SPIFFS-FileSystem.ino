#include <FS.h>
#include "SPIFFS.h"

int num=0;
String str;
String s;


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
  Serial.println("Formatou SPIFFS");
}

void openFS(void) {
  if (!SPIFFS.begin()) {
    Serial.println("\nErro ao abrir o sistema de arquivos");
  }
  else {
    Serial.println("\nSistema de arquivos aberto com sucesso!");
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("inicio");
  delay(200);
  //formatFile();
  Serial.println("abrir arquivo");
  openFS(); 
  num = random(5000);//gerando numero aleatorio para salvar no arquivo
  str = String(num);//convertendo o int para String
  writeFile(str , "/logsAula.txt");
  Serial.println("ler arquivo");
  String test = readFile("/logsAula.txt");

}

void loop() {

}




















































