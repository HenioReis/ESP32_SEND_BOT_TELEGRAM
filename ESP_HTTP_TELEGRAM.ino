/*Autor Henio Reis
 * canal Maker IoT
 * email:heniooliver@hotmail.com
 * Proj com esp32 e telegram bot para controle em automação inteligente pelo celular.
 * Adaptado para esp8266 e esp32
 */
 
#if defined(ESP8266)
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#else
#include <WiFi.h>
#include <WiFiClientSecure.h>
#endif
#define HOST "..."; //api.telegram.org
#define SSL_PORT 443 
#define JSON_BUFF_SIZE 10000

const char BotToken[]= "";
char ssid[] = "";
char password[] = "";
const int led = 2;
int ledStatus = 0;
int Bot_mtbs = 1000; 
long Bot_lasttime; 

WiFiClientSecure client_ssl;
Telegram BOT (BotToken, client_ssl);

void setup() {
  Serial.begin(115200);

  // Attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);

  // Set WiFi to station mode and disconnect from an AP if it was Previously
  // connected
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  pinMode(ledPin, OUTPUT);

  IniciaConnect();

}

void loop(){

if (millis() > Bot_lasttime + Bot_mtbs)  {
    int NewMessages = BOT.get_Updates(BOT.last_message_received + 1);

    while (NewMessages) {
      Serial.println("got response");
      menege_new_messages(NewMessages);
      NewMessages = BOT.get_Updates(BOT.last_message_received + 1);
    }

    Bot_lasttime = millis();
  }
}



void menege_new_messages(int NewMessages) {

  for (int i = 0; i < NewMessages; i++) {

    // Inline buttons with callbacks when pressed will raise a callback_query message
    if (BOT.message[i].type == "callback_query") {
      Serial.print("Call back button pressed by: ");
      Serial.println(BOT.message[i].from_id);
      Serial.print("Data on the button: ");
      Serial.println(BOT.message[i].text);
      send_message(BOT.message[i].from_id, BOT.message[i].text, "");
    } else {
      String chat_id = String(BOT.message[i].chat_id);
      String text = BOT.message[i].text;

      String from_name = BOT.message[i].from_name;
      if (from_name == "") from_name = "Guest";

     if (text == "Led On") {
        digitalWrite(ledPin, HIGH);   // turn the LED on (HIGH is the voltage level)
        ledStatus = 1;
        send_message(chat_id, "Led ON", "");
      }
  
      if (text == "Led Off") {
        ledStatus = 0;
        digitalWrite(ledPin, LOW);    // turn the LED off (LOW is the voltage level)
        send_message(chat_id, "Led OFF", "");
      }
  
      if (text == "Status Led") {
        if(ledStatus){
          send_message(chat_id, "Led onboard ON", "");
        } else {
          send_message(chat_id, "Led onboard OFF", "");
        }
      }
      if (text == "Opcao teclado") {
      String keyboardJson = "[[\"Led On\", \"Led Off\"],[\"Status\"]]";
      Bot.sendMessageWithReplyKeyboard(chat_id, "Escolha uma das opções", "", keyboardJson, true);
    }

      if (text == "/start") {
        String benvindo = "Welcome. Instruções para usuario do taclado.\n";
        benvindo += "Automação com Telegram e Esp32/Esp8266.\n\n";
        benvindo += "Led On : liga led onboard\n";
        benvindo += "Led Off : desliga led onboard\n";
        benvindo += "Opção : Mostra teclado\n";
        benvindo += "Status : Mostra ultimo estado do led\n";

        send_message(chat_id, benvindo, "Markdown");
      }
    }
  }
}
