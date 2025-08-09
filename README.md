# Arduino Mega Motor BLDC Control System

## Descripción del Proyecto

Sistema avanzado de control para motor BLDC (Brushless DC) utilizando Arduino Mega 2560 y driver IR2136. Este proyecto implementa un control de 6 pasos con interrupciones externas para el manejo preciso de motores trifásicos sin escobillas.

## Características Principales

### 🔧 Hardware
- **Microcontrolador**: Arduino Mega 2560
- **Driver de Motor**: IR2136 (Three Phase Bridge Driver)
- **Motor**: BLDC trifásico de 6 pasos
- **Control PWM**: Intensidad variable por ADC
- **Interrupciones**: 3 entradas digitales con detección de flancos

### ⚡ Funcionalidades
- **Control de 6 pasos**: Secuencia binaria optimizada (1,3,7,6,4,0)
- **Interrupciones unificadas**: Pines 2 (INT0), 3 (INT1), 18 (INT5)
- **PWM dinámico**: Control de intensidad vía ADC A0
- **Sistema híbrido**: Salidas altas PWM + salidas bajas digitales
- **Configuración IR2136**: Pines HIN1-3 (PWM) y LIN1-3 (Digital)

## Configuración de Hardware

### Conexiones Arduino Mega → IR2136
```
Pin 4 (HIN1) → IR2136 HIN1 (Fase A Alta)
Pin 5 (HIN2) → IR2136 HIN2 (Fase B Alta) 
Pin 6 (HIN3) → IR2136 HIN3 (Fase C Alta)
Pin 7 (LIN1) → IR2136 LIN1 (Fase A Baja)
Pin 8 (LIN2) → IR2136 LIN2 (Fase B Baja)
Pin 9 (LIN3) → IR2136 LIN3 (Fase C Baja)
Pin 10     → IR2136 ENABLE
Pin A0     → Potenciómetro (Control PWM)
```

### Entradas de Control
```
Pin 2  (INT0) → Sensor/Switch Entrada 1
Pin 3  (INT1) → Sensor/Switch Entrada 2  
Pin 18 (INT5) → Sensor/Switch Entrada 3
```

## Secuencia de Control BLDC

El sistema utiliza una secuencia de 6 pasos optimizada para motores BLDC:

| Paso | Binario | HIN1 | LIN1 | HIN2 | LIN2 | HIN3 | LIN3 | Estado Motor |
|------|---------|------|------|------|------|------|------|--------------|
| 1    | 001     | PWM  | OFF  | OFF  | ON   | OFF  | ON   | 000110       |
| 2    | 011     | OFF  | OFF  | OFF  | ON   | PWM  | OFF  | 100100       |
| 3    | 111     | OFF  | OFF  | PWM  | OFF  | OFF  | ON   | 100001       |
| 4    | 110     | OFF  | OFF  | OFF  | OFF  | OFF  | ON   | 001001       |
| 5    | 100     | OFF  | ON   | OFF  | OFF  | PWM  | OFF  | 011000       |
| 6    | 000     | OFF  | ON   | PWM  | OFF  | OFF  | OFF  | 010010       |

## Características Técnicas

### Control PWM
- **Frecuencia**: 976 Hz (configurable)
- **Resolución**: 8 bits (0-255)
- **Control**: ADC A0 (0-100%)
- **Aplicación**: Solo salidas altas (HIN1, HIN2, HIN3)

### Sistema de Interrupciones
- **Tipo**: Detección de cambios (CHANGE)
- **Procesamiento**: Función unificada `interrupcion_unificada()`
- **Lógica**: Lectura binaria de 3 bits simultánea
- **Debounce**: Implementado por software

### Compatibilidad IR2136
- **Alimentación**: 15V (Motor), 5V (Lógica)
- **Dead Time**: Configurado en hardware
- **Protecciones**: Sobrecorriente, sobrecalentamiento
- **Bootstrap**: Capacitores externos requeridos

## Archivos del Proyecto

- `arduinoMega.ino` - Código principal del sistema
- `README.md` - Documentación del proyecto
- `.gitignore` - Exclusiones de Git
- `codigopwm.ino` - Código PWM adicional
- `arduinoMega_integrado.ino` - Versión integrada

## Uso

1. **Conexión del hardware** según el diagrama de pines
2. **Carga del código** en Arduino Mega 2560
3. **Configuración del IR2136** con alimentación adecuada
4. **Ajuste de intensidad** con potenciómetro en A0
5. **Control de secuencia** con switches en pines de interrupción

## Monitor Serie

El sistema proporciona información detallada por puerto serie:
- Estado de inicialización
- Secuencia binaria actual
- Valores ADC y PWM en tiempo real
- Debug de estados de motor

```
=== SISTEMA DE CONTROL MOTOR INICIADO ===
NUEVA SECUENCIA BINARIA:
Binario 1 -> Paso 1 (Motor: 000110)
Binario 3 -> Paso 2 (Motor: 100100)
...
CONTROL PWM HABILITADO:
- ADC A0 controla intensidad PWM de salidas altas
```

## Licencia

Este proyecto está bajo licencia MIT - ver el archivo LICENSE para detalles. - Sistema de Interrupciones

Este proyecto implementa un sistema de manejo de interrupciones por flancos de subida y bajada para tres entradas digitales en un Arduino Mega.

## Características

- **Interrupciones por flancos**: Detecta tanto flancos de subida como de bajada
- **Tres entradas digitales**: Utiliza los pines 2, 3 y 18 del Arduino Mega
- **Debounce por software**: Previene lecturas múltiples por ruido
- **Monitoreo en tiempo real**: Muestra el estado y contadores vía Serial
- **LED indicador**: Señal visual de funcionamiento

## Pines Utilizados

| Pin | Función | Interrupción | Descripción |
|-----|---------|--------------|-------------|
| 2   | Entrada 1 | INT0 | Primera entrada digital |
| 3   | Entrada 2 | INT1 | Segunda entrada digital |
| 18  | Entrada 3 | INT5 | Tercera entrada digital (específico de Mega) |
| 13  | LED | - | LED indicador de estado |

## Configuración del Hardware

### Conexiones Básicas
- Conecta las señales de entrada a los pines 2, 3 y 18
- Los pines tienen pull-up interno activado (señal en LOW cuando se activa)
- El LED interno (pin 13) indica el funcionamiento

### Esquema de Conexión
```
Señal 1 -----> Pin 2 (INT0)
Señal 2 -----> Pin 3 (INT1)  
Señal 3 -----> Pin 18 (INT5)
LED     -----> Pin 13 (interno)
```

## Funcionamiento

### Interrupciones
- Cada entrada está configurada para disparar en cualquier cambio (`CHANGE`)
- Las funciones de interrupción detectan si es flanco de subida o bajada
- Se implementa debounce por software (50ms) para evitar lecturas erróneas

### Monitoreo
- El sistema muestra información cada 2 segundos por Serial
- Contador de interrupciones para cada entrada
- Estado actual (HIGH/LOW) de cada pin
- Tiempo de funcionamiento del sistema

### Indicadores Visuales
- LED parpadea cada segundo para indicar funcionamiento normal
- Parpadeo inicial de 3 veces al arrancar
- Mensajes detallados por puerto serie

## Uso

1. **Cargar el código**: Sube el archivo `.ino` al Arduino Mega
2. **Abrir Monitor Serie**: Configura a 9600 baudios
3. **Conectar señales**: Conecta las señales de entrada a los pines correspondientes
4. **Observar**: El sistema mostrará las interrupciones en tiempo real

## Salida del Monitor Serie

```
=== Arduino Mega - Sistema de Interrupciones ===
Configurando interrupciones para 3 entradas digitales...
Interrupciones configuradas:
- Pin 2 (INT0): Entrada 1
- Pin 3 (INT1): Entrada 2
- Pin 18 (INT5): Entrada 3
Esperando señales...

INT1 -> FLANCO DE BAJADA en Pin 2
INT2 -> FLANCO DE SUBIDA en Pin 3
==== ESTADO DEL SISTEMA ====
Entrada 1 (Pin 2): 1 interrupciones - Estado: LOW
Entrada 2 (Pin 3): 1 interrupciones - Estado: HIGH
Entrada 3 (Pin 18): 0 interrupciones - Estado: HIGH
Tiempo activo: 5 segundos
=============================
```

## Personalización

### Cambiar Pines
Para usar diferentes pines, modifica las constantes al inicio del código:
```cpp
const int PIN_ENTRADA_1 = 2;   // Cambiar por el pin deseado
const int PIN_ENTRADA_2 = 3;   // Debe ser un pin con interrupción
const int PIN_ENTRADA_3 = 18;  // Solo en Arduino Mega/Leonardo
```

### Ajustar Debounce
Para cambiar el tiempo de debounce:
```cpp
const unsigned long DEBOUNCE_DELAY = 50; // Cambiar valor en ms
```

### Modificar Frecuencia de Reporte
Para cambiar cada cuánto se muestra el estado:
```cpp
if(millis() - ultimo_reporte >= 2000) { // Cambiar 2000 por ms deseados
```

## Compatibilidad

- **Arduino Mega 2560**: Totalmente compatible
- **Arduino Uno**: Compatible solo con 2 entradas (pines 2 y 3)
- **Arduino Leonardo**: Compatible con modificaciones menores

## Notas Técnicas

- El Arduino Mega tiene 6 interrupciones externas (INT0-INT5)
- Este código usa 3 de las 6 disponibles
- Las interrupciones tienen mayor prioridad que el loop principal
- Se recomienda mantener las funciones de interrupción lo más cortas posible

## Troubleshooting

**Problema**: No se detectan interrupciones
- Verificar conexiones de los pines
- Comprobar que las señales lleguen correctamente
- Revisar configuración del Monitor Serie (9600 baudios)

**Problema**: Demasiadas interrupciones falsas
- Aumentar el valor de `DEBOUNCE_DELAY`
- Verificar la calidad de las señales de entrada
- Añadir condensadores de filtrado si es necesario
