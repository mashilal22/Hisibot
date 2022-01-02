//Include Library 
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Arduino.h>

#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif

#include <Firebase_ESP_Client.h>
//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// OLED Define Address
#define OLED_WIDTH 128
#define OLED_HEIGHT 64
#define OLED_ADDR   0x3C

// Insert your network credentials
#define WIFI_SSID "Keputih 56B"
#define WIFI_PASSWORD "keputih5603"

// Insert Firebase project API Key
#define API_KEY "AIzaSyDxnj6owMtU3uk0PvaVWjzqSKilZU0jO3Q"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://tensimeterhisibot-be77e-default-rtdb.asia-southeast1.firebasedatabase.app/" 

Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT);

//Define Firebase Data object
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
int intValue;
float floatValue;
bool signupOK = false;

//ESP32 Pin
const int airPressure = 34;
const int sensorPin = 36;

//  Variable pulse sensor 
unsigned long starttime = 0;         
float sensorValue = 0;        
int countTime = 9;
int heartrate = 0;
boolean counted = false;

//  Variable air pressure sensor
int x = 0;        // Membaca nilai ADC     
float vout = 0;   // Membaca nilai dalam satuan Mv
float kpa = 0;    // Nilai Kpa
float mmhg = 0;   // Nilai mmhg
float newVal = 0; // variable konvert menggunakan map()
float darah1, darah2, selisih, sistole, diastole;
   
//PulseSensorPlayground pulseSensor;

void setup() {   
  Serial.begin(9600);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

   /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  //Setup Oled
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  display.clearDisplay();

  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(25, 0);
  display.println("HISIBOT");
  display.setCursor(8, 20);
  display.println("Begin...");
  display.display();
  delay(3000);

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(10, 0);
  display.println("Wait for");
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(20, 20);
  display.println("Data...");
  display.display();

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  delay(2000);
}

void loop() {
   x = analogRead(airPressure);
   vout = x*(3.3/4095.0); // Mencari tegangan yang terukur dari nilai adc
   kpa = ((vout/3.3)-0.04) / 0.009; // Konvert ke kPa
   mmhg = (float)(kpa * 7.50062) + 18.0;  // Konvert ke mmhg 
   newVal = mmhg;

   starttime = millis();
   while(millis() < starttime+5000){
      sensorValue = analogRead(sensorPin);
      //Serial.println(sensorValue);
      if (sensorValue   > 3500 && counted == false)  // Threshold value is 550 (~ 2.7V)
      {      
        countTime++;      
        Serial.print ("count = ");      
        Serial.println(countTime);      
        digitalWrite(2,HIGH);      
        delay(50);      
        digitalWrite(2,LOW);
        counted = true;
      } else if (sensorValue < 2000){    
        counted = false;
        digitalWrite(2, LOW);
      }
   }
   
   heartrate = countTime*12;           
   countTime = 0;
   
   if(newVal >= 105 && newVal <= 140){
      darah1 = newVal;
      selisih = darah2 - darah1;
      darah2 = darah1;
  
      if(selisih = 0.50){
        sistole = darah1;
        diastole = darah1 - 40;
        
        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(WHITE);
        display.setCursor(7, 0);
        display.println("Tensimeter");
        display.setTextSize(2);
        display.setTextColor(WHITE);
        display.setCursor(0, 20);
        display.print("S:");
        display.println(sistole, 0);
        display.setTextSize(2);
        display.setTextColor(WHITE);
        display.setCursor(80, 20);
        display.print("D:");
        display.println(diastole, 0);
        display.setTextSize(2);
        display.setTextColor(WHITE);
        display.setCursor(35, 45);
        display.print("BPM:");
        display.println(heartrate);
        display.display();
        delay(100);
      }
   }
   
   delay(50); // considered best practice in a simple sketch.

   //Send data to firebase
   if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();
    // Write an Int number on the database path test/int
    if (Firebase.RTDB.setInt(&fbdo, "hisibot/sistole", sistole)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    
    if (Firebase.RTDB.setInt(&fbdo, "hisibot/diastole", diastole)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    // Write an Float number on the database path test/float
    if (Firebase.RTDB.setFloat(&fbdo, "hisibot/bpm", heartrate)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
  }
}
