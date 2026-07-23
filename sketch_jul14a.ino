#include <LiquidCrystal.h>
#include <LiquidCrystal_I2C.h>
//rs,rw,enable, d0-d7 | auto sets pinmodes to out
LiquidCrystal lcd(52,11,10,9,8,7,6,5,4,3,2);
LiquidCrystal_I2C i2c_lcd(0x27, 16, 2);

const int SENSOR_PIN = A0;
const int MAX_WATER = 895; 
const int MIN_WATER = 575;

const int SWITCH_PIN = 22;
const int POT_PIN = A1;
const int POT_MAX = 976;
const int POT_MIN = 46;

const int LEN_PIN = ;
const int REN_PIN = ;
const int LPWM_PIN = 12;
const int RPWM_PIN = 13;

// Timer & Tracking Variables
bool last_switch_state = HIGH; // Tracks previous state to detect transitions
unsigned long startTime = 0;
unsigned long elapsedTime = 0;

void setup() {
  Serial.begin(9600); 

  lcd.begin(20,4); lcd.noCursor();
  pinMode(SWITCH_PIN, INPUT_PULLUP); 

  // Initialize I2C LCD
  i2c_lcd.init();
  i2c_lcd.backlight();
  i2c_lcd.noCursor(); // Hides the underline cursor
  i2c_lcd.noBlink();  // Hides the blinking block cursor

  i2c_lcd.setCursor(0, 0);
  i2c_lcd.print("Level: ");
  i2c_lcd.setCursor(0, 1);
  i2c_lcd.print("Time: ");
}

void loop() {
  bool switch_value = digitalRead(SWITCH_PIN);
  int raw_pot = analogRead(POT_PIN);

  int ena_value = constrain(map(raw_pot, POT_MIN, POT_MAX, 0, 255), 0, 255);
  int pot_percent = constrain(map(raw_pot, POT_MIN, POT_MAX,0,100), 0, 100);
  
  //-------------------------------------------------
  // TIMER
  //-------------------------------------------------
  if (switch_value == LOW) {
    // Detect transition from OFF -> ON to reset timer
    if (last_switch_state == HIGH) {
      startTime = millis();
    }
    elapsedTime = millis() - startTime;
  }
  last_switch_state = switch_value;

  controlPump(switch_value, ena_value);
  

  int water_zone = map(waterReading(), 580, 890, 19, 0);
  Serial.println(water_zone);
  display(water_zone);

  delay(1);
}

void controlPump(bool switch_value, int ena_value){
  if(switch_value == HIGH){
    digitalWrite(REN_PIN, LOW);
    digitalWrite(LEN_PIN, LOW);
    analogWrite(RPWM_PIN, 0);
    analogWrite(LPWM_PIN, 0);
  }else{
    digitalWrite(REN_PIN, HIGH);
    digitalWrite(LEN_PIN, HIGH);
    analogWrite(RPWM_PIN, ena_value);
    analogWrite(LPWM_PIN, 0);
  }
}
void display(int water_zone){
  for(int row = 0; row < 4; row++){
    lcd.setCursor(0,row);
    for(int i = 0; i < 20; i++)
      lcd.write(i < water_zone ? 255 : ' ');
  }
}

void displayI2C(int pot_percent, unsigned long total_seconds) {
  // Format MM:SS
  unsigned int minutes = total_seconds / 60;
  unsigned int seconds = total_seconds % 60;

  // Print Potentiometer Percentage
  i2c_lcd.setCursor(5, 0);
  i2c_lcd.print(pot_percent);
  i2c_lcd.print("%   "); // Extra spaces clear leftover characters from 100% -> 9%

  // Print Formatted Time
  i2c_lcd.setCursor(6, 1);
  if (minutes < 10) i2c_lcd.print("0");
  i2c_lcd.print(minutes);
  i2c_lcd.print(":");
  if (seconds < 10) i2c_lcd.print("0");
  i2c_lcd.print(seconds);
}

int waterReading(){
  int sum = 0;
  for(int i = 0; i < 5; i++){
    sum += analogRead(SENSOR_PIN);
    delay(1);
  }
  //average value (sampling)
  return sum/5;
}
