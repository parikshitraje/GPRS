#include "Adafruit_FONA.h"
#include<EEPROM.h>

#define FONA_RX 7
#define FONA_TX 8
#define FONA_RST 9

// this is a large buffer for replies
char replybuffer[255];


// This is for Ashioto Mat Varibale


String str;
int mats[] = {2, 3, 4, 5, 6};
int matscount = 5;
int reading[5] = {LOW, LOW, LOW, LOW, LOW}, buttonState[5], lastButtonState[5] = {LOW, LOW, LOW, LOW, LOW};

long a = 0;
long b = 0;
int address = 0;
byte byteRead;
long count = 0;
long prevCount = 0;
long pCount = 0 , c[5] = {0, 0, 0, 0, 0};
long lastDebounceTime[5] = {0, 0, 0, 0, 0};
long debounceDelay = 50;

long t1 = 0; // hold current count
long t2 = 0; // hold last time count updated
//long t5 = 0; // EEPROM update timer
int E1 = 0;
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

  Serial.begin(9600);
  Serial.println(F("FONA basic test"));Serial.println("Waiting for 10 Sec");
  delay(10000);
  Serial.println("Ready");
  Serial.println(F("Initializing....(May take 3 seconds)"));

  fonaSerial->begin(4800);
  if (! fona.begin(*fonaSerial)) {
    Serial.println(F("Couldn't find FONA"));
    while (1);
    /// Set Mat pins as a Input
    delay(100);
    for (int i = 0; i < matscount; i++) {
      pinMode(mats[i], INPUT);
    }

  }
  count = EEPROMRead(0);
  pCount = count;
  Serial.println("Waiting for 5 Sec");
  delay(5000);
 // ashiotogprson();
  delay(2000);


}






void loop() {

 
  
    for (int i = 0; i < matscount; i++) {
      reading[i] = digitalRead(mats[i]);
    }

    for (int i = 0; i < matscount ; i++) {

      if (reading[i] != lastButtonState[i]) {
        lastDebounceTime[i] = millis();
      }

      if ((millis() - lastDebounceTime[i]) > debounceDelay) {
        if (reading[i] != buttonState[i]) {
          buttonState[i] = reading[i];

          if (buttonState[i] == HIGH) {
            c[i]++;
            //Serial.print(i);
            //Serial.print("--");
            count = count + 1;
            //Serial.println(count);
          }
        }
      }
    }
    t1 = millis() / 1000;
    long t3 = t1 - t2;
    long t4 = 180 - t3 ;
    str = "#" + String(count) + "$" + "%" + String(t4) + "^";

    //str = String(count);
    if (prevCount != count)
    {
      //for (int i = 0; i < matscount; i++) {
      //Serial.print(c[i]);
      //Serial.print(" ");
      //}
      Serial.println(str);
    }
if(t3 >= 60 && a == 0){
  
   ashiotogprson();
   Serial.println("ON");
   a = 1;
   b = 0;
  
 }
if(t4 == 60 && a == 1){
  
   ashiotogprsoff();
   Serial.println("OFF");
   a = 0;
   t2 = t1;
  }

    if (t4 == 90 && b == 0) {

      //if (count - pCount >= 20)
      // {

      ashiotopost(count);
      //ashiotogprsoff();
      pCount = count;

      // }
      // Serial.print(" -- ");
      // Serial.print(t3);
      //Serial.print(" -- ");
      //Serial.print(t4);
      Serial.println(str);
      b=1;
     // a = 1;
      //t2 = t1;

    }

    if (count - pCount >= 100)
    {
      EEPROMWrite(0, count);
      //Serial.print("IN");
      pCount = count;


      //Serial.print("EEPROM");

    }
    prevCount = count;
    for (int i = 0; i < matscount; i++)
    {
      lastButtonState[i] = reading[i];
    }
   // b=0;
    //}
    //}
    // flush input
    flushSerial();
    while (fona.available()) {
      Serial.write(fona.read());
    }
  
}//void loop closing

long EEPROMRead(long add)
{
  long four = EEPROM.read(add);
  long three = EEPROM.read(add + 1);
  long two = EEPROM.read(add + 2);
  long one = EEPROM.read(add + 3);

  return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
}

void ashiotogprson() {
  if (!fona.enableGPRS(true))
    Serial.println(F("Failed to turn on"));
}
void flushSerial() {
  while (Serial.available())
    Serial.read();
}

void ashiotopost(long count) {
  uint16_t statuscode;
  int16_t length;

  char url[80] = "ashioto.xyz/count_update";

  char data[80] ;
  sprintf(data, "{\"count\":%lu ,\"eventCode\":\"deepotsav16\", \"gateID\": 1}", count);



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
void ashiotogprsoff() {
  if (!fona.enableGPRS(false))
    Serial.println(F("Failed to turn off"));
}

void EEPROMWrite(int add, long people)
{
  byte four = (people & 0xFF);
  byte three = ((people >> 8) & 0xFF);
  byte two = ((people >> 16) & 0xFF);
  byte one = ((people >> 24) & 0xFF);
  //Serial.print("reach");
  EEPROM.write(add, four);
  EEPROM.write(add + 1, three);
  EEPROM.write(add + 2, two);
  EEPROM.write(add + 3, one);

}

