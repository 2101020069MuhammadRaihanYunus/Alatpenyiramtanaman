#include <LiquidCrystal_I2C.h>
#include <NewPing.h> // Include  library used for measuring the distance using HC-SR 06 sonar sensor
#include "DHT.h"
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SimpleTimer.h>
#include <SPI.h>
#define TRIGGER_PIN  D5  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN   D6  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 500 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.
#define DHTPIN 13
#define DHTTYPE DHT22
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

int pump = 2;
DHT dht(DHTPIN, DHTTYPE);

char auth[] = "95BnqvrR-0-tVrM8F5h6W6B_ZvN0wFOc";
char ssid[] = "Pilus";                                   
char pass[] = "23456789";

float TankHeight, WaterLevelMAX, GapbwSonar, SonarReading, ActualReading,  Temp;
int percentage;

SimpleTimer timer;

//Sensor Kelembapan Tanah
void sendSensor()
{                         
  int value = analogRead(A0);
  Serial.print(value);
  lcd.setCursor(0, 1);
  lcd.print("S:");
  lcd.print(value);
  Blynk.virtualWrite(V0, value);
  
   if (value > 480 ) {
    digitalWrite(pump, HIGH);
    lcd.setCursor(11, 0);
    lcd.print("P:ON");
    WidgetLED LED(V7);
    LED.on();}
  else {
    digitalWrite(pump, LOW);
    lcd.setCursor(11, 0);
    lcd.print("P:OF");
    WidgetLED LED(V7);
    LED.off();
  }
 
}

//Sensor Ultrasonik
void UltraSensor() {
  TankHeight= 20;
  WaterLevelMAX=0.9*TankHeight;
  GapbwSonar=TankHeight-WaterLevelMAX;
    SonarReading=sonar.ping_cm();
  Temp= SonarReading-GapbwSonar;
  ActualReading= WaterLevelMAX-Temp;
  percentage=(ActualReading/WaterLevelMAX*100);
   Blynk.virtualWrite(V4, percentage);
  Serial.println(percentage);
  lcd.setCursor(8,1);
  lcd.print("A:");
  lcd.print(percentage);
  lcd.print("%");
}

//Sensor Suhu
  void Tempsensor() {
   float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C \n"));

  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(t);
  lcd.print("C");

  Blynk.virtualWrite(V2, t);
  Blynk.virtualWrite(V3, h);

}


void setup() {

  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);
  timer.setInterval(100L, sendSensor);
  timer.setInterval(100L, UltraSensor);
  timer.setInterval(1000L, Tempsensor);
  pinMode(2,OUTPUT);
  lcd.begin();                      // initialize the lcd 
  // Print a message to the LCD.
  lcd.backlight();
  dht.begin();


}
void loop() {

  Blynk.run();
  timer.run();
  delay(100);
}
