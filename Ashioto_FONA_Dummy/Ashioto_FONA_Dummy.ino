#include "Adafruit_FONA.h"

#define FONA_RX 7
#define FONA_TX 8
#define FONA_RST 4

// this is a large buffer for replies
char replybuffer[255];
long count1 = 0;
long count2 = 0;
// We default to using software serial. If you want to use hardware serial
// (because softserial isnt supported) comment out the following three lines 
// and uncomment the HardwareSerial line
#include <SoftwareSerial.h>
SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);
SoftwareSerial *fonaSerial = &fonaSS;

// Hardware serial is also possible!
//  HardwareSerial *fonaSerial = &Serial1;

// Use this for FONA 800 and 808s
Adafruit_FONA fona = Adafruit_FONA(FONA_RST);
// Use this one for FONA 3G
//Adafruit_FONA_3G fona = Adafruit_FONA_3G(FONA_RST);

uint8_t readline(char *buff, uint8_t maxbuff, uint16_t timeout = 0);

uint8_t type;

void setup() {
  while (!Serial);

  Serial.begin(115200);
 // Serial.println(F("FONA basic test"));
  //Serial.println(F("Initializing....(May take 3 seconds)"));

  fonaSerial->begin(4800);
  if (! fona.begin(*fonaSerial)) {
    Serial.println(F("Couldn't find FONA"));
    while (1);
  }
  type = fona.type();
  Serial.println(F("FONA is OK"));
  Serial.print(F("Found "));
  switch (type) {
    case FONA800L:
      Serial.println(F("FONA 800L")); break;
    default: 
      Serial.println(F("???")); break;
  }
  
  // Print module IMEI number.
  char imei[15] = {0}; // MUST use a 16 character buffer for IMEI!
  uint8_t imeiLen = fona.getIMEI(imei);
  if (imeiLen > 0) {
    Serial.print("Module IMEI: "); Serial.println(imei);
  }

  //printMenu();
  
  
}
   
    
    
    
    
void loop(){
    int a = random(30,70);
     Serial.println(a);
    int b = random(20,50);
    Serial.println(b);

count1 = count1 + a;
count2 = count2 + b; 
   
        
        ashiotogprson();
        
        ashiotopost1(count1);
        //ashiotogprsoff();
        Serial.print("GATE 1 SEND");  
        delay(60000);
        //ashiotogprson();
        ashiotopost2(count2);
        ashiotogprsoff();
        Serial.print("GATE 2 SEND");
        delay(60000);
 
}

void flushSerial() {
  while (Serial.available())
    Serial.read();
}

void ashiotopost1(long c){
   uint16_t statuscode;
        int16_t length;
        char url[80] = "ashioto.xyz/count_update";
        
       
        char data[80] ;
        sprintf(data , "{\"count\":%lu, \"eventCode\":\"test_event\", \"gateID\": 1}",c);

        flushSerial();
        Serial.print(F("http://"));// readline(url, 79);
        Serial.println(url);
        Serial.println(data);

        Serial.println(F("****"));
        if (!fona.HTTP_POST_start(url, F("text/plain"), (uint8_t *) data, strlen(data), &statuscode, (uint16_t *)&length)) {
          Serial.println("Failed!");
          return;
        }
        while (length > 0) {
          while (fona.available()) {
            char c = fona.read();

#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)
            loop_until_bit_is_set(UCSR0A, UDRE0); /* Wait until data register empty. */
            UDR0 = c;
#else
            Serial.write(c);
#endif

            length--;
            if (! length) return;
          }
        }
        Serial.println(F("\n****"));
        fona.HTTP_POST_end();
       
  }
  void ashiotopost2(long d){
   uint16_t statuscode;
        int16_t length;
        char url[80] = "ashioto.xyz/count_update";
        
       
        char data[80] ;
        sprintf(data , "{\"count\":%lu, \"eventCode\":\"test_event\", \"gateID\": 2}",d);

        flushSerial();
        Serial.print(F("http://"));// readline(url, 79);
        Serial.println(url);
        Serial.println(data);

        Serial.println(F("****"));
        if (!fona.HTTP_POST_start(url, F("text/plain"), (uint8_t *) data, strlen(data), &statuscode, (uint16_t *)&length)) {
          Serial.println("Failed!");
          return;
        }
        while (length > 0) {
          while (fona.available()) {
            char c = fona.read();

#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)
            loop_until_bit_is_set(UCSR0A, UDRE0); /* Wait until data register empty. */
            UDR0 = c;
#else
            Serial.write(c);
#endif

            length--;
            if (! length) return;
          }
        }
        Serial.println(F("\n****"));
        fona.HTTP_POST_end();
       
  }

 void ashiotogprsoff(){
  if (!fona.enableGPRS(false))
          Serial.println(F("Failed to turn off"));
  }

void ashiotogprson(){
if (!fona.enableGPRS(true))
          Serial.println(F("Failed to turn on"));
 }

