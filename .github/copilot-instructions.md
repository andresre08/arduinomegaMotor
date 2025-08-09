<!-- Use this file to provide workspace-specific custom instructions to Copilot. For more details, visit https://code.visualstudio.com/docs/copilot/copilot-customization#_use-a-githubcopilotinstructionsmd-file -->

# Arduino Mega Interrupt Project - Copilot Instructions

This is an Arduino Mega project focused on interrupt handling for digital inputs. When working with this codebase:

## Project Context
- Target hardware: Arduino Mega 2560
- Programming language: C++ (Arduino flavor)
- Main functionality: Interrupt-driven digital input monitoring
- Three digital inputs with rising/falling edge detection
- Real-time monitoring via Serial communication

## Coding Standards
- Use Arduino-style C++ conventions
- Keep interrupt service routines (ISR) short and efficient
- Use `volatile` keyword for variables accessed in ISRs
- Follow Arduino naming conventions (camelCase for variables, UPPER_CASE for constants)
- Add comprehensive comments for hardware-specific code

## Hardware Considerations
- Arduino Mega has 6 external interrupts (INT0-INT5)
- This project uses pins 2 (INT0), 3 (INT1), and 18 (INT5)
- Internal pull-up resistors are enabled
- Built-in LED on pin 13 for status indication

## Best Practices
- Implement software debouncing for reliable interrupt handling
- Use `millis()` for non-blocking timing operations
- Provide clear Serial output for debugging and monitoring
- Consider interrupt priority and execution time
- Document pin assignments and hardware connections clearly

## Common Patterns
- State tracking with volatile variables
- Debounce timing with millis() comparison
- Edge detection by comparing current and previous states
- Non-blocking main loop with periodic status updates
- Comprehensive error handling and status reporting
