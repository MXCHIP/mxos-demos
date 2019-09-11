/**
 ******************************************************************************
 * @file    sflash.h
 * @author  Snow Yang
 * @version V1.0.0
 * @date    21-May-2019
 * @brief   SPI flash driver.
 ******************************************************************************
 */

#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <stdbool.h>

enum
{
    DIR_R,
    DIR_W
};
typedef uint8_t sFlashDir_t;

typedef void (*progress_t)(uint32_t n);
typedef int (*fop_t)(uint32_t addr, uint8_t *buf, uint32_t n);

uint8_t sFlash_readStatusReg1(void);
uint8_t sFlash_readStatusReg2(void);
uint32_t sFlash_readJEDECID(void);
void sFlash_powerDown(void);
int sFlash_Erase(uint32_t addr, uint32_t n, progress_t uprogress);
int sFlash_Write(uint32_t addr, uint8_t *buf, uint32_t n);
void sFlash_Read(uint32_t addr, uint8_t *buf, uint32_t n);
int sFlash_EraseWrite(uint32_t addr, uint32_t n, fop_t ufread, progress_t uprogress);
int sFlash_writeSR(uint8_t reg1, uint8_t reg2);
void sFlash_useErase32Block(bool use);