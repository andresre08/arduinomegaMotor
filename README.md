# Arduino Mega - Sistema de Interrupciones

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
