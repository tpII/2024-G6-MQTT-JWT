#include <WiFi.h>

// Librerías que hay que descargar
#include <CustomJWT.h>
#include <PubSubClient.h>

// Datos de Wi-Fi
const char* ssid = "alumnosInfo";
const char* pass = "InformaticaUNLP";

// MQTT Broker
const char *mqtt_broker = "163.10.143.16";
const char *topic = "test-conexion/prueba";
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

int i = 0;

void setup() {
  Serial.begin(115200);
  // Espera a que el Serial inicialize
  // NO FUNCIONA, Serial.print() NO FUNCIONA EN SETUP
  while (!Serial) {};
  delay(1000);

  // Generación del JWT --------------------------------------------------------------------------------------------------------------------------------------

  unsigned long inicioJwt = millis();

  // Alocación de memoria para JWT
  jwt.allocateJWTMemory();
  char string[] = "{\"temp\":22.5,\"speed\":25.1}";
  jwt.encodeJWT(string);
  // Se imprime el token
  Serial.printf("Token: %s\n", jwt.out);

  unsigned long finJwt = millis();

  unsigned long tiempoJwt = finJwt - inicioJwt;
  // Conexión a la red Wi-Fi ---------------------------------------------------------------------------------------------------------------------------------

  unsigned long inicioCon = millis();

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
  client.publish(topic, jwt.out);
  client.subscribe(topic);

  unsigned long finCon = millis();

  // Tiempo de ejecución
  unsigned long tiempoCon = finCon - inicioCon;

  Serial.print("Tiempo de generación de JWT: ");
  Serial.print(tiempoJwt);
  Serial.print(" milisegundos\n");

  Serial.print("Tiempo de conexión: ");
  Serial.print(tiempoCon);
  Serial.print(" milisegundos\n");
}

void callback(char *topic, byte *payload, unsigned int length) {
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
  //delay(1000);
  client.publish(topic, "Never");
  delay(1000);
  client.publish(topic, "gonna");
  delay(1000);
  client.publish(topic, "give");
  delay(1000);
  client.publish(topic, "you");
  delay(1000);
  client.publish(topic, "up");
  delay(1000);
  client.publish(topic, "Never");
  delay(1000);
  client.publish(topic, "gonna");
  delay(1000);
  client.publish(topic, "let");
  delay(1000);
  client.publish(topic, "you");
  delay(1000);
  client.publish(topic, "down");
  delay(1000);
  client.publish(topic, "Never");
  delay(1000);
  client.publish(topic, "Never");
  delay(1000);
  client.publish(topic, "gonna");
  delay(1000);
  client.publish(topic, "run");
  delay(1000);
  client.publish(topic, "around");
  delay(1000);
  client.publish(topic, "and");
  delay(1000);
  client.publish(topic, "desert");
  delay(1000);
  client.publish(topic, "you");
  delay(1000);
}
