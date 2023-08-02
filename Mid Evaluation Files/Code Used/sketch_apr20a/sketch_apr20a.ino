// Potentiometer is connected to GPIO 34 (Analog ADC1_CH6) 
#include <LiquidCrystal_I2C.h>
const int potPin = 34;

// set the LCD number of columns and rows
int lcdColumns = 16;
int lcdRows = 2;
int potValue = 0;

const int sampleWindow = 100;
unsigned int sample;
int db;

// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);  

void setup(){

  pinMode (potPin, INPUT);
  
  // initialize LCD
  lcd.init();
  // turn on LCD backlight                      
  lcd.backlight();
}

void loop(){

  unsigned long startMillis = millis(); // Start of sample window
  
  float peakToPeak = 0; //peak-to-peak level
  
  unsigned int signalMax = 0; //minimum value
  unsigned int signalMin = 1024; //maximum value
  
  // collect data for 50 mS
  while (millis() - startMillis < sampleWindow)
  {

    sample = analogRead(potPin); //get reading from microphone
    
    if (sample < 1024) // toss out spurious readings
    {
      if (sample > signalMax)
      {
        signalMax = sample; // save just the max levels
      }
      else if (sample < signalMin)
      {
        signalMin = sample; // save just the min levels
      }
    }
  }
  
  peakToPeak = signalMax - signalMin; // max - min = peak-peak amplitude
  
  Serial.println(peakToPeak);
  
  db = map(peakToPeak, 20, 300, 30, 75); //calibrate for deciBels
  
  lcd.setCursor(0, 0);
  lcd.print("Loudness: ");
  lcd.print(db);
  lcd.print("dB");
  
  if (db <= 39)
  {
    lcd.setCursor(0, 1);
    lcd.print("Level: Quite");
  }
  else if (db > 39 && db < 55)
  {
    lcd.setCursor(0, 1);
    lcd.print("Level: Moderate");
  }
  else if (db >= 55)
  {
    lcd.setCursor(0, 1);
    lcd.print("Level: High");
  }
  delay(500);
  lcd.clear();
}
