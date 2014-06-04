#include <Ethernet.h>
#include <EthernetUdp.h>
#include <SPI.h>    
#include <OSCBundle.h>
#include <OSCBoards.h>
EthernetUDP Udp;
IPAddress ip(10, 0, 0, 10);
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte vpIp[3][4] = { { 10, 0, 0, 11 },  { 10, 0, 0, 12 },  { 10, 0, 0, 13 } } ;
const unsigned int inPort = 10000;
const unsigned int outPort = 4352;
int value;
int Matched;
EthernetClient vpClient[3];


/* **************************************************************** */
/* ************************ SET UP ******************************** */
/* **************************************************************** */
void setup() {
/* ************ Launch ethernet server  *************************** */
  Ethernet.begin(mac,ip);
  Udp.begin(inPort);
/********** Launch Serial Communication for visca commands ******** */
  Serial.begin(9600);
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
      MessageIN.route("/vp.1", vp1);
      MessageIN.route("/vp.2", vp2);
      MessageIN.route("/vp.3", vp3);
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
/* **************************** VP 2 ****************************** */
/* **************************************************************** */
void vp2(OSCMessage &msg, int addrOffset ){
    Matched = msg.match("/shutter", addrOffset);
    if(Matched == 8){  
        getValue (msg, 0);
        shutter (2, value);
    }
}

/* **************************************************************** */
/* **************************** VP 3 ****************************** */
/* **************************************************************** */
void vp3(OSCMessage &msg, int addrOffset ){
    Matched = msg.match("/shutter", addrOffset);
    if(Matched == 8){  
        getValue (msg, 0);
        shutter (3, value);
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

  --vp; // 1, 2, 3 => 0, 1, 2 pour index de tableau
   
 char c = (vpClient[vp].read());
    if ( value == 1 ) {
        vpClient[vp].print("%1AVMT 31\r");
        Serial.print(c);
        while ( vpClient[vp].available() )
           {       
            }
          vpClient[vp].stop();
      }  
    else {
        vpClient[vp].print("%1AVMT 30\r");
        Serial.print(c);
        while ( vpClient[vp].available() )
           {        
            }
            vpClient[vp].stop();
      }
 
}
