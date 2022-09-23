/*
  Rui Santos
  Complete project details at Complete project details at https://RandomNerdTutorials.com/esp32-http-get-post-arduino/

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

/*
Code components
- thingsspeak (onem2m) [data publishing] (onem2m working integrate)
- Rpm sensor (recheck and try sudheer code)
- Motor Actuator (recheck and try sudheer code)
- PID logic (not working -- logically correct ??)
- Update of gloabal state remotely (tested by printing the values of global)
*/

#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include "ThingSpeak.h"
#include <arduino-timer.h>

//Wifi credentials to log in
const char* ssid = "Galaxy M21142D";
const char* password = "123456789";

//Your Domain name with URL path or IP address for backend
const char* serverName = "http://192.168.68.197:4000";

//Things speak credentials
WiFiClient  things_speak_client;
unsigned long myChannelNumber = 1848016;
const char * myWriteAPIKey = "V7RRRSX51528RIVP";
HTTPClient http_om2m;
String ToSend = "[";




//Global state
//user parameters
double k_p = 2, k_i = 5, k_d = 1, desired = 10;
bool run = false;
//system keep track of 
double lastError = 0, cumError = 0;
unsigned long previousTime = 0;


//RPM sensor
const int rpm_sampling_time = 500; //milliseconds
const int interruptPin = 13;
auto timer = timer_create_default();
int spokes= 0;


// Motor 
int motor1Pin1 = 27;
int motor1Pin2 = 25;
int enable1Pin = 14;
const int freq = 5000;
const int pwmChannel = 0;
const int resolution = 8;
int motor_speed_to_set = 255, motor_speed_read = 0;

 


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
  ThingSpeak.begin(things_speak_client);  // Initialize ThingSpeak


  // rpm senor pin, setup.
  pinMode(interruptPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(interruptPin), counter, RISING);
  timer.every(rpm_sampling_time, get_cur_speed);

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
  Update_state(); 
  timer.tick();  
  //Serial.println("current speed" + String(motor_speed_read));
  

  if(run == true)
  {
    String data = String(motor_speed_read);
    if(ToSend != "[")
      ToSend = ToSend + ", " + data;
    else
      ToSend = ToSend + data;
    //Serial.println("data : " + ToSend);
  }

  motor_speed_to_set = rpm_duty(computePID(motor_speed_read));
  Serial.println("rpm: " + String(motor_speed_read) + ", dutyCylcles: " + String(rpm_duty(motor_speed_read)));

  ledcWrite(pwmChannel, motor_speed_to_set);
  delay(100);
}



double computePID(double inp){     
  //PerviousTime and LastError and cullilateive error  are intialized to 0 in start.
  //They are set to 0 when run is set to false.
  //Every iteration of loop when run is true previousTime and Last error are updated by computePID.
  if(previousTime != 0 && lastError != 0)
  {  
    unsigned long int currentTime = millis();                //get current time
    //Serial.println("Time lag: " + String(currentTime - previousTime));

    double elapsedTime = (double)(currentTime - previousTime) / 60000;        //compute time elapsed from previous computation
   
    double error = desired - inp;                                // determine error
    cumError += error * elapsedTime;                // compute integral
    double rateError = (error - lastError)/elapsedTime;   // compute derivative
  
    double out = k_p*error + k_i*cumError + k_d*rateError;                //PID output               
    lastError = error;                                //remember current error
    previousTime = currentTime;                        //remember current time
    //Serial.println("Error: " + String(error) + ", cumError: " + String(cumError) + ", rateError: " + String(rateError));
    //Serial.println("P: " +  String(error * k_p) + ", I: " + String(cumError * k_i) + ", D: " + String(rateError * k_d));
    return out;                                        //have function return the PID output
  }
  else
  {
    lastError = desired - inp;
    previousTime = millis();
    cumError = 0;
    return desired;
  }
}

int rpm_duty(double rpm)
{
  int ans = (rpm * 255/720);
  if(ans > 255)
    return 255;
  else if(ans < 0)
    return 0;
  else
    return ans;
}

void Update_state()
{
  static unsigned long int lastTime = 0;
  static unsigned long int timerDelay = 30000;

  if ((millis() - lastTime) > timerDelay)
  {
    if(WiFi.status() == WL_CONNECTED)
    {
      String user_params = httpGETRequest("/");
      JSONVar parameters = JSON.parse(user_params);
  
      // JSON.typeof(jsonVar) can be used to get the type of the var
      if (JSON.typeof(parameters) == "undefined") {
        Serial.println("Parsing rpm_sensor_reading failed!");
        return;
      }
    
      setGlobalState(parameters);
      PrintGlobalState();
    }
    else 
      Serial.println("WiFi Disconnected");

    lastTime = millis();
  }
}

String httpGETRequest(const char* endpoint) {
  WiFiClient client;
  HTTPClient http;
    
  String base = serverName;
  String path = endpoint;  
  http.begin(client, base + path);
  int httpResponseCode = http.GET();
  
  if (httpResponseCode == 200)
  {
    String payload = http.getString();
    http.end();
    return payload;
  }
  else
  {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
    http.end();
    return "not json parsable string";
  }
}

void setGlobalState(JSONVar state)
{
  if(run == true && state["data"][0]["run"] == false)
  {
    ToSend = ToSend + "]";
    Push_oneM2M(ToSend);
    //Serial.println(ToSend);
    ToSend = "[";
  }

  k_p =  state["data"][0]["k_p"];
  k_d = state["data"][0]["k_d"];
  k_i = state["data"][0]["k_i"];
  run = state["data"][0]["run"];
  desired = state["data"][0]["desired"];   
  if(run == false)
  {
    cumError = 0;
    lastError = 0;
    previousTime = 0;
  }
}

void PrintGlobalState()
{
  Serial.print("Gobal state: ");
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

  Serial.print(cumError);
  Serial.print(",");

  Serial.print(lastError);
  Serial.print(",");

  Serial.print(previousTime);
  Serial.println(" ");
}


//The data is stored in motor_speed_read
bool get_cur_speed(void *)
{
  //motor_speed_read (essence of rpm module)
  //it update motor_speed_read every (rpm_sampling time (frequency))
  static int number_of_holes = 20;
  motor_speed_read = (spokes* (60000/rpm_sampling_time)) / number_of_holes;
  spokes= 0;
  return true;
}

void counter()
{
  spokes++;
}

void Push_oneM2M(String data)
{
  static int i=0;
  const String CSE_IP = "192.168.68.197";
  const int CSE_PORT = 5089;
  const bool HTTPS = false;
  const String OM2M_ORGIN = "admin:admin";
  const String OM2M_MN = "/~/in-cse/in-name/";
  const String OM2M_AE = "PID_control_of_DC_motor_speed";
  const String OM2M_DATA_CONT =  "Node-1/Data";

  String server="http://" + String() + CSE_IP + ":" + String() + CSE_PORT + String()+OM2M_MN;
  //String server="https://" + String() + CSE_IP +  String()+OM2M_MN;
  
  //Serial.println(data);
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
  //Serial.println(req_data);
  int code = http_om2m.POST(req_data);
  http_om2m.end();
  Serial.println(code);
}

bool push_now()
{
  static unsigned long int last_pushed, next_push_after;
  static bool init = false;
  if(init == false)
  {
    last_pushed = millis();
    next_push_after = 10000;
    //next_push_after = 32000;
    init = true;
  }

  unsigned long int current_time = millis();
  if(current_time - last_pushed > next_push_after)
  {
    last_pushed = current_time;
    return true;
  }
  else
  {
    return false;
  }

}