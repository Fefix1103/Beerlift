// Pins
#define S1 6
#define S_TOP 2
#define S_BOT 3
#define RELAIS_UP 4
#define RELAIS_DOWN 5
#define LED 13

// Flags
bool IsMotorRunningUp = false;
bool IsMotorRunningDown = false;
bool IsLiftOnTop = false;
bool IsLiftOnBottom = false;

void setup() {
  // Set pullups for buttons
  pinMode(S1, INPUT_PULLUP);
  pinMode(S_TOP, INPUT_PULLUP);
  pinMode(S_BOT, INPUT_PULLUP);

  // Debug LED
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  // Attach interrupts
  //attachInterrupt(digitalPinToInterrupt(S1), BeerButtonPressed, FALLING);
  attachInterrupt(digitalPinToInterrupt(S_TOP), BeerReachedTheTop, FALLING);
  attachInterrupt(digitalPinToInterrupt(S_BOT), BeerHitRockBottom, FALLING);

  // Set outputs for relais
  pinMode(RELAIS_UP, OUTPUT);
  pinMode(RELAIS_DOWN, OUTPUT);

  // Set outputs to low
  StopMotor();
}

void loop() {
  // Check S1
  if (digitalRead(S1) == LOW) {
    digitalWrite(LED, HIGH);
    BeerButtonPressed();
  }
  else {
    digitalWrite(LED, LOW);
  }

  // Don't search the beer when motor is running
  if (IsMotorRunning()) {
    return;
  }

  // Check position every 3 seconds and send it back down if position is unknown
  WhereIsTheBeer();
}

void BeerButtonPressed() {
  // When position is unknown don't do anything
  if (!IsLiftOnTop && !IsLiftOnBottom) {
    return;
  }

  // When motor is running switch direction
  if (IsMotorRunning()) {
    switch(IsMotorRunningUp) {
      case true:
        HideTheBeer();
        break;
      case false:
        ShowTheBeer();
        break;
    }
  }
  
  // When motor isn't running send beer either up or down
  switch(IsLiftOnTop) {
    case true:
      HideTheBeer();
      break;
    case false:
      ShowTheBeer();
      break;
  }
}

void BeerReachedTheTop() {
  StopMotor();
  SwitchLiftPosition(true);
}

void BeerHitRockBottom() {
  StopMotor();
  SwitchLiftPosition(false);
}

void SwitchLiftPosition(bool isLiftOnTop) {
  IsLiftOnTop = isLiftOnTop;
  IsLiftOnBottom = !isLiftOnTop;
}

void StopMotor() {
  digitalWrite(RELAIS_UP, LOW);
  digitalWrite(RELAIS_DOWN, LOW);
  IsMotorRunningUp = false;
  IsMotorRunningDown = false;
}

void RunMotor(bool runUpwards) {
  IsMotorRunningUp = runUpwards;
  IsMotorRunningDown = !runUpwards;
  
  switch(runUpwards) {
    case true:
      digitalWrite(RELAIS_UP, HIGH);
      digitalWrite(RELAIS_DOWN, LOW);
      break;
    case false:
      digitalWrite(RELAIS_UP, LOW);
      digitalWrite(RELAIS_DOWN, HIGH);
      break;  
  }
}

void HideTheBeer() {
  RunMotor(false);
}

void ShowTheBeer() {
  RunMotor(true);
}

void WhereIsTheBeer() {
  // Check only if position is unknown
  if (IsLiftOnTop != IsLiftOnBottom) {
    return;
  }

  // Check buttons for position
  if (digitalRead(S_TOP) == LOW) {
    SwitchLiftPosition(true);
    return;
  }
  else if (digitalRead(S_BOT) == LOW) {
    SwitchLiftPosition(false);
    return;
  }

  // Send beer down if position is unknown
  else if (IsMotorRunning() == false) {
    HideTheBeer();
  }
}

bool IsMotorRunning() {
  return IsMotorRunningUp && IsMotorRunningDown;
}
