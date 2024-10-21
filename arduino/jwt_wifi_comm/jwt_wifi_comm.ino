#include <WiFi.h>

// Librerías que hay que descargar
#include <CustomJWT.h>
#include <PubSubClient.h>

// Datos de JWT
// Clave de seguridad
char key[] = "que_se_pudra_Flanders";
// Objeto CustomJWT con la clave y la longitud esperada del payload
CustomJWT jwt(key, 256);

// Datos de Wi-Fi
const char* ssid = "";
const char* pass = "";

void setup() {
  Serial.begin(115200);
  // Espera a que el Serial inicialize
  // NO FUNCIONA, Serial.print() NO FUNCIONA EN SETUP
  while (!Serial) {};
  delay(100);

  // Tiempo de inicio
  unsigned long StartTime = millis();

  // Generación del JWT --------------------------------------------------------------------------------------------------------------------------------------

  // Alocación de memoria para JWT
  jwt.allocateJWTMemory();
  char string[] = "{\"temp\":22.5,\"speed\":25.1}";
  jwt.encodeJWT(string);
  // Se imprime el token
  Serial.printf("Token completo: %s.%s.%s\n", jwt.header, jwt.payload, jwt.signature);

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

  // Tiempo de finalización
  unsigned long CurrentTime = millis();
  // Tiempo de ejecución
  unsigned long ElapsedTime = CurrentTime - StartTime;
}

void loop() {
  delay(3000);

  Serial.printf("Token completo: %s.%s.%s\n", jwt.header, jwt.payload, jwt.signature);
  Serial.print("Estado de la conexión: ");
  Serial.print(WiFi.status());
  Serial.print("\n");
}
