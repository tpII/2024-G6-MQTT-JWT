#include <CustomJWT.h>

// Clave de seguridad
char key[] = "tester";
// Objeto CustomJWT con la clave y la longitud esperada del payload
CustomJWT jwt(key, 256);

void setup() {
  Serial.begin(9600);
  jwt.allocateJWTMemory();
  char string[] = "{\"temp\":22.5,\"speed\":25.1}";
  jwt.encodeJWT(string);
  Serial.printf("Header: %s\nHeader Length: %d\n", jwt.header, jwt.headerLength);
  Serial.printf("Payload: %s\nPayload Length: %d\n", jwt.payload, jwt.payloadLength);
  Serial.printf("Signature: %s\nSignature Length: %d\n", jwt.signature, jwt.signatureLength);
  Serial.printf("Final Output: %s\nFinalOutput Length: %d\n", jwt.out, jwt.outputLength);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.printf("Token completo: %s.%s.%s\n", jwt.header, jwt.payload, jwt.signature);
  delay(4000);
}
