#include "Libs.h"
#include "Config.h"
#include "EEPROMFunc.h"
#include "ClockFunc.h"
#include "WiFiFunc.h"
#include "JSONSensor.h"
#include "JWT.h"
//#include "API.hpp"
#include "BOX.h"

/*
 * Declaración del Objeto BOX pasándole las diferentes variables de configuración
 * Este Objeto representa el dispositivo y realiza las funciones de medición, adopta los distintos estados (traducidos a colores), se enciende,m se apaga, etc.
 */
BOX BOX(sensorPin, RED, GREEN, BLUE, microSensitivity, microGain, threshold, millis(), analogValue, boxStatus, boxDB, boxDBSPL);

// El Objeto del Servidor Web
ESP8266WebServer server(80);
JSONSensor jsonSensor;
// Manejar los bloques HTML de las páginas web que gestiona el Servidor Web
#include "GeneralTemplate.h"
#include "AccesPointTemplate.h";
#include "ClientPointTemplate.h";

X509List cert(NONOISE_CERTIFICATE_ROOT);
WiFiClientSecure secured_client;

NonoiseApi boxApi(apiKey, secured_client);

StaticJsonDocument<1024> doc;

void startAPServer();

void setup()
{
  
  if(debugMode){
    Serial.begin(115200);
    Serial.println();
    Serial.println("Disconnecting previously connected WiFi");
  }
  
  //WiFi.disconnect(); //Desconecta la conexión WiFi previa si la hay
  EEPROM.begin(512); //Iniciaiza EEPROM
  
  delay(10);
  
  if(debugMode){
    Serial.println();
    Serial.println();
    Serial.println("Startup");
  }
  
  GetEPROM_WifiData(); //Intenta leer las credenciales del acceso WiFi grabadas en la EEPROM: c_ssid, c_psswd
  GetEPROM_BoxModeData(); //Intenta leer el modo de configuración grabado en la EEPROM

  delay(10);
  
  WiFi.begin(c_ssid.c_str(), c_psswd.c_str()); //Inicia la conexión WiFi
  secured_client.setTrustAnchors(&cert); // Add root certificate for api.apidomain.io
  
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  
  Serial.print("\nWiFi connected. IP address: ");
  Serial.println(WiFi.localIP());

  
  // Start Internal Clock
  startBoxClock();
  
  //Hacer test para ver si es una conexión WiFi válida
  if (testWifi())
  {
    if(debugMode){ // Un debug mode para omitir algunos Serial.print()
      Serial.println("Succesfully Connected!!!");
      Serial.print("IP address:\t");
      Serial.println(WiFi.localIP()); 
      Serial.print("Wifi Status:\t");
      Serial.println(WiFi.status());
    }

    // Comprobar si el servidor DNS inició y admite la posterior conexión desde un cliente conectado a la red a acceder via: http://NONOISE.local al dispositivo.
    if (MDNS.begin("NONOISE")) { Serial.println("MDNS responder started"); }else{ Serial.println("MDNS responder NOT started"); } 
    
    startClientServer(); //Inicia el Servidor Web para cliente
    
    Serial.println("HTTP server started");
  }
  else
  {
    Serial.println("Turning the HotSpot On");
    
    if (MDNS.begin("NONOISE")) { Serial.println("MDNS responder started"); }else{ Serial.println("MDNS responder NOT started"); } //Igual que arriba
    
    startAPServer(); //Inicia el servidor web modo Access Point
    setupAP();//Configura el Access Point
  }

  Serial.println();
  Serial.println("Waiting after setup AP");
  
  while ((WiFi.status() != WL_CONNECTED))
  {
    Serial.print(".");
    delay(100);
    server.handleClient();
  }

  BOX.wakeUP(); // Despierta la caja

}
void loop() {

  if(!loop_start){
    Serial.print("NONOISE STARTED: ");
    printTime();
    loop_start = true;
  }
    
  /*
   * // runTime es una función booleana en ClockFunc.h que devuelve true si la caja está en horario de funcionamiento.
   * Esta función se podría llamar más veces dentro del loop() y permitir que aunque la caja no envie datos por no ser su horario de funcionamiento siga mostrando los colores de estado
   * y poder configurarle esta opción.
   * 
   */
  if(runTime()){ 
    boxRunTime = true;    
  }

  if(boxRunTime){
    BOX.runUP(); //pone en modo "funcionando" el dispositivo
  }

  if(prevStateControl==false){ // Si el dispositivo se acaba de encender boxMode = wakeup, si no el boxMode a su valor de configuración.
      boxMode = "wakeup";
  }else{
    boxMode = BOX.getBoxMode(boxModeConf);
  }
    
  prevStateControl = true; //Ya no vuelve a estar "wakeup" hasta que se apague el dispositivo y se vuelva a encender

  if ((WiFi.status() == WL_CONNECTED) || (WiFi.status() == 3))
  {
    if(millis() - previousMillis >= interval) { //Mantiene a la escucha el servidor http,refresca cada "interval"
      MDNS.update(); 
      server.handleClient();
      //server.handleClient();
    //     WiFi.disconnect();                    
    //     server_start();
      previousMillis=millis();
    }
    delay(1); 
  }else //Si no hay conexión WiFi
  {
    Serial.print("WiFi NO CONECTADA");

    // Realizo diez intentos para reconectar
    while(wifi_intents<=10){
      wifi_intents++;
      Serial.print("WIFI Try: ");
      Serial.println(wifi_intents);
      delay(3000);
    }

    // Si no se conecta de nuevo al WiFi se pone en modo de configuración
    Serial.println("Turning the HotSpot On");
    startAPServer(); // Lanzo función startAPServer definida más abajo y que activa y pone a la escucha el servidor web interno, el Access Point para config del dispositivo
    setupAP(); //Configuro Access Point
    
  }

  if(boxRunTime==false){
    if(millis() - previousIntervalBoxWaiting >= intervalBoxWaiting) { 
      printTime(); //Print la hora por el serial (para debuguear).
      BOX.runOFF(); //Aviso luminoso cuando la caja está en espera de ponerse en el modo configurado de funcionamiento horario. Aviso cada 30 segundos
      previousIntervalBoxWaiting = millis();
    }
  }
  if(millis() - previousMillisSensor >= intervalSensor) {
    
   
  
    int sensor = BOX.getStatusValue(); // El Modo de la NONOISE 0=silence, 1=moderate, 2=warning, 3=danger
    int analog = BOX.getAnalog(); // El valor analógico de la lectura
    double voltage = BOX.getVoltage(); // El voltage leído
    double db = BOX.getDB(); // Los decibelios (dB)
    double db_spl = BOX.getDBSPL(); // Los dB SPL ( Sound pressure level )
      
    long timer = millis(); 
    
    if(doc.size()<=1){
        doc.add(jsonSensor.buildJson(sensor, analog, voltage, db, db_spl, timer, boxMode));
      //delay(6000);
      }     
    previousMillisSensor = millis();
  }
  
  if ((WiFi.status() == WL_CONNECTED) || (WiFi.status() == 3))
  {
    //MDNS.update();
    if(millis() - previousMillisApi >= intervalApi) {

      String json;
      serializeJson(doc, json);
      //renewToken(json);
      doc.clear();
      //Serial.println(apiToken);
      boxApi.sendSensorData(apiKey, json);
      json = "";
      previousMillisApi=millis();
      //MDNS.update();
    }
  }

      
}

/*************
 * 
 * Función que lanza el servidor web interno si la wifi está conectada
 * 
 */
 
void startAPServer()
{
  Serial.println("");
  if (WiFi.status() == WL_CONNECTED)
    Serial.println("WiFi connected");
    Serial.print("Local IP: ");
    Serial.println(WiFi.localIP());
    Serial.print("SoftAP IP: ");
    Serial.println(WiFi.softAPIP());
    createWebServer();
    // Start the server
    server.begin();
    Serial.println("Server started");
}

/*************
 * 
 * Activo el Punto de Acceso Wifi al Servidor Web Interno en modo configuración (setup Access Point, aceptará el registro de la configuración WiFi para sair a internet.
 * El modo de funcionamiento de la caja n=Night, d=Day, f=Fulltime, t=Test/Debug
 * También permite test de led
 * 
 */
 
void setupAP(void)
{
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  int n = WiFi.scanNetworks();
  Serial.println("scan completed");
  if (n == 0)
    Serial.println("No WiFi Networks found");
  else
  {
    Serial.print(n);
    Serial.println(" Networks found");
    for (int i = 0; i < n; ++i)
    {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
      delay(10);
    }
  }
  Serial.println("");
  /*
   * Este loop que sigue es un escaneo de redes WiFi
   */
  st = "<ol>";
  for (int i = 0; i < n; ++i)
  {
    // Imprime el SSID y el RSSI para cada red encontrada
    st += "<li>";
    st += WiFi.SSID(i);
    st += " (";
    st += WiFi.RSSI(i);

    st += ")";
    st += (WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*";
    st += "</li>";
  }
  st += "</ol>";
  
  delay(100);
  /* Aquí se define la red wifi para conectarse al dispositivo, WIFI_AP_NAME, WIFI_AP_PSWD están en Config.h */
  WiFi.softAP(WIFI_AP_NAME, WIFI_AP_PSWD);
  Serial.println("Initializing_Wifi_accesspoint");
  BOX.redAP();
  startAPServer();
  Serial.println("over");
}

/*************
 * 
 *  Aquí se define el servidor web http para el modo Configuración  así como las url/llamadas via navegador que va a aceptar
 *  En este modo nos conectamos al dispositivo
 * 
 */
void createWebServer()
{
    MDNS.update(); 
    
    server.on("/", []() {

      IPAddress ip = WiFi.softAPIP();

      String led = server.arg("LED");
      
      Serial.println(led);
      if(led=="ON"){
        BOX.delic(500);
       
      }
      if(led=="OFF"){
        BOX.off();
      }

      String action = server.arg("ACTION");

      if(action=="RESTART"){
        ESP.restart();
      }
      /*
      String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
      */
      server.setContentLength(sizeof(ap_page));
      server.send(200, "text/html", ap_page);
      content_html = "";
    });
    
    server.on("/scan", []() {
      //setupAP();
      IPAddress ip = WiFi.softAPIP();
      String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);

      content = "<!DOCTYPE HTML>\r\n<html>go back";
      server.send(200, "text/html", content);
    });

    server.on("/setting", []() {
  
      // Get EEPROM Wifi credentials
      String qsid = server.arg("ssid");
      String qpass = server.arg("pass");

      // Get Box Mode
      String qbmode = server.arg("bmode");
      
      if (qbmode.length() > 0){
        WriteEPROM_BoxModeData(qbmode); // Save EEPROM Box Mode (n=night, d=day, f=full, t=test)
      }else if (qsid.length() > 0 && qpass.length() > 0) {
        WriteEPROM_WifiData(qsid, qpass); // Save EEPROM Wifi credentials


       /* 
        *  Este trozo de código comentado que sigue se deja solo para que se vea como se vacía el Sketch principal para que todo sea un poco más legible
        *  En este caso corresponde a la función de encima WriteEPROM_WifiData, o sea se ha metido la funcionalidad de escribir en la EEPROM al recibir
        *  los datos por parte del usuario desde la web de configuración que se ve en el código comentado en una función (WriteEPROM_WifiData) dentro del archivo EEPROMFunc.h y así liberamos espacio
        *  en el .ino
        *  
        */
      /*
      //Si se reciben datos por parte del usuario, si son el sid y la passwd y no están vacíos, escríbelos en la EEPROM
      if (qsid.length() > 0 && qpass.length() > 0) {
        Serial.println("clearing eeprom");
        for (int i = 0; i < 96; ++i) {
          EEPROM.write(i, 0);
        }
        Serial.println(qsid);
        Serial.println("");
        Serial.println(qpass);
        Serial.println("");

        Serial.println("writing eeprom ssid:");
        for (int i = 0; i < qsid.length(); ++i)
        {
          EEPROM.write(i, qsid[i]);
          Serial.print("Wrote: ");
          Serial.println(qsid[i]);
        }
        Serial.println("writing eeprom pass:");
        for (int i = 0; i < qpass.length(); ++i)
        {
          EEPROM.write(32 + i, qpass[i]);
          Serial.print("Wrote: ");
          Serial.println(qpass[i]);
        }
        EEPROM.commit();
        Serial.print("EEPROM.commit ");
        */
        content = "{\"Success\":\"guardado en la eeprom... reiniciando dispositivo\"}";
        statusCode = 200;
        ESP.reset(); // Reinicia el dispositivo
      } else {
        
        content = "{\"Error\":\"404 not found\"}";
        statusCode = 404;
        Serial.println("Sending 404");
        
        server.sendHeader("Location", "/",true);   //Redirección web
        server.send(200, "text/html",content_html);
      }
      
      server.sendHeader("Access-Control-Allow-Origin", "*"); // Headers HTML
      server.send(statusCode, "application/json", content);
      
    });
}

/*************
 * 
 * Esta función activa el servidor web cliente, permite el acceso al dispositivo una vez está funcionando.
 * Se accede mediante la dirección http://nonoise.local 
 * Para acceder al dispositivo hay que estar dentro de la misma red a la que está conectado el mismo.
 * En este servidor web prácticamente no se ha implementado nada. Pero podría mostrar las mismas estadísticas que se muestran en la API externa o incluso nuevas funcionalidades
 * Hace llamadas a Template.h que básicamente incluye el html y el css de la interfaz web
 */
 
void startClientServer(){

    //handlePageHeader();
    //handlePageFooter();
    
    server.on("/", [](){
      
      //handleRoot();
      server.setContentLength(sizeof(client_page));
      server.send(200, "text/html", client_page);
      
    });

    server.on("/labs", [](){

      //handleLabs();
      
      String led = server.arg("LED");
      
      Serial.println(led);
      if(led=="ON"){
        digitalWrite(5,HIGH);
        delay(20);
      }
      if(led=="OFF"){
        digitalWrite(5,LOW);
        delay(20);
      }
      server.setContentLength(sizeof(client_page));
      server.send(200, "text/html", client_page);
    });
    
    server.on("/test.svg", drawGraph);
    server.on("/inline", []() {
      server.send(200, "text/plain", "this works as well");
    });
    server.onNotFound(handleNotFound);
    server.begin();
}
