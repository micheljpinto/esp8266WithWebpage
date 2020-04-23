/******* Author: Automation and C. Engineering, Michel de Jesus Pinto *******/
/********************** Date:  04/2020 **************************************/
/*********************** micheljpinto@gmail.com *****************************/

/* Webserver using microcontroler esp8266, developed with Arduino 
framework system on VsCode Editor (recomended for best visualization
code). This code generate one webpage, acessed in the choiced ipadress,
and using json protocol for better compatibility with javascript   */

//SYSTEM INCLUDES
  #include <ESP8266WiFi.h>  
  #define ARDUINOJSON_ENABLE_ARDUINO_STRING 1
  #include <ArduinoJson.h> // version use in project of 5.13.5 

//Webserver Includes
  #include <ESPAsyncWebServer.h>
  #include <ESPAsyncTCP.h>
  #include "index.h"
/************************************SYS MACROS AND DEFINES****************************************/
  //#define MODEAP                //If define the wifi settings working AP mode, if not STATIO mode 
  #define SERIAL_SPEED 230400
  //IMPORTANT! Adjust OUT according, the pin of ucontroler desired for to be controlled
  #define OUT1  16
  #define OUT2  5
  #define OUT3  4
  #define OUT4  0

/**************************************************************************************************/  

/*************************************** DATA OF CONECTIONS****************************************/
  #define MY_STATIC_IP    192,168,1,154   // adjust for your config //  
  #define SERVER_IP       192,168,1,1     // adjust for your config //  
  #define SUBNET_MASK     255,255,255,0   // adjust for your config // 
  #define SSID            "SSID"      // adjust for your config // 
  #define PASSWORD        "PASS"      // adjust for your config // 
/************************************** END DATA OF CONECTIONS*************************************/

/***************************************PROTOTYPES OF FUNCTIONS************************************/
  //WEBSERVER
  void setupWebServer();
  String parserJsonActuatorWrite(String);
  int convertChar(String);
  String returnStates();

/*************************************END PROTOTYPE FUNCTIONS***************************************/

/**************************************************VARIABLES****************************************/
  WiFiClient client;
  AsyncWebServer server(80); 
  
/**********************************************WIFI************************************************/
    
  #ifdef MODEAP
    
    const char* ssid =      "espTeste";        
    const char* password =  "mich1983";   
    //if network using microcontroler that acess point
    
    void setupWithAP() {
      WiFi.softAP(ssid, password);
      IPAddress myIP = WiFi.softAPIP();
      WiFi.begin();
      Serial.println("Acess Point - Adress IP conected: ");
      Serial.println(myIP);
      // Print ESP8266 Local IP Address
      // Serial.println(WiFi.localIP());
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
      Serial.println("Conecting WIFI network...");
      
      while (WiFi.status() != WL_CONNECTED) {
        delay(50);
        Serial.print(".");
      }
        
    }
  #endif
                    
/********************************************END WIFI**********************************************/

/*******************************************WEBSERVER***********************************************/
          
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
      ;
      request->send(200,"text/html",parserJsonActuatorWrite(stream));
    });   //fim server on 

   server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      String mainPage= MAIN_page;
      
      request->send(200, "text/html", mainPage);
    });
  
    server.on("/returnstate", HTTP_GET, [](AsyncWebServerRequest *request){
      String resp= returnStates();
      
      request->send(200, "json/text", resp);
    });
    
    server.begin();
    Serial.println("HTTP server started");
  }

/*****************************************END WEBSERVER*********************************************/
          
/******************************************OTHERS FUNCTIONS*****************************************/
  String parserJsonActuatorWrite(String json){
    
    const size_t capacity = JSON_OBJECT_SIZE(2) + 30;
    DynamicJsonBuffer jsonBuffer(capacity);

    //const char* json = "{\"id\":\"OUT1\",\"status\":false}";
    JsonObject& root = jsonBuffer.parseObject(json);

    if (!root.success()) {
      Serial.println("Json received error at endpoint /writeatuador! ");
      return "3";
    }

    const char* id = root["id"]; // "OUT1"
        
    bool status = root["status"]; // false
    //Serial.println(id);
    Serial.println(status);
    int id2 = convertChar(String(id));
    bool state= !digitalRead(id2);

    if(state==status)
      return ( state?"1":"0");
    else{
      digitalWrite(id2,!status);
      return (status?"1":"0");
    }
  }

  int convertChar(String aId){
    int temp;
    
      if(aId=="OUT1"){
        temp=OUT1;
      }
      else if(aId=="OUT2"){
        temp=OUT2;
      }
      else if(aId=="OUT3"){
        temp=OUT3;
      } else {
        temp=OUT4;
      }
    //Serial.println(temp);
    return temp;
  }

  String returnStates(){
    const size_t capacity = JSON_ARRAY_SIZE(4) + JSON_OBJECT_SIZE(1) + 4*JSON_OBJECT_SIZE(2);
    DynamicJsonBuffer jsonBuffer(capacity);

    JsonObject& root = jsonBuffer.createObject();
    JsonArray& actuators = root.createNestedArray("actuators");

    JsonObject& actuators_0 = actuators.createNestedObject();
    actuators_0["id"] = "OUT1";
    actuators_0["status"] = !digitalRead(OUT1);
    
    JsonObject& actuators_1 = actuators.createNestedObject();
    actuators_1["id"] = "OUT2";
    actuators_1["status"] = !digitalRead(OUT2);

    JsonObject& actuators_2 = actuators.createNestedObject();
    actuators_2["id"] = "OUT3";
    actuators_2["status"] = !digitalRead(OUT3);

    JsonObject& actuators_3 = actuators.createNestedObject();
    actuators_3["id"] = "OUT4";
    actuators_3["status"] = !digitalRead(OUT4);

    String lastStates="";
    root.printTo(lastStates);
    return lastStates;
  }
/******************************************END FUNCTIONS********************************************/

/****************************************VOID SETUP /LOOP*******************************************/      
  void setup() { 
    Serial.begin(SERIAL_SPEED);
      while (!Serial); 
      delay(50);
    #ifdef MODEAP
      setupWithAP();
    #else 
      setupWithSTATION();
    #endif
    setupWebServer();
    pinMode(OUT1,OUTPUT);
    pinMode(OUT2,OUTPUT);
    pinMode(OUT3,OUTPUT);
    pinMode(OUT4,OUTPUT);
    pinMode(LED_BUILTIN,OUTPUT);
    digitalWrite(OUT1,HIGH);
    digitalWrite(OUT2,HIGH);
    digitalWrite(OUT3,HIGH);
    digitalWrite(OUT4,HIGH);
    
    Serial.print("\nNetwork conected");
    Serial.print("\nIP address: ");
    Serial.println(WiFi.localIP());
    
  }

  void loop() { 
   
  }
