void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(A1, INPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  int rawReading = analogRead(A1);
  float volts = rawReading * (5000.0/1023.0);
  Serial.println(volts);
//  Serial.println(" ");

}
