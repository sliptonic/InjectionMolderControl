const int buttonPin = 11;     // the number of the pushbutton pin
const int buttonPin2 = 12;

// variables will change:
int buttonState = 0;         // variable for reading the pushbutton status
int buttonState2 = 0;

void setup() {

  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);
  pinMode(buttonPin2, INPUT);  
  Serial.begin(9600);      // open the serial port at 9600 bps:    
}

void loop(){
  
//  // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);
  buttonState2 = digitalRead(buttonPin2);
  
  Serial.print("button state:");
  Serial.print(buttonState);
  Serial.print(", ");
  Serial.print(buttonState2);
  Serial.println("");


}
