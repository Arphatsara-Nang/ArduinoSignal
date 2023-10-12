float AB = 0.0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  Serial.printf("%f %f %d\n",sin(AB),cos(AB),0);
  AB = AB + 0.2;
  delay(150);
}