/******* Author: Automation and C. Engineering, Michel de Jesus Pinto *******/
/********************** Date:  04/2020 **************************************/
/*********************** micheljpinto@gmail.com *****************************/

/* Webserver using microcontroler esp8266, developed with Arduino 
framework system on VsCode Editor (recomended for best visualization
code). This code generate an webpage, acessed in the choiced ipadress,
and using json protocol for better compatibility with javascript   */

//SYSTEM INCLUDES
  #include <ESP8266WiFi.h>  
  #define ARDUINOJSON_ENABLE_ARDUINO_STRING 1
  #include <ArduinoJson.h> // version use in project of 5.13.5 
  #include <SHT31.h>
  #include<BH1750.h>
//Webserver Includes
  #include <ESPAsyncWebServer.h>
  #include <ESPAsyncTCP.h>
  #include "index.h"
/************************************SYS MACROS AND DEFINES****************************************/
  //#define MODEAP                //If define the wifi settings working AP mode, if not STATIO mode 
  #define SERIAL_SPEED 230400
  //IMPORTANT! Adjust OUT according, the pin of ucontroler desired for to be controlled
  
  #define SDA  0
  #define SCL  2
  int OUTPUTS[4]= {16,5,4,14};
  #define QT_RELAYS 4 //quantity of realays.

/**************************************************************************************************/  

/*************************************** DATA OF CONECTIONS****************************************/
  #define MY_STATIC_IP    192,168,0,100   // adjust for your config //  
  #define SERVER_IP       192,168,0,254     // adjust for your config //  
  #define SUBNET_MASK     255,255,255,0   // adjust for your config // 
  #define SSID            "default0"      // adjust for your config // 
  #define PASSWORD        "@hfj0601"      // adjust for your config // 
/************************************** END DATA OF CONECTIONS*************************************/

/***************************************PROTOTYPES OF FUNCTIONS************************************/
  //WEBSERVER
  void setupWebServer();
  String parserJsonActuatorWrite(String);
  int convertChar(String);
  String returnStates();
  String returnHumidityLightTemperature();

/*************************************END PROTOTYPE FUNCTIONS***************************************/

/**************************************************VARIABLES****************************************/
  //WiFiClient client;
  AsyncWebServer server(80); 
  SHT31 sht;
  BH1750 lightMeter;
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
    
    server.on("/returnsensors", HTTP_GET, [](AsyncWebServerRequest *request){
      //String resp= returnHumidityLightTemperature().c_str;
      
      request->send(200, "json/text", returnHumidityLightTemperature());
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
        temp=OUTPUTS[0];
      }
      else if(aId=="OUT2"){
        temp=OUTPUTS[1];
      }
      else if(aId=="OUT3"){
        temp=OUTPUTS[2];
      } else {
        temp=OUTPUTS[3];
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
    actuators_0["status"] = !digitalRead(OUTPUTS[0]);
    
    JsonObject& actuators_1 = actuators.createNestedObject();
    actuators_1["id"] = "OUT2";
    actuators_1["status"] = !digitalRead(OUTPUTS[1]);

    JsonObject& actuators_2 = actuators.createNestedObject();
    actuators_2["id"] = "OUT3";
    actuators_2["status"] = !digitalRead(OUTPUTS[2]);

    JsonObject& actuators_3 = actuators.createNestedObject();
    actuators_3["id"] = "OUT4";
    actuators_3["status"] = !digitalRead(OUTPUTS[3]);

    String lastStates="";
    root.printTo(lastStates);
    return lastStates;
  }

  String returnHumidityLightTemperature(){
    //DEBUG
    //Serial.println(lightMeter.readLightLevel());
    sht.read(0);         // default = true/fast       slow = falsedelay(10);
    //Serial.println(sht.getTemperature());
    //Serial.println(sht.getHumidity());
    //END DEBUG
    const size_t capacity = JSON_OBJECT_SIZE(4);
    DynamicJsonBuffer jsonBuffer(capacity);

    JsonObject& root = jsonBuffer.createObject();
    root["sensor"] = 1;
    root["temp"] = sht.getTemperature();
    root["hum"] = sht.getHumidity();
    root["lum"] = lightMeter.readLightLevel();
    
    String ret="";
    root.printTo(ret);
    return ret;
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

    for(int cont=0 ; cont<QT_RELAYS;cont++){
      pinMode(OUTPUTS[cont],OUTPUT); //Configure
      digitalWrite(OUTPUTS[cont],HIGH);
    }
    
  
    // CONFIGURE WIRE SENSORS   
    Wire.begin(SDA,SCL);
    //Wire.setClock(100000);
    //Start sensor of temperature, humidity and light
    sht.begin(0x44,&Wire); 
    lightMeter.begin(BH1750::ONE_TIME_LOW_RES_MODE, 0x23,&Wire); 
    //Serial feedbacks
    Serial.print("\nNetwork conected");
    Serial.print("\nIP address: ");
    Serial.println(WiFi.localIP());
  
  }

  void loop() { 
   
  }
