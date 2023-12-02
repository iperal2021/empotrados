#include <LiquidCrystal.h>
#include <TimerOne.h>
#include <Thread.h>
#include <ThreadController.h>
#include <StaticThreadController.h>
#include <DHT.h>
#include <avr/wdt.h>

// Pines a utilizar
LiquidCrystal lcd(12, 11, 5, 4, 3, 8);
const int TRIGGER = 10;
const int ECHO = 7;
const int LED_RED = A0;
const int LED_GREEN = 6;
const int BUTTON = 2;
const int JS_X = A2;
const int JS_Y = A1;
const int JS_DIGITAL = 13;
const int DHT_PIN = 9;
#define DHTTYPE DHT11
DHT dht(DHT_PIN, DHTTYPE);

// Variables
int X_axix;
int Y_axix;
int temperature;
int humidity;
long duration;
long distance;
int STATE = 0;
int randNumber;
unsigned long tiempoInicio;
bool admin = false;
unsigned long buttonPressStartTime = 0;
unsigned long timeWhenPress = 0;
unsigned long timePressed = 0;
volatile bool isPress = false;

// Listas a usar durante el programa
float price_list[5] = { 1.00, 1.10, 1.25, 1.50, 2.00 };
String cafe_list[5] = { "CAFE SOLO", "CAFE CORTADO", "CAFE DOBLE", "CAFE PREMIUM", "CHOCOLATE" };
String admin_list[4] = { "Ver Temperatura", "Ver Distancia", "Ver Contador", "Modificar Precios" };

// Definimos los threads
ThreadController controller = ThreadController();
Thread ultrasonido = Thread();
Thread humedad = Thread();
Thread temperatura = Thread();
Thread contador = Thread();

// Menu que muestras las bebidas y su respectivo precio
void cafe_menu() {
  analogWrite(LED_GREEN, 0);
  analogWrite(LED_RED, 0);
  lcd.setCursor(0, 0);
  lcd.print(cafe_list[STATE]);
  lcd.setCursor(0, 1);
  lcd.print(price_list[STATE]);
}

// Función de lectura del Joystick para moverse por los distintos menús
void menu_control(int a, int b) {
  X_axix = analogRead(JS_X);
  Y_axix = analogRead(JS_Y);

  // Sentencia IF usada para movernos por los menus
  if (X_axix > 600) {
    STATE += 1;
    lcd.clear();
    delay(300);
    if (STATE == a) {
      STATE = 0;
    }
  } else if (X_axix < 300) {
    STATE -= 1;
    delay(300);
    lcd.clear();
    if (STATE == -1) {
      STATE = b;
    }
  }
}

// Menu admin
void admin_menu() {
  analogWrite(LED_GREEN, 255);
  analogWrite(LED_RED, 255);
  lcd.setCursor(0, 0);
  lcd.print(admin_list[STATE]);
}

// Submenus accesibles desde Admin
void admin_selector() {
  lcd.clear();
  switch (STATE) {
    case 0:
      show_temperature();
    case 1:
      distance_show();
    case 2:
      timer();
    case 3:
      prices();
  }
}

// threads de temperatura y humedad
// que muestran las lecturas del sensor DHT11

// Thread temperatura
void temperature_thread() {
  lcd.clear();
  temperature = dht.readTemperature();
  lcd.print("Temperature:");
  lcd.setCursor(0, 1);
  lcd.print(temperature);
  delay(1500);
}

// Thread humedad
void humidity_thread() {
  lcd.clear();
  humidity = dht.readHumidity();
  lcd.print("Humidity:");
  lcd.setCursor(0, 1);
  lcd.print(humidity);
}

// Controller de los threads anteriores
void show_temperature() {
  lcd.clear();
  while (Y_axix > 300) {
    Y_axix = analogRead(JS_Y);
    controller.run();
  }
}

// Thread que mide la distancia
void distance_thread() {
  digitalWrite(TRIGGER, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER, LOW);
  duration = pulseIn(ECHO, HIGH);
  distance = duration * 0.034 / 2;
}

// Se muestra la distancia
void distance_show() {
  lcd.clear();

  while (Y_axix > 300) {
    Y_axix = analogRead(JS_Y);
    lcd.setCursor(0, 0);
    lcd.print("Distancia:");
    lcd.setCursor(0, 1);
    lcd.print(distance);
  }
}

// Thread que actualiza el tiempo que lleva encendida la placa
void timer_thread() {
  tiempoInicio = millis();
  lcd.clear();
  lcd.print(tiempoInicio);
  delay(100);
}

// Submenu del contador
void timer() {
  while (Y_axix > 300) {
    Y_axix = analogRead(JS_Y);
    contador.run();
  }
}

// Función previa al cambio de precios para elegir a que bebida se aplicará
void prices() {
  STATE = 0;
  lcd.clear();
  bool var = true;
  Y_axix = analogRead(JS_Y);
  while (Y_axix > 400) {
    menu_control(5, 4);
    lcd.setCursor(0, 0);
    lcd.print(cafe_list[STATE]);

    if (Y_axix > 700) {
      lcd.clear();
      price_change();
    }
  }
  lcd.clear();
}

// Función que permite el cambio de precios
void price_change() {
  lcd.clear();
  while (Y_axix > 300) {
    X_axix = analogRead(JS_X);
    Y_axix = analogRead(JS_Y);
    lcd.setCursor(0, 0);
    lcd.print("Nuevo Precio:");
    lcd.setCursor(0, 1);
    lcd.print(price_list[STATE]);

    if (X_axix > 600) {
      price_list[STATE] += 0.05;
      delay(700);
      lcd.clear();
    } else if (X_axix < 300) {
      price_list[STATE] -= 0.05;
      delay(700);
      lcd.clear();
    }
  }
}

// Función de servicio de bebida
void serving() {
  lcd.clear();
  randNumber = random(4, 8);
  lcd.print("Sirviendo...");
  for (int i = 1; i < 255; i++) {
    analogWrite(LED_GREEN, i);
    delay(randNumber * 1000 / 253);
  }
  analogWrite(LED_GREEN, 0);
  lcd.clear();
  lcd.print("RETIRE BEBIDA");
  delay(3000);
  lcd.clear();
}

// Callback de la interrupción
void button_press() {
  Serial.println("FALLLL");
  // Guardar el tiempo cuando se presionó el botón
  buttonPressStartTime = millis();
  //isPress = true;
}

// Función de reseteo
void(* resetFunc) (void) = 0;

void setup() {
  // put your setup code here, to run once:
  tiempoInicio = millis();
  Serial.begin(9600);

  lcd.begin(16, 2);

  // Inicializamos todos los pines
  pinMode(TRIGGER, OUTPUT);
  pinMode(ECHO, INPUT);

  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);

  pinMode(JS_DIGITAL, INPUT_PULLUP);
  pinMode(BUTTON, INPUT_PULLUP);


  // Setup de los threads necesarios
  temperatura.enabled = true;
  temperatura.setInterval(1000);
  temperatura.onRun(temperature_thread);

  humedad.enabled = true;
  humedad.setInterval(1000);
  humedad.onRun(humidity_thread);

  contador.enabled = true;
  contador.setInterval(200);
  contador.onRun(timer_thread);

  ultrasonido.enabled = true;
  ultrasonido.setInterval(200);
  ultrasonido.onRun(distance_thread);

  controller.add(&temperatura);
  controller.add(&humedad);

  // Iniciamos el sensor de temperatura/humedad
  dht.begin();

  // Primera parte del código. Solo se ejecuta 1 vez
  lcd.print("Cargando...");
  for (int i = 0; i <3; i++) {
    analogWrite(LED_RED, 255);
    Serial.println("LED ON"); 
    delay(500); 
    analogWrite(LED_RED, 0);
    Serial.println("LED OFF"); 
    delay(500);
  }
  lcd.clear();
  // Declaramos la interrupción
  noInterrupts();
  attachInterrupt(digitalPinToInterrupt(BUTTON), button_press, RISING);
}

void loop() {
  // Activamos el ultrasonidos para saber cuando hay una persona delante
  interrupts();
  ultrasonido.run();
  lcd.setCursor(0, 0);
  lcd.print("Servicio");
  admin = false;
  while (distance < 100) {

    ultrasonido.run();
    
    if (digitalRead(JS_DIGITAL) == LOW) {
      if (admin){
        admin_slector();
      } else {
        serving();
      }
    }

    if (admin) {
      menu_control(4, 3);
      admin_menu();
    } else {
      menu_control(5, 4);
      cafe_menu();
    }

    if (digitalRead(BUTTON) == LOW) && (isPress == true) {
      Serial.println("2");
      Serial.println(admin);
      isPress = false;
      timeWhenPress = millis();
    }

    timePressed = buttonPressStartTime - timeWhenPress;
    Serial.println(timePressed);
    if (timePressed > 5000) && (isPress == false) {
      admin = !admin;
      buttonPressStartTime = 0;
      timeWhenPress = 0;
      lcd.clear();
    } else if (timePressed >= 2000 && timePressed <= 3000) {
      resetFunc();
    }

    // Si dejamos de detectar a la persona volvemos al modo de espera
    if (distance > 120) {
      lcd.clear();
      analogWrite(LED_GREEN, 0);
      analogWrite(LED_RED, 0);
      return;
    }
  }
}
