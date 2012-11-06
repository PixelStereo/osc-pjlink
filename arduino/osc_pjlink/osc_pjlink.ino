#include <Ethernet.h>
#include <SPI.h>
#include <ArdOSC.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte ip[] = { 10, 0, 0, 10 };
byte vp3[] = { 10, 0, 0, 13 }; 
int serverPort = 10000;

EthernetClient client;
OSCServer server;

void setup()
{
  Ethernet.begin(mac, ip);
  Serial.begin(19200);
  client.connect(vp3, 4352);
  server.begin(serverPort);
  server.addCallback("/pjlink.1/shutter",&shutter1);
  delay(1000);
  Serial.println("connecting...");
  if (client.connected()) {
    Serial.println("connected");
  } else {
    Serial.println("connection failed");
  }
}

void loop()
{ 
    if(server.aviableCheck()>0){
}
  if (client.available()) {
    char c = client.read();
    Serial.print(c);
  }

  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
    for(;;)
      ;
  }
}

void shutter1(OSCMessage *_mes) {
  int value = _mes->getArgInt32(0);
  if(value == 1){
      Serial.println("UN");
      client.println("%1AVMT 31\r");
  }
  else{
      Serial.print("ZERO");
      client.print("%1AVMT 30\r");
   }
}


