#include "JSONSensor.h"

JSONSensor::JsonDocument JSONSensor::buildJson(int sensor, int analog, float voltage, double db, double db_spl, long timer, String boxMode) {
  
    StaticJsonDocument<256> jsonDoc;
    JsonObject root = jsonDoc.to<JsonObject>();
    /*
    root["sensor"] = sensor;
    root["analog"] = analog;
    root["voltage"] = voltage;
    root["db"] = db;
    root["db_spl"] = db_spl;
    root["time"] = timer;
    root["ip"] = ip;
    root["mode"] = boxMode;
    */
    
    
    JsonArray data = root.createNestedArray("data");
    data.add(sensor);
    data.add(analog);
    data.add(voltage);
    data.add(db);
    data.add(db_spl);
    data.add(timer);
    data.add(boxMode);
    
        
    return data;
}
