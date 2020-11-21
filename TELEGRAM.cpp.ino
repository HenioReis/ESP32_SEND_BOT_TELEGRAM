
#include <Arduino.h>
#include <ArduinoJson.h>
#include <Client.h>

struct message{
  String text;
  String chat_id;
  String sender;
  String date;
};

class Telegram
{
  public:
    Telegram(const char* token, Client &client);
    void IniciaConnect();
    String send_message(String chat_id, String text);
    //String send_message(String chat_id, String text, TelegramKeyboard &keyboard_markup, bool one_time_keyboard = true, bool resize_keyboard = true);
    String PostMessage(String msg);
    message get_Updates();

  private:
      String readPayload();
      const char* token;
      int last_message_recived;

      Client *client;
};


Telegram::Telegram(const char* token, Client &client)  {
  this->client = &client;
  this->token=token;
}

void Telegram::IniciaConnect()  {
  if(!client->connected()){
    client->connect(HOST, SSL_PORT);
  }
}


String Telegram::PostMessage(String msg) {
    IniciaConnect();
    client->println("POST /bot"+String(token)+"/sendMessage"+" HTTP/1.1");
    client->println("Host: api.telegram.org");
      client->println("Content-Type: application/json");
      client->println("Connection: close");
      client->print("Content-Length: ");
      client->println(msg.length());
      client->println();
      client->println(msg);

    return readPayload();
}

messege Telegram::get_Updates()  {
    IniciaConnect();
    //Send your request to api.telegram.org
    String getRequest = "GET /bot"+String(token)+"/getUpdates?limit=1&offset="+String(last_message_recived)+" HTTP/1.1";
    client->println(getRequest);
    client->println("User-Agent: curl/7.37.1");
    client->println("Host: api.telegram.org");
    client->println("Accept: */*");
    client->println();

    String payload = readPayload();
      if (payload != "") {
      message m;
      StaticJsonBuffer<JSON_BUFF_SIZE> jsonBuffer;
      JsonObject & root = jsonBuffer.parseObject(payload);



      if(root.success()){

        int update_id = root["result"][0]["update_id"];
        update_id = update_id+1;

        if(last_message_recived != update_id ){
          String sender = root["result"][0]["message"]["from"]["username"];
          String text = root["result"][0]["message"]["text"];
          String chat_id = root["result"][0]["message"]["chat"]["id"];
          String date = root["result"][0]["message"]["date"];

          m.sender = sender;
          m.text = text;
          m.chat_id = chat_id;
          m.date = date;
          last_message_recived=update_id;
          return m;
        }else{
          m.chat_id = "";
          return m;
        }
      }
      else{
        Serial.println("");
        Serial.println("Message too long, skipped.");
        Serial.println("");
        int update_id_first_digit=0;
        int update_id_last_digit=0;
        for(int a =0; a<3; a++){
          update_id_first_digit= payload.indexOf(':',update_id_first_digit+1);
        }
        for(int a =0; a<2; a++){
          update_id_last_digit= payload.indexOf(',',update_id_last_digit+1);
        }
      last_message_recived = payload.substring(update_id_first_digit+1,update_id_last_digit).toInt() +1;
      }
    }
  }

String Telegram::send_message(String chat_id, String text)  {
  if( chat_id != "0" && chat_id != ""){
    StaticJsonBuffer<JSON_BUFF_SIZE> jsonBuffer;
    JsonObject& buff = jsonBuffer.createObject();
    buff["chat_id"] = chat_id;
    buff["text"] = text;

    String msg;
    buff.printTo(msg);
    return PostMessage(msg);
  } else {
    Serial.println("Chat_id not defined");
  }
}

String Telegram::readPayload(){
  char c;
  String payload="";
    while (client.connected()) {
    payload = client.readStringUntil('\n');
    if (payload == "\r") {
      break;
     }
    }
  payload = client.readStringUntil('\r');
  Serial.println(payload);
  return payload;
}
