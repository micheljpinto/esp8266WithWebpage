/******* Author: Eng. Michel de Jesus Pinto *******/
/*************** Date:  04/2020********************/

/* Webserver using microcontroler esp8266, developed with Arduino framework system on VsCode Editor (recomended for best visualization code). This code generate one webpage, acessed in the choiced ipadress, and using json protocol for better compatibility with javascript   */

//SYSTEM INCLUDES
  #include <ESP8266WiFi.h> 
  #include <MySQL_Connection.h>
  #include <MySQL_Cursor.h>            
  #define ARDUINOJSON_ENABLE_ARDUINO_STRING 1
  #include <ArduinoJson.h> // last version of 5.XX 

//Webserver Includes
  #include <ESPAsyncWebServer.h>
  #include <ESPAsyncTCP.h>
  #include "index.h"

//


/**********************************************SYS MACROS AND DEFINES**************************************************/

  //#define MODEAP              //If define the wifi settings working AP mode, if not STATIO mode 
  #define DEBUG                 //Compila ou não o serial print, para visualizacao das saídas dos sistema "debug"
    
  #define REQUEST_TIME_SQL 120000   //tempo de requisição do servidor em milisegundos
  
  #define QTDE_OF_SENSORS 3
  //#define WRITE_SQL_ACTUATORS 
  #define SERIAL_SPEED 230400

/**********************************************************************************************************************/  

/***************************************** DATA OF CONECTIONS**************************************/
  #define MY_STATIC_IP    192,168,1,154   // adjust for your config //  
  #define SERVER_IP       192,168,1,1     // adjust for your config //  
  #define SUBNET_MASK     255,255,255,0   // adjust for your config // 
  #define SERVER_SQL_IP    192,168,1,64   // adjust for your config // 

  #define SSID       "default0"           // adjust for your config // 
  #define PASSWORD  "@hfj0601"            // adjust for your config // 

  #define USER_DB      "michel"           // adjust for your config // 
  #define PASSWORD_DB  "mich1983"         // adjust for your config // 


/*************************************** END DATA OF CONECTIONS************************************/

/***************************************PROTOTYPES OF FUNCTIONS************************************/

  //WEBSERVER
  void setupWebServer();
  void deserealizeJsonReceivingActuatorsStatetWrite(String);
  //MYSQL
  void setupSQL();
  void sqlUpdateStatus(int actuator, int status);
  
   

/*************************************END PROTOTYPE FUNCTIONS***************************************/

/**************************************************VARIABLES****************************************/
  //VARIAVEIS SQL
  char userDB[] =       USER_DB;                  
  char passwordDB[] =   PASSWORD_DB;   
  //char* INSERT_SQL = "INSERT INTO begreen.mesurement ( SensorID, VALUE) VALUES ( '1', '1')";
  IPAddress server_addr(SERVER_SQL_IP);     
  WiFiClient client;
  WiFiClient client2;
  MySQL_Connection conn(&client2);
  MySQL_Cursor* cursor;
  AsyncWebServer server(80); 


/************************************************CONFIG WIFI*********************************************************/
    
    #ifdef MODEAP
      
      const char* ssid = "espTeste";        
      const char* password =  "12345678";   

      //if network using microcontroler that acess point
      
      void setupWithAP() {

        WiFi.mode(WIFI_AP);
        WiFi.softAP(ssid, password);
        IPAddress myIP = WiFi.softAPIP();
        //WiFi.begin();
        Serial.print("Endereco IP do Acess Point: ");
        Serial.println(myIP);
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
        Serial.println("Sistema inativo enquanto rede nao se conectar...");
        Serial.println("Conectando a rede WIFI...");
        
        while (WiFi.status() != WL_CONNECTED) {
          delay(500);
          Serial.print(".");
        }
        Serial.print("Estação conectada ao IP: ");
        Serial.println(WiFi.localIP());
    
      }
    #endif
                    

/**************************************************END WIFI**********************************************************/

/************************************************* SQL*********************************************************/
   
  void setupSQL(){
    Serial.print("Conectando ao servidor MySQL...  \n\r");

    if (conn.connect(server_addr, 3306, userDB, passwordDB)){
      Serial.println("Link com banco de dados OK.");
    }   
    else
      Serial.println("Link com banco de dados falhou");
      Serial.println(".");
    conn.close();
  }

  void sqlUpdateStatus(int actuator, int status)
  {
    // Sample query
    char INSERT_DATA[] = "UPDATE teste.tab1 SET STATUS=%d where POS=%d";
    char query[100]; 
    sprintf(query, INSERT_DATA, status, actuator );
    // Execute the query
    MySQL_Cursor *cursor= new MySQL_Cursor(&conn);
    cursor->execute(query);
    
  }
      
/**************************************************END SQL**********************************************************/


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
      Serial.println(stream);
      deserealizeJsonReceivingActuatorsStatetWrite(stream);
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
    //Substituir essa rotina por IOs 
    digitalWrite(LED_BUILTIN,!digitalRead(LED_BUILTIN));
    if(conn.connected())
      sqlUpdateStatus(1,0);
     
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
    #ifdef WRITE_SQL_ACTUATORS
      sendEepromReadsToDatabase();
    #endif
    setupSQL();
    pinMode(LED_BUILTIN,OUTPUT);
    Serial.println("\nConectado a Rede");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    
  }

  void loop() {
    
   
  }
