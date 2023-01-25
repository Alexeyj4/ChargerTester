#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeSerif9pt7b.h>

const int ce_meas_pin=2; //(D2)ce pin from charger test
const int i_meas_pin=14; //(A0) I charge measure
const int u_meas_pin=15; //(A1) U charge measure 
const int load_on_pin=3; //(D3) load control
const int btn_pin=5; //(D5) button pin
const int ntc_pin=4; //(D4) ntc-resistor test pin
const int u84_pin=8; //(D8) u 8.4v supply pin
const int led_r_pin=7; //(D7) red led pin
const int led_g_pin=6; //(D6) green led pin

int load_on_flag=1; //0-off 1-on

const int SCREEN_WIDTH=128; // OLED display width, in pixels
const int SCREEN_HEIGHT=64; // OLED display height, in pixels
const int first_string=12;  //first string on LCD
const int second_string=28;  //second string on LCD
const int third_string=44;  //third string on LCD
const int fourth_string=62;  //fourth string on LCD

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1); // Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)

void setup() { 
  // put your setup code here, to run once: 
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println("SSD1306 allocation failed");
    for(;;);
  }
  
  pinMode(ce_meas_pin, INPUT_PULLUP); 
  pinMode(i_meas_pin, INPUT);
  pinMode(u_meas_pin, INPUT);
  pinMode(load_on_pin, OUTPUT);  
  pinMode(ntc_pin, OUTPUT); 
  pinMode(btn_pin, INPUT_PULLUP); 
  pinMode(u84_pin, OUTPUT);
  pinMode(led_r_pin, OUTPUT);
  pinMode(led_g_pin, OUTPUT);
  
  analogReference(INTERNAL); //1,1V internal

  display.setFont(&FreeSerif9pt7b);
  display.setTextSize(1);             
  display.setTextColor(WHITE);  

  load(0);
  delay(200);
  u84(1);

         
}

int btn(){  //if button pressed return 1
  if(digitalRead(btn_pin)==HIGH) return false;
    else return true;
}

void red(int i){  //on/off red led
  digitalWrite(led_r_pin,i);
}

void green(int i){  //on/off green led
  digitalWrite(led_g_pin,i);
}

void u84(int i){  //on/off 8.4v supply
  digitalWrite(u84_pin,i);
}  

void load(int i){ //on/off load
  digitalWrite(load_on_pin,i);
}

void show(){
  display.clearDisplay();
  display.setCursor(0,first_string);  
  display.print("U=");
  display.print(analogRead(u_meas_pin));
  display.setCursor(0,second_string); 
  display.print("I=");
  display.print(analogRead(i_meas_pin));
  display.setCursor(0,third_string);
  display.print("CE=");
  display.print(digitalRead(ce_meas_pin));
  display.display();
  
}

void loop() {  
  // put your main code here, to run repeatedly:
  show();

//  if(btn()){
//    red(1);
//    load(0);
//    delay(200);
//    green(1);
//    u84(1);
//    delay(200);
//    show();
//    delay(3000); 
//    u84(0);
//    red(0);
//    green(0);  
//    
//  }
  

  
}
