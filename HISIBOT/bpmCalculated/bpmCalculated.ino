//  Variables
//int sensorPin = 0;         // Pulse Sensor PURPLE WIRE connected to ANALOG PIN 0
//int LED13 = 2;            //  The on-board Arduion LED

int Signal;                // holds the incoming raw data. Signal value can range from 0-1024
int Threshold = 550;       // Determine which Signal to "count as a beat", and which to ingore.

int sensorPin = 36;                                // A0 is the input pin for the heart rate sensor
float sensorValue = 0;                             // Variable to store the value coming from the sensor
int count = 9;
unsigned long starttime = 0;
int heartrate = 0;
boolean counted = false;


// The SetUp Function:
void setup() {
  pinMode(2,OUTPUT);   // pin that will blink to your heartbeat!
  Serial.begin(9600);     // Set's up Serial Communication at certain speed.
}

// The Main Loop Function
void loop() {
//  Signal = analogRead(sensorPin);  
//  Serial.println(Signal);                 // Send the Signal value to Serial Plotter.
//  if(Signal > Threshold){                 // If the signal is above "550", then "turn-on" Arduino's on-Board LED.
//     digitalWrite(LED13,HIGH);
//  } else {
//     digitalWrite(LED13,LOW);             //  Else, the sigal must be below "550", so "turn-off" this LED.
//  }
//  delay(10);
  starttime = millis();
  while(millis() < starttime+5000){
      sensorValue = analogRead(sensorPin);
      //Serial.println(sensorValue);
      if (sensorValue > 3500 && counted == false)  // Threshold value is 550 (~ 2.7V)
      {      
        count++;      
        Serial.print ("count = ");      
        Serial.println(count);      
        digitalWrite(2,HIGH);      
        delay(50);      
        digitalWrite(2,LOW);
        counted = true;
      } else if (sensorValue < 2000){    
        counted = false;
        digitalWrite(2, LOW);
      }
  }
  heartrate = count*12;          // Multiply the count by 6 to get beats per minute  
  Serial.println();  
  Serial.print("BPM = ");  
  Serial.println(heartrate);    // Display BPM in the Serial Monitor
  Serial.println();
  count = 0;
}
