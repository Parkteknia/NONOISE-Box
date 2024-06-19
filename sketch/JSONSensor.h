#include <ArduinoJson.h>

#define CONFIG_JSON_SIZE 256 //Размер json config ----

class JSONSensor{
public:
    using JsonDocument = StaticJsonDocument<CONFIG_JSON_SIZE>;
    JsonDocument buildJson(int sensor, int analog, float voltage, double db, double db_spl, long timer, String boxMode); 
};
