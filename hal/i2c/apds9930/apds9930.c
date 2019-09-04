/**
 ******************************************************************************
 * @file    apds9930.c
 * @author  William Xu
 * @version V1.0.0
 * @date    17-Mar-2015
 * @brief   apds9930 user controller operation
 ******************************************************************************
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 ******************************************************************************
 */

#include "mxos.h"
#include "APDS9930.h"

/************** I2C/SPI buffer length ******/
#define APDS_BUFFER_LEN 3

merr_t APDS9930_I2C_bus_write(uint8_t reg_addr, uint8_t *reg_data, uint8_t cnt)
{
  uint8_t array[APDS_BUFFER_LEN];
  uint8_t stringpos;

  array[0] = reg_addr;
  for (stringpos = 0; stringpos < cnt; stringpos++)
  {
    array[stringpos + 1] = *(reg_data + stringpos);
  }

  return mhal_i2c_write(MXOS_I2C, APDS9930_ID, array, cnt + 1);
}

merr_t APDS9930_I2C_bus_read(uint8_t *reg_data, uint8_t cnt)
{
  return mhal_i2c_read(MXOS_I2C, APDS9930_ID, reg_data, cnt);
}

merr_t APDS9930_Write_RegData(uint8_t reg_addr, uint8_t reg_data)
{
  return APDS9930_I2C_bus_write(0x80 | reg_addr, &reg_data, 1);
}

merr_t APDS9930_Read_RegData(uint8_t reg_addr, uint8_t *reg_data)
{
  merr_t err = APDS9930_I2C_bus_write(0xA0 | reg_addr, NULL, 0);
  require_noerr_quiet(err, exit);
  err = APDS9930_I2C_bus_read(reg_data, 1);
exit:
  return err;
}

merr_t APDS9930_Clear_intrtrupt(void)
{
  return APDS9930_I2C_bus_write(0x80 | CLIT_ADDR, NULL, 0);
}

void apds9930_enable()
{
  //Disable and Powerdown
  APDS9930_Write_RegData(ENABLE_ADDR, APDS9930_DISABLE);
  APDS9930_Write_RegData(ATIME_ADDR, ATIME_256C);
  APDS9930_Write_RegData(PTIME_ADDR, PTIME_10C);
  APDS9930_Write_RegData(WTIME_ADDR, WTIME_74C);
  APDS9930_Write_RegData(CONFIG_ADDR, RECONFIG);
  APDS9930_Write_RegData(PPULSE_ADDR, PPULSE_MIN);
  //Config
  APDS9930_Write_RegData(CONTROL_ADDR, PDRIVE_100 | PDIODE_CH1 | PGAIN_1x | AGAIN_1x);

  //Enable APDS9930
  APDS9930_Write_RegData(ENABLE_ADDR, WEN | PEN | AEN | PON);
  //must delay > 12ms
  mos_msleep(20);
}

merr_t apds9930_data_readout(uint16_t *Prox_data, uint16_t *Lux_data)
{
  merr_t err = kNoErr;
  uint8_t CH0L_data = 0, CH0H_data = 0, CH1L_data = 0, CH1H_data = 0, ProxL_data = 0, ProxH_data = 0, status = 0;
  uint16_t CH0_data = 0, CH1_data = 0;
  int IAC1 = 0, IAC2 = 0, IAC = 0;
  float B = 1.862, C = 0.746, D = 1.296, ALSIT = 400, AGAIN = 1;
  float LPC = 0;

  err = APDS9930_Read_RegData(STATUS_ADDR, &status);
  require_noerr_quiet(err, exit);

  err = APDS9930_Read_RegData(Ch0DATAL_ADDR, &CH0L_data);
  require_noerr_quiet(err, exit);

  err = APDS9930_Read_RegData(Ch0DATAH_ADDR, &CH0H_data);
  require_noerr_quiet(err, exit);

  err = APDS9930_Read_RegData(Ch1DATAL_ADDR, &CH1L_data);
  require_noerr_quiet(err, exit);

  err = APDS9930_Read_RegData(Ch1DATAH_ADDR, &CH1H_data);
  require_noerr_quiet(err, exit);

  err = APDS9930_Read_RegData(PDATAL_ADDR, &ProxL_data);
  require_noerr_quiet(err, exit);

  err = APDS9930_Read_RegData(PDATAH_ADDR, &ProxH_data);
  require_noerr_quiet(err, exit);

  *Prox_data = ProxH_data << 8 | ProxL_data;

  LPC = GA * DF / (ALSIT * AGAIN);

  CH0_data = CH0H_data << 8 | CH0L_data;
  CH1_data = CH1H_data << 8 | CH1L_data;

  IAC1 = (int)(CH0_data - B * CH1_data);
  IAC2 = (int)(C * CH0_data - D * CH1_data);
  IAC = Max(IAC1, IAC2);

  *Lux_data = (int)(IAC * LPC);

  APDS9930_Clear_intrtrupt();

exit:
  return err;
}

merr_t apds9930_sensor_init(void)
{
  merr_t err = kNoErr;
  uint8_t device_id;

  mhal_i2c_pinmux_t pinmux = {
      .sda = MXOS_SDA,
      .scl = MXOS_SCL,
  };

  mhal_i2c_close(MXOS_I2C);

  /*int apds9930 sensor i2c device*/
  err = mhal_i2c_open(MXOS_I2C, I2C_ADDR_WIDTH_7BIT, 100000, &pinmux);
  require_noerr_quiet(err, exit);

  err = APDS9930_Clear_intrtrupt();
  require_noerr_quiet(err, exit);

  err = APDS9930_Read_RegData(ID_ADDR, &device_id);
  require_noerr_quiet(err, exit);

  require_action_quiet(device_id == APDS9930_ID, exit, err = kNotFoundErr);
  apds9930_enable();

exit:
  return err;
}

merr_t apds9930_sensor_deinit(void)
{
  return mhal_i2c_close(MXOS_I2C);
}
