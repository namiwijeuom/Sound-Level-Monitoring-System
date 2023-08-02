/* Fill-in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID "TMPL6R8cIYZ6A"
#define BLYNK_TEMPLATE_NAME "Quickstart Template"
#define BLYNK_AUTH_TOKEN "DKvORhrPqRP_jFrWe91_ADIEHiUbJ54X"

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

// Potentiometer is connected to GPIO 34 (Analog ADC1_CH6) 
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <LiquidCrystal_I2C.h>

//Wifi connection
const char* ssid = "Namina_Wijetunga";
const char* password = "Iamthebest1ever@";
int WiFiStatus;
String Get_WiFiStatus(int Status){
    switch(Status){
        case WL_IDLE_STATUS:
        return "WL_IDLE_STATUS";
        case WL_SCAN_COMPLETED:
        return "WL_SCAN_COMPLETED";
        case WL_NO_SSID_AVAIL:
        return "WL_NO_SSID_AVAIL";
        case WL_CONNECT_FAILED:
        return "WL_CONNECT_FAILED";
        case WL_CONNECTION_LOST:
        return "WL_CONNECTION_LOST";
        case WL_CONNECTED:
        return "WL_CONNECTED";
        case WL_DISCONNECTED:
        return "WL_DISCONNECTED";
    }
}

const int potPin = 34;

// set the LCD number of columns and rows
int lcdColumns = 16;
int lcdRows = 2;
int potValue = 0;

const int sampleWindow = 50;
unsigned int sample;
int db;

// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);  

BlynkTimer timer;

// This function is called every time the Virtual Pin 5 state changes
BLYNK_WRITE(V5)
{
  int pinValue = param.asInt(); 
  Blynk.virtualWrite(V5, pinValue);
}

// This function is called every time the device is connected to the Blynk.Cloud
BLYNK_CONNECTED()
{
  // Change Web Link Button message to "Congratulations!"
  Blynk.setProperty(V3, "offImageUrl", "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations.png");
  Blynk.setProperty(V3, "onImageUrl",  "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations_pressed.png");
  Blynk.setProperty(V3, "url", "https://docs.blynk.io/en/getting-started/what-do-i-need-to-blynk/how-quickstart-device-was-made");
}

// This function sends device's uptime every second to Virtual Pin 2.
void myTimerEvent()
{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V2, millis() / 1000);
  Blynk.virtualWrite(V5, db);
}

void setup(){

  pinMode (potPin, INPUT);
  // initialize LCD
  lcd.init();
  // turn on LCD backlight                      
  lcd.backlight();

  //Serial.begin(115200);
  lcd.setCursor(0, 0);
  lcd.println("Welcome");
  lcd.clear();
  
  WiFi.begin(ssid, password);
  WiFiStatus = WiFi.status();
    while(WiFiStatus != WL_CONNECTED){
    delay(250);
    WiFiStatus = WiFi.status();
    //lcd.println(Get_WiFiStatus(WiFiStatus));
    //lcd.clear();
    }
  //lcd.clear();
  //lcd.setCursor(0, 0);
  //lcd.print("WiFi Enabled");
  //lcd.clear();
  
  lcd.setCursor(0, 0);
  lcd.print("Welcome");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());  
  delay(500);
  
  lcd.clear();
  //Conneting to the Blynk IoT Server
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);
  // You can also specify server:
  //Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password, "blynk.cloud", 80);
  //Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password, IPAddress(192,168,1,100), 8080);

  // Setup a function to be called every second
  timer.setInterval(1000L, myTimerEvent);
}

void loop(){
  
  Blynk.run();
  timer.run();
  
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
  
  //Serial.println(peakToPeak);
  
  db = map(peakToPeak, 20, 400, 30, 75); //calibrate for deciBels
  
  lcd.setCursor(0, 0);
  lcd.print("Loudness: ");
  lcd.print(db);
  lcd.print("dB");
  
  if (db <= 40)
  {
    lcd.setCursor(0, 1);
    lcd.print("Level: Quite");
  }
  else if (db > 40 && db < 60)
  {
    lcd.setCursor(0, 1);
    lcd.print("Level: Moderate");
  }
  else if (db >= 60)
  {
    lcd.setCursor(0, 1);
    lcd.print("Level: High");
  }
  delay(600);
  lcd.clear();
}
