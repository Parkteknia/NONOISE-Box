# NONOISE-BOX
Prototipo de dispositivo IoT para el análisis de ruido en la ciudad (2022)

**IMPORTANTE: Revisa las NOTAS al final**

## Hardware

#### NodeMCU Amica V2 - Chip ESP8266
Es una placa similar a Arduino pero con conectividad WIFI
#### MAX9814 con AGC
Micrófono analógico de alta ensibilidad
#### DS3231 RTC I2C
Reloj àra mantener la hora configurada del dispositivo aunque se apague
#### Módulo de placa LED RGB SMD, KY-009
Estados y alertas lumínicas del dispositivo

## Programación de la ESP8266
La api_key que usará el dispositivo se crea de forma local mediante la llave privada de nuestra API y se programa en la placa mediante el archivo config. Además el archivo config se usa para establecer la url de la api remota, el nombre o id del dispositivo (opcional) y los datos de la red WIFI que anunciará, entre otros.

Para programar la NONOISE-BOX se ha usado Arduino IDE.
## Configuración
Cuando conectamos la NONOISE-BOX por primera vez se pone en modo Configuración, lo que significa que podremos acceder a ella a través de la nueva red Wifi NONOISE-BOX y la clave de acceso “12ad34min”.

Una vez estemos conectados a la red Wifi "NONOISE-BOX" podremos acceder vía navegador web a la dirección http://192.168.4.1 y entrar en la pagina de configuración del dispositivo.

Básicamente podremos hacer tres cosas:
#### 1. Modos de funcionamiento
+ **Night**: Modo por defecto. Trabaja entre las 22:30h y las 8:00h de la mañana siguiente
+ **Day**: Trabaja entre las 8:00h y las 22:30h
+ **Full**: Trabaja 24h
+ **Test**: Modo para desarrolladores
#### 2. Credenciales de acceso al Wifi de nuestra Red
El nombre de nuestra red wifi y su clave para acceso a la API remota
#### 3. Test de LED y reinicio del dispositivo.

## Niveles de ruido
El dispositivo está programado para medir cuatro franjas de dBs (decibelios).

 + **Silence** (Luz blanca en la NONOISE, en gris en las estadísticas): de 0db a 24db
 + **Moderate** (Luz verde): de 24dB a 34db
 + **Warning** (Luz ámbar) de 34dB a 44dB
 + **Danger** (Luz roja) de 44dB hasta 60dB

## Grabación de datos y estadísticas
Si queremos que el dispositivo registre el histórico de las medidas que toma, deberemos conectarla a Internet, por lo que deberemos proporcionar una red y clave Wifi válidas y con conexión a Internet.

Una vez que hemos registrado la red Wifi la NONOISE se reinicia e intenta conectarse a ella, si lo consigue saldrá del modo de configuración y empezará a enviar las lecturas a un servidor en Internet que las grabará en una base de datos. Si no se conecta al Wifi volverá al modo de configuración.

## Envío de datos
Para enviar los datos al servidor web se utilza una implementación de Firebase JWT para Arduino, la cual permite cifrar los datos en formato json mediante una clave y enviarlos como un token al servidor HTTP (80)

**Sin embargo, para cumplir con los estándares de seguridad debe implementarse la conexión https a través del puerto 443 u otro puerto SSL.**

La API recibe los siguientes tados (excluyendo la IP que la obtiene mediante la proia conexión y created_at en tiempo de ejecución)

```php
// ak = La API key del dispositivo
$api_key = $_GET['ak'];

// tk = El token cifrado con los datos
$tok = JWTUtil::verifyToken($api_key, $_GET['tk']);
```
Si las comprobaciones de la API key son válidas, se obtiene descifrando el propio token:
```
"box_code" => "604f4ff4",
"ip" => getUserIP(),
"mode" => $data[6],
"analog" => $data[1],
"voltage" => $data[2],
"db" => $data[3],
"db_spl" => $data[4],
"status" =>$data[0],
"created_at" => date("Y-m-d H:i:s")
```

## Visualización de datos
Para la visualización de los datos se creó una interfaz web PHP en el propio servidor de la API, a través de la cual i previa autorización mediante API key, el usuario podía visualizar la siguiente información:

#### Gráfica diaria
![Day graph](https://github.com/Parkteknia/NONOISE-Box/blob/main/docs/images/nonoise-24h.png)
#### Gráfica semanal y anual
![Global graph](https://github.com/Parkteknia/NONOISE-Box/blob/main/docs/images/nonoise-global.png)
#### Tabla de registros
![Stats table](https://github.com/Parkteknia/NONOISE-Box/blob/main/docs/images/nonoise-logs.png)

## Sketch Arduino
Las partes importantes del código están comentadas. Hacemos una descripción general de los archivos del sketch.

**Archivo principal**: NONOISE_BOX.ino, el cual incluye:

```
• "Libs.h" (Carga de librerías principales utilizadas)
 ◦ <ESP8266WiFi.h>
 ◦ <WiFiClientSecure.h>
 ◦ <ESP8266HTTPClient.h>
 ◦ <ESP8266WebServer.h>
 ◦ <ESP8266mDNS.h>
 ◦ <EEPROM.h>
 ◦ <Wire.h> // incluye librería para interfaz I2C
 ◦ <RTClib.h>

• #include "Config.h" (Las variables de configuración del dispositivo)
• #include "EEPROMFunc.h" (Gestión de EEPROM)
• #include "ClockFunc.h" (Gestión del reloj)
• #include "WiFiFunc.h" (Gestión de conexión Wifi)
• #include "JWT.h"(Token cifrado por comunicación de datos)
  ◦ <ArduinoJson.h>
  ◦ <CustomJWT.h>

• #include "API.hpp" (Gestión de peticiones en la API externa)
• #include "BOX.cpp" (El objeto BOX, sensores del dispositivo, estados, colores, etc).
  ◦ #include "BOX.h"
```
## NOTAS

El desarrollo de esta idea se detuvo en el momento que se intentó implementar SSL. El uso conjunto de la librería JWT en la placa y la inclusión de las librerías necesarias HTTPS, así como los certificados para la conexión SSL, creaban desboaramientos de memoria difíciles de manejar. Mi conclusión es que si se usa SSL es mejor no cargar JWT y usar el propio certificado para cifrar los datos y para ello se debe modificar el código. O también puedes substituir la ESP8266 (2Mb Ram) por una ESP32 (4Mb Ram)
