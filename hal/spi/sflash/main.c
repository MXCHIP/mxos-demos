/**
 ******************************************************************************
 * @file    main.c
 * @author  Snow Yang
 * @version V1.0.0
 * @date    21-May-2019
 * @brief   SPI flash demo.
 ******************************************************************************
 */

/*
 Module          SPI Flash
+------+        +------+
|  CLK |--------| CLK  |
| MISO |--------| MISO |
| MOSI |--------| MOSI |
|   CS |--------| CS   |
|  GND |--------| GND  |
+------+        +------+
*/

#include "mxos.h"
#include "sflash.h"

#define app_log(M, ...) custom_log("APP", M, ##__VA_ARGS__)

#define FLASH_TEST_ADDR 0x00
#define FLASH_TEST_SIZE 64

typedef struct
{
  uint8_t id;
  const char *name;
} flash_vendor_t;

const flash_vendor_t flash_vendor_list[] = {
    {
        .id = 0xef,
        .name = "Winbond",
    },
    {
        .id = 0xc8,
        .name = "GigaDevice",
    },
    {
        .id = 0xc2,
        .name = "MXIC",
    },
    {
        .id = 0xa1,
        .name = "FudanMicro",
    },
    {
        .id = 0x0b,
        .name = "XTX",
    },
};

static uint8_t wdata[FLASH_TEST_SIZE];
static uint8_t rdata[FLASH_TEST_SIZE];

void dumphex(uint8_t *buf, uint32_t len)
{
  printf("-----------------------------------------------------");
  for (int i = 0; i < len; i++)
  {
    if (i % 16 == 0)
    {
      printf("\r\n%04X: ", i);
    }
    printf("%02X ", buf[i]);
  }
  printf("\r\n");
}

int main(void)
{
  mhal_spi_pinmux_t pinmux = {
      .miso = MXOS_MISO,
      .mosi = MXOS_MOSI,
      .clk = MXOS_SCK,
  };
  mhal_spi_open(MXOS_SPI, &pinmux);
  mhal_spi_format(MXOS_SPI, 1000000, 0, 8);
  mhal_gpio_open(MXOS_CS, OUTPUT_PUSH_PULL);
  mhal_gpio_high(MXOS_CS);

  app_log("SPI flash demo.");

  uint32_t jedecid = sFlash_readJEDECID();
  app_log("JEDEC ID: %06lx", jedecid);
  for (int i = 0; i < sizeof(flash_vendor_list) / sizeof(flash_vendor_t); i++)
  {
    if (flash_vendor_list[i].id == jedecid >> 16)
    {
      app_log("Vendor: %s", flash_vendor_list[i].name);
      break;
    }
  }
  uint8_t reg = sFlash_readStatusReg1();
  app_log("Status: %02x", reg);

  app_log("Test address = 0x%x, length = %d bytes", FLASH_TEST_ADDR, FLASH_TEST_SIZE);

  app_log("Reading ...");
  sFlash_Read(FLASH_TEST_ADDR, rdata, FLASH_TEST_SIZE);
  dumphex(rdata, FLASH_TEST_SIZE);

  app_log("Erasing ...");
  memset(wdata, 0xFF, FLASH_TEST_SIZE);
  sFlash_Erase(FLASH_TEST_ADDR, FLASH_TEST_SIZE, NULL);
  sFlash_Read(FLASH_TEST_ADDR, rdata, FLASH_TEST_SIZE);
  if (memcmp(wdata, rdata, FLASH_TEST_SIZE) != 0)
  {
    dumphex(rdata, FLASH_TEST_SIZE);
    app_log("Error");
    return 1;
  }
  app_log("OK");

  app_log("Writting ...");
  memset(wdata, 0x66, FLASH_TEST_SIZE);
  sFlash_Write(FLASH_TEST_ADDR, wdata, FLASH_TEST_SIZE);
  sFlash_Read(FLASH_TEST_ADDR, rdata, FLASH_TEST_SIZE);
  if (memcmp(wdata, rdata, FLASH_TEST_SIZE) != 0)
  {
    dumphex(rdata, FLASH_TEST_SIZE);
    app_log("Error");
    return 1;
  }
  app_log("OK");

  return 0;
}

void sFlash_DelayMs(uint32_t ms)
{
  mos_msleep(ms);
}

void sFlash_Command(uint8_t cmd, uint8_t *arg, uint32_t argc, uint8_t *buf, uint32_t n, sFlashDir_t dir)
{
  mhal_gpio_low(MXOS_CS);
  mhal_spi_write_and_read(MXOS_SPI, &cmd, NULL, 1);
  if (arg && argc > 0)
  {
    mhal_spi_write_and_read(MXOS_SPI, arg, NULL, argc);
  }
  if (buf && n > 0)
  {
    if (dir == DIR_W)
    {
      mhal_spi_write_and_read(MXOS_SPI, buf, NULL, n);
    }
    else
    {
      mhal_spi_write_and_read(MXOS_SPI, NULL, buf, n);
    }
  }
  mhal_gpio_high(MXOS_CS);
}