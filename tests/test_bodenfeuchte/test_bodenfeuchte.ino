const int soilMoistPin = A5;
const int soilMoistLevelLow = 527;    //Dieser Wert soll von euch entsprechend angepasst werden
const int soilMoistLevelHigh = 200;   //Dieser Wert soll von euch entsprechend angepasst werden

void setup() {
  Serial.begin(115200);  
  pinMode(soilMoistPin, INPUT);
}

void loop() {
  int soilMoist = analogRead(soilMoistPin);
  Serial.print("Analog Value: ");
  Serial.print(soilMoist);
  // Auswertung der Bodenfeuchtigkeit in Prozent
  int percentSoilMoist = map(soilMoist, soilMoistLevelHigh, soilMoistLevelLow, 100, 0);
  Serial.print("\t"); 
  Serial.print(percentSoilMoist);
  Serial.println(" %");
  delay(2000);
}