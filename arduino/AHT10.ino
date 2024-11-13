// Descargar la libreria de "Adafruit AHTX0" y sus dependencias (se instalan con este)
#include <Adafruit_AHTX0.h>

Adafruit_AHTX0 aht;

void setup() {
  Serial.begin(115200);
  Serial.println("Prueba del sensor");

  if (! aht.begin()) {
    Serial.println("Probando encontrar el sensor");
    while (1) delay(10);
  }
  Serial.println("Se encontro el sensor");
}

void loop() {
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);// populate temp and humidity objects with fresh data
  Serial.print("Temperature: "); Serial.print(temp.temperature); Serial.println(" degrees C");
  Serial.print("Humidity: "); Serial.print(humidity.relative_humidity); Serial.println("% rH");

  delay(1000);
}