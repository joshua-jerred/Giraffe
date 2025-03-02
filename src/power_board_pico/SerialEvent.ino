String inputString = "";      // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete

int ledState = LOW;
unsigned long previousMillis = 0;
const long interval = 1000;

void processLed() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }

    digitalWrite(LED_BUILTIN, ledState);
  }
}

void processComms() {
  // print the string when a newline arrives:
  if (stringComplete) {
    Serial.println("ack");

    inputString = "";
    stringComplete = false;
  }

  while (Serial.available()) {
    char inChar = (char)Serial.read();

    inputString += inChar;
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  // initialize serial:
  Serial.begin(9600);

  // reserve 200 bytes for the inputString:
  inputString.reserve(200);
}

void loop() {
  processLed();
  processComms();


}



