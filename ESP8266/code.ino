#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#ifndef UNIT_TEST
#include <Arduino.h>
#endif
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <WiFiClient.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPUpdateServer.h>
IRsend irsend(4); 
#define REDPIN D0
#define GREENPIN D1
#define BLUEPIN D3
const int dataPin = 12;   //Outputs the byte to transfer
const int loadPin = 13;   //Controls the internal transference of data in SN74HC595 internal registers
const int clockPin = 15;  //Generates the clock signal to control the transference of data
int redNow=0;
int blueNow;
int greenNow;
int redNew;
int blueNew;
int greenNew;
ESP8266WebServer server(80); //Web server object. Will be listening in port 80 (default for HTTP)
void setup() {
  /*WiFi.begin("Rajwadu..", "10mbpsplan"); //Connect to the WiFi network
  while (WiFi.status() != WL_CONNECTED) { //Wait for connection
    delay(500);
    Serial.println("Waiting to connect…");
  }
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP()); //Print the local IP to access the server*/
  WiFi.softAP("prz","12345678");
  IPAddress ipp = WiFi.softAPIP();
  server.on("/genericArgs", handleGenericArgs); //Associate the handler function to the path
  server.begin(); //Start the server
  irsend.begin();
  pinMode(REDPIN,OUTPUT);
  pinMode(GREENPIN,OUTPUT);
  pinMode(BLUEPIN,OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(loadPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(D3,OUTPUT);
  redNow = random(255);
  blueNow = random(255);
  greenNow = random(255);
  digitalWrite(loadPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, 0); 
  digitalWrite(loadPin, HIGH);
}

boolean AC=false,TV=false,F1=false,F2=false,L1=false,L2=false,L3=false,L4=false,F1AUTO=false,F2AUTO=false;
int L1B=1023,F1L=12,F2L=12;
float AT=99,RT=99.53;
String st="";
int stable=0,normal=1,music=0;
byte up=false;
void loop() {
  server.handleClient();
}
void update(){
  st="";
  st="{ 'AC':"+String(AC);
  st += ", 'TV':"+String(TV);
  st +=", 'F1':"+String(F1);
  st +=", 'F2':"+String(F2);
  st +=", 'L1':"+String(L1);
  st +=", 'L2':"+String(L2);
  st +=", 'L3':"+String(L3);
  st +=", 'L4':"+String(L4);
  st +=", 'F1AUTO':"+String(F1AUTO);
  st +=", 'F2AUTO':"+String(F2AUTO);
  st +=", 'L1B':"+String(L1B);
  st += ", 'F1L':"+String(F1L)+", 'F2L':";
  st += String(F2L)+" }";
  server.send(200, "text/plain", st);
}
void handleGenericArgs() {
    if(server.argName(0)=="TV"){
      if(server.arg(0)=="0"){TV=false;}
      else{TV=true;}up=true;}
    else if(server.argName(0)=="F1"){
      if(server.arg(0)=="0"){F1=false;}
      else{F1=true;}up=true;}
    else if(server.argName(0)=="F2"){
      if(server.arg(0)=="0"){F2=false;}
      else{F2=true;}up=true;}
    else if(server.argName(0)=="AC"){
      if(server.arg(0)=="0"){AC=0;}
      else{AC=true;}up=true;}
    else if(server.argName(0)=="L1"){
      if(server.arg(0)=="0"){L1=false;}
      else{L1=true;}up=true;}
    else if(server.argName(0)=="L2"){
      if(server.arg(0)=="0"){L2=false;}
      else{L2=true;}up=true;}
    else if(server.argName(0)=="L3"){
      if(server.arg(0)=="0"){L3=false;}
      else{L3=true;}up=true;}
    else if(server.argName(0)=="L4"){
      if(server.arg(0)=="0"){L4=false;}
      else{L4=true;}up=true;}
    else if(server.argName(0)=="update"){
      if(server.arg(0)=="1"){update();}}
    else if(server.argName(0)=="roomt"){
      if(server.arg(0)=="1"){server.send(200, "text/plain", String((int)RT));}}
    else if(server.argName(0)=="act"){
      if(server.arg(0)=="1"){server.send(200, "text/plain", String((int)AT));}}
    else if(server.argName(0)=="lbright"){
      st = server.arg(0);
      L1B=st.toInt();
    }
    else if(server.argName(0)=="flevel"){
      st= server.arg(0);
      F1L=st.toInt();
      F1AUTO=false;
    }
    else if(server.argName(0)=="f1auto"){
      if(server.arg(0)=="1"){
        F1AUTO = true;
      }
    }
    else if(server.argName(0)=="ir"){
      uint32_t code = strtoul(server.arg(0).c_str(), NULL, 10);
      #if SEND_NEC
        irsend.sendNEC(code, 32);
      #endif  // SEND_NEC
    }
    else if(server.argName(0)="led_mode"){
      if(server.arg(0)=="1"){
        normal=1;music=0;stable=0;
      }else if(server.arg(0)=="2"){
        normal=0;music=1;stable=0;
      }else if(server.arg(0)=="3"){
        normal=0;music=0;stable=1;
        analogWrite(D0,server.arg(1).toInt());
        analogWrite(D1,server.arg(2).toInt());
        analogWrite(D3,server.arg(3).toInt());
      }
    }
    server.send(200, "text/plain", "1");
    if(up==true){
      updateshift();
      up=false;
    }
}

void updateshift(){
  st="";
  char buf[8];
  (AC==true)?(st=st+"1"):(st=st+"0");
  (TV==true)?(st=st+"1"):(st=st+"0");
  (F1==true)?(st=st+"1"):(st=st+"0");
  (F2==true)?(st=st+"1"):(st=st+"0");
  (L1==true)?(st=st+"1"):(st=st+"0");
  (L2==true)?(st=st+"1"):(st=st+"0");
  (L3==true)?(st=st+"1"):(st=st+"0");
  (L4==true)?(st=st+"1"):(st=st+"0");
  int data =0;
  st.toCharArray(buf,8);
  for (int i=0; i< strlen(buf); i++)  // for every character in the string  strlen(s) returns the length of a char array
  {
    data *= 2; // double the result so far
    if (buf[i] == '1') data++;  //add 1 if needed
  }
  digitalWrite(loadPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, data); 
  digitalWrite(loadPin, HIGH);
}
