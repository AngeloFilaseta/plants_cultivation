#define ONE_MINUTE 60000 //60 seconds delay
#define BOUNCING_DELAY 1000

//BUTTTONS PINS
#define STATE_BUTTON 2

// SENSORS PINS
#define DHT22_PIN 13
#define WATER_PUMP_PIN 7
#define STRIP_LED_PIN 9
#define CS_MOISTURE_PIN A3

//LCD PINS
#define A_LCD A0

#define ERROR_IN_SENSOR -999
#define WATER_NEEDED_HUM 33
#define MAX_TEMP 30
#define MIN_TEMP 20
#define MAX_HUM  70
#define MIN_HUM 40

//TIME VALUES
#define WATER_CHECK_MILLIS 3600000 // 1 hour
#define PWM_CONTRAST 85
#define VEGETATIVE_DAY 57600000  // 16 hours
#define VEGETATIVE_NIGHT 28800000  // 8 hours

#define FLOWERING_DAY 43200000  //12 hours
#define FLOWERING_NIGHT 43200000  //12 hours

//VALUES
#define WET_VALUE 610
#define DRY_VALUE 860

#define DHTTYPE DHT22

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <DS1307RTC.h>
#include <DHT.h>
#include "SoilSensor.h"
#include "WaterPump.h"
#include "DigitalLed.h"

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
DHT dht(DHT22_PIN, DHTTYPE);
SoilSensor* soil_sensor = new SoilSensor(CS_MOISTURE_PIN);
WaterPump* water_pump = new WaterPump(WATER_PUMP_PIN);
DigitalLed* strip_led = new DigitalLed(STRIP_LED_PIN);

float temp = 0;
float air_hum = 0;
int soil_hum = 0;
char flowering_state = 0;
char night_mode = 0;
int button_state = 0;

unsigned long actual_day_time = VEGETATIVE_DAY;
unsigned long actual_night_time = VEGETATIVE_NIGHT;

//Used for bouncing management
unsigned long last_time_pressed = 0;
//Used to check water once in a while
unsigned long last_check_water_millis = 0;
//Used to check light once in a while
unsigned long last_check_light_millis = 0;

unsigned int last_check_light_millis_diff = 0;
unsigned long int last_check_light_minutes= 0;
unsigned long int last_time_day = 0;
unsigned long int last_time_night = 0;

short int day_minutes = (actual_day_time / 60000) % 60;
short int day_hours = (actual_day_time / 60000) /60;
short int night_minutes = (actual_night_time / 60000) % 60;
short int night_hours = (actual_night_time / 60000) /60;

void setup()
{
  Serial.begin(9600);
  lcd.begin(20,4); 
  dht.begin();
  water_pump -> init();
  attachInterrupt(digitalPinToInterrupt(STATE_BUTTON), change_light, FALLING);
  pinMode(STATE_BUTTON, INPUT);
  pinMode(WATER_PUMP_PIN, OUTPUT);
  pinMode(STRIP_LED_PIN, OUTPUT);
}

/**
 * Main Loop
 */
void loop()
{
  air_hum = dht.readHumidity();
  temp= dht.readTemperature();
  soil_hum = soil_sensor -> get_humidity();
  manage_water(soil_hum);
  manage_light();
  print_data_on_lcd(temp, air_hum, soil_hum);
  print_data_on_serial(temp, air_hum, soil_hum);
  delay(ONE_MINUTE);
}

/**
 * Print interesting data on lcd.
 */
void print_data_on_lcd(float temp, float air_hum, int soil_hum)
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp:");
  if(isnan(temp))
  {
    lcd.print("ERR");
  }
  else
  {
    lcd.print(round(temp));
    lcd.print("C");
  }
  lcd.print(" Hum:");
  if(isnan(air_hum))
  {
    lcd.print("ERR");
  } 
  else
  {
    lcd.print(round(air_hum));
    lcd.print("%");
  }
  lcd.setCursor(0, 2);
  lcd.print("Soil:");
  lcd.print(soilHumidityMapped(soil_hum));
  lcd.print("%");
  lcd.print(" (");
  lcd.print(soil_hum);
  lcd.print(")");
  lcd.setCursor(0, 3);
  if(night_mode)
  {
    lcd.print("OFF ");
  }else
  {
    lcd.print("ON ");
  }
  lcd.print(": ");
  if(last_check_light_minutes/60 > 0)
  {
    lcd.print(last_check_light_minutes / 60);
    lcd.print("h");
  }
  lcd.print(last_check_light_minutes % 60);
  lcd.print("m / ");
  if(night_mode)
  {
    lcd.print(night_hours);
    lcd.print("h");
    lcd.print(night_minutes);
    lcd.print("m");
  }
  else
  {
    lcd.print(day_hours);
    lcd.print("h");
    lcd.print(day_minutes);
    lcd.print("m");
  }
}

/**
 * Print interesting data on serial.
 */
void print_data_on_serial(float temp, float air_hum, int soil_hum)
{
  Serial.print("\nTemperature: ");
  if(isnan(temp))
  {
    Serial.print("Error");
  }
  else
  {
    Serial.print(temp);
    Serial.print("C°");
  }
 
  Serial.print("\nAir Humidity: ");
  if(isnan(air_hum))
  {
    Serial.print("Error");
  }
  else
  {
    Serial.print(air_hum);
    Serial.print("%");
  }
  Serial.print("\nSoil:");
  Serial.print(soilHumidityMapped(soil_hum));
  Serial.print("%");
  Serial.print(" (");
  Serial.print(soil_hum);
  Serial.print(")");

  Serial.print("\nLights: ");
  if(flowering_state)
  {
    Serial.print("F");
  }
  else
  {
    Serial.print("V");
  }
  Serial.print("\nNight Mode: ");
  if(night_mode){
    Serial.print("true");
  }else{
    Serial.print("false");
  }
  
}

/**
 * Water the plants if the soil humidity is too dry. The check is done every WATER_CHECK_MILLIS milliseconds.
 */
void manage_water(int soil_hum)
{
  if ((unsigned long)(millis() - last_check_water_millis) >= WATER_CHECK_MILLIS) 
  {
    if(soil_hum <= WATER_NEEDED_HUM)
    {
      water_pump -> send_water();  
    }
    last_check_water_millis = millis();
  }
}

/**
 * Manage the light. Check if we are in flowering or vegetative state. Change the mode if needed and turn on or off the led strip.
 */
void manage_light()
{
  swap_light_mode_if_needed();
  if(night_mode){
    strip_led->off();
  } else {
    strip_led->on();
  }
}

void update_light()
{
  last_check_light_millis = millis();
  change_light();
}


void swap_light_mode_if_needed()
{
 
  if(night_mode) //NIGHT MODE
  {
    last_check_light_minutes = (millis() - last_time_day) / ONE_MINUTE;
    if(millis() - last_check_light_millis > actual_night_time){
      update_light();
    }
  }
  else //DAY MODE
  {
      last_check_light_minutes = (millis() - last_time_night) / ONE_MINUTE;
     if(millis() - last_check_light_millis > actual_day_time){
      update_light();
    }
  }
}

void change_light()
{
  Serial.print("Change Light Called");
  if(millis() - last_time_pressed > BOUNCING_DELAY){
    last_time_pressed = millis();
    night_mode = !night_mode;
    last_time_day = millis();
    last_time_night = millis();

  }
}

int soilHumidityMapped(int analog_value)
{
    int soilMoistureValue = map(analog_value , WET_VALUE, DRY_VALUE, 100, 0);
    if(soilMoistureValue > 100){
      soilMoistureValue = 100;
    }
    else if(soilMoistureValue < 0)
    {
      soilMoistureValue = 0;
    }
    return soilMoistureValue;
}
