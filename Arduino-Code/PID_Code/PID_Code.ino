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
#include "ThingSpeak.h" // always include thingspeak header file after other header files and custom macros
#include <Timer.h>

const char* ssid = "Redmi";
const char* password = "e2cbcdb98369";

//Your Domain name with URL path or IP address with path
const char* serverName = "http://192.168.43.197:4000";


unsigned long lastTime = 0;
unsigned long timerDelay = 20000;


double k_p = 2;
double k_i = 5;
double k_d = 1;
bool run = false;


double rpm_sensor_reading, output, desired = 10;
unsigned long previousTime;
double lastError;
double cumError;


//Things speek variables
WiFiClient  client;
unsigned long myChannelNumber = 1848016;
const char * myWriteAPIKey = "V7RRRSX51528RIVP";

//OneM2M global variables
#define MAIN_SSID "Galaxy M21142D"
#define MAIN_PASS "tqqi2636"

#define CSE_IP      "192.168.125.197"
#define CSE_PORT    5089
#define HTTPS     false
#define OM2M_ORGIN    "admin:admin"
#define OM2M_MN     "/~/in-cse/in-name/"
#define OM2M_AE     "PID_control_of_DC_motor_speed"
#define OM2M_DATA_CONT  "Node-1/Data"

const char* ntpServer = "pool.ntp.org";
unsigned long epochTime; 
long randNumber;

HTTPClient http_om2m;





Timer t;
int i = 0;
const int number_of_holes = 20;
const int interruptPin = 13;

// Motor A
int motor1Pin1 = 27;
int motor1Pin2 = 25;
int enable1Pin = 14;

// Setting PWM properties
const int freq = 5000;
const int pwmChannel = 0;
const int resolution = 8;
int motor_speed_to_set, motor_speed_read;

 


void setup() {
  //intializing serial monitor
  Serial.begin(115200);
  configTime(0, 0, ntpServer);


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
  ThingSpeak.begin(client);  // Initialize ThingSpeak


  // rpm senor pin, setup.
  pinMode(interruptPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(interruptPin), counter, RISING);
  t.every(500, get_cur_speed);

  //Motor declaring and setting pins.
  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(enable1Pin, OUTPUT);
  ledcSetup(pwmChannel, freq, resolution);
  ledcAttachPin(enable1Pin, pwmChannel);
  digitalWrite(motor1Pin1, HIGH);
  digitalWrite(motor1Pin2, LOW);
  ledcWrite(pwmChannel, motor_speed_to_set);
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


  t.update();  
  motor_speed_read;
  if(run == true)
  {
    ThingSpeak.writeField(myChannelNumber, 1, motor_speed_read, myWriteAPIKey);
    Push_oneM2M(String(motor_speed_read));
  }

  motor_speed_to_set = computePID(motor_speed_read);
  ledcWrite(pwmChannel, motor_speed_to_set);
  delay(100);
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


void get_cur_speed()
{
    Serial.print("Speed:  ");
    Serial.print(i * 120 / number_of_holes);
    Serial.println(" rpm");
    motor_speed_read = i * 120 / number_of_holes;
    i = 0;
}

void counter()
{
    i++;
}


unsigned long getTime() 
{
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    //Serial.println("Failed to obtain time");
    return(0);
  }
  time(&now);
  return now;
}


void Push_oneM2M(String data)
{
  static int i=0;

  String server="http://" + String() + CSE_IP + ":" + String() + CSE_PORT + String()+OM2M_MN;

  Serial.println(data);
  http_om2m.begin(server + String() +OM2M_AE + "/" + OM2M_DATA_CONT + "/");

  http_om2m.addHeader("X-M2M-Origin", OM2M_ORGIN);
  http_om2m.addHeader("Content-Type", "application/json;ty=4");
  http_om2m.addHeader("Content-Length", "100");

  String label = "Node-1";

  String req_data = String() + "{\"m2m:cin\": {"

  + "\"con\": \"" + data + "\","

  + "\"rn\": \"" + "cin_"+String(i++) + "\"," 

  + "\"lbl\": \"" + label + "\","

  + "\"cnf\": \"text\""

  + "}}";

 //is rn andcnf needed?
  int code = http_om2m.POST(req_data);
  http_om2m.end();
  Serial.println(code);
}