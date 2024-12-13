#include <WiFi.h>

// Librerías que hay que descargar
#include <CustomJWT.h>
#include <PubSubClient.h>
#include <Adafruit_AHTX0.h>

// Datos de Wi-Fi
const char* ssid = "alumnosInfo";
const char* pass = "InformaticaUNLP";

// MQTT Broker
const char *mqtt_broker = "163.10.140.50";
const char *topic = "mediciones/temp_hum";
const char *mqtt_username = "esp32";
const char *mqtt_password = "public";
const int mqtt_port = 1122;

WiFiClient espClient;
PubSubClient client(espClient);

// Datos de JWT
// Clave de seguridad
char key[] = "taller-proyecto-2";
// Objeto CustomJWT con la clave y la longitud esperada del payload
CustomJWT jwt(key, 256);

// AHT10
Adafruit_AHTX0 aht;

int i = 0;

char mensaje[512];

void setup() {
  Serial.begin(115200);
  // Espera a que el Serial inicialize
  while (!Serial) {};
  delay(1000);

  // Alocación de memoria para JWT
  jwt.allocateJWTMemory();

  // Conexión a la red Wi-Fi ---------------------------------------------------------------------------------------------------------------------------------

  // Inicia la conexión
  WiFi.begin(ssid, pass);
  // Indicación de inicio de conexión
  Serial.print("Conectando a la red Wi-Fi..."); 
  // Muestra puntos hasta que se conecte
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConectado a la red Wi-Fi\n");

  // Conexión al sever MQTT ----------------------------------------------------------------------------------------------------------------------------------

  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
  while (!client.connected()) {
    String client_id = "esp32-client-";
    client_id += String(WiFi.macAddress());
    Serial.printf("El cliente %s intenta conectarse al broker MQTT\n", client_id.c_str());
    if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("ESP32 conectado\n");
    } else {
      Serial.print("Error de estado: ");
      Serial.print(client.state());
      Serial.print("\n");
      delay(2000);
    }
  }
  //client.publish(topic, jwt.out);
  client.subscribe(topic);

  // Inicialización del AHT10
  if (!aht.begin()) {
    Serial.println("Probando encontrar el sensor");
    delay(10);
  }
}

void callback(char *topic, byte *payload, unsigned int length) {
  /*for (int i = 0; i < length; i++) {
    mensaje[i] = (char) payload[i];
  }
  mensaje[i] = "0";*/

  Serial.print("\nNuevo mensaje en el tópico: ");
  Serial.println(topic);
  Serial.print("\nMensaje:");
  for (int i = 0; i < length; i++) {
    Serial.print((char) payload[i]);
  }
  Serial.println();
  Serial.println("-----------------------\n");
}

void loop() {
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);   // Obtiene los datos del sensor

  unsigned long inicio = micros();

  Serial.print("Temperatura: "); Serial.print(temp.temperature); Serial.println(" C°");
  Serial.print("Humedad: "); Serial.print(humidity.relative_humidity); Serial.println("% rH");
  // Generación del JWT --------------------------------------------------------------------------------------------------------------------------------------
  //char string[] = "{\"temp\":22.5,\"speed\":\"25.1\"}";
  //char string[] = "{\"temp\":" + temp + ",\"humidity\":" + humidity + "}\0";
  char string[50];
  // Temperatura y humedad
  //snprintf(string, sizeof(string), "{%.1f,%.1f\}", temp.temperature, humidity.relative_humidity);
  snprintf(string, sizeof(string), "{\"temp\":%.1f,\"humidity\":%.1f}", temp.temperature, humidity.relative_humidity);
  //snprintf(string, sizeof(string), "{\"temp\":%.1f,\"humidity\":\"%.1f\"}", temp.temperature, humidity.relative_humidity);

  Serial.print(string);
  Serial.print("\n");
  jwt.encodeJWT(string);
  // Se imprime el token
  //Serial.printf("Token: %s\n", jwt.out);
  // Se envía el token al servidor
  //client.publish(topic, jwt.out);
  client.publish(string, jwt.out);

  unsigned long fin = micros();

  unsigned long tiempo = fin - inicio;

  Serial.print("Tiempo en microsegundos: ");
  Serial.print(tiempo);
  Serial.print("\n\n");

  delay(5000);
}