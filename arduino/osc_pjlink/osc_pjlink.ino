#include <Ethernet.h>
#include <SPI.h>
#include <ArdOSC.h>
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte ip[] = { 10, 0, 0, 10 };
byte vpIp[3][4] = { { 10, 0, 0, 11 },  { 10, 0, 0, 12 },  { 10, 0, 0, 13 } } ;
EthernetClient vpClient[3];
int serverPort = 10000;
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
  if(server.aviableCheck()>0) {
      } 
}

void shutter1(OSCMessage *_mes) {
  shutter( 1, _mes );
}
void shutter2(OSCMessage *_mes) {
  shutter( 2, _mes );
}
void shutter3(OSCMessage *_mes) {
  shutter( 3, _mes );
}
void shutter(byte vp, OSCMessage *_mes) {

  --vp; // 1, 2, 3 => 0, 1, 2 pour index de tableau

    int value = _mes->getArgInt32(0);

    if ( ! vpClient[vp].connected() )
    {
       // connexion au vp
       Serial.println("connecting...");
       int ret = vpClient[vp].connect(vpIp[vp], 4352);
       if ( (ret == 0) || !vpClient[vp].connected() )
       {
          Serial.println("connection failed");
          return;
       }
       else {
      Serial.println("connected");
       }
    }
      if ( value == 1 ) {
        vpClient[vp].print("%1AVMT 31\r");
        vpClient[vp].stop();
      }
      else {
        vpClient[vp].print("%1AVMT 30\r");
        vpClient[vp].stop();
      }
}

      
      
      
