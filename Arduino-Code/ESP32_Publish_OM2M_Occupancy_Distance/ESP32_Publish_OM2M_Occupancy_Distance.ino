#include<HTTPClient.h>
#include <WiFi.h>
#include "time.h"

#define MAIN_SSID "Galaxy M21142D"
#define MAIN_PASS "tqqi2636"

#define CSE_IP      "192.168.125.197"
#define CSE_PORT    5089
#define HTTPS     false
#define OM2M_ORGIN    "admin:admin"
#define OM2M_MN     "/~/in-cse/in-name/"
#define OM2M_AE     "PID_control_of_DC_motor_speed"
#define OM2M_DATA_CONT  "Node-1/Data"


long randNumber;

HTTPClient http_om2m;
String ToSend = "[";



void setup() {
  Serial.begin(115200);
  WiFi.begin(MAIN_SSID,MAIN_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("#");
  }

  
}

void loop() {  
  //Sensor Block  BME 

 float distance=random(0,400);
 
  String data = String(distance);
  if(ToSend != "[")
    ToSend = ToSend + ", " + data;
  else
    ToSend = ToSend + data;



  if(push_now() == true) 
  {
    ToSend = ToSend + "]";
    Push_oneM2M(ToSend);
    ToSend = "[";
  }



  delay(100);
}


void Push_oneM2M(String data)
{
  static int i=0;

  String server="http://" + String() + CSE_IP + ":" + String() + CSE_PORT + String()+OM2M_MN;

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