// === STATE VARIABLES ===
struct LEDController {
  unsigned long lastToggleTime;
  unsigned long blinkInterval;
  byte outputState;
  byte pinNumber;
};

LEDController led1 = {0, 1000, LOW, 8};
LEDController led2 = {0, 1000, LOW, 9};

struct UserInput {
  int targetLED;
  int newInterval;
};

UserInput userCmd = {0, 0};

// === TASK DECLARATIONS ===
void processSerialCommands();
void updateLED1();
void updateLED2();

// === TASK SCHEDULER CONFIGURATION ===
void (*scheduledTasks[])() = {
  processSerialCommands,
  updateLED1,
  updateLED2
};

const byte TASK_COUNT = sizeof(scheduledTasks) / sizeof(scheduledTasks[0]);

// === INITIALIZATION ===
void setup() {
  pinMode(led1.pinNumber, OUTPUT);
  pinMode(led2.pinNumber, OUTPUT);
  Serial.begin(9600);
  
  led1.lastToggleTime = millis();
  led2.lastToggleTime = millis();
}

// === CYCLIC EXECUTIVE SCHEDULER ===
void loop() {
  // Execute all tasks in round-robin fashion
  for (byte taskIndex = 0; taskIndex < TASK_COUNT; taskIndex++) {
    scheduledTasks[taskIndex]();
  }
}

// === TASK IMPLEMENTATIONS ===

void processSerialCommands() {
  if (Serial.available() <= 0) return;
  
  userCmd.targetLED = requestInteger("Select LED (1 or 2):");
  userCmd.newInterval = requestInteger("Enter blink period (ms):");

  switch (userCmd.targetLED) {
    case 1:
      led1.blinkInterval = userCmd.newInterval;
      break;
    case 2:
      led2.blinkInterval = userCmd.newInterval;
      break;
  }
}

void updateLED1() {
  unsigned long currentTime = millis();
  
  if ((currentTime - led1.lastToggleTime) >= led1.blinkInterval) {
    led1.lastToggleTime = currentTime;
    led1.outputState = (led1.outputState == LOW) ? HIGH : LOW;
    digitalWrite(led1.pinNumber, led1.outputState);
  }
}

void updateLED2() {
  unsigned long currentTime = millis();
  
  if ((currentTime - led2.lastToggleTime) >= led2.blinkInterval) {
    led2.lastToggleTime = currentTime;
    led2.outputState = (led2.outputState == LOW) ? HIGH : LOW;
    digitalWrite(led2.pinNumber, led2.outputState);
  }
}

// === UTILITY FUNCTIONS ===
int requestInteger(const char* message) {
  Serial.println(message);
  
  while (Serial.available() == 0) {
    // Wait for input
  }
  
  int value = Serial.parseInt();
  
  // Clear input buffer
  while (Serial.available() > 0) {
    Serial.read();
  }
  
  return value;
}
