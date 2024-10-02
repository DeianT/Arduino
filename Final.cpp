#include <Servo.h>
#include <IRremote.h>

// Pines de los motores
int enableA = 5;
int enableB = 6;
int adelante1 = 11;
int atras1 = 9;
int adelante2 = 7;
int atras2 = 8;

// Pines del LED
int rojo = 4;
int azul = 12;
int verde = 0;

// Pines de los sensores y servo
int sensorIR = 2;  // Pin del receptor IR
int button = 0;    // Botón presionado
int servomotorPin = 3;
int trig = A5;
int echo = A4;

int pir1 = 10;  // PIR 1
int pir2 = 1;   // PIR 2

// Estado de los PIR
int pirState1 = LOW;
int pirState2 = LOW;

// Variables para el modo manual/automático
bool modoManual = true;
Servo servoPing;

// Umbral para el sensor PING
int umbralDistancia = 30;  // Distancia en cm

int mapCodeToButton(unsigned long code) 
{
  // Check for codes from this specific remote
  if ((code & 0x0000FFFF) == 0x0000BF00) {
    // No longer need the lower 16 bits. Shift the code by 16
    // to make the rest easier.
    code >>= 16;
    // Check that the value and inverse bytes are complementary.
    if (((code >> 8) ^ (code & 0x00FF)) == 0x00FF) {
      return code & 0xFF;
    }
  }
  return -1;
}

int readInfrared() 
{
  int result = -1;
  // Check if we've received a new code
  if (IrReceiver.decode()) {
    // Get the infrared code
    unsigned long code = IrReceiver.decodedIRData.decodedRawData;
    // Map it to a specific button on the remote
    result = mapCodeToButton(code);
    // Enable receiving of the next value
    IrReceiver.resume();
  }
  return result;
}

void setup() {
  // Configurar los pines
  pinMode(adelante1, OUTPUT);
  pinMode(atras1, OUTPUT);
  pinMode(adelante2, OUTPUT);
  pinMode(atras2, OUTPUT);
  pinMode(enableA, OUTPUT);
  pinMode(enableB, OUTPUT);
  digitalWrite(enableA, HIGH);
  digitalWrite(enableB, HIGH);

  pinMode(rojo, OUTPUT);
  pinMode(azul, OUTPUT);
  pinMode(verde, OUTPUT);

  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);

  pinMode(pir1, INPUT);
  pinMode(pir2, INPUT);

  ledRGB(HIGH, LOW, LOW);

  servoPing.attach(servomotorPin);
  servoPing.write(90);  // Inicialmente el sensor mira hacia el frente

  Serial.begin(9600);
  IrReceiver.begin(sensorIR);

  // Esperar estabilización de los sensores PIR
  Serial.println("Esperando estabilización de los PIR...");
  delay(2000);
  Serial.println("Sistema listo.");
  ledRGB(LOW, LOW, LOW);
}

void loop() {
  button = readInfrared();
  delay(10);
  
  if (button == 0){
    modoManual = !modoManual;
    if(modoManual){
      detener();
      ledRGB(LOW, LOW, LOW);
    }
  }
  
  if (modoManual) {
    // Modo Manual
    if (modoManual) {
      controlarModoManual(button);
    }
  } else {
    // Modo Automático
    modoAutomatico();
  }
}

void controlarModoManual(unsigned long codigo) {
  switch (codigo) {
    case 12:  // Tecla "0" - Parar
      detener();
      ledRGB(LOW, LOW, LOW);
      break;
    case 16:  // Tecla "1" - Avanzar
      avanzar();
      ledRGB(LOW, LOW, HIGH);
      break;
    case 17:  // Tecla "2" - Retroceder
      retroceder();
      ledRGB(LOW, LOW, HIGH);
      break;
    case 18:  // Tecla "3" - Girar a la izquierda
      girarIzquierda();
      ledRGB(LOW, LOW, HIGH);
      break;
    case 20:  // Tecla "4" - Girar a la derecha
      girarDerecha();
      ledRGB(LOW, LOW, HIGH);
      break;
    case 26:  // Tecla "9" - Parar completamente
      detener();
      ledRGB(LOW, LOW, LOW);
      break;
    default:
      Serial.println("Tecla no asignada.");
      break;
  }
}

// Funciones para el modo automático
void modoAutomatico() {
  ledRGB(HIGH, HIGH, HIGH);
  int pirVal1 = digitalRead(pir1);
  int pirVal2 = digitalRead(pir2);

  long distancia = medirDistancia();

  if (distancia < umbralDistancia) {
    detener();
    Serial.println("Obstáculo detectado, deteniéndose.");
  } else {
    if (pirVal1 == HIGH) {
      girarIzquierda();
      servoPing.write(0);  // Apunta el PING a la izquierda
    } else if (pirVal2 == HIGH) {
      girarDerecha();
      servoPing.write(180);  // Apunta el PING a la derecha
    } else {
      avanzar();
      servoPing.write(90);  // Mantener el PING mirando al frente
    }
  }
}

// Función para medir la distancia con el sensor PING
long medirDistancia() {
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  long duracion = pulseIn(echo, HIGH);
  long distancia = duracion * 0.034 / 2;
  return distancia;
}

// Funciones para controlar el movimiento del robot
void avanzar() {
  digitalWrite(adelante1, HIGH);
  digitalWrite(atras1, LOW);
  digitalWrite(adelante2, HIGH);
  digitalWrite(atras2, LOW);
  Serial.println("Avanzando.");
}

void retroceder() {
  digitalWrite(adelante1, LOW);
  digitalWrite(atras1, HIGH);
  digitalWrite(adelante2, LOW);
  digitalWrite(atras2, HIGH);
  Serial.println("Retrocediendo.");
}

void girarIzquierda() {
  digitalWrite(adelante1, LOW);
  digitalWrite(atras1, HIGH);
  digitalWrite(adelante2, HIGH);
  digitalWrite(atras2, LOW);
  Serial.println("Girando a la izquierda.");
}

void girarDerecha() {
  digitalWrite(adelante1, HIGH);
  digitalWrite(atras1, LOW);
  digitalWrite(adelante2, LOW);
  digitalWrite(atras2, HIGH);
  Serial.println("Girando a la derecha.");
}

void detener() {
  digitalWrite(adelante1, LOW);
  digitalWrite(atras1, LOW);
  digitalWrite(adelante2, LOW);
  digitalWrite(atras2, LOW);
  Serial.println("Detenido.");
}

void ledRGB(bool red, bool green, bool blue)
{
  digitalWrite(rojo, red);
  digitalWrite(verde, green);
  digitalWrite(azul, blue);
}