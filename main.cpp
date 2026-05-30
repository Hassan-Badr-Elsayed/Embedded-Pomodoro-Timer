#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define redled 9
#define blueled 3
#define greenled 10
#define pushbtn 5
#define pushbtn2 4

#define buzz 6

bool lastbutton = false;
int currentState = 0;             
unsigned long previousMillis = 0; 
unsigned long buzzerMillis = 0;   
int remainingSeconds = 0;         

int cycleCount = 1;               
const int TOTAL_CYCLES = 5;       
bool phaseChanged = false;         
bool buzzerActive = false;         

void initSystem() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WELCOME!");
  lcd.setCursor(0, 1);
  lcd.print("Press to Start");
  
  digitalWrite(blueled, HIGH); 
  digitalWrite(redled, LOW);
  digitalWrite(greenled, LOW);
}

void printTime(int totalSecs) {
  int minutes = totalSecs / 60;
  int seconds = totalSecs % 60;

  lcd.setCursor(0, 1);
  lcd.print("Time: ");
  if (minutes < 10) lcd.print("0");
  lcd.print(minutes);
  lcd.print(":");
  if (seconds < 10) lcd.print("0");
  lcd.print(seconds);
  lcd.print("    "); 
}

void handleBuzzer(unsigned long currentMillis) {
  if (buzzerActive) {
    if (currentMillis - buzzerMillis < 1000) { 
      digitalWrite(buzz, HIGH);
    } else {
      digitalWrite(buzz, LOW);
      buzzerActive = false; 
    }
  }
}

void triggerAlarm(unsigned long currentMillis) {
  buzzerActive = true;
  buzzerMillis = currentMillis; 
}

void handleWelcomeState(bool btn, unsigned long currentMillis) {
  if (btn == 1 && lastbutton == 0) {
      delay(50); 
      currentState = 1; 
      cycleCount = 1;
      remainingSeconds = 25; 
      previousMillis = currentMillis;  
      phaseChanged = true;
  }
  lastbutton = btn; 
}

void handleWorkState(unsigned long currentMillis) {
  digitalWrite(greenled, HIGH);
  digitalWrite(redled, LOW);
  digitalWrite(blueled, LOW);

  if (phaseChanged) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Work Session! S");
    lcd.print(cycleCount);
    phaseChanged = false;
  }
  
  if (currentMillis - previousMillis >= 1000) {
      previousMillis = currentMillis; 
      
      if (remainingSeconds > 0) {
          remainingSeconds--; 
          printTime(remainingSeconds); 
      } else {
          triggerAlarm(currentMillis); 
          currentState = 2;            
          remainingSeconds = 5;        
          phaseChanged = true;
      }
  }
}

void handleBreakState(unsigned long currentMillis) {
  digitalWrite(redled, HIGH);
  digitalWrite(greenled, LOW);
  digitalWrite(blueled, LOW);

  if (phaseChanged) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Break Time!  S");
    lcd.print(cycleCount);
    phaseChanged = false;
  }
  
  if (currentMillis - previousMillis >= 1000) {
      previousMillis = currentMillis;
      
      if (remainingSeconds > 0) {
          remainingSeconds--; 
          printTime(remainingSeconds);
      } else {
          triggerAlarm(currentMillis);
          cycleCount++; 
          
          if (cycleCount >= TOTAL_CYCLES) {
              currentState = 0; 
              initSystem();
          } else {
              currentState = 1; 
              remainingSeconds = 25;
              phaseChanged = true;
          }
      }
  }
}

void setup() {
  lcd.init();
  lcd.backlight();
  
  pinMode(redled, OUTPUT);
  pinMode(blueled, OUTPUT);
  pinMode(greenled, OUTPUT);
  pinMode(buzz, OUTPUT);
  pinMode(pushbtn, INPUT);
  pinMode(pushbtn2, INPUT);
  
	initSystem();
}

void loop() {
     

  unsigned long currentMillis = millis();
  bool btn = digitalRead(pushbtn);
  bool btn2 = digitalRead(pushbtn2);

  handleBuzzer(currentMillis);

  if (currentState == 0) {
      handleWelcomeState(btn, currentMillis);
  } 
  else if (currentState == 1) {
      handleWorkState(currentMillis); 
  } 
  else if (currentState == 2) {
      handleBreakState(currentMillis);
  }
  
  if (btn2 == 1)
  {
   currentState = 0; 
   initSystem();
   cycleCount = 1; 
   remainingSeconds = 25;
   digitalWrite(buzz, LOW); 
   buzzerActive = false;    
  }
 
}
