/*  
 *   For sending RF signals from battery operated ATtiny85
 *   Code by Thomas Friberg (https://github.com/tomtheswede)
 *   Updated 23/10/2016
 */

//Device parameters
const unsigned int channel = 1; //So the message can be picked up by the right receiver
const unsigned int devType = 1; //Reads as "1" corresponding with BTN type
const unsigned int sensorName1 = 5; //The number of this device - needs to be unique.

//Interrupt variables
volatile bool triggered=false;

//General variables
const unsigned int sendPin = 2; //RF pin
long lastTrigger;
bool sendByte[4];
bool longPressPrimer=false;
bool longerPressPrimer=false;
bool longestPressPrimer=false;
int highBits=0;

void setup() {
  // put your setup code here, to run once:
  pinMode(sendPin,OUTPUT);
  encodeMessage(1);
  encodeMessage(1);
  encodeMessage(1);
  triggered=false;
  longPressPrimer=true;
  longerPressPrimer=true;
  longestPressPrimer=true;
  lastTrigger=millis();
}

void loop() {
  if (longPressPrimer && millis()-lastTrigger>700) {
    encodeMessage(2);
    encodeMessage(2);
    encodeMessage(2);
    longPressPrimer=false;
  }
  else if (longerPressPrimer && millis()-lastTrigger>1900) {
    encodeMessage(3);
    encodeMessage(3);
    encodeMessage(3);
    longerPressPrimer=false;
  }
  else if (longestPressPrimer && millis()-lastTrigger>4000) {
    encodeMessage(4);
    encodeMessage(4);
    encodeMessage(4);
    longestPressPrimer=false;
  }
}

void encodeMessage(int msgType) { //message should read 15 1 6 14 or 1111 0001 0110 1110
  //regular pulses for receiver to calibrate
  for (int i=0; i <= 6; i++){
    pulse(0);
  }
  delay(1);
  
  encodeNumber(15); //Start  1111 to initiate message
  encodeNumber(channel); //number 4
  encodeNumber(11); //comma
  encodeNumber(devType); //number 9
  encodeNumber(11); //comma
  encodeNumber(sensorName1); //number 9
  encodeNumber(11); //comma
  encodeNumber(msgType); //number 9
  if (highBits%2) { //Equals 1/true if odd number of high bits
    encodeNumber(13);//End  1101 - WIP on parity byte
  }
  else { //No partiy bit required
    encodeNumber(12);//End  1100 - WIP on parity byte
  }
  delay(1);
}

void pulse(bool logic) {
  if (logic) {
    digitalWrite(sendPin,HIGH);
    delayMicroseconds(575); //665us realtime
    digitalWrite(sendPin,LOW);
    delayMicroseconds(250); //360us realtime
  }
  else {
    digitalWrite(sendPin,HIGH);
    delayMicroseconds(250);
    digitalWrite(sendPin,LOW);
    delayMicroseconds(600);
  }
}

void encodeNumber(int num) {
  
  for (int i=0; i<4; i++) {
    if(bitRead(num,3-i)) { //a one
      pulse(1);
      highBits=highBits+1;
    }
    else{ //a zero
      pulse(0);
    }
  }
}
