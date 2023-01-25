#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeSerif9pt7b.h>

const float u_min=8.3;
const float u_max=8.4;
const float i_min=1.20;
const float i_max=1.30;

const float u_thr=6.9;  //threshold when thermal protection on (NTC on)
const float i_thr=0.05; //threshold when thermal protection on (NTC on)


const float u_coef=0.0120317;  //u_float=u_meas*8.35/694
const float i_coef=0.0014339;  //i_float=i_meas*1.236/862

const int ce_pin=2; //(D2)ce pin from charger test
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
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  pinMode(ce_pin, INPUT_PULLUP); 
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
}

bool btn(){  //if button pressed return true
  if(digitalRead(btn_pin)==HIGH) return false;
    else return true;
}

bool ce(){  //test CE (charge enable) signal from charger
  if(digitalRead(ce_pin)==HIGH) return false;
    else return true;  
}

void red(int s){  //on/off red led
  digitalWrite(led_r_pin,s);
}

void green(int s){  //on/off green led
  digitalWrite(led_g_pin,s);
}

void u84(int s){  //on/off 8.4v supply
  digitalWrite(u84_pin,s);
}  

void load(int s){ //on/off load
  digitalWrite(load_on_pin,s);
}

void ntc(int s){ //on/off NTC pin resistor for test thermal protection
  digitalWrite(ntc_pin,s);
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
  display.print(digitalRead(ce_pin));
  display.display();
  
}

void show_welcome(){
  display.clearDisplay();
  display.setCursor(0,first_string);  
  display.println("Connect");
  display.println("to charger");
  display.display();
}

float u(){  //measure U and transform to volts
  return analogRead(u_meas_pin)*u_coef;  
}

float i(){  //measure I and transform to amperes
  return analogRead(i_meas_pin)*i_coef;  
}

void test(){
  u();//init
  i();//init
  red(0);green(0);
  bool ok=true;
  display.clearDisplay();
  display.display();
  delay(500);  

  //u test
  load(0);
  delay(500);  
  float ut=u();//u temp
  display.setCursor(0,first_string);  
  display.print("U=");
  display.print(ut);
  if(ut>u_min and ut<u_max){
    display.print(" ok");    
  }else{
    display.print(" bad");
    ok=false;
    red(1);green(0);
  }
  display.display();
  delay(500);
    
  //i test
  delay(500);
  load(1);
  delay(500);
  float it=i();//i temp
  load(0);
  display.setCursor(0,second_string);  
  display.print("I=");
  display.print(it);
  if(it>i_min and it<i_max){
    display.print(" ok");    
  }else{
    display.print(" bad");
    ok=false;
    red(1);green(0);
  }
  display.display();
  delay(500);

  //ntc test
  bool ntc_ok=true;
  display.clearDisplay();
  display.display();
  
  //ntc u test
  delay(500);
  load(0);
  delay(500);
  ntc(1);
  delay(500);
  ut=u();
  ntc(0);  
  display.setCursor(0,first_string);  
  display.print("U=");
  display.print(ut);
  if(ut<u_thr){
    display.print(" ntc ok");
  }else{
    display.print(" ntc bad");
    ntc_ok=false;
    ok=false;
    red(1);green(0);
  }  
  display.display(); 
  delay(500); 
  
  //ntc i test
  delay(500);
  load(1);
  delay(500);
  ntc(1);
  delay(500);
  it=i();
  load(0);
  ntc(0);
  display.setCursor(0,second_string);  
  display.print("I=");
  display.print(it);
    if(it<i_thr){
    display.print(" ntc ok");
  }else{
    display.print(" ntc bad");
    ntc_ok=false;
    ok=false;
    red(1);green(0);
  }
  
  display.display();
  delay(500);

  //8.4v test (end of charge)
  display.clearDisplay();
  display.display();
  load(0);delay(100);u84(1);  
  display.setCursor(0,first_string);  
  display.print("zel gorit?");
  display.setCursor(0,second_string);  
  display.print("ne migaet?");
  display.display();
  while(ce()){
    if(btn()){
      u84(0);
      if(ok==true){
        green(1);red(0);
      }else{
        red(1);green(0);
      }
      delay(500);
      return;      
    }
  }
  u84(0);     
  
}

void loop() {  
  // put your main code here, to run repeatedly:
  u84(0);
  show_welcome();
  while(!ce()){u84(0);}
  test();  
}
