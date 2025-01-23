#include <DHT11.h>
#include <LiquidCrystal.h>
#include <IRremote.h>
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
  pinMode(SENSOR_PIN, INPUT_PULLUP);  // Eingang mit Pull-up-Widerstand
  irrecv.enableIRIn(); // Start the receiver
  // Überprüfen, ob die RTC verfügbar ist
  if (!rtc.begin()) {
    Serial.println("RTC nicht gefunden!");
    while (1);
  }
  
  // Prüfen, ob die Uhr läuft
  if (!rtc.isrunning()) {
    Serial.println("RTC läuft nicht! Stelle die Zeit ein...");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));  // Stellt die Uhr auf Kompilierungszeit
  }
}

void loop() {
  //Bewässerung, falls genug Wasser vorhanden
  percHum = map(analogRead(A5), wet, dry, 100, 0);
  DateTime now = rtc.now();  // Aktuelle Zeit abrufen
  // UTC +1 (Mitteleuropäische Zeit)
  int hour_adjusted = now.hour() -7;  

  // Sicherstellen, dass es nicht über 23 hinausgeht
  hour_adjusted = hour_adjusted % 24;
  if (percHum < 15) {
    if (digitalRead(SENSOR_PIN) == LOW) {
      digitalWrite(12, HIGH);
      lcd.print("Bewässerung im");
      lcd.setCursor(0, 1);
      lcd.print("Gange. Pls Wait");
      lcd.setCursor(0, 0);
      delay(10000);
      digitalWrite(12,LOW);
      lcd.clear();
    } else {
      lcd.print("Wasser auffüllen");
      while(digitalRead(SENSOR_PIN) == HIGH){
      digitalWrite(5,HIGH);
      delay(10);
      digitalWrite(5,LOW);
      delay(10);
      }
      lcd.clear();
    }
  }
  int IRSignal = translateIR();
  switch(IRSignal)
  {
    case 1: 
            lcd.print("Luftfeuchtigkeit:");
            lcd.println(dht11.readHumidity());
            lcd.print("%");            
            delay(1000);
            lcd.clear();
            
            break;
    case 2: 
            lcd.print("Temperatur:");
            lcd.println(dht11.readTemperature());
            lcd.print("°C");
            delay(1000);
            lcd.clear();
            
            break;
    case 3: 
            lcd.print("Bodenfeuchtigkeit:");
            lcd.println(percHum);
            lcd.print("%");
            delay(1000);
            lcd.clear();
            
            break;
    case 4: 
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
