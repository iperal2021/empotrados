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

int STATE = 0;
float price_list[5] = {1.00, 1.10, 1.25, 1.50, 2.00};
String cafe_list[5] = {"CAFE SOLO", "CAFE CORTADO", "CAFE DOBLE", "CAFE PREMIUM", "CHOCOLATE"};

int randNumber;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  lcd.begin(16,2);

  pinMode(TRIGGER, OUTPUT);
  pinMode(ECHO, INPUT);

  pinMode(LED_RED,OUTPUT);
  pinMode(LED_GREEN,OUTPUT);
  pinMode(BUTTON, INPUT);
  pinMode(JS_DIGITAL, INPUT_PULLUP);

  lcd.print("Cargando...");
  for (int i = 0; i < 3; i++){
    analogWrite(LED_RED,255);
    delay(1000);
    analogWrite(LED_RED,0);
    delay(1000);
  }
}

void cafe_menu() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(cafe_list[STATE]);
  lcd.setCursor(0,1);
  lcd.print(price_list[STATE]);
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

  if(X_axix > 500){
    STATE +=1;
    if (STATE == 5){
      STATE = 0;
    }
  }else if (X_axix < 480){
    STATE -=1;
    if (STATE == -1){
      STATE = 4;
    }
  }

  if (digitalRead(JS_DIGITAL) == LOW){
    lcd.clear();
    randNumber = random(4, 8);
    lcd.print("Sirviendo...");
    for (int i = 1; i< 255; i++){
      analogWrite(LED_GREEN, i);
      delay(randNumber*1000/253);
    }
    analogWrite(LED_GREEN, 0);
    lcd.clear();
    lcd.print("RETIRE BEBIDA");
    delay(3000);
  }
  cafe_menu();

}
