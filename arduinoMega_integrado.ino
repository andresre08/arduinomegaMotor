/*
 * Arduino Mega - Control de Motor BLDC de 6 Pasos
 * 
 * Sistema de control básico para motor brushless de 6 pasos
 * usando secuencia de conmutación estándar
 */

const int HIN1 = 4;     // Fase A alta
const int HIN2 = 5;     // Fase B alta
const int HIN3 = 6;     // Fase C alta
const int LIN1 = 7;     // Fase A baja
const int LIN2 = 8;     // Fase B baja
const int LIN3 = 9;     // Fase C baja
const int ENABLE = 10;  // Pin de habilitación
const int LED_PIN = 13; // LED incorporado de la placa

void setup() {
  Serial.begin(115200); // Habilitamos comunicación serial para monitoreo
  
  // Configurar pines de salida
  pinMode(LED_PIN, OUTPUT);
  pinMode(HIN1, OUTPUT);
  pinMode(LIN1, OUTPUT);
  pinMode(HIN2, OUTPUT);
  pinMode(LIN2, OUTPUT);
  pinMode(HIN3, OUTPUT);
  pinMode(LIN3, OUTPUT);
  pinMode(ENABLE, OUTPUT);

  // Habilitar el puente del motor
  digitalWrite(ENABLE, HIGH);
  
  Serial.println("=== Motor BLDC 6 Pasos Iniciado ===");
  Serial.println("Secuencia de pasos activada");
}

void setPhase(bool h1, bool l1, bool h2, bool l2, bool h3, bool l3) {
  // Aplicar valores invertidos para control del motor
  digitalWrite(HIN1, !h1); 
  digitalWrite(LIN1, !l1);
  digitalWrite(HIN2, !h2); 
  digitalWrite(LIN2, !l2);
  digitalWrite(HIN3, !h3); 
  digitalWrite(LIN3, !l3);
}

void setDelay() {
  delay(10); // Retraso de 10ms entre pasos
}

void loop() {
  // Control del LED: encendido durante pasos 1-3, apagado durante pasos 4-6
  
  // Paso 1: HIN1=0, LIN1=0, HIN2=0, LIN2=1, HIN3=1, LIN3=0
  digitalWrite(LED_PIN, HIGH);
  setPhase(0, 0, 0, 1, 1, 0);
  Serial.println("Paso 1: 000110");
  setDelay();

  // Paso 2: HIN1=1, LIN1=0, HIN2=0, LIN2=1, HIN3=0, LIN3=0
  setPhase(1, 0, 0, 1, 0, 0);
  Serial.println("Paso 2: 100100");
  setDelay();

  // Paso 3: HIN1=1, LIN1=0, HIN2=0, LIN2=0, HIN3=0, LIN3=1
  setPhase(1, 0, 0, 0, 0, 1);
  Serial.println("Paso 3: 100001");
  setDelay();
  
  // Paso 4: HIN1=0, LIN1=0, HIN2=1, LIN2=0, HIN3=0, LIN3=1
  digitalWrite(LED_PIN, LOW);
  setPhase(0, 0, 1, 0, 0, 1);
  Serial.println("Paso 4: 001001");
  setDelay();

  // Paso 5: HIN1=0, LIN1=1, HIN2=1, LIN2=0, HIN3=0, LIN3=0
  setPhase(0, 1, 1, 0, 0, 0);
  Serial.println("Paso 5: 011000");
  setDelay();

  // Paso 6: HIN1=0, LIN1=1, HIN2=0, LIN2=0, HIN3=1, LIN3=0
  setPhase(0, 1, 0, 0, 1, 0);
  Serial.println("Paso 6: 010010");
  setDelay();
}