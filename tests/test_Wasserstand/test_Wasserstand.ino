int Sensor = A4;
int alterMesswert = 0; // Variable, um den vorherigen Messwert zu vergleichen
char printBuffer[128];

void setup()
{
Serial.begin(9600);
}

void loop()
{
  int messwert = analogRead(Sensor); // Sensor auslesen und in der Variable Messwert speichern

  if(((alterMesswert>=messwert) && ((alterMesswert - messwert) > 10)) || ((alterMesswert<messwert) && ((messwert - alterMesswert) > 10)))
{
  sprintf(printBuffer,"Messwert ist %d\n",Sensor, messwert);
  Serial.print(printBuffer);
  alterMesswert = messwert;
}
}