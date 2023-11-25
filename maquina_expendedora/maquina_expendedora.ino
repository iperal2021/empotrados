#include <LiquidCrystal.h>
#include "TimerOne.h"
#include <Thread.h>
#include <ThreadController.h>
#include <StaticThreadController.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <avr/wdt.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

int TRIGGER = 10;
int ECHO = 7;
int LED_RED = A0;
int LED_GREEN = 6;
int BUTTON = 8;
int JS_X = A2;
int JS_Y = A1;
int JS_DIGITAL = 13;
int TH = 9;

long duration;
int distance;
unsigned long tiempoTranscurrido;
static unsigned long tiempoInicio;

int X_axix;
int Y_axix;

int STATE = 0;
float price_list[5] = {1.00, 1.10, 1.25, 1.50, 2.00};
String cafe_list[5] = {"CAFE SOLO", "CAFE CORTADO", "CAFE DOBLE", "CAFE PREMIUM", "CHOCOLATE"};
String admin_list[4] = {"Ver Temperatura", "Ver Distancia Sensor", "Ver Contador", "Modificar Precios"};

int randNumber;

void cafe_menu() {
  lcd.setCursor(0,0);
  lcd.print(cafe_list[STATE]);
  lcd.setCursor(0,1);
  lcd.print(price_list[STATE]);
}

void admin_menu() {
  while (true)
  {
    analogWrite(LED_GREEN, 255);
    analogWrite(LED_RED, 255);
    X_axix = analogRead(JS_X);
    Y_axix = analogRead(JS_Y);
    Serial.println(Y_axix);
    if(X_axix > 500){
      STATE +=1;
      lcd.clear();
      if (STATE == 4){
        STATE = 0;
      }
    }else if (X_axix < 480){
      STATE -=1;
      lcd.clear();
      if (STATE == -1){
        STATE = 3;
      }
    }

    lcd.setCursor(0,0);
    lcd.print(admin_list[STATE]);

    if (digitalRead(JS_DIGITAL) == LOW){
      switch (STATE) {
        case 0:
          //show_temperature();
          break;
        case 1:
          //distance();
          break;
        case 2:
          timer();
          break;
        case 3:
          prices();
          break;
      }
    }
    /*
    if (Y_axix < 400)
    {
      return;
    }
    */
  }
}

/*
void show_temperature(){

}

void distance(){

}
*/
void timer(){
  lcd.clear();
  while (true)
  {
    X_axix = analogRead(JS_X);
    Y_axix = analogRead(JS_Y);
    lcd.setCursor(0,0);
    tiempoTranscurrido = millis() - tiempoInicio;
    lcd.print(tiempoTranscurrido);
    delay(100);
    if (Y_axix < 400)
    {
      return;
    }
  }
}

void prices(){
  STATE = 0;
  lcd.clear();
  while (true)
  {
    X_axix = analogRead(JS_X);
    Y_axix = analogRead(JS_Y);
    if(X_axix > 500)
    {
      STATE +=1;
      if (STATE == 5){
        STATE = 0;
      }
    } else if (X_axix < 480)
    {
      STATE -=1;
      if (STATE == -1)
      {
        STATE = 4;
      }
    }
    lcd.print(price_list[STATE]);

    if (digitalRead(JS_DIGITAL) == LOW)
    {
      price_change();
    }
    
    if (Y_axix < 400)
    {
      return;
    }
  }
}

void price_change()
{
  while (true)
  {
    X_axix = analogRead(JS_X);
    Y_axix = analogRead(JS_Y);
    lcd.setCursor(0,0);
    lcd.print("Nuevo Precio:");
    lcd.setCursor(0,1);
    lcd.print(price_list[STATE]);
    if(X_axix > 500)
    {
      price_list[STATE] += 0.05;
    } else if (X_axix < 480)
    {
      price_list[STATE] -= 0.05;
    }
    if (Y_axix < 400)
    {
      return;
    }
  }
  
}

void serving() {
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

void setup() {
  // put your setup code here, to run once:
  tiempoInicio = millis();
  attachInterrupt(digitalPinToInterrupt(BUTTON), admin_menu, RISING);

  Serial.begin(9600);
  lcd.begin(16,2);

  pinMode(TRIGGER, OUTPUT);
  pinMode(ECHO, INPUT);

  pinMode(LED_RED,OUTPUT);
  pinMode(LED_GREEN,OUTPUT);
  pinMode(BUTTON, INPUT);
  pinMode(JS_DIGITAL, INPUT_PULLUP);
  pinMode(BUTTON, INPUT_PULLUP);


  lcd.print("Cargando...");
  for (int i = 0; i < 3; i++){
    analogWrite(LED_RED,255);
    delay(1000);
    analogWrite(LED_RED,0);
    delay(1000);
  }
  lcd.clear();
}

void loop() {
  // put your main code here, to run repeatedly:
  /*
  digitalWrite(TRIGGER, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER, LOW);
  duration = pulseIn(ECHO, HIGH);
  distance = duration * 0.034 / 2;
  */
  X_axix = analogRead(JS_X);
  Y_axix = analogRead(JS_Y);
  
  if(X_axix > 500){
    STATE +=1;
    lcd.clear();
    if (STATE == 5){
      STATE = 0;
    }
  }else if (X_axix < 480){
    STATE -=1;
    lcd.clear();
    if (STATE == -1){
      STATE = 4;
    }
  }

  if (digitalRead(JS_DIGITAL) == LOW){
    serving();
  }
  admin_menu();

}
