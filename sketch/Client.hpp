/*************
 * 
 *  Esta función procesa/deserializa la respuesta JSON del servidor
 *  El objeto JSON puede contener no solo si la conexión y el procesamiento de datos fue correcto, sino también instrucciones.
 *  Etá implementada pero no se le ha dado funcionalidad.
 * 
 */
void processResponse(String response)
{
  String dataJson;
  dataJson = response;
  DynamicJsonDocument dataJsonDes(1024);  //Reservo una variable dinamica de 1024bytes para guardar el archivo JSON deserializado.
  deserializeJson(dataJsonDes, dataJson); //Deserializo el contenido de dataJson y lo guardo en dataJsonDes.
  //int sensor = dataJsonDes["data"]["sensor"];
  String message = dataJsonDes["result_log"];
  //Serial.println(sensor);   // Mostrar respuesta por serial
  //Serial.println(message);
}
