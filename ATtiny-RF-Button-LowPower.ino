/*  
 *   For sending 433MHz RF signals from battery operated ATtiny85
 *   Code by Thomas Friberg (https://github.com/tomtheswede)
 *   Updated 17/04/2017
 */

//Device parameters
const unsigned long devID = 183339503; // 00001010111011011000100111101111 So the message can be picked up by the right receiver
const unsigned long devType = 1; //Reads as "1" corresponding with BTN type

//General variables
const byte sendPin = 7; //RF pin. pin 3 for rfbutton v0.4
const byte typePreamble[4] = {120,121,122,123}; //01111000 for register, 01111001 for basic message type
long lastTrigger=millis();
bool longPressPrimer=true;
bool longerPressPrimer=true;
bool longestPressPrimer=true;
byte msgType=1;
byte msgLengths[4]={32,8,16,32};
byte msgBuffer[9]; //Max 9 bytes for transmission
int msgLength;
int k;

void setup() {
  // put your setup code here, to run once:
  pinMode(sendPin,OUTPUT);

  //pulse(1); //For calibration pre-read of reciever
  //pulse(0); //For calibration pre-read of reciever
  //delay(2);
  //for (int rep=0; rep<5; rep++) {
  //  for (int i=31; i>=0; i--) {
  //    pulse(bitRead(devID,i));
  //  }
  //  pulse(0); //to end the message timing
  //  delay(2);
  //}
  //delay(400);
  encodeMessage(0,devID); //For registration study
  //encodeMessage(1,0); //regular button push message
}

void loop() {
  if (longPressPrimer && millis()-lastTrigger>1100) {
    encodeMessage(1,1);
    longPressPrimer=false;
  }
  else if (longerPressPrimer && millis()-lastTrigger>2500) {
    encodeMessage(2,532);
    longerPressPrimer=false;
  }
  else if (longestPressPrimer && millis()-lastTrigger>5000) {
    encodeMessage(0,devID);
    longestPressPrimer=false;
  }
}

void pulse(bool logic) {
  if (logic) {
    digitalWrite(sendPin,HIGH);
    delayMicroseconds(720);  //797us realtime
    digitalWrite(sendPin,LOW);
    delayMicroseconds(60);   //416us realtime
  }
  else {
    digitalWrite(sendPin,HIGH);
    delayMicroseconds(320);  //416us realtime
    digitalWrite(sendPin,LOW);
    delayMicroseconds(470);  //797us realtime
  }
}

void encodeMessage(byte msgType,unsigned long msg) {
  msgLength=msgLengths[msgType];
  k=0;
  //construct the message
  for (int i=0; i<8; i++) {  //6 bit preamble with 2 bit msg type - 8 bit total
    bitWrite(msgBuffer[k/8],7-(k%8),bitRead(typePreamble[msgType],7-i));
    k++;
  }
  for (int i=0; i<32; i++) {  //32 bit device ID
    bitWrite(msgBuffer[k/8],7-(k%8),bitRead(devID,31-i));
    k++;
  }
  for (int i=0; i<msgLength; i++) {  //72,48,56,72 bit message length
    bitWrite(msgBuffer[k/8],7-(k%8),bitRead(msg,msgLength-1-i));
    k++;
  }
  
  //Send the message
  pulse(1); //For calibration pre-read of reciever
  pulse(0); //For calibration pre-read of reciever
  delay(2);
  for (int rep=0; rep<5; rep++) {
    for (int i=0; i<msgLength+40; i++) {
      pulse(bitRead(msgBuffer[i/8],7-(i%8)));
    }
    pulse(0); //to end the message timing
    delay(2);
  }
  delay(3);
}
