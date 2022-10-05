// Motor A
int motor1Pin1 = 27;
int motor1Pin2 = 25;
int enable1Pin = 14;

int sensor = 13;
unsigned long start_time = 0;
unsigned long end_time = 0;
int steps = 0;
float steps_old = 0;
float temp = 0;
float rps = 0;
float rpm = 0;
float error = 0;

// Setting PWM properties
const int freq = 30000;
const int pwmChannel = 0;
const int resolution = 8;
int dutyCycle = 255;
float consspeed = 1000;
float voltage = 4.26;
float area = 0, currtime = millis(), prevtime = millis();
float preverror = 0;
float var = 0;

#include <WiFi.h>
double Setpoint, Input, Output;

// Specify the links and initial tuning parameters
double Kp = 0.3, Ki = 0.005, Kd = 5;

#include "ThingSpeak.h"

const char *ssid = "Galaxy M21142D";   // your network SSID (name)
const char *password = "123456789"; // your network password

WiFiClient client;

unsigned long myChannelNumber = 1848145;
const char *myWriteAPIKey = "GV7P12GFV84Y7NYE";

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 30000;

float prop()
{
    error = consspeed - rpm;
    float voltage = Kp * error;
    return voltage;
}
float integral()
{
    error = consspeed - rpm;
    float dt = currtime - prevtime;
    dt = dt / 1000;
    area = area + (error * dt);
    area = Ki * area;
    Serial.print("Area : ");
    Serial.println(area);
    return area;
}
float diff()
{
    float differror = consspeed - rpm - preverror;
    float dt = currtime - prevtime;
    Serial.print("DIFFERROR :");
    Serial.println(differror * Kd / dt);
    float slope = differror * Kd / dt;
    error = consspeed - rpm;
    return slope;
}
void setup()
{
    // sets the pins as outputs:
    pinMode(motor1Pin1, OUTPUT);
    pinMode(motor1Pin2, OUTPUT);
    pinMode(enable1Pin, OUTPUT);

    // configure LED PWM functionalitites
    ledcSetup(pwmChannel, freq, resolution);

    // attach the channel to the GPIO to be controlled
    ledcAttachPin(enable1Pin, pwmChannel);
    ledcWrite(pwmChannel, 170);
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);

    ThingSpeak.begin(client); // Initialize ThingSpeak

    // testing
    //  Serial.print("Testing DC Motor...");

    // lcd.begin(16, 2);
    pinMode(sensor, INPUT_PULLUP);
    // lcd.setCursor(0,0);
    //    Serial.print(" STEPS - 0");
    // lcd.setCursor(0,1);
    //    Serial.print(" RPS   - 0.00");

    digitalWrite(motor1Pin1, LOW);
    digitalWrite(motor1Pin2, HIGH);
    delay(2000);

    if (WiFi.status() != WL_CONNECTED)
        {
            Serial.print("Attempting to connect");
            while (WiFi.status() != WL_CONNECTED)
            {
                WiFi.begin(ssid, password);
                delay(5000);
            }
            Serial.println("\nConnected.");
        }
    
}

void loop()
{

    preverror = error;

    // Move the DC motor forward at maximum speed

    // ledcWrite(pwmChannel, dutyCycle );
    // delay(100);

    start_time = millis();
    end_time = start_time + 1000;
    while (millis() < end_time)
    {
        if (digitalRead(sensor))
        {
            steps = steps + 1;
            while (digitalRead(sensor))
                ;
        }
    }
    temp = steps - steps_old;
    steps_old = steps;
    rps = temp / 20;

    rpm = rps * 60;
    Serial.print("rpm : ");
    Serial.println(rpm);
    currtime = millis();
    voltage = voltage + prop() + integral() + diff();
    float a = 255 * voltage / 12;
    if (a > 255.00)
    {
        a = 255;
    }
    float curdutycycle = max(a, var);
    Serial.print("DutyCycle : ");
    Serial.println(curdutycycle);
    Serial.print("Error : ");
    Serial.println(error);
    ledcWrite(pwmChannel, curdutycycle);
    prevtime = currtime;
    // preverror = error;

    //int x = ThingSpeak.writeField(myChannelNumber, 1, rpm, myWriteAPIKey);

    if (200!= 200)
    {
        Serial.println("Channel update successful.");
    }
    //    else{
    //      Serial.println("Problem updating channel. HTTP error code " + String(x));
    //    }

    //    delay(1000);
}