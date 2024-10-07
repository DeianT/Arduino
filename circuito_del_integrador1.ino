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
// Pines de los sensores y servo
int sensorIR = 2;  // Pin del receptor IR
int servomotorPin = 3;
int trig = A5;
int echo = A4;
int pirIzq = 12;  // PIR 1
int pirDer = 10;   // PIR 2
//codigo del control
unsigned long button = 0;    // Botón presionado
// Estado de los PIR
int pirState1 = LOW;
int pirState2 = LOW;
// Variables para el modo manual/automático
bool modoManual = true;
Servo servoPing;
// Umbral para el sensor PING
int umbralDistancia = 20;  // Distancia en cm

unsigned long readInfrared() 
{
  unsigned long code = 0;
  // Check if we've received a new code
  if (IrReceiver.decode()) {
    // Get the infrared code
    code = IrReceiver.decodedIRData.decodedRawData;
    // Map it to a specific button on the remote
    Serial.println(code);
    button = code;
    // Enable receiving of the next value
    IrReceiver.resume();
  }
  return code;
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

  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(pirIzq, INPUT);
  pinMode(pirDer, INPUT);


  servoPing.attach(servomotorPin);
  servoPing.write(90);  // Inicialmente el sensor mira hacia el frente

  Serial.begin(9600);
  IrReceiver.begin(sensorIR);

  // Esperar estabilización de los sensores PIR
  Serial.println("Esperando estabilización de los PIR...");
  delay(2000);
  Serial.println("Sistema listo.");
  digitalWrite(rojo, HIGH);
}

void loop() {
  button = readInfrared();
  delay(10);
  if(button == 3125149440){
    digitalWrite(rojo, LOW);
  }
  Serial.println(button);
  
  if (button == 3125149440){
    modoManual = !modoManual;
    if(modoManual){
      detener();
      servoPing.write(90);
    }
  }
  
  if (modoManual) {
      controlarModoManual(button);
  } else {
    modoAutomatico();
  }
}

void controlarModoManual(unsigned long codigo) {
  switch (codigo) {
    case 3208707840:  // Tecla "0" - Parar
      detener();
      break;
    case 3108437760:  // Tecla "1" - Avanzar
      avanzar();
      break;
    case 3927310080:  // Tecla "2" - Retroceder
      retroceder();
      break;
    case 3141861120:  // Tecla "3" - Girar a la izquierda
      girarIzquierda();
      break;
    case 3158572800:  // Tecla "4" - Girar a la derecha
      girarDerecha();
      break;
    default:
      //Serial.println("Tecla no asignada.");
      break;
  }
}

// Funciones para el modo automático
void modoAutomatico() {
  int pirVal1 = digitalRead(pirIzq);
  int pirVal2 = digitalRead(pirDer);
  Serial.println(pirVal1);

  long distancia = medirDistancia();

if (distancia < umbralDistancia) {
  detener();
  delay(300);
} else {
  if (pirVal1 == LOW) {
    girarIzquierda();
    servoPing.write(120);
    delay(1500);
  } else if (pirVal2 == LOW) {
    girarDerecha();
    servoPing.write(60);
    delay(1500);
  } else {
    avanzar();
    servoPing.write(90);
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
  digitalWrite(adelante1, LOW);
  digitalWrite(atras1, HIGH);
  digitalWrite(adelante2, LOW);
  digitalWrite(atras2, HIGH);
  Serial.println("Avanzando.");
}
void retroceder() {
  digitalWrite(adelante1, HIGH);
  digitalWrite(atras1, LOW);
  digitalWrite(adelante2, HIGH);
  digitalWrite(atras2, LOW);
  Serial.println("Retrocediendo.");
}
void girarIzquierda() {
  digitalWrite(adelante1, LOW);
  digitalWrite(atras1, HIGH);
  digitalWrite(adelante2, LOW);
  digitalWrite(atras2, LOW);
  Serial.println("Girando a la izquierda.");
}
void girarDerecha() {
  digitalWrite(adelante1, LOW);
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
void ledRGB(bool red){
  digitalWrite(rojo, red);
}