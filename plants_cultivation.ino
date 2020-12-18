#define DELTA_DELAY 6000 //60 seconds delay
#define SERVO_DELAY 1000 // 1 second delay

//PINS
#define DHT11_PIN 3

//TEMP LEDS
#define HOT_LED_PIN 4
#define COLD_LED_PIN 6

//HUMIDITY LEDS

#define HIGH_LED_PIN 8
#define LOW_LED_PIN 10

//OTHER PINS
#define SERVO_PIN 11

//VALUES
#define ERROR_IN_SENSOR -999
#define MAX_TEMP 30
#define MIN_TEMP 20
#define MAX_HUM  70
#define MIN_HUM 40
#define CRITICAL_LOW_HUM 30
#define WATER_MINUTES 50

//DAY AND NIGHT
#define DAY_HOURS 20
#define NIGHT_HOURS 4

//OTHERS
#define N_LEDS 2

#include "HumiditySensor.h"
#include "DigitalLed.h"
#include "Servomotor.h"

char night_mode = 0;

int minute_counter = 0;

HumiditySensor* hum_sensor = new HumiditySensor(DHT11_PIN);
DigitalLed* cold_led = new DigitalLed(COLD_LED_PIN);
DigitalLed* hot_led = new DigitalLed(HOT_LED_PIN);

DigitalLed* low_led = new DigitalLed(LOW_LED_PIN);
DigitalLed* high_led = new DigitalLed(HIGH_LED_PIN);

Servomotor* servo = new Servomotor(SERVO_PIN);

DigitalLed* temp_leds [] = {cold_led, hot_led};
DigitalLed* hum_leds [] = {low_led, high_led};

void setup()
{
  Serial.begin(9600);
  int minuteCounter = 0;
  set_light(!night_mode);
}

void loop()
{
  int chk = hum_sensor->init();
  int temp = hum_sensor -> get_temp();
  int hum = hum_sensor -> get_humidity();
  print_data_on_serial(temp, hum);
  manage_light();
  manage_water();
  if(error_check(temp, hum)){
    all_led_blink();
  }
  else
  {
    power_led_on(temp, hum);
    if(hum <= 999)
    {
      send_water_splash();
    }
  }
  delay(DELTA_DELAY);
}

//power on only the right led, based on temperature and humidity
void power_led_on(int temp, int hum)
{
  all_led_off();
  temp_led_on(temp);
  hum_led_on(hum);
}

char error_check(int temp, int hum)
{
  if(temp == ERROR_IN_SENSOR || temp == ERROR_IN_SENSOR)
  {
    return 1;
  }
  return 0;
}

void temp_led_on(int temp)
{
  if(temp > MAX_TEMP)
  {
    hot_led->on();
  }
  else if(temp < MIN_TEMP)
  {
    cold_led->on();
  }
}

void hum_led_on(int hum)
{
  if(hum > MAX_HUM)
  {
    high_led->on();
  }
  else if(hum < MIN_HUM)
  {
    low_led->on();
  }
}

//turn off all the leds at once
void all_led_off()
{
    for (int i = 0; i < N_LEDS; i++) {
      temp_leds[i]->off();
      hum_leds[i]->off();
   }
}

void print_data_on_serial(int temp, int humidity)
{
  Serial.print("\nTemperature: ");
  Serial.print(temp);
  Serial.print("C°");
  Serial.print("\nHumidity: ");
  Serial.print(humidity);
  Serial.print("%");
}

void all_led_blink()
{
    for (int i = 0; i < N_LEDS; i++) {
      temp_leds[i]->on();
      hum_leds[i]->on();
   }
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

void manage_water()
{
  if(minute_counter % WATER_MINUTES == 0){
    send_water_splash();
  }
}

void set_light(char value)
{
  //TODO
}

void send_water_splash()
{
  /*
  servo->turn_on();
  servo->set_to_max();
  delay(SERVO_DELAY);
  servo->set_to_min();
  delay(SERVO_DELAY);
  servo->turn_off();
  */
}
