// SerialPrint

// This code demonstrates a simple serial communication example using the Piduino library. 
// It sends a predefined message ("Hello World, Have a nice day!") over the serial port and 
// simultaneously prints the same message to the console for debugging. The process repeats 
// every 500 milliseconds, ensuring the message is fully transmitted before each iteration.

// This example code is in the public domain.

#include <Piduino.h> // Include the Piduino library for serial communication and utilities

// Console baud rate for debugging output
const unsigned long ConsoleBaudrate = 115200;

// Serial communication constants
const unsigned long SerialBaudrate = 38400; // Baud rate for serial communication
const uint8_t SerialConfig = SERIAL_8E1;   // Serial configuration: 8 data bits, even parity, 1 stop bit

// Message to send over serial
const String hello = "Hello World, Have a nice day!";

void setup() {
  
  // Initialize the console for debugging
  Console.begin(ConsoleBaudrate);
  Console.print("SerialPrint to ");
  Console.println(Serial.portName()); // Print the name of the serial port

  // Initialize the serial communication with the specified baud rate and configuration
  Serial.begin(SerialBaudrate, SerialConfig);
  while (!Serial) {
    ; // Wait for the serial port to connect (needed for native USB ports)
  }
}

void loop() {

  // Send the "hello" message over serial
  Serial.print(hello);
  Serial.flush(); // Ensure the message is fully transmitted
  
  // Print the same message to the console for debugging
  Console.print(hello);
  Console.println();

  // Wait for 500 milliseconds before repeating
  delay(500);
}

