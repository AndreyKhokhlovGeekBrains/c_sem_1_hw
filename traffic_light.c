int buttonPin = 2;        // Button for pedestrians
int carGreenPin = 13;     // Green LED for cars
int carYellowPin = 12;    // Yellow LED for cars
int carRedPin = 11;       // Red LED for cars
int pedGreenPin = 10;     // Green LED for pedestrians
int pedRedPin = 9;        // Red LED for pedestrians

int buttonState = 0;
int lastButtonState = 0;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

unsigned long lastStateChangeTime = 0;  // Time when the last state change occurred
unsigned long redLightDuration = 5000;  // Red light duration (5 seconds)
unsigned long blinkingYellowTime = 1500; // Time for yellow blinking (1.5 seconds per blink)
unsigned long totalBlinkingTime = 4500; // Total time for yellow blinking (3 blinks)

bool pedestrianButtonPressed = false;
bool buttonEnabled = true;

enum State {CAR_GREEN, CAR_YELLOW_BLINKING, CAR_RED, PED_GREEN, WAIT_AFTER_GREEN};
State currentState = CAR_GREEN;

void setup() {
  pinMode(carGreenPin, OUTPUT);
  pinMode(carYellowPin, OUTPUT);
  pinMode(carRedPin, OUTPUT);
  pinMode(pedGreenPin, OUTPUT);
  pinMode(pedRedPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP); // Using internal pull-up resistor

  // Initial states: cars green, pedestrians red
  digitalWrite(carGreenPin, HIGH);  
  digitalWrite(pedRedPin, HIGH);    
  Serial.begin(9600);
}

void loop() {
  int reading = digitalRead(buttonPin);

  // Debounce logic
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading == LOW && buttonEnabled) {  // Button pressed (active LOW with pull-up resistor)
      pedestrianButtonPressed = true;
      buttonEnabled = false;  // Disable the button until the next cycle
    }
  }

  lastButtonState = reading;

  switch (currentState) {
    case CAR_GREEN:
      if (pedestrianButtonPressed) {
        pedestrianButtonPressed = false;
        currentState = CAR_YELLOW_BLINKING;
        lastStateChangeTime = millis();
        digitalWrite(carGreenPin, LOW);  // Turn off the car green light
      }
      break;

    case CAR_YELLOW_BLINKING:
      if (millis() - lastStateChangeTime < totalBlinkingTime) {
        blinkYellow();
      } else {
        currentState = CAR_RED;
        lastStateChangeTime = millis();
        digitalWrite(carYellowPin, LOW);  // Turn off the yellow light
        digitalWrite(carRedPin, HIGH);    // Turn on the car red light
        digitalWrite(pedRedPin, LOW);     // Turn off pedestrian red light
        digitalWrite(pedGreenPin, HIGH);  // Turn on pedestrian green light
      }
      break;

    case CAR_RED:
      if (millis() - lastStateChangeTime > redLightDuration) {  // Car red light duration
        currentState = PED_GREEN;
        lastStateChangeTime = millis();
      }
      break;

    case PED_GREEN:
      if (millis() - lastStateChangeTime > redLightDuration) {  // Pedestrian green light duration
        currentState = WAIT_AFTER_GREEN;
        lastStateChangeTime = millis();
        digitalWrite(pedGreenPin, LOW);  // Turn off pedestrian green light
        digitalWrite(pedRedPin, HIGH);   // Turn on pedestrian red light
        digitalWrite(carRedPin, LOW);    // Turn off car red light
        digitalWrite(carGreenPin, HIGH);  // Turn on car green light
      }
      break;

    case WAIT_AFTER_GREEN:
      if (millis() - lastStateChangeTime > 5000) {  // Additional delay after green light ends
        currentState = CAR_GREEN;
        buttonEnabled = true;  // Re-enable the button for the next cycle
      }
      break;
  }
}

void blinkYellow() {
  if ((millis() / 500) % 2 == 0) {  // Blinking every 500ms
    digitalWrite(carYellowPin, HIGH);
  } else {
    digitalWrite(carYellowPin, LOW);
  }
}
