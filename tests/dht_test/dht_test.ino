#include <DHT11.h>
#define DHT11PIN 10

DHT11 dht11(2);

void  setup()
{
  Serial.begin(9600);
 
}

void loop() {
    // Attempt to read the humidity value from the DHT11 sensor.
    int humidity = dht11.readHumidity();

    // Check the result of the reading.
    // If there's no error, print the humidity value.
    // If there's an error, print the appropriate error message.
    if (humidity != DHT11::ERROR_CHECKSUM && humidity != DHT11::ERROR_TIMEOUT) {
        Serial.print("Humidity: ");
        Serial.print(humidity);
        Serial.println(" %");
    } else {
        // Print error message based on the error code.
        Serial.println(DHT11::getErrorString(humidity));
    }
}