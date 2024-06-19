//Función booleana que devuelve true o false en función de si eldispositivo está conectado a una WiFi o no.

bool testWifi(void)
{
  int c = 0;
  Serial.println("Waiting for WiFi to connect");
  while ( c < 20 ) {
    if (WiFi.status() == WL_CONNECTED)
    {
      return true;
    }
    delay(500);
    Serial.print("*");
    c++;
  }
  Serial.println("");
  Serial.println("Connection timed out, opening AP or Hotspot");
  return false;
}
