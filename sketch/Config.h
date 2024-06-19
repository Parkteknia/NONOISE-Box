//App
bool debugMode = true; // En el código hay diferentes Serial.print que no se muestran el el monitor serial si esta variable está en "false"
bool clientWifiServer = true; //Activa o No el modo de acceso WiFi cliente

// Microphone MAX9814
const int microSensitivity = -40;     // microphone sensitivity in dB
const int microGain = 50;

//PINS para el LED RGB
#define BLUE 12
#define GREEN 14
#define RED 13

//Clock DS3231
DateTime boxClock;

int analogValue = 0;
bool prevStateControl = false;
String boxMode;
String boxModeConf = "n"; //test, night,day, full 
int sensorPin  =  A0; // El pin analógico de la NodeMCU ESP8266
int threshold = 480; // Valor del Umbral "analógico", para usarlo con micrófonos cuya ganancia se ajusta mediante regulador manual (YA, NO SE USA EN ESTE DISPOSITIVO), se podría eliminar
double boxDB = 0; // decibelios a 0
double boxDBSPL = 0; // decibelios SPL a 0


// 5 = Started
int boxStatus = 5; // Estado inicial de la caja 5 = Started
bool boxRunTime = false;
//Variables
int i = 0;
int statusCode;
const char* ssid;
const char* passphrase;
String st;
String content;
String content_html;

bool loop_start= false; //Loop iniciado


// Diferentes intervalos de tiempo
unsigned long previousMillis = 0; // Tiempo general
unsigned long previousMillisApi = 0; // Tiempo previo de llamada a la API
unsigned long previousMillisSensor = 0;
unsigned long intervalSensor = 500;
unsigned long interval = 25000; // Inervalo general
unsigned long intervalApi = 3000; // Intervalo de llamada a la API (15 segundos)
unsigned long previousIntervalBoxWaiting = 0; // Tiempo previo desde que la caja estuvo esperando ponerse en funcionamiento según su horario configurado
unsigned long intervalBoxWaiting = 30000; // Intervalo de tiempo en volver a mostrar la señal lumínica que indica que está en modo de espera


//WIFI AP
char* WIFI_AP_NAME = "NONOISE-BOX"; // Nombre de la red Wifi para configurar el dispositivo
char* WIFI_AP_PSWD = "12ad34min"; // Password de la red Wifi para configurar el dispositivo

//WIFI Cient
String c_ssid; // Credenciales WiFi del cliente
String c_psswd; // Credenciales WiFi del cliente

//WIFI
int wifi_intents = 0;
// Se declaran aquí estas funciones para que no den error al compilar el Sketch al no encontrarlas definidas, ya que se declaran más adelante.
bool testWifi(void);
void launchWeb(void);
void setupAP(void);

//SERVERS HTML
String site_title     = "NONOISE-Box";
String nono_version   = "v0.1";
String web_header = ""; // El header de la página HTML en Template.h
String web_content = "";// El body de la página HTML  en Template.h
String web_footer = ""; // El footer de la página HTML  en Template.h

//API credentials
String url = "http://api.apidomain.io"; //SSL problemas, aquí se define la URL de la API
String getUrl;
char* apiUrl = "nonoise/api";
char* apiKey = "c932f4b1518d8781da54a6c4773ec23d"; // La key de acceso a la API se define en el lado de la API Server
char* apiToken = "";
const char* deviceId = "";
