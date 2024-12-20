#include <DHT11.h>
#include <LiquidCrystal.h>
const int rs = 2,
          en = 3,
          d4 = 6,
          d5 = 7,
          d6 = 8,
          d7 = 9;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const int dry = 527;
const int wet = 200;

DHT11 dht11(10);

const int waterAnalogPin = A4;
const int waterSensorPowerPin = 11;
const int waterLevelLow = 50;
const int waterLevelMiddle = 280;
const int waterLevelHigh = 330;

#define key1 A0  //connect wire 1 to pin A0
#define key2 A1  //connect wire 2 to pin A1
#define key3 A2  //connect wire 3 to pin A2
#define key4 A3  //connect wire 4 to pin A3


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  pinMode(waterAnalogPin, INPUT);
  pinMode(waterSensorPowerPin, OUTPUT);
  pinMode(key1, INPUT_PULLUP);  // set pin as input
  pinMode(key2, INPUT_PULLUP);  // set pin as input
  pinMode(key3, INPUT_PULLUP);  // set pin as input
  pinMode(key4, INPUT_PULLUP);  // set pin as input
  pinMode(1,OUTPUT);
}

int Wasserstand() {
  digitalWrite(waterSensorPowerPin, HIGH);  // Schaltet den Strom für den Sensor ein
  delay(100);
  int waterAnalog = analogRead(waterAnalogPin);
  digitalWrite(waterSensorPowerPin, LOW);  //Sensor wieder aus
  return waterAnalog;                      //Rückgabe des Wasserstands als analoger Wert
}

int Bodenfeuchte() {
  int sensval = analogRead(A5);
  int percHum = map(sensval, wet, dry, 100, 0);  //Skala von 0-100 (Prozentwert) in der der gemessene Wert Umgewandelt wird
  return percHum;
}

int Luftfeuchtigkeit() {
  int hum = dht11.readHumidity();
  return hum;  //Rückgabe der Luftfeuchtigkeit
}

int Temperatur() {
  int temp = dht11.readTemperature();
  return temp;  //Rückgabe der Temperatur
}




void loop() {
  //Bewässerung, falls genug Wasser vorhanden
  if (Bodenfeuchte < 15) {
    if (Wasserstand() > waterLevelMiddle) {
      digitalWrite(12, HIGH);
      lcd.display();
      lcd.print("Bewässerung im");
      lcd.setCursor(0, 1);
      lcd.print("Gange. Pls Wait");
      lcd.setCursor(0, 0);
      delay(10000);
      digitalWrite(12,LOW);
      lcd.clear();
      lcd.noDisplay();
      lcd.clear();
    } else {
      lcd.display();
      lcd.print("Wasser auffüllen");
      while(Wasserstand() < waterLevelMiddle){
      digitalWrite(1,HIGH);
      delay(10);
      digitalWrite(1,LOW);
      delay(10);
      }
    }
  }
  int key1S = digitalRead(key1);  // read if key1 is pressed
  int key2S = digitalRead(key2);  // read if key1 is pressed
  int key3S = digitalRead(key3);  // read if key1 is pressed
  int key4S = digitalRead(key4);  // read if key1 is pressed
  if (!key1S) {
    lcd.display();
    lcd.print("Boden:");
    lcd.print(Bodenfeuchte());
    lcd.print("%");
    delay(2000);
    lcd.noDisplay();
    lcd.clear();
  } else if (!key2S) {
    lcd.display();
    lcd.print("Luft:");
    lcd.print(Luftfeuchtigkeit());
    lcd.print("%");
    delay(2000);
    lcd.noDisplay();
    lcd.clear();
  } else if (!key3S) {
    lcd.display();
    lcd.print("Temperatur:");
    lcd.print(Temperatur());
    lcd.print("°C");
    delay(2000);
    lcd.noDisplay();
    lcd.clear();
  } else if (!key4S) {
    lcd.display();
    lcd.print("Wasserstand:");
    if (Wasserstand() <= waterLevelLow) {
      lcd.setCursor(0, 1);
      lcd.print("Leer");
    } else if (Wasserstand() > waterLevelLow && Wasserstand() <= waterLevelMiddle) {
      lcd.setCursor(0, 1);
      lcd.print("Fast Leer");
    } else if (Wasserstand() > waterLevelMiddle && Wasserstand() <= waterLevelHigh) {
      lcd.setCursor(0, 1);
      lcd.print("Fast Leer");
    } else if (Wasserstand() > waterLevelHigh) {
      lcd.setCursor(0, 1);
      lcd.print("Voll genug");
    }
    delay(2000);
    lcd.setCursor(0, 0);
    lcd.noDisplay();
    lcd.clear();
  }
}
