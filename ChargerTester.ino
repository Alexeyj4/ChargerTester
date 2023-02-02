#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
//#include <FontsRus/TimesNRCyr7.h>//high. not long
#include <FontsRus/CourierCyr8.h>//long. not high
//#include <FontsRus/FreeMono8.h>//long. not high
//#include <FontsRus/FreeSans8.h>//high. not long
//#include <FontsRus/FreeSerif8.h>//high. not long

const float u_min=8.28;
const float u_max=8.5;
const float i_min=1.20;
const float i_max=1.38;

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

bool ce();
float u();
void load(int s);
void u84(int s);


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1); // Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)

void self_test(){
  display.clearDisplay();
  display.display();
  display.setCursor(0,first_string);  
  display.cp437(true);  
  display.println("калибровка..");
  display.display();    
  load(0);
  delay(100);
  u84(1);
  delay(100);
  if(ce()){
  display.println("отключите зу");
  display.display();
    }
  while(ce()){}
  delay(100);
  float ut=u();//u temp
  display.println(ut);    
  u84(0);
  if(ut>u_min and ut<u_max){
    display.println("ok");    
    display.display();
  }else{
    display.println("плох");
    display.display();
    while(1){}
  }
  delay(1000);
}  

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

  display.setFont(&CourierCyr8pt8b);
  display.setTextSize(1);             
  display.setTextColor(WHITE); 
  u();//init
  i();//init
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

void show_welcome(){
  red(0);green(0);
  display.clearDisplay();
  display.setCursor(0,first_string);  
  display.println("подключите");
  display.println("зу");
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
  display.setCursor(0,first_string);
  delay(100);  

  //u test
  load(0);
  delay(500);  
  float ut=u();//u temp
  display.print("U=");
  display.print(ut);
  if(ut>u_min and ut<u_max){
    display.println(" ok");    
  }else{
    display.println(" плох");
    ok=false;
    red(1);green(0);
  }
  display.display();
  delay(250);
    
  //i test
  delay(100);
  load(1);
  delay(100);
  float it=i();//i temp
  load(0);
  display.print("I=");
  display.print(it);
  if(it>i_min and it<i_max){
    display.println(" ok");    
  }else{
    display.println(" плох");
    ok=false;
    red(1);green(0);
  }
  display.display();
  delay(100);

  //ntc test
  bool ntc_ok=true;
//  display.clearDisplay();
//  display.display();
//  display.setCursor(0,first_string); 
  
  //part 1 - ntc u test
  delay(100);
  load(0);
  delay(100);
  ntc(1);
  delay(250);
  ut=u();
  ntc(0);     
  display.print(ut);
  if(ut<u_thr){
    display.println("ntc ok");
  }else{
    display.println("ntc плох");
    ntc_ok=false;
    ok=false;
    red(1);green(0);
  }  
  display.display(); 
  delay(100); 
  
  //part 2 - ntc i test
  delay(100);
  load(1);
  delay(100);
  ntc(1);
  delay(100);
  it=i();
  load(0);
  ntc(0);  
  display.print(it);
    if(it<i_thr){
    display.println("ntc ok");
  }else{
    display.println("ntc плох");
    ntc_ok=false;
    ok=false;
    red(1);green(0);
  }
  
  display.display();
  delay(2000);

  //8.4v test (end of charge)
  display.clearDisplay();
  display.display();  
  display.setCursor(0,first_string);  
  display.println("на зу");
  display.println("зеленый");
  display.println("мигает?");
  display.display();
  while(!btn()){
    if(!ce())return;
  }
  
  display.clearDisplay();
  display.display();
  load(0);
  delay(100);
  u84(1);  
  display.setCursor(0,first_string);  
  display.println("на зу");  
  display.println("зел. горит?");  
  display.print("не мигает?");
  display.display();
  while(!btn()){
    if(!ce()){
      u84(0);
      return;
    }  
  }
  u84(0);  
  display.clearDisplay();  
  display.setCursor(0,first_string); 
  if(ok==true){
    green(1);red(0);
    display.println("тест - ok");
  }else{
    red(1);green(0);
    display.println("тест - плох");
  }    
  display.display();
  while(ce()){};
  red(0);green(0); 
}

void loop() {  
  // put your main code here, to run repeatedly:
  u84(0);
  self_test();
  show_welcome();
  while(!ce()){u84(0);}
  test();  
}
