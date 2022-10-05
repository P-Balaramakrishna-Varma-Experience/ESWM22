#include <arduino-timer.h>

#define number_of_holes 20
#define interruptPin 13


// Motor
int motor1Pin1 = 27;
int motor1Pin2 = 25;
int enable1Pin = 14;

const int freq = 5000;
const int pwmChannel = 0;
const int resolution = 8;
int motor_speed;


//RPM sensor
int current_speed = 0;
int sample_time = 1000;
int spokes = 0;


auto timer = timer_create_default();


void setup()
{
    //RPM sensor
    pinMode(interruptPin, INPUT);
    attachInterrupt(digitalPinToInterrupt(interruptPin), counter, RISING);
    timer.every(sample_time, Measure_rpm);

    //Motor    
    pinMode(motor1Pin1, OUTPUT);
    pinMode(motor1Pin2, OUTPUT);
    pinMode(enable1Pin, OUTPUT);
    ledcSetup(pwmChannel, freq, resolution);
    ledcAttachPin(enable1Pin, pwmChannel);
    digitalWrite(motor1Pin1, HIGH);
    digitalWrite(motor1Pin2, LOW);

    Serial.begin(115200);
    Serial.print("Testing DC Motor...");
}

void loop()
{
    timer.tick(); 
    if(Serial.available())
    {
        motor_speed = (Serial.readString()).toInt();  
    }
    ledcWrite(pwmChannel, motor_speed);
        
    Serial.println("speed: "  + String(current_speed));
    Serial.println("duty cycles" + String(motor_speed));
    delay(sample_time);
}

bool Measure_rpm(void *)
{
    current_speed = (spokes * (60000/sample_time)) / number_of_holes;
    spokes = 0;
    return true;
}

void counter()
{
    spokes++;
}