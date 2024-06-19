
// Lee las credenciales de acceso al WiFi guardadas en la EEPROM, el ssid y la password.

void GetEPROM_WifiData(){

    Serial.println("Reading EEPROM ssid");
  
    String esid;
    for (int i = 0; i < 32; ++i)
    {
      esid += char(EEPROM.read(i));
    }
    Serial.println();
    Serial.print("SSID: ");
    Serial.println(esid);
    Serial.println("Reading EEPROM pass");
  
    String epass = "";
    for (int i = 32; i < 96; ++i)
    {
      epass += char(EEPROM.read(i));
    }
    c_ssid = esid;
    c_psswd= epass;
    
    Serial.print("PASS: ");
    Serial.println(epass);
    delay(1000);
}
// Escribe las credenciales de acceso al WiFi en la EEPROM, el ssid y la password.
void WriteEPROM_WifiData(String qsid, String qpass){

  Serial.println(qsid);
  Serial.println(qpass);
  
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
  }
}
// Lee la configuración boxMode guardada en la EEPROM
void GetEPROM_BoxModeData(){
    String box_mode = "";
    for (int i = 96; i < 97; ++i)
    {
      box_mode += char(EEPROM.read(i));
    }
    if(box_mode.length() > 0){
      boxModeConf = box_mode;
    }
    Serial.println(boxModeConf);
}
// Escribe la configuración boxMode en la EEPROM
void WriteEPROM_BoxModeData(String bmode){

    if (bmode.length() > 0){
      for (int i = 0; i < bmode.length(); i++){
        EEPROM.write(96 + i, bmode[i]);
      }
      EEPROM.commit();
    }
  
}
