#include <WiFi.h>
#include <PubSubClient.h>

// Wi-Fi
const char* ssid = "";
const char* pass = "";

// MQTT Broker
const char *mqtt_broker = "broker.emqx.io";
const char *topic = "emqx/esp32";
const char *mqtt_username = "emqx";
const char *mqtt_password = "public";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

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
    Serial.printf("The client %s connects to the public MQTT broker\n", client_id.c_str());
    if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("Public EMQX MQTT broker connected");
    } else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
  // Publish and subscribe
  client.publish(topic, "Hi, I'm ESP32 ^^");
  client.subscribe(topic);
}

void callback(char *topic, byte *payload, unsigned int length) {
    Serial.print("Message arrived in topic: ");
    Serial.println(topic);
    Serial.print("Message:");
    for (int i = 0; i < length; i++) {
        Serial.print((char) payload[i]);
    }
    Serial.println();
    Serial.println("-----------------------");
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(1000);
}
