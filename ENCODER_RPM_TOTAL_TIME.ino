const int encoder_a = 2; // Pin 2
const int encoder_b = 3; // Pin 3
volatile int encoder_pulse_counter = 0;
volatile bool direction = false;
volatile uint32_t lastMicro = 0;
volatile uint32_t pulseLength = 0;
volatile bool motorActive = false; // Variable to track motor activity
uint32_t motorStartTime = 0; // Timestamp of motor start time
uint32_t motorStopTime = 0;  // Timestamp of motor stop time

void encoderPinChangeA() {
  if (!encoder_pulse_counter) {
    lastMicro = micros();
    motorActive = true; // Motor is active
    motorStartTime = millis(); // Record motor start time
  }
  encoder_pulse_counter += 1;
  if (encoder_pulse_counter == 360) {
    pulseLength = micros() - lastMicro;
    encoder_pulse_counter = 0;
  }
  direction = digitalRead(encoder_a) == digitalRead(encoder_b);
}

void setup() {
  Serial.begin(115200);
  pinMode(encoder_a, INPUT_PULLUP);
  pinMode(encoder_b, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), encoderPinChangeA, RISING);
}

void printTimestamp(uint32_t timestamp) {
  int hours = timestamp / 3600000;
  int minutes = (timestamp % 3600000) / 60000;
  int seconds = (timestamp % 60000) / 1000;
  int milliseconds = timestamp % 1000;

  Serial.print(hours);
  Serial.print(':');
  if (minutes < 10) Serial.print('0');
  Serial.print(minutes);
  Serial.print(':');
  if (seconds < 10) Serial.print('0');
  Serial.print(seconds);
  Serial.print('.');
  if (milliseconds < 10) Serial.print("00");
  else if (milliseconds < 100) Serial.print('0');
  Serial.print(milliseconds);
}

void loop() {
  if (motorActive) {
    static uint32_t temp = 0;
    noInterrupts();
    temp = pulseLength;
    motorActive = false; // Reset motor activity flag
    motorStopTime = millis(); // Record motor stop time
    interrupts();
    Serial.print("\n Motor Start Time: ");
    printTimestamp(motorStartTime);
    Serial.print("\n Motor Stop Time: ");
    printTimestamp(motorStopTime);
    Serial.print("\n Rotation Per Second = ");
    if (direction) Serial.print(" Clockwise:: ");
    Serial.println((float)1000000 / (float)temp, 2);
  }
  delay(1000);
}
