#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DS1307.h>
#include <DHT.h>
#include <FS.h>

DHT dht(DHTPIN, DHTTYPE);
DS1307 rtc(4, 5);
Time t;
int horaAtual, minutoAtual, segundoAtual, horaIrrigacao1, minutoIrrigacao1, horaIrrigacao2, minutoIrrigacao2, horaIrrigacao3, minutoIrrigacao3,  irrigou1, irrigou2, irrigou3;

ESP8266WebServer server(8110);
IPAddress ip(192,168,0,110);
IPAddress gateway(192,168,0,1);
IPAddress subnet(255,255,255,0);
IPAddress dns(192,168,0,1);

#define pin_led A0
#define pin_snsr 16
#define DHTPIN 14 
#define DHTTYPE DHT22   // DHT 22, AM2302, AM2321

int rele = 2;
int luz = 0;
int m_state;
int luz_state;
int modo_auto;
int intervalo;
char ssid[] = "Willy2";
char pass[] = "40028922";



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
  pinMode(luz, OUTPUT);
  digitalWrite(rele, HIGH);
  digitalWrite(luz, HIGH);
  WiFi.begin(ssid,pass);
  Serial.begin(115200);
  WiFi.config(ip, dns, gateway, subnet);

    rtc.halt(false);
    //rtc.setDOW(MONDAY);      //Define o dia da semana
    //rtc.setTime(1, 1, 0);     //Define o horario
    //rtc.setDate(28, 10, 2019);   //Define o dia, mes e ano

    horaIrrigacao1 = 14;
    minutoIrrigacao1 = 0; 
    horaIrrigacao2 = 14;
    minutoIrrigacao2 = 30;
    horaIrrigacao3 = 16; 
    minutoIrrigacao3 = 30;  
    irrigou1 = 0; 
    irrigou2 = 0; 
    irrigou3 = 0;
    intervalo = 5;
 
    rtc.setSQWRate(SQW_RATE_1);
    rtc.enableSQW(true);
    
  
  //Blynk.begin(auth, ssid, pass);
  dht.begin();
  
  while(WiFi.status()!=WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  server.on("/", serveIndexFile);
  server.on("/temperatura",getTemp);
  server.on("/5/on", turnOnRele);
  server.on("/5/off", turnOffRele);
  server.on("/rele", toggleRele);
  server.on("/luz/on", turnOnLuz);
  server.on("/luz/off", turnOffLuz);
  server.on("/luz", toggleLuz);
  server.on("/ledstate",getLEDState);
  server.on("/umidade", getUmi);
  server.on("/usolo",getUmiSolo);
  server.on("/mstate", getState);
  server.on("/luzstate", getLuzState);
  server.on("/rtcon", turnOnRtc); //ligar modo automatico
  server.on("/rtcoff", turnOffRtc); //desligar modo automatico
  server.on("/rtc", toggleRtc); 
  server.on("/rtcstate", rtcState);
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

  String luz2 = readFile("/luz.txt");
  if(luz2 == "Luz Ligada") 
  {
    digitalWrite(luz, LOW);
  }
  else if(luz2 == "Luz Desligada") 
  {
    digitalWrite(luz, HIGH);
  }


  String rtc2 = readFile("/rtc.txt");
  if(rtc2 == "Irrigação Automática Desligada") 
  {
    modo_auto = 1;
  }
  else if(rtc2 == "Irrigação Automática Ligada") 
  {
    modo_auto = 0;
  }
  
}

void loop()
{
    //determina o horário atual
    t = rtc.getTime();
    horaAtual = t.hour;
    minutoAtual = t.min;
    segundoAtual = t.sec;
    server.handleClient();
    
    if(modo_auto == 0){
      funAuto();
    }
    
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

void getLuzState()
{
  luz_state = digitalRead(luz);
  if(luz_state == LOW){
    String luz_state2 = "Luz Ligada";
    server.send(200,"text/plain", luz_state2);
    }else{
      String luz_state2 = "Luz Desligada";
      server.send(200,"text/plain", luz_state2);
    }
}

void rtcState()
{
  int rtc_state = modo_auto;
  if(rtc_state == 0){
    String rtc_state2 = "Irrigação Automática Ligada";
    server.send(200,"text/plain", rtc_state2);
    }else{
      String rtc_state2 = "Irrigação Automática Desligada";
      server.send(200,"text/plain", rtc_state2);
    }
}


void getTemp()
{
  float t = dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit

  if (isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

 // Blynk.virtualWrite(V6, t);
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

 // Blynk.virtualWrite(V5, h);
  String h2 = String(h);
  server.send(200,"text/plain", h2);
}

void getUmiSolo()
{
  String u2;
  if(digitalRead(pin_snsr) == 0){
      u2 = "Úmido"; 
    } else{
        u2 = "Seco"; 
      }


  //Blynk.virtualWrite(V9, u2);
  server.send(200,"text/plain", u2);
}


void turnOnRele(){
  digitalWrite(rele, LOW);
  String led_state = "Motor Ligado";
  writeFile(led_state, "/state.txt");
  serveIndexFile();  
  }

void turnOffRele(){
  digitalWrite(rele, HIGH);
  String led_state = "Motor Desligado";
  writeFile(led_state, "/state.txt");
  serveIndexFile();
  }
  
void toggleRele(){
digitalWrite(rele, !digitalRead(rele));
  if(digitalRead(rele) == HIGH){
  String led_state = "Motor Desligado";
  writeFile(led_state, "/state.txt");
  serveIndexFile();
  } else {
      String led_state = "Motor Ligado";
      writeFile(led_state, "/state.txt");
      serveIndexFile();
  }
}


void funAuto(){
        
        int nHoraAtual = ((horaAtual*3600) + (minutoAtual*60) + (segundoAtual));
        int nHoraProg1 = ((horaIrrigacao1*3600) + (minutoIrrigacao1*60));
        int nHoraProgInt1 = ((horaIrrigacao1*3600) + (minutoIrrigacao1*60) + (intervalo));
        int nHoraProg2 = ((horaIrrigacao2*3600) + (minutoIrrigacao2*60));
        int nHoraProgInt2 = ((horaIrrigacao2*3600) + (minutoIrrigacao2*60) + (intervalo));
        int nHoraProg3 = ((horaIrrigacao3*3600) + (minutoIrrigacao3*60));
        int nHoraProgInt3 = ((horaIrrigacao3*3600) + (minutoIrrigacao3*60) + (intervalo));
       
        //verifica se é o horário da primeira irrigação
        if ((nHoraProg1 < nHoraAtual) && (nHoraAtual < nHoraProgInt1) && (irrigou1 == 0) ){
      
          digitalWrite(rele, LOW);
          digitalWrite(luz, LOW);
          
        } else if((nHoraProg1 < nHoraAtual) && (nHoraAtual > nHoraProgInt1)){
            
            digitalWrite(rele, HIGH);
            digitalWrite(luz, HIGH);
            irrigou1 = 1;
            
            }
      
        //verifica se é o horário da segunda irrigação
        if ((nHoraProg2 < nHoraAtual) && (nHoraAtual < nHoraProgInt2) && (irrigou2 == 0) ){
      
          digitalWrite(rele, LOW);
          digitalWrite(luz, LOW);
          
        } else if((nHoraProg2 < nHoraAtual) && (nHoraAtual > nHoraProgInt2)){
            
            digitalWrite(rele, HIGH);
            digitalWrite(luz, HIGH);
            irrigou2 = 1;
            
            }
      
        //verifica se é o horário da terceira irrigação
        if ((nHoraProg3 < nHoraAtual) && (nHoraAtual < nHoraProgInt3) && (irrigou3 == 0) ){
      
          digitalWrite(rele, LOW);
          digitalWrite(luz, LOW);
          
        } else if((nHoraProg3 < nHoraAtual) && (nHoraAtual > nHoraProgInt3)){
            
            digitalWrite(rele, HIGH);
            digitalWrite(luz, HIGH);
            irrigou3 = 1;
            
            }
      
        //reseta ao fim do dia
        if (horaAtual == 0 && minutoAtual == 0){
          irrigou1 = 0;
          irrigou2 = 0;
          irrigou3 = 0; 
        }


}

void turnOnRtc(){
  modo_auto = 0;
  String rtc_state = "Irrigação Automática Ligada";
  writeFile(rtc_state, "/rtc.txt");
  serveIndexFile();
  }

void turnOffRtc(){
  modo_auto = 1;
  String rtc_state = "Irrigação Automática Desligada";
  writeFile(rtc_state, "/rtc.txt");
  serveIndexFile();  
  }


void toggleRtc(){
  if(modo_auto == 0){
    modo_auto = 1;
    String rtc_state = "Irrigação Automática Desligada";
    writeFile(rtc_state, "/rtc.txt");
    serveIndexFile();
    }else if (modo_auto == 1){
      modo_auto = 0;
      String rtc_state = "Irrigação Automática Ligada";
      writeFile(rtc_state, "/rtc.txt");
      serveIndexFile();
      }
}
  
void turnOnLuz(){
  digitalWrite(luz, LOW);
  String luz_state = "Luz Ligada";
  writeFile(luz_state, "/luz.txt");
  serveIndexFile();  
  }

void turnOffLuz(){
  digitalWrite(luz, HIGH);
  String luz_state = "Luz Desligada";
  writeFile(luz_state, "/luz.txt");
  serveIndexFile();
  }
  
void toggleLuz(){
digitalWrite(luz, !digitalRead(luz));
  if(digitalRead(luz) == HIGH){
  String luz_state = "Luz Desligada";
  writeFile(luz_state, "/luz.txt");
  serveIndexFile();
  } else {
      String luz_state = "Luz Ligada";
      writeFile(luz_state, "/luz.txt");
      serveIndexFile();
  }
}


