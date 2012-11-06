#include <Ethernet.h>
#include <SPI.h>
#include <ArdOSC.h>
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte ip[] = { 10, 0, 0, 10 };
byte vp1[] = { 10, 0, 0, 11 }; 
byte vp2[] = { 10, 0, 0, 12 }; 
byte vp3[] = { 10, 0, 0, 13 }; 
int serverPort = 10000;
EthernetClient client;
OSCServer server;

void setup() {
  Ethernet.begin(mac, ip);
  Serial.begin(19200);
  server.begin(serverPort);
  server.addCallback("/pjlink.1/shutter",&shutter1);
  server.addCallback("/pjlink.2/shutter",&shutter2);
  server.addCallback("/pjlink.3/shutter",&shutter3);
  delay(1000);
  }

void loop() { 
  // reception de messages OSC
    if(server.aviableCheck()>0){
          }
  if (client.available()) {
    char c = client.read();
    Serial.print(c);
    }
  }
void shutter1(OSCMessage *_mes) {
  //deconnection
     client.stop();
    int value = _mes->getArgInt32(0);
    // connexion au vp
     Serial.println("connecting...");
        client.connect(vp1, 4352);
    if (!client.connected()) {
          Serial.println("connection failed");
          return;
          }
  
    Serial.println("connected");
  if(value == 1){
      Serial.println();
      Serial.println("UN");
      client.print("%1AVMT 31\r");
  }
  else{
      Serial.println();
      Serial.println("ZERO");
      client.print("%1AVMT 30\r");
   }
}

void shutter2(OSCMessage *_mes) {
  //deconnection
     client.stop();
    int value = _mes->getArgInt32(0);
    // connexion au vp
     Serial.println("connecting...");
        client.connect(vp2, 4352);
    if (!client.connected()) {
          Serial.println("connection failed");
          return;
          }
  
    Serial.println("connected");
  if(value == 1){
      Serial.println();
      Serial.println("UN");
      client.print("%1AVMT 31\r");
  }
  else{
      Serial.println();
      Serial.println("ZERO");
      client.print("%1AVMT 30\r");
   }
}

void shutter3(OSCMessage *_mes) {
  //deconnection
     client.stop();
    int value = _mes->getArgInt32(0);
    // connexion au vp
     Serial.println("connecting...");
        client.connect(vp3, 4352);
    if (!client.connected()) {
          Serial.println("connection failed");
          return;
          }
  
    Serial.println("connected");
  if(value == 1){
      Serial.println();
      Serial.println("UN");
      client.print("%1AVMT 31\r");
  }
  else{
      Serial.println();
      Serial.println("ZERO");
      client.print("%1AVMT 30\r");
   }
}
