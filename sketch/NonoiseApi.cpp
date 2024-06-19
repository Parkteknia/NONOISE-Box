/*
   Copyright (c) 2022 Pera Andreu All right reserved.
    
   NonoiseApi - Library to communicate from ESP8266 in a NONOISE-Box to API.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "NonoiseApi.h"

#define ZERO_COPY(STR)    ((char*)STR.c_str())
#define BOT_CMD(STR)      buildCommand(F(STR))

NonoiseApi::NonoiseApi(const String& api_key, Client &client) {
  updateApiKey(api_key);
  this->client = &client;
}
void NonoiseApi::updateApiKey(const String& api_key) {
  _api_key = api_key;
}
void NonoiseApi::updateToken(const String& token) {
  _token = token;
}
String NonoiseApi::getApiKey() {
  return _api_key;
}
String NonoiseApi::getToken() {
  return _token;
}
String NonoiseApi::buildCommand(const String& cmd) {
  String command;

  command += F("?tk=");
  command += _token;
  command += F("&ak=");
  command += _api_key;
  command += F("&cmd=");
  command += cmd;

  return command;
}

String NonoiseApi::sendGetToApi(const String& command) {
  String body, headers;
  
  // Connect with api.apidomain.io not already connected
  if (!client->connected()) {
    #ifdef NONOISE_DEBUG  
        Serial.println(F("[BOT]Connecting to server"));
    #endif
    if (!client->connect(NONOISE_HOST, NONOISE_SSL_PORT)) {
      #ifdef NONOISE_DEBUG  
        Serial.println(F("[BOT]Conection error"));
      #endif
    }
  }
  if (client->connected()) {

    #ifdef NONOISE_DEBUG  
        Serial.println("sending: " + command);
    #endif  

    client->print(F("GET /nonoise/api"));
    client->print(command);
    client->println(F(" HTTP/1.0"));
    client->println(F("Host:" NONOISE_HOST));
    client->println(F("Accept: application/json"));
    client->println(F("Cache-Control: no-cache"));
    client->println();

    readHTTPAnswer(body, headers);
  }

  return body;
}

bool NonoiseApi::readHTTPAnswer(String &body, String &headers) {
  int ch_count = 0;
  unsigned long now = millis();
  bool finishedHeaders = false;
  bool currentLineIsBlank = true;
  bool responseReceived = false;

  while (millis() - now < longPoll * 1000 + waitForResponse) {
    while (client->available()) {
      char c = client->read();
      responseReceived = true;

      if (!finishedHeaders) {
        if (currentLineIsBlank && c == '\n') {
          finishedHeaders = true;
        } else {
          headers += c;
        }
      } else {
        if (ch_count < maxMessageLength) {
          body += c;
          ch_count++;
        }
      }

      if (c == '\n') currentLineIsBlank = true;
      else if (c != '\r') currentLineIsBlank = false;
    }

    if (responseReceived) {
      #ifdef NONOISE_DEBUG  
        Serial.println();
        Serial.println(body);
        Serial.println();
      #endif
      break;
    }
  }
  return responseReceived;
}

bool NonoiseApi::getMe() {
  String response = sendGetToApi(BOT_CMD("getMe")); // receive reply from api.apidomain.io
  DynamicJsonDocument doc(maxMessageLength);
  DeserializationError error = deserializeJson(doc, ZERO_COPY(response));
  closeClient();

  if (!error) {
    if (doc.containsKey("result")) {
      name = doc["result"]["first_name"].as<String>();
      userName = doc["result"]["username"].as<String>();
      return true;
    }
  }

  return true;
}

bool NonoiseApi::sendSensorData(const String& api_key, const String& api_token) {
  updateToken(api_token);
  String response = sendGetToApi(BOT_CMD("sensorMe")); // receive reply from api.apidomain.io
  DynamicJsonDocument doc(maxMessageLength);
  DeserializationError error = deserializeJson(doc, ZERO_COPY(response));
  closeClient();

  if (!error) {
    Serial.println("NO API ERROR");
    if (doc.containsKey("result")) {
      //name = doc["result"]["first_name"].as<String>();
      //userName = doc["result"]["username"].as<String>();
      return true;
    }
  }

  return false;
}

bool NonoiseApi::checkForOkResponse(const String& response) {
  int last_id;
  DynamicJsonDocument doc(response.length());
  deserializeJson(doc, response);

  // Save last sent message_id
  last_id = doc["result"]["message_id"];
  if (last_id > 0) last_sent_message_id = last_id;

  return doc["ok"] | false;  // default is false, but this is more explicit and clear
}

void NonoiseApi::closeClient() {
  if (client->connected()) {
    #ifdef NONOISE_DEBUG  
        Serial.println(F("Closing client"));
    #endif
    client->stop();
  }
}
