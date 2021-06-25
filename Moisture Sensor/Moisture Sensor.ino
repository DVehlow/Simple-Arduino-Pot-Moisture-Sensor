#include <Adafruit_NeoPixel.h>

//Variables
struct Sensors
{
  uint8_t LightLevel;
  uint8_t MoistureLevel;
  bool Update;
}SensorValues;

//LDR Setup 
#define LDR_PIN A1
const int Darkness = 40;
const int Lightness = 900;
#define OverBrightPer 80
#define UnderBrightPer 43
#define BRI_LED 1

//Moisture Sensor Setup
#define MOI_PIN A0
const int AirValue = 900;
const int WaterValue = 680;
#define MOI_LED 0
#define OverWaterPer 85
#define UnderWaterPer 20

//LED Colours
#define NeedWater 255,0,0 //Red
#define HappyWater 0,255,0 //Green
#define OverWater 0,0,255 //Blue

#define LowLight 255,85,0
#define HappyLight 255,255,0
#define TooMuchLight 255,0,0

//LED Setup
#define LED_PIN 5
#define NUM_LED 2

Adafruit_NeoPixel pixels(NUM_LED, LED_PIN, NEO_GRB + NEO_KHZ800);

#define UPDATES_PER_SECOND  2

#define DELAYVAL 500

//Function Decleration
void initTimer();
void setMoistureColour(uint8_t MoisturePercentage, uint8_t PixelNumber);
void setBrightColour(uint8_t BrightPercentage, uint8_t PixelNumber);

//Interrupts
ISR(TIMER1_COMPA_vect){
  SensorValues.LightLevel=map(analogRead(LDR_PIN), Darkness, Lightness, 0, 100);
  SensorValues.MoistureLevel=map(analogRead(MOI_PIN), AirValue, WaterValue, 0, 100);
  Serial.print("LDR: ");Serial.print(SensorValues.LightLevel);Serial.print("  MOI: "); Serial.println(analogRead(MOI_PIN));
  //Make loop update LEDs
  SensorValues.Update=1;
}

void setup(){
  Serial.begin(9600);
  //Init Variable Struct
  SensorValues.LightLevel=0;
  SensorValues.MoistureLevel=0;
  SensorValues.Update=0;

  //Init Timer 
  initTimer();

  //Start LEDS
  pixels.begin();
  pixels.setBrightness(125);
}

void loop(){
  if (SensorValues.Update==1)
  {
    pixels.clear();
    setMoistureColour(SensorValues.MoistureLevel,MOI_LED);
    setBrightColour(SensorValues.LightLevel,BRI_LED);
    pixels.show();
  }
}

void initTimer(){
  //Stop Interrupts
  cli();

  //set timer1 interrupt at 1Hz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 15624;// = (16*10^6) / (1*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);

  //Allow Interrupts
  sei();
}

void setMoistureColour(uint8_t MoisturePercentage, uint8_t PixelNumber){
  if (MoisturePercentage>=OverWaterPer){
    pixels.setPixelColor(PixelNumber,OverWater);
  }else if (MoisturePercentage<OverWaterPer && MoisturePercentage>UnderWaterPer){
    pixels.setPixelColor(PixelNumber,HappyWater);
  }else{
    pixels.setPixelColor(PixelNumber,NeedWater);
  }
}

void setBrightColour(uint8_t BrightPercentage, uint8_t PixelNumber){
  if (BrightPercentage>=OverBrightPer){
    pixels.setPixelColor(PixelNumber,TooMuchLight);
  }else if (BrightPercentage<OverBrightPer && BrightPercentage>UnderBrightPer){
    pixels.setPixelColor(PixelNumber,HappyLight);
  }else{
    pixels.setPixelColor(PixelNumber,LowLight);
  }
}