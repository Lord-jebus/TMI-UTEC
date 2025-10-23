#include <SoftwareSerial.h>   // Incluimos la librería  SoftwareSerial  
SoftwareSerial BT(9,8);    // Definimos los pines RX y TX del Arduino conectados al Bluetooth
 
void setup()
{
  BT.begin(38400);       // Inicializamos el puerto serie BT (Para Modo AT 2)
  Serial.begin(9600);   // Inicializamos  el puerto serie  
}
 
void loop()
{
  if(BT.available())    // Si llega un dato por el puerto BT se envía al monitor serial
    Serial.write(BT.read());
 
  if(Serial.available())  // Si llega un dato por el monitor serial se envía al puerto BT
     BT.write(Serial.read());
}
