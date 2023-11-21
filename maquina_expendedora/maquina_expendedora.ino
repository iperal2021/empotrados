#include <LiquidCrystal.h>
#include "TimerOne.h"
#include <Thread.h>
#include <ThreadController.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>


LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

int TRIGGER = 10;
int ECHO = 7;
int LED_RED = A0;
int LED_GREEN = 6;
int BUTTON = 10;
int JS_X = A2;
int JS_Y = A1;
int JS_DIGITAL = 13;
int TH = 9;

long duration;
int distance;

int X_axix;
int Y_axix;

void setup() {
  // put your setup code here, to run once:
  lcd.begin(16,2);

  pinMode(TRIGGER, OUTPUT);
  pinMode(ECHO, INPUT);

  pinMode(LED_RED,OUTPUT);
  pinMode(LED_GREEN,OUTPUT);
  pinMode(BUTTON, INPUT);

  lcd.print("Cargando...");
  for (int i = 0; i < 3; i++){
    analogWrite(LED_RED,255);
    delay(1000);
    analogWrite(LED_RED,0);
    delay(1000);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(TRIGGER, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER, LOW);
  duration = pulseIn(ECHO, HIGH);
  distance = duration * 0.034 / 2;
 
  X_axix = analogRead(JS_X);
  Y_axix = analogRead(JS_Y);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Servicio");

}
