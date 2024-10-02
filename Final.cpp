#include <Servo.h>
#include <IRremote.h>

// Pines de los motores
int adelante1 = 5;
int atras1 = 6;
int adelante2 = 7;
int atras2 = 8;

// Pines del LED
int rojo = 4;
int azul = 12;
int verde = 0;

// Pines de los sensores y servo
int sensorIR = 2;  // Pin del receptor IR
int servomotorPin = 3;
int trig = A5;
int echo = A4;

int pir1 = 10;  // PIR 1
int pir2 = 9;   // PIR 2 (ajustado al pin digital 9)

// Estado de los PIR
int pirState1 = LOW;
int pirState2 = LOW;

// Variables para el modo manual/automático
bool modoManual = false;
Servo servoPing;

// Umbral para el sensor PING
int umbralDistancia = 30;  // Distancia en cm

// Inicializar la recepción de infrarrojo
IRrecv irrecv(sensorIR);
decode_results resultados;

void setup() {
  // Configurar los pines
  pinMode(adelante1, OUTPUT);
  pinMode(atras1, OUTPUT);
  pinMode(adelante2, OUTPUT);
  pinMode(atras2, OUTPUT);

  pinMode(rojo, OUTPUT);
  pinMode(azul, OUTPUT);
  pinMode(verde, OUTPUT);

  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);

  pinMode(pir1, INPUT);
  pinMode(pir2, INPUT);

  servoPing.attach(servomotorPin);
  servoPing.write(90);  // Inicialmente el sensor mira hacia el frente

  Serial.begin(9600);
  irrecv.enableIRIn();  // Habilitar la recepción de IR

  // Esperar estabilización de los sensores PIR
  Serial.println("Esperando estabilización de los PIR...");
  delay(3000);
  Serial.println("Sistema listo.");
}

void loop() {
  if (modoManual) {
    // Modo Manual
    if (irrecv.decode(&resultados)) {
      controlarModoManual(resultados.value);
      irrecv.resume();  // Preparar para recibir el próximo comando
    }
  } else {
    // Modo Automático
    modoAutomatico();
  }
}

void controlarModoManual(unsigned long codigo) {
  switch (codigo) {
    case 0xFF00BF00:  // Tecla "Encendido/Apagado"
      modoManual = !modoManual;  // Alternar entre manual y automático
      if (modoManual) {
        Serial.println("Modo manual activado.");
      } else {
        Serial.println("Modo automático activado.");
      }
      detener();  // Detener el robot al cambiar de modo
      break;
    case 0xF30CBF00:  // Tecla "0" - Parar
      detener();
      break;
    case 0xEF10BF00:  // Tecla "1" - Avanzar
      avanzar();
      break;
    case 0xEE11BF00:  // Tecla "2" - Retroceder
      retroceder();
      break;
    case 0xED12BF00:  // Tecla "3" - Girar a la izquierda
      girarIzquierda();
      break;
    case 0xEB14BF00:  // Tecla "4" - Girar a la derecha
      girarDerecha();
      break;
    case 0xEA15BF00:  // Tecla "5" - Avanzar rápido
      avanzarRapido();
      break;
    case 0xE916BF00:  // Tecla "6" - Retroceder rápido
      retrocederRapido();
      break;
    case 0xE718BF00:  // Tecla "7" - Giro suave izquierda
      girarSuaveIzquierda();
      break;
    case 0xE619BF00:  // Tecla "8" - Giro suave derecha
      girarSuaveDerecha();
      break;
    case 0xE51ABF00:  // Tecla "9" - Parar completamente
      detener();
      break;
    default:
      Serial.println("Tecla no asignada.");
      break;
  }
}

// Funciones para el modo automático
void modoAutomatico() {
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

void avanzarRapido() {
  // Ajusta la velocidad para avanzar más rápido
  Serial.println("Avanzando rápido.");
}

void retrocederRapido() {
  // Ajusta la velocidad para retroceder más rápido
  Serial.println("Retrocediendo rápido.");
}

void girarSuaveIzquierda() {
  // Ajusta los motores para girar suavemente a la izquierda
  Serial.println("Girando suavemente a la izquierda.");
}

void girarSuaveDerecha() {
  // Ajusta los motores para girar suavemente a la derecha
  Serial.println("Girando suavemente a la derecha.");
}