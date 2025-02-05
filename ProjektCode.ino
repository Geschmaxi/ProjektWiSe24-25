#include <DHT11.h>
#include <LiquidCrystal.h>
#include <IRremote.hpp>
#include <Wire.h>
#include <RTClib.h>
const int rs = 2,
          en = 3,
          d4 = 6,
          d5 = 7,
          d6 = 8,
          d7 = 9;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const int dry = 527;
const int wet = 200;
int percHum = 0;

RTC_DS1307 rtc;

#define SENSOR_PIN 4

DHT11 dht11(10);

int receiver = 11; // Signal Pin of IR receiver to Arduino Digital Pin 11

/*-----( Declare objects )-----*/
IRrecv irrecv(receiver);     // create instance of 'irrecv'
//vairable uses to store the last decodedRawData
uint32_t last_decodedRawData = 0;
/*-----( Function )-----*/
int translateIR() // takes action based on IR code received
{
  
  uint32_t value = irrecv.decodedIRData.decodedRawData;

    if (irrecv.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT) {
      value = last_decodedRawData;  // Wiederholung des letzten Signals
    } else {
      last_decodedRawData = value;  // Neues Signal speichern
    }
  //map the IR code to the remote key
  switch (irrecv.decodedIRData.decodedRawData)
  {

    case 0xF30CFF00: return 1;   break;
    case 0xE718FF00: return 2;   break;
    case 0xA15EFF00: return 3;   break;
    case 0xF708FF00: return 4;   break;
    default:
      return 0;
  }// End Case
  //store the last decodedRawData
  last_decodedRawData = irrecv.decodedIRData.decodedRawData;
  delay(500); // Do not get immediate repeat
} //END translateIR

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  pinMode(5,OUTPUT);
  pinMode(12,OUTPUT);
  pinMode(A0,OUTPUT);
  pinMode(SENSOR_PIN, INPUT_PULLUP);  // Eingang mit Pull-up-Widerstand
  irrecv.enableIRIn(); // Start the receiver
  // Überprüfen, ob die RTC verfügbar ist
  
}

void loop() {
  //Bewässerung, falls genug Wasser vorhanden
  percHum = map(analogRead(A3), wet, dry, 100, 0);
  DateTime now = rtc.now();  // Aktuelle Zeit abrufen
  // UTC +1 (Mitteleuropäische Zeit)
  int hour_adjusted = now.hour() -7; 
  hour_adjusted = hour_adjusted % 24; 
  lcd.setCursor(0, 0);       //bis Zeile 97 Ausgabe der aktuellen Zeit und Datum  
  lcd.print("Datum: ");
  lcd.print(now.day());
  lcd.print(".");
  lcd.print(now.month());
  lcd.print(".");
  lcd.print(now.year());

  lcd.setCursor(0, 1);
  lcd.print("Zeit:  ");
  if (hour_adjusted < 10) lcd.print("0"); // Falls nötig, führende Null hinzufügen
  lcd.print(hour_adjusted);
  lcd.print(":");
  if (now.minute() < 10) lcd.print("0");
  lcd.print(now.minute());
  lcd.print(":");
  if (now.second() < 10) lcd.print("0");
  lcd.print(now.second());
  lcd.setCursor(0, 0);
  if(dht11.readTemperature()>=30){    //Temperatur sollte möglichst nicht 
    digitalWrite(A0,HIGH);            //über 30°C bleiben
    while(dht11.readTemperature()>=30){
      delay(10);
    }
    digitalWrite(A0,LOW);
  }
  if(hour_adjusted == 7)  //7 Uhr Licht an
    digitalWrite(13,HIGH);
  if(hour_adjusted==21)   //21 Uhr Licht aus
    digitalWrite(13,LOW);
  if(now.day()%3==0)  //Lüfter läuft an jedem Dritten tag des Monats
    if(hour_adjusted==12)//von 12 Uhr
      digitalWrite(A0,High);
    if(hour_adjusted==13)//bis 13 Uhr
      digitalWrite(A0,LOW);
  if (percHum < 15) {
    if (digitalRead(SENSOR_PIN) == LOW) {   //Wasserstand hoch genug starte Bewässerung
      digitalWrite(12, HIGH);     //Pumpe an
      lcd.print("Bewaesserung im");
      lcd.setCursor(0, 1);
      lcd.print("Gange. Pls Wait");
      lcd.setCursor(0, 0);
      delay(10000);
      digitalWrite(12,LOW);       //Pumpe aus
      lcd.clear();
    } else {
      lcd.print("Wasser auffüllen");
      while(digitalRead(SENSOR_PIN) == HIGH){   //Loop wenn Wasserstand zu niedrig
          if(hour_adjusted<22 && hour_adjusted > 7){
                digitalWrite(5,HIGH);     //strom an Buzzer an  
                delay(10);
                digitalWrite(5,LOW);      //strom an buzzer aus
                delay(10);
          }
      }
      lcd.clear();
    }
  }
  int IRSignal = translateIR();
  switch(IRSignal)
  {
    case 1:       //Ausgabe der Luftfeuchtigkeit
            lcd.print("Luftfeuchtigkeit:");
            lcd.println(dht11.readHumidity());
            lcd.print("%");            
            delay(1000);
            lcd.clear();
            
            break;
    case 2:       //Ausgabe der Temperatur
            lcd.print("Temperatur:");
            lcd.println(dht11.readTemperature());
            lcd.print("°C");
            delay(1000);
            lcd.clear();
            
            break;
    case 3:        //Ausgabe der Bodenfeuchtigkeit
            lcd.print("Bodenfeuchtigkeit:");
            lcd.println(percHum);
            lcd.print("%");
            delay(1000);
            lcd.clear();
            
            break;  
    case 4:         //Ausgabe, ob genug Wasser vorhanden ist
            lcd.print("WasserStand:");
            if(digitalRead(SENSOR_PIN)==LOW){
              lcd.println("Genug");
            }
            else{
              lcd.println("Zu Wenig");
            }

            delay(1000);
            lcd.clear();
            
            break;
    default:
      break;
    IRSignal=0;
  }

}
