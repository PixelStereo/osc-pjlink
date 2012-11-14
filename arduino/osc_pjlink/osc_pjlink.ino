#include <Ethernet.h>
#include <SPI.h>
#include <ArdOSC.h>
// start lcd
#include <Wire.h>
#include <Adafruit_MCP23017.h>
#include <Adafruit_RGBLCDShield.h>
// The shield uses the I2C SCL and SDA pins. On classic Arduinos
// this is Analog 4 and 5 so you can't use those for analogRead() anymore
// However, you can connect other I2C sensors to the I2C bus and share
// the I2C bus.
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();
// These #defines make it easy to set the backlight color
#define WHITE 0x7
/****************   Définir des caractères spéciaux LCD (8 caractères max) ********************/

byte Char_UP[8] = { // Flèche haut (tableau de 8 octets)
  B00000, // définition de chaque octet au format binaire
  B00000, // 1 pour pixel affiché - 0 pour pixel éteint
  B00100, // les 3 bits de poids forts ne sont pas écrits car inutiles
  B01110,
  B11111,
  B00000,
  B00000,
  B00000
};
//
byte Char_DOWN[8] = { // Flèche bas
  B00000,
  B00000,
  B11111,
  B01110,
  B00100,
  B00000,
  B00000,
  B00000
};
//
byte Char_RIGHT[8] = { // Flèche droite
  B00000,
  B00000,
  B10000,
  B11000,
  B11100,
  B11000,
  B10000,
  B00000
};
//
byte Char_LEFT[8] = { // Flèche gauche
  B00000,
  B00000,
  B00001,
  B00011,
  B00111,
  B00011,
  B00001,
  B00000
};
//
byte Char_SELECT[8] = { // Select
  B00000,
  B00000,
  B01110,
  B01110,
  B01110,
  B01110,
  B00000,
  B00000
};
//
byte Char_NONE[8] = { // Char.vide
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};
/****************  Fin caractères spéciaux LCD ********************/

/************* Constantes programme ****************/
// boutons
const int btnNONE = 0;
const int btnRIGHT = 1;
const int btnLEFT = 2;
const int btnUP = 3;
const int btnDOWN = 4;
const int btnSELECT = 5;

//texte boutons
const char bt_txt[6][10] = {
  "Bt.nul",
  "Bt.droite",
  "Bt.gauche",
  "Bt.haut",
  "Bt.bas",
  "Bt.select" };

/************* Variables ****************/
int adc_key_in = 0;
int lcd_key = 0;
int old_lcd_key = 0;
// end lcd
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
    lcd.begin(16, 2);        // initialiser la LCD library, 16 caractères, 2 lignes
  lcd.setBacklight(WHITE);
  lcd.setCursor(0,0);      // placer le curseur à la première ligne, première position
  lcd.print("IP:");
  lcd.print(ip[0],DEC);
  lcd.print(".");
  lcd.print(ip[1],DEC);
  lcd.print(".");
  lcd.print(ip[2],DEC);
  lcd.print(".");
  lcd.print(ip[3],DEC);
  lcd.print(" ");     
  lcd.setCursor(0,1);      // placer le curseur à la première ligne, première position
  lcd.print("Port UDP : ");
  lcd.print(serverPort);
  // création car. spéciaux LCD
  lcd.createChar(0, Char_NONE); // char vide pour bt.none
  lcd.createChar(1, Char_RIGHT);
  lcd.createChar(2, Char_LEFT);
  lcd.createChar(3, Char_UP);
  lcd.createChar(4, Char_DOWN);
  lcd.createChar(5, Char_SELECT);
  delay(3000);
  lcd.clear();
  }

void loop() { 
  lcd_key = read_LCD_buttons();  // lire la valeur du bouton (->fonction)
  // reception de messages OSC
  if(server.aviableCheck()>0) {
      } 
  for ( byte vp = 0 ; vp < 3 ; vp++ )
  while ( vpClient[vp].available() )
      {
      char c = vpClient[vp].read();
      lcd.print(c);
      }
  for (int i=0; i<=5; i++)
  {
    lcd.setCursor(0,0);      // placer le curseur à la première ligne, première position
    lcd.print(bt_txt[lcd_key]);  // txt. boutons
    lcd.write(lcd_key); // no. symboles spéciaux LCD
    lcd.print(" "); 
    lcd.print(adc_key_in); // valeur A/D résistance bouton
    lcd.print("   "); 
    if (lcd_key > 0) old_lcd_key = lcd_key;
  } //end for

  // Dernier bouton utilisé
  if (old_lcd_key > 0){
    lcd.setCursor(0,1);
    lcd.print("Dernier bt = ");
    lcd.write(old_lcd_key); // car. spéciaux lcd
    lcd.print("    ");
  }
  
}
/************* Fonctions ****************/


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
 char c = (vpClient[vp].read());
    if ( value == 1 ) {
        vpClient[vp].print("%1AVMT 31\r");
         lcd.clear ();
        lcd.print ("UN");
        Serial.print(c);
        while ( vpClient[vp].available() )
           {       
             lcd.setCursor(0,0);      // placer le curseur à la première ligne, première position  
                lcd.print(c);
            }
          vpClient[vp].stop();
      }  
    else {
        vpClient[vp].print("%1AVMT 30\r");
        Serial.print(c);
        lcd.clear ();
        lcd.print ("ZERO");
        while ( vpClient[vp].available() )
           {        
              lcd.setCursor(0,0);      // placer le curseur à la première ligne, première position 
                lcd.print(c);
            }
            vpClient[vp].stop();
      }
 
}

//  Lire la valeur A/D retournée selon le bouton choisi
int read_LCD_buttons()
{
    uint8_t buttons = lcd.readButtons();
if (buttons) {
  if (buttons & BUTTON_RIGHT) { return btnRIGHT; }
  if (buttons & BUTTON_UP)  {return btnUP; }
  if  (buttons & BUTTON_DOWN) { return btnDOWN; }
  if (buttons & BUTTON_LEFT)  { return btnLEFT; }
  if (buttons & BUTTON_SELECT) { return btnSELECT;   }
}
 else { return btnNONE; } // si aucun appui sur bouton, on retoune la constante btnNONE ...
}
