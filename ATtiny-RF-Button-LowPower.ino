/*  
 *   For sending 433MHz RF signals from battery operated ATtiny85
 *   Code by Thomas Friberg (https://github.com/tomtheswede)
 *   Updated 18/12/2016
 */

//Device parameters
const unsigned long devID = 183339503; // 00001010111011011000100111101111 So the message can be picked up by the right receiver
const unsigned long devType = 1; //Reads as "1" corresponding with BTN type

//General variables
const unsigned int sendPin = 2; //RF pin
const boolean preamble[] = {0,0,0,0,1,1,1,1,1,1,0,0}; //252
const boolean regPreamble[] = {0,0,0,0,1,1,1,1,1,1,0,1}; //253
long lastTrigger;
bool longPressPrimer=false;
bool longerPressPrimer=false;
bool longestPressPrimer=false;

void setup() {
  // put your setup code here, to run once:
  pinMode(sendPin,OUTPUT);
  longPressPrimer=true;
  longerPressPrimer=true;
  longestPressPrimer=true;
  lastTrigger=millis();
  encodeMessage(1); //regular button push message
}

void loop() {
  if (longPressPrimer && millis()-lastTrigger>700) {
    encodeMessage(2);
    longPressPrimer=false;
  }
  else if (longerPressPrimer && millis()-lastTrigger>1900) {
    encodeMessage(3);
    longerPressPrimer=false;
  }
  else if (longestPressPrimer && millis()-lastTrigger>4000) {
    sendRegisterMessage();
    longestPressPrimer=false;
  }
}

void pulse(bool logic) {
  if (logic) {
    digitalWrite(sendPin,HIGH);
    delayMicroseconds(550); //665us realtime
    digitalWrite(sendPin,LOW);
    delayMicroseconds(200); //360us realtime
  }
  else {
    digitalWrite(sendPin,HIGH);
    delayMicroseconds(200);
    digitalWrite(sendPin,LOW);
    delayMicroseconds(550);
  }
}

void sendRegisterMessage() {
  for (int rep=0; rep<5; rep++) {
    //preamble 8 bits with register message
    for (int i=0; i<12; i++) {
      pulse(regPreamble[i]);
    }
    //device ID
    for (int i=0; i<32; i++) {
      if(bitRead(devID,32-1-i)) { //a one
         pulse(1);   //a one
      }
      else{ //a zero
        pulse(0);  //a zero
      }
    }
    //Message - what device type it is
    for (int i=0; i<32; i++) {
      if(bitRead(devType,32-1-i)) {
        pulse(1);   //a one
      }
      else{
        pulse(0);  //a zero
      }
    }
    pulse(0); //to end the message timing
    delay(2);
  }
  delay(3);
}

void encodeMessage(unsigned long msg) {
  for (int rep=0; rep<5; rep++) {
    //preamble 8 bits without register message
    for (int i=0; i<12; i++) {
      pulse(preamble[i]);
    }
    //device ID
    for (int i=0; i<32; i++) {
      if(bitRead(devID,32-1-i)) { //a one
         pulse(1);   //a one
      }
      else{ //a zero
        pulse(0);  //a zero
      }
    }
    //Message - what device type it is
    for (int i=0; i<32; i++) {
      if(bitRead(msg,32-1-i)) {
        pulse(1);   //a one
      }
      else{
        pulse(0);  //a zero
      }
    }
    pulse(0); //to end the message timing
    delay(2);
  }
  delay(3);
}
