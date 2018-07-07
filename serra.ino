#include <Wire.h>
#include <CCS811.h>
#include <LiquidCrystal.h>
#include <Time.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include "Timer.h"
#include <VSync.h>


#define ADDR              0x5B
#define WAKE_PIN          4
#define DHTPIN            2         
#define DHTTYPE           DHT22  
#define LED               13
#define pinLuce     8
#define pinPompa    9
#define pinVentola  5



int sensVal;
int secco;
int t_secco;
int ore_luce_accesa = 12;
int t_ore_luce_accesa=12;
int ore_luce_spenta = 12;
int t_ore_luce_spenta=12;
int minuti_pompa_accesa = 5;
int t_minuti_pompa_accesa=5;
int minuti_pompa_spenta = 25;
int t_minuti_pompa_spenta=25;
int tastiera = 0;
boolean modalita;
int t_modalita;
boolean timer_luce1;
boolean timer_pump2;  
int t_timer_luce1;
int t_timer_pump2;
Timer timer_luce;
Timer timer_pump;
boolean t_ventola;
int d_ventola;
CCS811 sensor;
DHT_Unified dht(DHTPIN, DHTTYPE);
uint32_t delayMS;
int CO2;
ValueSender<12> sender;
ValueReceiver<8> receiver;

int tt;

float ttt;

//--------------------------------------------------------------------------------------------------------------------------Void Setup------------------------------------------------------------------------------------------------------------------------------------------

void setup() {
  Serial.begin(19200);
  dht.begin();  
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  
  dht.humidity().getSensor(&sensor);
  
  receiver.observe(ore_luce_accesa);
  receiver.observe(ore_luce_spenta);
  receiver.observe(minuti_pompa_accesa);
  receiver.observe(minuti_pompa_spenta);
  receiver.observe(t_modalita);
  receiver.observe(t_timer_luce1);
  receiver.observe(t_timer_pump2);
  receiver.observe(d_ventola);

  sender.observe(t_ore_luce_accesa);
  sender.observe(t_ore_luce_spenta);
  sender.observe(t_minuti_pompa_accesa);
  sender.observe(t_minuti_pompa_spenta);
  sender.observe(t_modalita);
  sender.observe(CO2);
  sender.observe(tt);
  sender.observe(t_timer_luce1);
  sender.observe(t_timer_pump2);
  sender.observe(t_secco);




  timer_luce.after(ore_luce_accesa*3600000,luce);
  timer_pump.after(minuti_pompa_accesa*60000,pump);
  



  pinMode(pinVentola, OUTPUT);
  pinMode(pinLuce, OUTPUT);
  pinMode(LED,OUTPUT);
  pinMode(pinPompa, OUTPUT);
  digitalWrite(LED,LOW);
  pinMode(pinPompa, OUTPUT);
  
  
  
  timer_luce1=true;
  timer_pump2=true;
  t_timer_luce1=1;
  t_timer_pump2=1;
  
  modalita=false;
  t_modalita=0;
}

//--------------------------------------------------------------------------------------------------------------------------Void Loop-------------------------------------------------------------------------------------------------------------------------------------------


void loop() {
  receiver.sync();
  modalita=boolean(t_modalita);
  
  
  if (modalita==true) {
    automatic(); 
    digitalWrite(LED,HIGH);

    timer_pump.update();
    timer_luce.update();


  }
  if (modalita==false) {
    digitalWrite(LED,LOW);
    timer_luce1=boolean(t_timer_luce1);
    timer_pump2=boolean(t_timer_pump2);
    
    
    manual();
  }
  t_ore_luce_accesa=ore_luce_accesa;
  t_ore_luce_spenta=ore_luce_spenta;
  t_minuti_pompa_accesa=minuti_pompa_accesa;
  t_minuti_pompa_spenta=minuti_pompa_spenta;
  t_modalita=int(modalita);
  tt = ttt*10;
  t_secco=secco;
  tastiera=tastiera;
  delay(1000);
  
  sender.sync();


}

//--------------------------------------------------------------------------------------------------------------------------Ciclo Automatico------------------------------------------------------------------------------------------------------------------------------------

void automatic() {
 
  sensVal = analogRead(A0);
  secco = map(sensVal, 0, 1023
 , 0, 100);
  sensors_event_t event;  
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    ttt=-1;
  }
  else {
    ttt=event.temperature;
  }
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
  
  }
  else {

  }
  sensor.getData();


  CO2 = sensor.readCO2();


  
  if(CO2>400) {
    digitalWrite(pinVentola,LOW); 
  }
  if(CO2<=400) {
    digitalWrite(pinVentola,HIGH); 
  }
  if(ttt>=30.00) {
    digitalWrite(pinVentola,HIGH);
  }
  else
  {
    digitalWrite(pinVentola,LOW); 
  }

  ("ore pompa accesa"); Serial.println(minuti_pompa_accesa);
  
  delay(2000);
}

//--------------------------------------------------------------------------------------------------------------------------Ciclo Manuale--------------------------------------------------------------------------------------------------------------------------------------

void manual() {
  t_ventola=boolean(d_ventola);
  
  if (timer_luce1==false) { 
    digitalWrite(pinLuce,HIGH);
  }
  if (timer_luce1==true) {
    digitalWrite(pinLuce,LOW);
  }
  if (timer_pump2==false) { 
    digitalWrite(pinPompa,HIGH);
  }
  if (timer_pump2==true) {
    digitalWrite(pinPompa,LOW);
  }
  if (t_ventola==true) { 
    digitalWrite(pinVentola,HIGH);
  }
  if (t_ventola==false) {
    digitalWrite(pinVentola,LOW);
  }
  sensor.getData();
  CO2 = sensor.readCO2();
  sensors_event_t event;  
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    ttt=-1;
  }
  else {
    ttt=event.temperature;
  }
  sensVal = analogRead(A0);
  secco = map(sensVal, 0, 1023, 0, 100);
}

//--------------------------------------------------------------------------------------------------------------------------Ciclo Luce------------------------------------------------------------------------------------------------------------------------------------------


}
void luce() {
  if (timer_luce1==true) {
    digitalWrite(pinLuce,HIGH);
    digitalWrite(LED,LOW);
    timer_luce1=false;
    timer_luce.after(ore_luce_spenta*3600000,luce);
  }
  else {
    digitalWrite(LED,HIGH);
    timer_luce1=true;
    timer_luce.after(ore_luce_accesa*3600000,luce);

  }
}

//--------------------------------------------------------------------------------------------------------------------------Ciclo pompa----------------------------------------------------------------------------------------------------------------------------------------

void pump(){
  if (timer_pump2==true) {
    digitalWrite(pinPompa,HIGH);
    timer_pump2=false;
    timer_pump.after(minuti_pompa_spenta*60000,pump);
  }
  else {
    digitalWrite(pinPompa,LOW);
    timer_pump2=true;
    timer_pump.after(minuti_pompa_accesa*60000,pump);

  }


}