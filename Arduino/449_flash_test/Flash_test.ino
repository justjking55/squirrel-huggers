// Pin where the relay is connected
const int relayPin = 7;

// Time intervals in milliseconds
const unsigned long interval = 3000; // 5 seconds

void setup() {
  // Initialize the relay pin as an output
  pinMode(relayPin, OUTPUT);
  // Start serial communication for plotting
  Serial.begin(9600);
  // Set relay to LOW initially to keep it off
  digitalWrite(relayPin, LOW);
}

void loop() {
  // Turn on the relay to trigger the flash
  digitalWrite(relayPin, HIGH);
  delay(100); // Flash duration (adjust if needed)

  // Turn off the relay
  digitalWrite(relayPin, LOW);

  // Wait for the specified interval before flashing again
  delay(interval - 100); // Subtract flash duration
}
