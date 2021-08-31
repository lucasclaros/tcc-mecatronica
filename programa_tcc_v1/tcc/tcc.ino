#include <EEPROM.h>

#define BLYNK_PRINT Serial


#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#include <BlynkSimpleEsp8266.h>
#include <DHT.h>
#include <FS.h>

IPAddress ip(192,168,0,110);
IPAddress gateway(192,168,0,2);
IPAddress subnet(255,255,255,0);
IPAddress dns(192,168,0,2);


ESP8266WebServer server(8110);
#define pin_led A0
#define pin_snsr 16

#define DHTPIN 4 
#define DHTTYPE DHT22   // DHT 22, AM2302, AM2321
DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;

int rele = 5;
int m_state;
char ssid[] = "Willy2";
char pass[] = "40028922";

char auth[] = "0909294076c844808a82925d185672c6";





/**
  * @desc escreve conteúdo em um arquivo
  * @param string state - conteúdo a se escrever no arquivo
  * @param string path - arquivo a ser escrito
*/
void writeFile(String state, String path) { 
  //Abre o arquivo para escrita ("w" write)
  //Sobreescreve o conteúdo do arquivo
  File rFile = SPIFFS.open(path,"w+"); 
  if(!rFile){
    Serial.println("Erro ao abrir arquivo!");
  } else {
    rFile.println(state);
    Serial.print("gravou estado: ");
    Serial.println(state);
  }
  rFile.close();
}
 
/**
  * @desc lê conteúdo de um arquivo
  * @param string path - arquivo a ser lido
  * @return string - conteúdo lido do arquivo
*/
String readFile(String path) {
  File rFile = SPIFFS.open(path,"r");
  if (!rFile) {
    Serial.println("Erro ao abrir arquivo!");
  }
  String content = rFile.readStringUntil('\r'); //desconsidera '\r\n'
  Serial.print("leitura de estado: ");
  Serial.println(content);
  rFile.close();
  return content;
}
 
/**
  * @desc inicializa o sistema de arquivos
*/
void openFS(void){
  //Abre o sistema de arquivos
  if(!SPIFFS.begin()){
    Serial.println("\nErro ao abrir o sistema de arquivos");
  } else {
    Serial.println("\nSistema de arquivos aberto com sucesso!");
  }
}
     

  
void setup()
{

  
  pinMode(pin_led, INPUT);
  pinMode(pin_snsr, INPUT);
  pinMode(rele, OUTPUT);
  digitalWrite(rele, HIGH);
  WiFi.begin(ssid,pass);
  Serial.begin(115200);
  WiFi.config(ip, dns, gateway, subnet);
  
  Blynk.begin(auth, ssid, pass);
  dht.begin();
  //timer.setInterval(250L, getTemp);
  
  while(WiFi.status()!=WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
 /* server.send(200,"text/html", webpage);
  server.on("/",[](){server.send(200,"text/html", webpage);});*/
  server.on("/", serveIndexFile);
  server.on("/temperatura",getTemp);
  server.on("/5/on", turnOnRele);
  server.on("/5/off", turnOffRele);
  server.on("/rele", toggleRele);
  server.on("/ledstate",getLEDState);
  server.on("/umidade", getUmi);
  server.on("/usolo",getUmiSolo);
  server.on("/mstate", getState);
  server.begin();
  openFS();

  // verifica o último estado do LED e ativa de acordo
  String state = readFile("/state.txt");
  if(state == "Motor Ligado") 
  {
    digitalWrite(rele, LOW);
  }
  else if(state == "Motor Desligado") 
  {
    digitalWrite(rele, HIGH);
  }
}

void loop()
{
  Blynk.run();
  timer.run();

  server.handleClient();
}


void serveIndexFile()
{
  File file = SPIFFS.open("/index.html","r");
  server.streamFile(file, "text/html");
  file.close();
}


void getLEDState()
{
  int led_state = analogRead(pin_led);
  led_state = map(led_state, 0, 600, 0, 100);
  String led_state2 = String(led_state);
  server.send(200,"text/plain", led_state2);
}

void getState()
{
  m_state = digitalRead(rele);
  if(m_state == LOW){
    String m_state2 = "Motor Ligado";
    server.send(200,"text/plain", m_state2);
    }else{
      String m_state2 = "Motor Desligado";
      server.send(200,"text/plain", m_state2);
    }
}


void getTemp()
{
  float t = dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit

  if (isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Blynk.virtualWrite(V6, t);
  String t2 = String(t);
  server.send(200,"text/plain", t2);
}

void getUmi()
{
  float h = dht.readHumidity();

  if (isnan(h)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Blynk.virtualWrite(V5, h);
  String h2 = String(h);
  server.send(200,"text/plain", h2);
}

void getUmiSolo()
{
  String u2;
  if(digitalRead(pin_snsr) == 0){
      u2 = "Úmido"; 
    } else {
        u2 = "Seco"; 
      }


  Blynk.virtualWrite(V9, u2);
  server.send(200,"text/plain", u2);
}


void turnOnRele(){
  digitalWrite(rele, LOW);
  String led_state = "Motor Ligado";
  writeFile(led_state, "/state.txt");
  serveIndexFile();
  //server.send(200,"text/plain", led_state);
  
  }

void turnOffRele(){
  digitalWrite(rele, HIGH);
  String led_state = "Motor Desligado";
  writeFile(led_state, "/state.txt");
  serveIndexFile();
  //server.send(200,"text/plain", led_state);
  }
  
void toggleRele(){
digitalWrite(rele, !digitalRead(rele));
serveIndexFile();
//server.send(200,"text/html", webpage);
}


