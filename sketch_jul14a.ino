#include <LiquidCrystal.h>

//rs,rw,enable, d0-d7 | auto sets pinmodes to out
LiquidCrystal lcd(12,11,10,9,8,7,6,5,4,3,2);

const int SENSOR_PIN = A0;
const int MAX_WATER = 895; 
const int MIN_WATER = 575;

void setup() {
  Serial.begin(9600); 
  lcd.begin(20,4); lcd.noCursor();
}

void loop() {
  int water_zone = map(waterReading(), 580, 890, 19, 0);
  Serial.println(water_zone);
  display(water_zone);
}

void display(int water_zone){
  for(int row = 0; row < 4; row++){
    lcd.setCursor(0,row);
    for(int i = 0; i < 20; i++)
      lcd.write(i < water_zone ? 255 : ' ');
  }
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