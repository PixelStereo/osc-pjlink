#include <SD.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <SPI.h>    
#include <OSCBundle.h>
#include <OSCBoards.h>
EthernetUDP Udp;
EthernetClient client;
IPAddress ip(192, 168, 0, 111);
byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte outIp[4] = { 
  192, 168, 0, 29 } 
;
byte vpIp[4] = { 
  192, 168, 0, 29               }
;
EthernetClient vpClient;
const unsigned int inPort = 10000;
const unsigned int outPort = 4352;
const unsigned int OSCoutPort = 9999;
int value;
int Matched;
const int chipSelect = 4;

/* **************************************************************** */
/* ************************ SET UP ******************************** */
/* **************************************************************** */
void setup() {
  Serial.begin(9600);

  /* ************ Launch ethernet server  *************************** */
  pinMode(10, OUTPUT);
  Ethernet.begin(mac,ip);
  Udp.begin(inPort);
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    // don't do anything more:
    return;
  }
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("config.txt");
  // if the file is available, write to it:
  if (dataFile) {
    byte index = 0;
    while (dataFile.available())    {
      OSCMessage DEBUGMsg("/debug");
      DEBUGMsg.add(dataFile.read());
      Udp.beginPacket(outIp, OSCoutPort);
      DEBUGMsg.send(Udp);
      Udp.endPacket(); 
      DEBUGMsg.empty();    
    }
    dataFile.close();
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening config.txt");
  } 
}


/* **************************************************************** */
/* *********************** THE LOOP ******************************* */
/* **************************************************************** */
void loop(){ 
  /* **************************************************************** */
  /* *************** UDP MESSAGES COMING OVER OSC******************** */
  /* **************************************************************** */
  OSCMessage MessageIN;
  int size;
  if( (size = Udp.parsePacket())>0)
  {
    while(size--)
      MessageIN.fill(Udp.read());
    if(!MessageIN.hasError()) {
      MessageIN.route("/vp", vp1);
    }
  }
}


/* **************************************************************** */
/* **************************** VP 1 ****************************** */
/* **************************************************************** */
void vp1(OSCMessage &msg, int addrOffset ){
  Matched = msg.match("/shutter", addrOffset);
  if(Matched == 8){  
    getValue (msg, 0);
    shutter (1, value);
  }
}


/* **************************************************************** */
/* ******************* GETVALUE (FLOAT OR INT ********************* */
/* **************************************************************** */
void getValue(OSCMessage &msg, int addrOffset){ 
  if (msg.isInt(0)){      
    value = msg.getInt(0);
  } 
  else if(msg.isFloat(0)){
    value = msg.getFloat(0);
  }
}


/* **************************************************************** */
/* ************************** SHUTTER ***************************** */
/* **************************************************************** */
void shutter(byte vp, int value) {
  OSCMessage OutMsg("/feedback/shutter");
  if (vpClient.connect(vpIp, 4352)) {
    if ( value == 1 ) {
      OutMsg.add(1);
      vpClient.print("%1AVMT 31\r");
      vpClient.stop();
    }  
    else {
      OutMsg.add(0);
      vpClient.print("%1AVMT 30\r");
      vpClient.stop();
    }  
    Udp.beginPacket(outIp, OSCoutPort);
    OutMsg.send(Udp);
    Udp.endPacket(); 
    OutMsg.empty();  
  } 
  else {
    OutMsg.add("Connection Failed");
    Udp.beginPacket(outIp, OSCoutPort);
    OutMsg.send(Udp);
    Udp.endPacket(); 
    OutMsg.empty();  
  }

} 














