//#define MPX5100_PIN A0
//float adcVal = 0;

int x = 0;        // Membaca nilai ADC
int batas = 20;     
float vout = 0;   // Membaca nilai dalam satuan Mv
float kpa = 0;    // Nilai Kpa
float mmhg = 0;   // Nilai mmhg
float newVal = 0; // variable konvert menggunakan map()

float darah1, selisih, darah2, sistole, diastole; 

void setup() {
  //pinMode(MPX5100_PIN, INPUT);
  Serial.begin(9600);
  Serial.println("llll");
}

void loop() {
  //Pressure Calibration
  x = analogRead(34);
  vout = x*(3.3/4095.0); // Mencari tegangan yang terukur dari nilai adc
  kpa = ((vout/3.0)-0.04) / 0.009; // Konvert ke kPa
  mmhg = (float)(kpa * 7.50062) + 18.0 ;  // Konvert ke mmhg
  //newVal = map(mmhg, -0.21, 293.29, 0, 300); // Konvert ke batas maksimal sphygmomanometer
  //newVal = constrain(newVal, 0, 300);
  newVal = mmhg;

  Serial.println(newVal);
  
  if(newVal >= 105 && newVal <= 135){
  //  Serial.println(newVal);
    darah1 = newVal;
    selisih = darah2 - darah1;
    darah2 = darah1;

    if(selisih = 0.50){
      sistole = darah1;
      diastole = darah1 - 40;

      Serial.print("Sistol:");
      Serial.println(sistole);
      Serial.print("Diastol:");
      Serial.println(diastole);
      Serial.println("-------------------------");

      delay(100);
    }

   delay(50);
  }
  
}
