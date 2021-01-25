/* LCD wiring:
VSS -> GND
VDD -> +5v                
V0 -> Arduino D3
RS -> Arduino D12
R/W -> GND
E -> Arduino D11
D0 -> N/C
D1 -> N/C
D2 -> N/C
D3 -> N/C
D4 -> Arduino D2
D5 -> Arduino D3
D6 -> Arduino D4
D7 -> Arduino D5
A -> Arduino A0
K -> GND
*/
#include <LiquidCrystal.h>;

#define DELTA_DELAY 60000 //60 seconds delay
#define BOUNCING_DELAY 700

//BUTTTONS PINS
#define STATE_BUTTON 2

// SENSORS PINS
#define DHT22_PIN 13
#define WATER_PUMP_PIN 8
#define STRIP_LED_PIN 9
#define CS_MOISTURE_PIN A1

//LCD PINS
#define V0_LCD 3
#define D4_LCD 4
#define D5_LCD 5
#define D6_LCD 6
#define D7_LCD 7
#define RS_LCD 12
#define E_LCD 11
#define A_LCD A0

//VALUES
#define ERROR_IN_SENSOR -999
#define WATER_NEEDED_HUM 33
#define MAX_TEMP 30
#define MIN_TEMP 20
#define MAX_HUM  70
#define MIN_HUM 40

//TIME VALUES
#define WATER_CHECK_MILLIS 18000000 // 5 hours

#define VEGETATIVE_DAY 720000000  //20 hours
#define VEGETATIVE_NIGHT 4400000  //4 hours

#define FLOWERING_DAY 43200000  //12 hours
#define FLOWERING_NIGHT 43200000  //12 hours

#define PWM_CONTRAST 85

#define DHTTYPE DHT22

#include <DHT.h>
#include "SoilSensor.h"
#include "WaterPump.h"
#include "DigitalLed.h"


LiquidCrystal lcd(RS_LCD, E_LCD, D4_LCD, D5_LCD, D6_LCD, D7_LCD);
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


void setup()
{
  Serial.begin(9600);
  dht.begin();
  water_pump -> init();
  attachInterrupt(digitalPinToInterrupt(STATE_BUTTON), set_light_state, FALLING);
  pinMode(STATE_BUTTON, INPUT);
  pinMode(WATER_PUMP_PIN, OUTPUT);
  pinMode(STRIP_LED_PIN, OUTPUT);
  pinMode(A_LCD, OUTPUT);
  digitalWrite(A_LCD, HIGH);
  analogWrite(V0_LCD, PWM_CONTRAST);
  lcd.begin(16, 2);
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
  print_data_on_lcd(temp, air_hum, soil_hum);
  print_data_on_serial(temp, air_hum, soil_hum);
  manage_light();
  delay(DELTA_DELAY);
}

/**
 * Print interesting data on lcd.
 */
void print_data_on_lcd(float temp, float air_hum, int soil_hum)
{
  lcd.setCursor(0, 0);
  lcd.print("T:");
  if(isnan(temp))
  {
    lcd.print("ERR");
  }
  else
  {
    lcd.print(round(temp));
    lcd.print("C");
    if(temp < MIN_TEMP || temp > MAX_TEMP)
    {
      lcd.print(" x");
    }
  }
  lcd.print("  H:");
  if(isnan(air_hum))
  {
    lcd.print("ERR");
  } 
  else
  {
    lcd.print(round(air_hum));
    lcd.print("%");
    if(air_hum < MIN_HUM || air_hum > MAX_HUM)
    {
      lcd.print(" x");
    }
  }

  lcd.setCursor(0, 1);
  lcd.print("S:");
  lcd.print(soil_hum);
  lcd.print("%");
  lcd.print(" M:");
  if(flowering_state)
  {
    lcd.print("F");
  }
  else
  {
    lcd.print("V");
  }

  lcd.print(" L:");
  if(night_mode)
  {
    lcd.print("N");
  }
  else
  {
    lcd.print("D");
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
  Serial.print("\nSoil Humidity: ");
  Serial.print(soil_hum);
  Serial.print("%");

  Serial.print("\nLights: ");
  if(flowering_state)
  {
    Serial.print("F");
  }
  else
  {
    Serial.print("V");
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
  state_check_change();
  check_light_mode();
  if(night_mode){
    strip_led->off();
  } else {
    strip_led->on();
  }

}

void check_light_mode()
{
  if(night_mode) //NIGHT MODE
  {
    if(millis() - last_check_light_millis > actual_night_time){
      last_check_light_millis = millis();
      night_mode = 0;
    }
  }
  else //DAY MODE
  {
     if(millis() - last_check_light_millis > actual_day_time){
      last_check_light_millis = millis();
      night_mode = 1;
    }
  }
}

/**
 * Check if the button was pressed. Change the day & night millisecond values if so.
 */
void state_check_change()
{
  if(flowering_state)
  {
    actual_day_time = FLOWERING_DAY;
    actual_night_time = FLOWERING_NIGHT;
  } else{
    actual_day_time = VEGETATIVE_DAY;
    actual_night_time = VEGETATIVE_NIGHT;
  }
}

/**
 * INTERRUPT FUNCTION CALLED WHEN BUTTON IS PRESSED
 */
void set_light_state()
{
  if(millis() - last_time_pressed > BOUNCING_DELAY){
    flowering_state = !flowering_state;
    last_time_pressed = millis();
  }  
}
