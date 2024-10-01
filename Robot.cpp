#include <IRremote.h>

bool manual = true;
int button = 0;
int cm = 0;
const int ledR = 4;
const int ledG = 0;
const int ledB = 12;

long readUltrasonicDistance(int triggerPin, int echoPin)
{
  pinMode(triggerPin, OUTPUT);  // Clear the trigger
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  // Sets the trigger pin to HIGH state for 10 microseconds
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  pinMode(echoPin, INPUT);
  // Reads the echo pin, and returns the sound wave travel time in microseconds
  return 0.01723 * pulseIn(echoPin, HIGH);
}

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

void setup()
{
  Serial.begin(9600);
  IrReceiver.begin(2);
  
  pinMode(ledR, OUTPUT);
  pinMode(ledG, OUTPUT);
  pinMode(ledB, OUTPUT);
}

void loop()
{
  button = readInfrared();
  delay(10);
  
  if (button == 0){
    manual = !manual;
    if(manual){
      //detenerse();
      ledRGB(LOW, LOW, LOW);
    }
  }
  
  if(manual){
    controlManual(button);
  }
  else {
    automatico();
  }
}

void controlManual(int input)
{
  //ledRGB(LOW, LOW, LOW);
  switch(input){
    case 1:
    	//avanzar
    	Serial.println("avanzar");
        ledRGB(LOW, LOW, HIGH);
    	break;
    case 9:
    	//retroceder
    	Serial.println("retroceder");
        ledRGB(LOW, LOW, HIGH);
    	break;
    case 4:
    	//girar a la izquierda
    	Serial.println("izquierda");
        ledRGB(LOW, LOW, HIGH);
    	break;
    case 6:
    	//girar a la derecha
    	Serial.println("derecha");
        ledRGB(LOW, LOW, HIGH);
    	break;
    case 5:
    	//detenerse
    	Serial.println("detenerse");
        ledRGB(LOW, LOW, LOW);
    	break;
  }
    
}

void automatico()
{
  ledRGB(HIGH, HIGH, HIGH);
  cm = readUltrasonicDistance(A5, A4);

  //acá controlar los sensores IR
  //rotar el servo 
  //controlar distancia
  //rotar robot

  /*if(cm < distanciaFrenado){
    avanzar
  }*/
  
  Serial.print(cm);
  Serial.println("auto");
}

void ledRGB(bool red, bool green, bool blue)
{
  digitalWrite(ledR, red);
  digitalWrite(ledG, green);
  digitalWrite(ledB, blue);
}