
// SerialPong

// This program demonstrates a basic "Serial Pong" application where it reads
// a message ("ping") from the serial port, converts it to lowercase, and sends
// it back as a response ("pong"). It also outputs the received and sent messages
// to the console for debugging purposes.
// The program initializes the console for debugging and sets up serial communication
// with a specified baud rate and configuration. It continuously listens for incoming
// serial data, processes it, and responds accordingly.

// This example code is in the public domain.


// Include the Piduino library for serial communication and utilities
#include <Piduino.h>  

// Console baud rate for debugging output
const unsigned long ConsoleBaudrate = 115200;

// Serial communication constants
const unsigned long SerialBaudrate = 38400; // Baud rate for serial communication
const uint8_t SerialConfig = SERIAL_8E1;   // Serial configuration: 8 data bits, even parity, 1 stop bit

void setup() {
  
  // Initialize the console for debugging
  Console.begin(ConsoleBaudrate);
  Console.print("SerialPong to ");
  Console.println(Serial.portName()); // Print the name of the serial port

  // Initialize the serial communication with the specified baud rate and configuration
  Serial.begin(SerialBaudrate, SerialConfig);
  while (!Serial) {
    ; // Wait for the serial port to connect (needed for native USB ports)
  }
}

void loop() {
  String ping; // Variable to store the received message

  // Check if there is any data available on the serial port
  while (Serial.available() > 0) {
    int c = Serial.read(); // Read one character from the serial port
    delay(10); // Small delay to ensure proper reading
    if (c > 0) { // If the character was successfully read
      ping.concat((char)c); // Append the character to the "ping" string
    }
  }

  // If a message ("ping") was received
  if (ping.length() > 0) {
    String pong; // Variable to store the response message

    // Print the received message to the console
    Console.print(ping); 
    Console.println(); 

    // Copy the received message and convert it to lowercase
    pong = ping;
    pong.toLowerCase();
    
    // Send the modified message ("pong") back over serial
    Serial.print(pong); 
    Serial.flush();
    
    // Print the modified message to the console
    Console.print(pong); 
    Console.println(); 
  }
}

