#define DELTA_DELAY 60000 //60 seconds delay
#define SERVO_DELAY 1000 // 1 second delay

// PINS
#define LED_STRIP_PIN 7
#define DHT11_PIN 8
#define WATER_PUMP_PIN 9
#define CS_MOISTURE_PIN A1

//VALUES
#define ERROR_IN_SENSOR -999
#define WATER_NEEDED_HUM 33
#define MAX_TEMP 30
#define MIN_TEMP 20
#define MAX_HUM  70
#define MIN_HUM 40
#define WATER_CHECK_MIN 30

//DAY AND NIGHT
#define DAY_HOURS 20
#define NIGHT_HOURS 4

#include "DigitalLed.h"
#include "HumiditySensor.h"
#include "SoilSensor.h"
#include "WaterPump.h"

char night_mode = 0;
int minute_counter = 0;

DigitalLed* led_strip = new DigitalLed(LED_STRIP_PIN);
HumiditySensor* dht_sensor = new HumiditySensor(DHT11_PIN);
SoilSensor* soil_sensor = new SoilSensor(CS_MOISTURE_PIN);
WaterPump* water_pump = new WaterPump(WATER_PUMP_PIN);

void setup()
{
  Serial.begin(9600);
  int minuteCounter = 0;
  set_light(!night_mode);
  dht_sensor->init();
  water_pump->init();
}

void loop()
{
  // Update Humidity and Temperature
  int temp = dht_sensor -> get_temp();
  int air_hum = dht_sensor -> get_humidity();
  int soil_hum = soil_sensor -> get_humidity();
  manage_light();
  manage_water(soil_hum);
  print_data_on_serial(temp, air_hum, soil_hum);
  delay(DELTA_DELAY);
}

void print_data_on_serial(int temp, int air_hum, int soil_hum)
{
  Serial.print("\nTemperature: ");
  if(temp == -999)
  {
    Serial.print("Error");
  }
  else
  {
    Serial.print(temp);
    Serial.print("C°");
  }
 
  Serial.print("\n Air Humidity: ");
  if(air_hum == -999)
  {
    Serial.print("Error");
  }
  else
  {
    Serial.print(air_hum);
    Serial.print("%");
  }
  Serial.print("\n Soil Humidity: ");
  Serial.print(soil_hum);
  Serial.print("%");
}

void manage_light()
{
  minute_counter++;

  if(!night_mode) //DAY
  {
     if(minute_counter > DAY_HOURS * 60)
     {
        night_mode = 1;
        minute_counter = 0;
     }
  } 
  else
  {
      if(minute_counter > NIGHT_HOURS * 60)
      {
        night_mode = 0;
        minute_counter = 0;
      }
  }
  set_light(!night_mode);
}

void manage_water(int soil_hum)
{
  if(minute_counter % WATER_CHECK_MIN == 0 )
  {
    if(soil_hum <= WATER_NEEDED_HUM)
    {
      water_pump -> send_water();  
    }
  }
}

void set_light(char value)
{
  if(value){
    led_strip->on();
  }
  else
  {
   led_strip->off();
  }
}
