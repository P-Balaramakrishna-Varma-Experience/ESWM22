/*
  Rui Santos
  Complete project details at Complete project details at https://RandomNerdTutorials.com/esp32-http-get-post-arduino/

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/


#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>

const char* ssid = "Galaxy M21142D";
const char* password = "tqqi2636";

//Your Domain name with URL path or IP address with path
const char* serverName = "http://192.168.125.197:4000";


unsigned long lastTime = 0;
unsigned long timerDelay = 5000;


double k_p = 2;
double k_i = 5;
double k_d = 1;
bool run = false;



double rpm_sensor_reading, output, desired = 10;
unsigned long previousTime;
double lastError;
double cumError;
 


void setup() {
  //intializing serial monitor
  Serial.begin(115200);

  //Connecting to wifi
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  desired = 0;                          //set point at zero degreesd
}

void loop() {
  //Send an HTTP GET request every 5 seconds
  if ((millis() - lastTime) > timerDelay) {
    if(WiFi.status() == WL_CONNECTED){
              
      String user_params = httpGETRequest("/");
      Serial.println(user_params);
      JSONVar parameters = JSON.parse(user_params);
  
      // JSON.typeof(jsonVar) can be used to get the type of the var
      if (JSON.typeof(parameters) == "undefined") {
        Serial.println("Parsing rpm_sensor_reading failed!");
        return;
      }
    
      Serial.print("JSON object = ");
      Serial.println(parameters);
      setGlobalState(parameters);
      PrintGlobalState();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }


  //rpm_sensor_reading = analogRead(A0);                //read from rotary encoder connected to A0
  //output = computePID(rpm_sensor_reading);
  //delay(100);
  //analogWrite(3, output);  
}

//rpm_sensor_reading the "/" "/del"
String httpGETRequest(const char* endpoint) {
  WiFiClient client;
  HTTPClient http;
  String payload = "{}"; 
    
  String base = serverName;
  String path = endpoint;  
  http.begin(client, base + path);
  int httpResponseCode = http.GET();
  
  if (httpResponseCode == 200) {
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }

  http.end();
  return payload;
}

double computePID(double inp){     
  double currentTime = millis();                //get current time
  double elapsedTime = (double)(currentTime - previousTime);        //compute time elapsed from previous computation
        
  double error = desired - inp;                                // determine error
  cumError += error * elapsedTime;                // compute integral
  double rateError = (error - lastError)/elapsedTime;   // compute derivative
 
  double out = k_p*error + k_i*cumError + k_d*rateError;                //PID output               
 
  lastError = error;                                //remember current error
  previousTime = currentTime;                        //remember current time
 
  return out;                                        //have function return the PID output
}

// messed up json structure
void setGlobalState(JSONVar state)
{
  k_p =  state["data"][0]["k_p"];
  k_d = state["data"][0]["k_d"];
  k_i = state["data"][0]["k_i"];
  run = state["data"][0]["run"];
  desired = state["data"][0]["desired"];   
}

void PrintGlobalState()
{
  Serial.print(k_p);
  Serial.print(",");
  Serial.print(k_d);
  Serial.print(",");
  Serial.print(k_i);
  Serial.print(",");
  Serial.print(desired);
  Serial.print(",");
  Serial.print(run);
  Serial.print(",");  
  Serial.println(" ");
}