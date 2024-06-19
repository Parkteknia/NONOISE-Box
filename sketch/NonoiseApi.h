
#ifndef NonoiseApi_h
#define NonoiseApi_h

#define NONOISE_DEBUG 1
#define ARDUINOJSON_DECODE_UNICODE 1
#define ARDUINOJSON_USE_LONG_LONG 1
#include <Arduino.h>
#include <ArduinoJson.h>
#include <Client.h>
#include "NonoiseCertificate.h"

#define NONOISE_HOST "api.apidomain.io"
#define NONOISE_SSL_PORT 443
#define HANDLE_MESSAGES 1

//unmark following line to enable debug mode
//#define _debug

typedef bool (*MoreDataAvailable)();
typedef byte (*GetNextByte)();
typedef byte* (*GetNextBuffer)();
typedef int (GetNextBufferLen)();

struct telegramMessage {
  String text;
  String chat_id;
  String chat_title;
  String from_id;
  String from_name;
  String date;
  String type;
  String file_caption;
  String file_path;
  String file_name;
  bool hasDocument;
  long file_size;
  float longitude;
  float latitude;
  int update_id;
  int message_id;  

  int reply_to_message_id;
  String reply_to_text;
  String query_id;
};

class NonoiseApi {
public:
  NonoiseApi(const String& api_key, Client &client);
  void updateApiKey(const String& api_key);
  void updateToken(const String& token);
  String getApiKey();
  String getToken();
  String sendGetToApi(const String& command);

  bool readHTTPAnswer(String &body, String &headers);
  bool getMe();
  bool sendSensorData(const String& api_key, const String& doc);
  
  String buildCommand(const String& cmd);

  bool checkForOkResponse(const String& response);
  telegramMessage messages[HANDLE_MESSAGES];
  long last_message_received;
  String name;
  String userName;
  int longPoll = 0;
  unsigned int waitForResponse = 1500;
  int _lastError;
  int last_sent_message_id = 0;
  int maxMessageLength = 1500;

private:
  // JsonObject * parseUpdates(String response);
  String _token;
  String _api_key;
  Client *client;
  void closeClient();
};

#endif
