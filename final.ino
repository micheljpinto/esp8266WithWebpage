/******* Author: Eng.Automation, Michel de Jesus Pinto *******/
/******** Date:  04/2020**************************************/
/******** micheljpinto@gmail.com *****************************/

/* Webserver using microcontroler esp8266, developed with Arduino 
framework system on VsCode Editor (recomended for best visualization
code). This code generate one webpage, acessed in the choiced ipadress,
and using json protocol for better compatibility with javascript   */

//SYSTEM INCLUDES
  #include <ESP8266WiFi.h>  
  #define ARDUINOJSON_ENABLE_ARDUINO_STRING 1
  #include <ArduinoJson.h> // last version of 5.XX 

//Webserver Includes
  #include <ESPAsyncWebServer.h>
  #include <ESPAsyncTCP.h>
  #include "index.h"

/**********************************************SYS MACROS AND DEFINES**************************************************/
  //#define MODEAP              //If define the wifi settings working AP mode, if not STATIO mode 
  #define REQUEST_TIME_SQL 120000   //tempo de requisição do servidor em milisegundos
  #define QTDE_OF_SENSORS 3
  #define SERIAL_SPEED 230400

/**********************************************************************************************************************/  

/***************************************** DATA OF CONECTIONS**************************************/
  #define MY_STATIC_IP    192,168,1,154   // adjust for your config //  
  #define SERVER_IP       192,168,1,1     // adjust for your config //  
  #define SUBNET_MASK     255,255,255,0   // adjust for your config // 
  #define SSID       "default0"           // adjust for your config // 
  #define PASSWORD  "@hfj0601"            // adjust for your config // 
/*************************************** END DATA OF CONECTIONS************************************/

/***************************************PROTOTYPES OF FUNCTIONS************************************/
  //WEBSERVER
  void setupWebServer();
  void parserJsonActuatorWrite(String);

/*************************************END PROTOTYPE FUNCTIONS***************************************/

/**************************************************VARIABLES****************************************/
  WiFiClient client;
  AsyncWebServer server(80); 

/************************************************CONFIG WIFI*********************************************************/
    
  #ifdef MODEAP
    
    const char* ssid = "espTeste";        
    const char* password =  "12345678";   
    //if network using microcontroler that acess point
    void setupWithAP() {
      WiFi.softAP(ssid, password);
      IPAddress myIP = WiFi.softAPIP();
      //WiFi.begin();
      Serial.println("Acess Point - Adress IP conected: ");
      Serial.println(myIP);
      // Print ESP8266 Local IP Address
      Serial.println(WiFi.localIP());
    }

  #else //if choice STATION mode...   
    IPAddress ipStatic(MY_STATIC_IP);
    IPAddress ipGateway(SERVER_IP);  
    IPAddress subnet(SUBNET_MASK);
    const char* ssid = SSID; 
    const char* password =  PASSWORD;  
        
    void setupWithSTATION(){
      
      //WiFi.mode(WIFI_STA);
      WiFi.config(ipStatic, ipGateway, subnet);
      WiFi.begin(ssid, password);
      Serial.println("Waiting network...");
      Serial.println("Conecting WIFI network...");
      
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }
      Serial.print("Adress IP conected: ");
      Serial.println(WiFi.localIP());
  
    }
  #endif
                    
/**************************************************END WIFI**********************************************************/

/*************************************************CONFIG WEBSERVER***************************************************/
          
  void setupWebServer(){
    
    server.on(//Write received status datas in actuator
      "/writeatuador",
      HTTP_POST,
      [](AsyncWebServerRequest * request){},
      NULL,
      [](AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total) {
      String stream;
      for (size_t i = 0; i < len; i++) {
        stream+=(char)data[i];
      }
      parserJsonActuatorWrite(stream);
      request->send(200,"text/html","OK");
    });   //fim server on 

   server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      String mainPage= MAIN_page;
      
      request->send(200, "text/html", mainPage);
    });
 
    server.begin();
    Serial.println("HTTP server started");
  }

/**************************************************END WEBSERVER*****************************************************/
          
/****************************************ENDPOINT FUNCTIONS*****************************************/


  void deserealizeJsonReceivingActuatorsStatetWrite(String json){
    
    const size_t capacity = JSON_OBJECT_SIZE(40) + 15;
    DynamicJsonBuffer jsonBuffer(capacity);
    //const char* json = "{\"id\":12,\"status\":1}";
    JsonObject& root = jsonBuffer.parseObject(json);

    if (!root.success()) {
      Serial.println("Json received error at endpoint /writeatuador! ");
      return;
    }

    int id = root["id"]; // 12
    int status = root["status"]; // 1
    Serial.println(json);
    //Change this line for IOS
    digitalWrite(LED_BUILTIN,!digitalRead(LED_BUILTIN));  
  }
      
  void setup() { 
    Serial.begin(SERIAL_SPEED);
      while (!Serial); 
    #ifdef MODEAP
      setupWithAP();
    #else 
      setupWithSTATION();
    #endif
    setupWebServer();
    pinMode(LED_BUILTIN,OUTPUT);
    Serial.println("\nNetwork conected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    
  }

  void loop() { 
   
  }
