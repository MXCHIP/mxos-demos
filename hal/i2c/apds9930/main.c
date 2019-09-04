/**
 ******************************************************************************
 * @file    main.c
 * @author  Snow Yang
 * @version V1.0.0
 * @date    21-May-2015
 * @brief   ambient light sensor control demo.
 ******************************************************************************
 */

#include "mxos.h"
#include "apds9930.h"

#define app_log(M, ...) custom_log("APP", M, ##__VA_ARGS__)

int main(void)
{
  merr_t err = kNoErr;
  uint16_t prox = 0;
  uint16_t lux = 0;

  app_log("Initialize APDS9930");

  err = apds9930_sensor_init();
  require_noerr_action(err, exit, app_log("ERROR: Unable to initialize APDS9930"));

  while (1)
  {
    mos_sleep(1);
    err = apds9930_data_readout(&prox, &lux);
    require_noerr_action(err, exit, app_log("ERROR: Read data failed"));
    app_log("Prox: %.1fmm  Lux: %d", (10239.0 - prox) / 100.0, lux);
  }

exit:
  return err;
}
