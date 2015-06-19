//combining display sample with thermister sample


//display segment
/*
 9-23-2012
 Spark Fun Electronics
 Nathan Seidle
 
 This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).

 Serial7Segment is an open source seven segment display. 

 This is example code that shows how to display basic numbers on the display.
 
 Note: This code expects the display to be listening at 9600bps. If your display is not at 9600bps, you can
 do a software or hardware reset. See the Wiki for more info: 
 http://github.com/sparkfun/Serial7SegmentDisplay/wiki/Special-Commands#wiki-baud
 
 To get this code to work, attached an Serial7Segment to an Arduino Uno using the following pins:
 Pin 8 on Uno (software serial TX) to RX on Serial7Segment
 VIN to PWR
 GND to GND
 
*/

#include <SoftwareSerial.h>

SoftwareSerial Serial7Segment(7, 8); //RX pin, TX pin

//thermister section
// which analog pin to connect
#define THERMISTORPIN A0         
// resistance at 25 degrees C
#define THERMISTORNOMINAL 10000      
// temp. for nominal resistance (almost always 25 C)
#define TEMPERATURENOMINAL 25   
// how many samples to take and average, more takes longer
// but is more 'smooth'
#define NUMSAMPLES 5
// The beta coefficient of the thermistor (usually 3000-4000)
#define BCOEFFICIENT 3950
// the value of the 'other' resistor
#define SERIESRESISTOR 10000    
 
int samples[NUMSAMPLES];
 
void setup(void) {
  Serial.begin(9600);
  //analogReference(EXTERNAL);
   Serial.println("OpenSegment Example Code");

  Serial7Segment.begin(9600); //Talk to the Serial7Segment at 9600 bps
  Serial7Segment.write('v'); //Reset the display - this forces the cursor to return to the beginning of the display
}
 
void loop(void) {
  uint8_t i;
  float average;
  float f;
  
  // take N samples in a row, with a slight delay
  for (i=0; i< NUMSAMPLES; i++) {
   samples[i] = analogRead(THERMISTORPIN);
   //Serial.print("read=");
  // Serial.println(i);
  // Serial.println(samples[i]);
   delay(100);
  }
 
  // average all the samples out
  average = 0;
  f=0;
  for (i=0; i< NUMSAMPLES; i++) {
     average += samples[i];
  }
  average /= NUMSAMPLES;
 
  Serial.print("Average analog reading "); 
  Serial.println(average);
 
  // convert the value to resistance
  average = 1023 / average - 1;
  average = SERIESRESISTOR / average;
  Serial.print("Thermistor resistance "); 
  Serial.println(average);
 
  float steinhart;
  steinhart = average / THERMISTORNOMINAL;     // (R/Ro)
  steinhart = log(steinhart);                  // ln(R/Ro)
  steinhart /= BCOEFFICIENT;                   // 1/B * ln(R/Ro)
  steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15); // + (1/To)
  steinhart = 1.0 / steinhart;                 // Invert
  steinhart -= 273.15;                         // convert to C
 
 f=(steinhart*9/5)+ 32;
 
  Serial.print("Temperature "); 
  Serial.print(steinhart);
  Serial.print(" *C   ");
  Serial.print(f);
  Serial.println(" *F ");
  
   char tempString[10]; //Used for sprintf
 sprintf(tempString, "%f \n", (float )13.1); //Convert deciSecond into a string that is right adjusted
 Serial.println(tempString); //Convert deciSecond into a string that is right adjusted
 //sprintf(tempString, "%4d", (int)f); //Convert deciSecond into a string that is right adjusted
  //sprintf(tempString, "%d", cycles); //Convert deciSecond into a string that is left adjusted (requires digit 1 command)
  //sprintf(tempString, "%04d", cycles); //Convert deciSecond into a string with leading zeros
  //sprintf(tempString, "%4X", cycles); //Count in HEX, right adjusted
  //int negativeCycles = cycles * -1;
  //sprintf(tempString, "%4d", negativeCycles); //Shows a negative sign infront of right adjusted number

  //Note: This method works well as long as your number is less than or equal to 4 digits.
  //14422 will cause the display to wrap (5 digits)
  //-5766 will cause the display to wrap (5 digits)
  //To fix this, send a 'v' character or look at how to control the digit placement
  //https://github.com/sparkfun/Serial7SegmentDisplay/wiki/Basic-Usage#wiki-cursor

  Serial7Segment.print(tempString); //Send serial string out the soft serial port to the S7S

  
  delay(1000);
}
