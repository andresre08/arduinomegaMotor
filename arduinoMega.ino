/*
 * Arduino Mega - Control de Motor con Interrupciones
 *
 * Sistema integrado que usa interrupciones para controlar
 * un motor de 6 pasos basado en señales externas
 *
 * Pines de entrada (interrupciones):
 * - Pin 2 (INT0): Entrada digital 1
 * - Pin 3 (INT1): Entrada digital 2
 * - Pin 18 (INT5): Entrada digital 3
 *
 * Pines de salida (motor):
 * - Pin 4 (HIN1): Fase A alta
 * - Pin 5 (HIN2): Fase B alta
 * - Pin 6 (HIN3): Fase C alta
 * - Pin 7 (LIN1): Fase A baja
 * - Pin 8 (LIN2): Fase B baja
 * - Pin 9 (LIN3): Fase C baja
 * - Pin 10 (ENABLE): Habilitación del puente
 * - Pin 13 (LED): LED incorporado de la placa
 *
 * SECUENCIA DE 6 PASOS CONTROLADA POR INTERRUPCIONES:
 * NUEVA LÓGICA BINARIA - Lee los 3 pines simultáneamente:
 * Binario 1 (001) -> Paso 1 del Motor
 * Binario 3 (011) -> Paso 2 del Motor
 * Binario 7 (111) -> Paso 3 del Motor
 * Binario 6 (110) -> Paso 4 del Motor
 * Binario 4 (100) -> Paso 5 del Motor
 * Binario 0 (000) -> Paso 6 del Motor
 */

// ===== DEFINICIÓN DE PINES =====

// Pines de entrada para interrupciones
const int PIN_ENTRADA_1 = 2;  // INT0
const int PIN_ENTRADA_2 = 3;  // INT1
const int PIN_ENTRADA_3 = 18; // INT5
const int ADC_PIN_A0 = A0;    // Pin analógico A0

// Pines de salida para control del motor (desde pin 4 en adelante)
const int HIN1 = 4;     // Fase A alta
const int HIN2 = 5;     // Fase B alta
const int HIN3 = 6;     // Fase C alta
const int LIN1 = 7;     // Fase A baja
const int LIN2 = 8;     // Fase B baja
const int LIN3 = 9;     // Fase C baja
const int ENABLE = 10;  // Pin de habilitación
const int LED_PIN = 13; // LED incorporado de la placa

// ===== VARIABLES GLOBALES =====

// Variables volátiles para uso en ISR
volatile int currentStep = 0b000110;  // Paso actual del motor (iniciar con Paso 1: 000110)
volatile bool stepChanged = false;    // Flag de cambio de paso
volatile int sequenceStep = 0;        // Paso en la secuencia (0-5)
volatile uint8_t lastBinaryState = 1; // Estado binario anterior (iniciar con 1 = Paso 1)
volatile int adcValue = 0;            // Valor ADC en porcentaje (0-100)
volatile int pwmValue = 0;            // Valor PWM calculado (0-255)
// Array con la secuencia binaria: 1, 3, 7, 6, 4, 0 (correspondiente a Pasos 1, 2, 3, 4, 5, 6)
const uint8_t binarySequence[6] = {1, 3, 7, 6, 4, 0};

// Array con la secuencia ordenada de pasos del motor
const int stepSequence[6] = {
    0b000110, // Paso 1: HIN1=0, LIN1=0, HIN2=0, LIN2=1, HIN3=1, LIN3=0
    0b100100, // Paso 2: HIN1=1, LIN1=0, HIN2=0, LIN2=1, HIN3=0, LIN3=0
    0b100001, // Paso 3: HIN1=1, LIN1=0, HIN2=0, LIN2=0, HIN3=0, LIN3=1
    0b001001, // Paso 4: HIN1=0, LIN1=0, HIN2=1, LIN2=0, HIN3=0, LIN3=1
    0b011000, // Paso 5: HIN1=0, LIN1=1, HIN2=1, LIN2=0, HIN3=0, LIN3=0
    0b010010  // Paso 6: HIN1=0, LIN1=1, HIN2=0, LIN2=0, HIN3=1, LIN3=0
};

void setup()
{
  Serial.begin(115200); // Inicializar comunicación serial para monitoreo
  pinMode(ADC_PIN_A0, INPUT);
  // Configuración adicional del ADC si es necesaria
  analogReference(DEFAULT);
  // Configurar pines de entrada con pull-up interno
  pinMode(PIN_ENTRADA_1, INPUT_PULLUP);
  pinMode(PIN_ENTRADA_2, INPUT_PULLUP);
  pinMode(PIN_ENTRADA_3, INPUT_PULLUP);

  // Configurar pines de salida para el motor
  pinMode(HIN1, OUTPUT);
  pinMode(LIN1, OUTPUT);
  pinMode(HIN2, OUTPUT);
  pinMode(LIN2, OUTPUT);
  pinMode(HIN3, OUTPUT);
  pinMode(LIN3, OUTPUT);
  pinMode(ENABLE, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  // Configurar interrupciones - todas apuntan a la misma función unificada
  attachInterrupt(digitalPinToInterrupt(PIN_ENTRADA_1), interrupcion_unificada, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PIN_ENTRADA_2), interrupcion_unificada, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PIN_ENTRADA_3), interrupcion_unificada, CHANGE);

  // Habilitar el puente del motor
  digitalWrite(ENABLE, HIGH);

  // Estado inicial del motor (Paso 1: binario 000110)
  setMotorStep(0b000110);

  Serial.println("=== SISTEMA DE CONTROL MOTOR INICIADO ===");
  Serial.println("NUEVA SECUENCIA BINARIA:");
  Serial.println("Binario 1 -> Paso 1 (Motor: 000110)");
  Serial.println("Binario 3 -> Paso 2 (Motor: 100100)");
  Serial.println("Binario 7 -> Paso 3 (Motor: 100001)");
  Serial.println("Binario 6 -> Paso 4 (Motor: 001001)");
  Serial.println("Binario 4 -> Paso 5 (Motor: 011000)");
  Serial.println("Binario 0 -> Paso 6 (Motor: 010010)");
  Serial.println("CONTROL PWM HABILITADO:");
  Serial.println("- ADC A0 controla intensidad PWM de salidas altas (HIN1, HIN2, HIN3)");
  Serial.println("- Salidas bajas (LIN1, LIN2, LIN3) siguen siendo digitales");
  Serial.print("Estado inicial: Binario=");
  Serial.print(lastBinaryState);
  Serial.print(" | Paso=");
  Serial.print(sequenceStep + 1);
  Serial.print(" | Motor=");
  Serial.println(currentStep, BIN);
  interrupcion_unificada();
}

void loop()
{

  // Verificar si hay cambio de paso por interrupción
  if (stepChanged)
  {
    adcValue = readADC_A0(); // Leer ADC como porcentaje (0-100)

    // Convertir porcentaje ADC a valor PWM (0-255)
    pwmValue = map(adcValue, 0, 100, 0, 255);

    //  // Debug output opcional
    //  Serial.print("ADC: ");
    //  Serial.print(adcValue);
    //  Serial.print("% | PWM: ");
    //  Serial.println(pwmValue);

    stepChanged = false;
    setMotorStep(currentStep);
  }
}

// ===== FUNCIONES DE CONTROL DEL MOTOR =====

void setMotorStep(int step)
{
  // Control del LED según el paso (pasos 4,5,6 = binarios 001001, 011000, 010010)
  // bool ledState = (step == 0b001001 || step == 0b011000 || step == 0b010010);
  // digitalWrite(LED_PIN, ledState ? HIGH : LOW);

  // Switch para ejecutar cada paso del motor usando patrones binarios
  switch (step)
  {
  case 0b100100: // Paso 1: HIN1=0, LIN1=0, HIN2=0, LIN2=1, HIN3=1, LIN3=0
    setPhase(0, 0, 0, 1, 1, 0);
    // Serial.println("Motor: Paso 1 (000110)");
    break;

  case 0b000110: // Paso 2: HIN1=1, LIN1=0, HIN2=0, LIN2=1, HIN3=0, LIN3=0
    setPhase(1, 0, 0, 1, 0, 0);
    // Serial.println("Motor: Paso 2 (100100)");
    break;

  case 0b010010: // Paso 3: HIN1=1, LIN1=0, HIN2=0, LIN2=0, HIN3=0, LIN3=1
    setPhase(1, 0, 0, 0, 0, 1);
    // Serial.println("Motor: Paso 3 (100001)");
    break;

  case 0b011000: // Paso 4: HIN1=0, LIN1=0, HIN2=1, LIN2=0, HIN3=0, LIN3=1
    setPhase(0, 0, 1, 0, 0, 1);
    // Serial.println("Motor: Paso 4 (001001)");
    break;

  case 0b001001: // Paso 5: HIN1=0, LIN1=1, HIN2=1, LIN2=0, HIN3=0, LIN3=0
    setPhase(0, 1, 1, 0, 0, 0);
    // Serial.println("Motor: Paso 5 (011000)");
    break;

  case 0b100001: // Paso 6: HIN1=0, LIN1=1, HIN2=0, LIN2=0, HIN3=1, LIN3=0
    setPhase(0, 1, 0, 0, 1, 0);
    // Serial.println("Motor: Paso 6 (010010)");
    break;

  default:
    setPhase(0, 0, 0, 0, 0, 0); // Apagado
    digitalWrite(LED_PIN, LOW);
    // Serial.println("Motor: Estado de reposo");
    break;
  }
}
/**
 * Controla las fases del motor con PWM para salidas altas
 * Las salidas altas (HIN1, HIN2, HIN3) usan PWM basado en el valor del ADC
 * Las salidas bajas (LIN1, LIN2, LIN3) siguen siendo digitales ON/OFF
 */
void setPhase(bool h1, bool l1, bool h2, bool l2, bool h3, bool l3)
{
  // Salidas altas con PWM (solo cuando están activas)
  if (!h1)
  { // invertimos la lógica
    analogWrite(HIN1, pwmValue);
  }
  else
  {
    analogWrite(HIN1, 0);
  }

  if (!h2)
  {
    analogWrite(HIN2, pwmValue);
  }
  else
  {
    analogWrite(HIN2, 0);
  }

  if (!h3)
  {
    analogWrite(HIN3, pwmValue);
  }
  else
  {
    analogWrite(HIN3, 0);
  }

  // Salidas bajas siguen siendo digitales (invertidas como en código original)
  digitalWrite(LIN1, !l1);
  digitalWrite(LIN2, !l2);
  digitalWrite(LIN3, !l3);
}

// ===== FUNCIONES DE INTERRUPCIÓN =====

/**
 * Función unificada de interrupción para todos los pines de entrada
 * Lee el estado binario de los 3 pines y determina el paso correspondiente
 * Basado en la secuencia binaria: 1, 3, 7, 6, 4, 0 (Pasos 1, 2, 3, 4, 5, 6)
 */
void interrupcion_unificada()
{
  // Leer estado binario actual de todos los pines
  uint8_t currentBinaryState = readInterruptPinsBinary();

  // Solo procesar si hay cambio de estado
  if (currentBinaryState != lastBinaryState)
  {
    // Buscar el valor binario en la secuencia para determinar el paso
    int stepIndex = findStepFromBinary(currentBinaryState);

    if (stepIndex >= 0)
    {
      // Paso válido encontrado
      sequenceStep = stepIndex;
      currentStep = stepSequence[sequenceStep];
      stepChanged = true;

      // Serial.print(" -> Paso ");
      // Serial.println(stepIndex + 1);
    }
    else
    {
      Serial.print("Binario no válido: ");
      Serial.println(currentBinaryState);
    }

    // Actualizar estado anterior
    lastBinaryState = currentBinaryState;
  }
} // ===== FUNCIONES AUXILIARES =====

/**
 * Busca un valor binario en la secuencia y retorna el índice del paso
 * Secuencia binaria: 1, 3, 7, 6, 4, 0 (correspondiente a Pasos 1, 2, 3, 4, 5, 6)
 * Retorna: índice del paso (0-5) o -1 si no se encuentra
 */
int findStepFromBinary(uint8_t binaryValue)
{
  for (int i = 0; i < 6; i++)
  {
    if (binarySequence[i] == binaryValue)
    {
      return i; // Retorna el índice del paso (0-5)
    }
  }
  return -1; // No encontrado
}

/**
 * Lee los tres pines de interrupción y forma un valor binario de 3 bits
 * Bit 0 (LSB): PIN_ENTRADA_1 (Pin 2 - INT0)
 * Bit 1:       PIN_ENTRADA_2 (Pin 3 - INT1)
 * Bit 2 (MSB): PIN_ENTRADA_3 (Pin 18 - INT5)
 * Retorna: Valor de 0 a 7 (3 bits)
 */
uint8_t readInterruptPinsBinary()
{
  uint8_t binaryValue = 0;

  // Leer cada pin y asignar al bit correspondiente
  if (digitalRead(PIN_ENTRADA_1))
  { // Pin 2 -> Bit 0 (peso 1)
    binaryValue |= (1 << 0);
  }

  if (digitalRead(PIN_ENTRADA_2))
  { // Pin 3 -> Bit 1 (peso 2)
    binaryValue |= (1 << 1);
  }

  if (digitalRead(PIN_ENTRADA_3))
  { // Pin 18 -> Bit 2 (peso 4)
    binaryValue |= (1 << 2);
  }

  return binaryValue;
}

// Función para establecer un paso específico del motor (mantenida para compatibilidad)
void goToStep(int stepNumber)
{
  if (stepNumber >= 1 && stepNumber <= 6)
  {
    sequenceStep = stepNumber - 1; // Convertir de 1-6 a 0-5
    currentStep = stepSequence[sequenceStep];
    stepChanged = true;
  }
}

void setDelay()
{
 // delay(10); // Retraso de 10ms
}

int readADC_A0()
{
  int rawValue = analogRead(ADC_PIN_A0); // Lectura cruda del ADC (0-1023)

  // Convertir a porcentaje con precisión matemática correcta
  // Multiplicar primero para evitar pérdida de precisión en división entera
  long percentage = ((long)rawValue * 100L) / 1023L;

  return (int)percentage; // Retornar como entero (0-100%)
}

/**
 * Convierte porcentaje ADC a valor PWM
 * @param adcPercent: Valor ADC en porcentaje (0-100)
 * @return: Valor PWM (0-255)
 */
int convertAdcToPwm(int adcPercent)
{
  // Mapear de 0-100% a 0-255 PWM
  return (adcPercent * 255) / 100;
}
