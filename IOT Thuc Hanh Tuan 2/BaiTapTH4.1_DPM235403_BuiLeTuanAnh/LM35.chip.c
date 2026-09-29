#include "wokwi-api.h"
#include <stdlib.h>

// Chip data.
typedef struct 
{
  pin_t VCC;
  pin_t OUT;
  pin_t GND;
  uint32_t temperature;
} chip_data_t;

// Returns true if the power source is connected correctly.
bool power_connected(void *data)
{
  chip_data_t *chip = (chip_data_t*)data;
  return pin_read(chip->VCC) && !pin_read(chip->GND);
}

// Timer function. Analog output based on temperature.
void chip_timer_callback(void *user_data) 
{
  if (power_connected(user_data))
  {
    chip_data_t *chip_data = (chip_data_t*)user_data;
    uint32_t temperature = attr_read(chip_data->temperature);
    float volts = 0.01 * temperature;
    pin_dac_write(chip_data->OUT, volts);
  }
}

// Chip initialization.
void chip_init(void) 
{
  chip_data_t *chip_data = malloc(sizeof(chip_data_t));
  chip_data->VCC = pin_init("VCC", INPUT);
  chip_data->GND = pin_init("GND", INPUT);
  chip_data->OUT = pin_init("OUT", ANALOG);
  chip_data->temperature = attr_init("temperature", 50);

  const timer_config_t config = 
  {
    .callback = chip_timer_callback,
    .user_data = chip_data,
  };

  timer_t timer_id = timer_init(&config);
  timer_start(timer_id, 10000, true);
}