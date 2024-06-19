#include "Client.hpp"
/*************
 * 
 *  Función que realiza la conexión HTTP a la API montada en el servidor externo para enviarle los datos leídos por el dispositivo
 * 
 */
bool ApiRequest(String apiKey, String apiToken)
{
   HTTPClient http;
   WiFiClient client;
   getUrl = url + "?ak=" + apiKey + "&tk=" + apiToken;
   Serial.println(getUrl);
   if (http.begin(client, getUrl)) //Iniciar conexión
   {
      Serial.print("[HTTP] GET...\n");
      int httpCode = http.GET();  // Realizar petición
      if (httpCode > 0) {
         Serial.printf("[HTTP] GET... code: %d\n", httpCode);

         if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
            //String payload = http.getString();   // Obtener respuesta del servidor
            //processResponse(payload); // La llamada a la función processResponse en Client.hpp
            return true;
         }
         
      }
      else {
         Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
         return false;
      }
      http.end();
      
   }
   else {
      Serial.printf("[HTTP} Unable to connect\n");
      return false;
   }
}
