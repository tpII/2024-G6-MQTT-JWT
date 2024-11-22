#include <WiFi.h>
#include <PubSubClient.h>

// Wi-Fi
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

uint i = 0;

void setup() {

  Serial.begin(115200);

  // Conexión a la red Wi-Fi

  // Inicia la conexión
  WiFi.begin(ssid, pass);

  // Indicación de inicio de conexión
  Serial.print("Conectando a la red Wi-Fi...");
  
  // Muestra puntos hasta que se conecte
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConectado a la red Wi-Fi");
  //Serial.print("Dirección IP: ");
  //Serial.print(WiFi.localIP());

  // Conexión al sever MQTT

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
  // Publish and subscribe
  client.publish(topic, "Hi, I'm ESP32 ^^");
  client.subscribe(topic);
}

void callback(char *topic, byte *payload, unsigned int length) {
    Serial.print("\nMessage arrived in topic: ");
    Serial.println(topic);
    Serial.print("\nMessage:");
    for (int i = 0; i < length; i++) {
        Serial.print((char) payload[i]);
    }
    Serial.println();
    Serial.println("-----------------------");
}

void loop() {
  char mensaje[10];
  delay(3000);
  itoa(i, mensaje, 10);
  client.publish(topic, mensaje);
  i++;
  //Serial.print("Estado de la conexión: ");
  //Serial.print(client.connected());
  //Serial.print("\n");
}
