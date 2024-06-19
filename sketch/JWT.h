

#include <CustomJWT.h>

void renewToken(String data_json){

  CustomJWT jwt(apiKey, 256);

  char string[data_json.length()+1];
  
  data_json.toCharArray(string, data_json.length()+1);
  jwt.allocateJWTMemory();
  jwt.encodeJWT(string);
  apiToken = jwt.out;
  jwt.clear();
  delay(100);
 
}
