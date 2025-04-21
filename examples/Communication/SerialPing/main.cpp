// SerialPing

// This code implements a serial communication example using the Piduino library. 
// It sends a "ping" message ("Hello World, Have a nice day!") over the serial 
// port and waits for a response ("pong"). If a response is received, it is 
// printed to the console, converted to lowercase, and sent back over the serial port. 
// The process repeats every 500 milliseconds, with all actions logged to the console for debugging purposes.

// This example code is in the public domain.

#include <Piduino.h>  // Include the Piduino library for serial communication and other utilities

// Console baud rate for debugging output
const unsigned long ConsoleBaudrate = 115200;

// Serial communication constants
const unsigned long SerialBaudrate = 38400; // Baud rate for the serial communication
const uint8_t SerialConfig = SERIAL_8E1;   // Serial configuration: 8 data bits, even parity, 1 stop bit

// Message to send over serial
const String ping = "Hello World, Have a nice day!";

void setup() {
  
  // Initialize the console for debugging
  Console.begin(ConsoleBaudrate);
  Console.print("SerialPing to ");
  Console.println(Serial.portName()); // Print the name of the serial port

  // Initialize the serial communication with the specified baud rate and configuration
  Serial.begin(SerialBaudrate, SerialConfig);
  while (!Serial) {
    ; // Wait for the serial port to connect (needed for native USB ports)
  }
}

void loop() {
  String pong; // Variable to store the received message

  // Send the "ping" message over serial
  Serial.print(ping);
  Serial.flush(); // Ensure the message is fully transmitted
  Console.print(ping); // Print the same message to the console for debugging
  Console.println();

  delay(500); // Wait for 500 milliseconds
  
  // Check if there is any data available on the serial port
  while (Serial.available() > 0) {
    int c = Serial.read(); // Read one character from the serial port

    delay(10); // Small delay to ensure proper reading
    if (c > 0) { // If the character was successfully read
      pong.concat((char)c); // Append the character to the "pong" string
    }
  }

  // If a response ("pong") was received
  if (pong.length() > 0) {

    // Print the received message to the console
    Console.print(pong); 
    Console.println(); 

    // Convert the received message to lowercase
    pong.toLowerCase();
    
    // Send the modified message back over serial
    Serial.print(pong); 
    Serial.flush();
    
    // Print the modified message to the console
    Console.print(pong); 
    Console.println(); 
  }
  
  delay(500); // Wait for 500 milliseconds before the next iteration
}


