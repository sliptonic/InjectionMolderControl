/* Serial 7-Segment Display Example Code

   Circuit:
   Arduino -------------- Serial 7-Segment
     5V   --------------------  VCC
     GND  --------------------  GND
     SDA  --------------------  SDA (A4 on older 'duino's)
     SCL  --------------------  SCL (A5 on older 'duino's)
*/
#include <Wire.h> // Include the Arduino SPI library

// Here we'll define the I2C address of our S7S. By default it
//  should be 0x71. This can be changed, though.
const byte s7sAddress = 0x71;


//Thermistor setup
// which analog pin to connect
#define THERMISTORPIN A0         
// resistance at 25 degrees C
#define THERMISTORNOMINAL 68000      
// temp. for nominal resistance (almost always 25 C)
#define TEMPERATURENOMINAL 31   
// how many samples to take and average, more takes longer
// but is more 'smooth'
#define NUMSAMPLES 5
// The beta coefficient of the thermistor (usually 3000-4000)
#define BCOEFFICIENT 3950
// the value of the 'other' resistor
#define SERIESRESISTOR 10000    
 
int samples[NUMSAMPLES];

//Here's some setup for checking the buttons
#define DEBOUNCE 10  // button debouncer, how many ms to debounce, 5+ ms is usually plenty

// here is where we define the buttons that we'll use. button "1" is the first, button "6" is the 6th, etc
byte buttons[] = {11, 12}; // the analog 0-5 pins are also known as 14-19
// This handy macro lets us determine how big the array up above is, by checking the size
#define NUMBUTTONS sizeof(buttons)
// we will track if a button is just pressed, just released, or 'currently pressed' 
byte pressed[NUMBUTTONS], justpressed[NUMBUTTONS], justreleased[NUMBUTTONS];

//char tempString[5];  // Will be used with sprintf to create strings
char tempString[5];


unsigned int setpoint = 300;
unsigned int actual_temp = 0;


//Some variables for debouncing the buttons
long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 50;    // the debounce time; increase if the output flickers

void setup()
{
  byte i;
  Serial.begin(9600);
  Serial.print("Button checker with ");
  Serial.print(NUMBUTTONS, DEC);
  Serial.println(" buttons");
  
  
  // Make input & enable pull-up resistors on switch pins
  for (i=0; i < NUMBUTTONS; i++)
  {
    pinMode(buttons[i], INPUT);
    //digitalWrite(buttons[i], LOW);
  }


  Wire.begin();  // Initialize hardware I2C pins

  // Clear the display, and then turn on all segments and decimals
  clearDisplayI2C();  // Clears display, resets cursor

  // Greeting
  s7sSendStringI2C("-HI-");

  setBrightnessI2C(255);  // High brightness
  delay(1500);
  setDecimalsI2C(0b000001);  // Turn on first decimal to separate S/P from numbers.

  // Clear the display before jumping into loop
  sprintf(tempString, "p%03d", setpoint);
  s7sSendStringI2C(tempString);

}

void loop()
{
  check_switches(); //see if the button state has changed
  
  
  if (justpressed[0]) {
    setpoint += 5;     
    sprintf(tempString, "p%03d", setpoint);
    s7sSendStringI2C(tempString);
    delay(1500);
  } 
  
  if (justpressed[1]) {
    setpoint -= 5;    
    sprintf(tempString, "p%03d", setpoint);
    s7sSendStringI2C(tempString);
    delay(1500);
  } 

  actual_temp = gettemp();
  sprintf(tempString, "A%03d", actual_temp);
  s7sSendStringI2C(tempString);


  delay(100);  // This will make the display update at 10Hz.*/
}

// This custom function works somewhat like a serial.print.
//  You can send it an array of chars (string) and it'll print
//  the first 4 characters in the array.
void s7sSendStringI2C(String toSend)
{
  Wire.beginTransmission(s7sAddress);
  for (int i=0; i<4; i++)
  {
    Wire.write(toSend[i]);
  }
  Wire.endTransmission();
}

// Send the clear display command (0x76)
//  This will clear the display and reset the cursor
void clearDisplayI2C()
{
  Wire.beginTransmission(s7sAddress);
  Wire.write(0x76);  // Clear display command
  Wire.endTransmission();
}

// Set the displays brightness. Should receive byte with the value
//  to set the brightness to
//  dimmest------------->brightest
//     0--------127--------255
void setBrightnessI2C(byte value)
{
  Wire.beginTransmission(s7sAddress);
  Wire.write(0x7A);  // Set brightness command byte
  Wire.write(value);  // brightness data byte
  Wire.endTransmission();
}

// Turn on any, none, or all of the decimals.
//  The six lowest bits in the decimals parameter sets a decimal 
//  (or colon, or apostrophe) on or off. A 1 indicates on, 0 off.
//  [MSB] (X)(X)(Apos)(Colon)(Digit 4)(Digit 3)(Digit2)(Digit1)
void setDecimalsI2C(byte decimals)
{
  Wire.beginTransmission(s7sAddress);
  Wire.write(0x77);
  Wire.write(decimals);
  Wire.endTransmission();
}


void check_switches()
//  Monitor the buttons and note changes in state so we can respond.

{
  static byte previousstate[NUMBUTTONS];
  static byte currentstate[NUMBUTTONS];
  static long lasttime;
  byte index;

  if (millis() < lasttime){ // we wrapped around, lets just try again
     lasttime = millis();
  }
  
  if ((lasttime + DEBOUNCE) > millis()) {
    // not enough time has passed to debounce
    return; 
  }

  // ok we have waited DEBOUNCE milliseconds, lets reset the timer
  lasttime = millis();
  
  for (index = 0; index < NUMBUTTONS; index++){
    currentstate[index] = digitalRead(buttons[index]);   // read the button
    
    if (currentstate[index] == previousstate[index]) {  //no change
      justpressed[index] = LOW;
      justreleased[index] = LOW;
    }
    
    if ((pressed[index] == LOW) && (currentstate[index] == HIGH)) {
          // pressed
          justpressed[index] = HIGH;
      }
    else if ((pressed[index] == HIGH) && (currentstate[index] == LOW)) {
          justreleased[index] = HIGH;
      }
  
    //set the pressed state and set previousstate so we see a change
    pressed[index] = currentstate[index];  
    previousstate[index] = currentstate[index];   
  }

}

int gettemp() {
  uint8_t i;
  float average;
 
  // take N samples in a row, with a slight delay
  for (i=0; i< NUMSAMPLES; i++) {
   samples[i] = analogRead(THERMISTORPIN);
   delay(10);
  }
 
  // average all the samples out
  average = 0;
  for (i=0; i< NUMSAMPLES; i++) {
     average += samples[i];
  }
  average /= NUMSAMPLES;
 
    // convert the value to resistance
  average = 1023 / average - 1;
  average = SERIESRESISTOR / average;
  
  float steinhart;
  steinhart = average / THERMISTORNOMINAL;     // (R/Ro)
  steinhart = log(steinhart);                  // ln(R/Ro)
  steinhart /= BCOEFFICIENT;                   // 1/B * ln(R/Ro)
  steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15); // + (1/To)
  steinhart = 1.0 / steinhart;                 // Invert
  steinhart -= 273.15;                         // convert to C
 
  return steinhart;
}
