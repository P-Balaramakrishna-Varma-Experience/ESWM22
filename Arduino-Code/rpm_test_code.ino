#include <Timer.h>
Timer t;

#define number_of_holes 20
#define interruptPin 13
int i = 0;

void setup() {
  pinMode(interruptPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(interruptPin), counter, RISING);
  Serial.begin(9600);
  t.every(500, print_speed);
}

void loop() {
  t.update();
}


void print_speed()
{
  Serial.print("Speed:  ");
  Serial.print(i * 120 / number_of_holes);
  Serial.println(" rpm");
  i = 0;
}

void counter() {
  i++;
}