/**
 ******************************************************************************
 * @file    sflash.c
 * @author  Snow Yang
 * @version V1.0.0
 * @date    21-May-2019
 * @brief   SPI flash driver.
 ******************************************************************************
 */

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "sflash.h"

#define CMD_WRIRE_ENABLE 0x06
#define CMD_WRITE_DISABLE 0x04
#define CMD_READ_STATUS_R1 0x05
#define CMD_READ_STATUS_R2 0x35
#define CMD_WRITE_STATUS_R 0x01
#define CMD_PAGE_PROGRAM 0x02
#define CMD_QUAD_PAGE_PROGRAM 0x32
#define CMD_BLOCK_ERASE64KB 0xd8
#define CMD_BLOCK_ERASE32KB 0x52
#define CMD_SECTOR_ERASE 0x20
#define CMD_CHIP_ERASE 0xC7
#define CMD_ERASE_SUPPEND 0x75
#define CMD_ERASE_RESUME 0x7A
#define CMD_POWER_DOWN 0xB9
#define CMD_HIGH_PERFORM_MODE 0xA3
#define CMD_CNT_READ_MODE_RST 0xFF
#define CMD_RELEASE_PDOWN_ID 0xAB
#define CMD_MANUFACURER_ID 0x90
#define CMD_READ_UNIQUE_ID 0x4B
#define CMD_JEDEC_ID 0x9f

#define CMD_READ_DATA 0x03
#define CMD_FAST_READ 0x0B
#define CMD_READ_DUAL_OUTPUT 0x3B
#define CMD_READ_DUAL_IO 0xBB
#define CMD_READ_QUAD_OUTPUT 0x6B
#define CMD_READ_QUAD_IO 0xEB
#define CMD_WORD_READ 0xE3

#define SR1_BUSY_MASK 0x01
#define SR1_WEN_MASK 0x02

#define PAGE_SIZE 256
#define SECTOR_SIZE 4096

#define FLASH_BLK04K_SIZE 0x1000  //04K
#define FLASH_BLK32K_SIZE 0x8000  //32K
#define FLASH_BLK64K_SIZE 0x10000 //64K

#define ALIGIN04K(x) ((x) & ~(FLASH_BLK04K_SIZE - 1))

#define BUF_SIZE 0x1000

static int sFlash_WaitBusy(uint32_t interval);
static void sFlash_WriteEnable(void);
static void sFlash_WriteDisable(void);
static void sFlash_read(uint32_t addr, uint8_t *buf, uint16_t n);
static void sFlash_fastread(uint32_t addr, uint8_t *buf, uint16_t n);
static int sFlash_eraseBlock(uint32_t addr, uint32_t blksiz);
static int sFlash_eraseSector(uint32_t addr);
static int sFlash_erase32Block(uint32_t addr);
static int sFlash_erase64Block(uint32_t addr);
static int sFlash_eraseAll(void);
static int sFlash_pageWrite(uint32_t addr, uint8_t *buf, uint16_t n);

void sFlash_DelayMs(uint32_t ms);
void sFlash_Command(uint8_t cmd, uint8_t *arg, uint32_t argc, uint8_t *buf, uint32_t n, sFlashDir_t dir);

static bool useErase32Block = true;

uint8_t sFlash_readStatusReg1(void)
{
    uint8_t r[1];
    sFlash_Command(CMD_READ_STATUS_R1, NULL, 0, r, sizeof(r), DIR_R);
    return r[0];
}

uint8_t sFlash_readStatusReg2(void)
{
    uint8_t r[1];
    sFlash_Command(CMD_READ_STATUS_R2, NULL, 0, r, sizeof(r), DIR_R);
    return r[0];
}

uint32_t sFlash_readJEDECID(void)
{
    uint8_t r[3];
    sFlash_Command(CMD_JEDEC_ID, NULL, 0, r, sizeof(r), DIR_R);
    return r[0] << 16 | r[1] << 8 | r[2];
}

void sFlash_powerDown(void)
{
    sFlash_Command(CMD_POWER_DOWN, NULL, 0, NULL, 0, DIR_R);
}

void sFlash_useErase32Block(bool use)
{
    useErase32Block = use;
}

int sFlash_EraseWrite(uint32_t addr, uint32_t n, fop_t ufread, progress_t uprogress)
{
    int rc = 0;
    uint32_t saddr = ALIGIN04K(addr);
    uint32_t eaddr = ALIGIN04K(addr + n + FLASH_BLK04K_SIZE - 1);

    uint8_t *wbuf = malloc(BUF_SIZE);

    uint32_t w, r, blksiz;
    uint32_t s = addr;

    while (1)
    {
        if (saddr % FLASH_BLK64K_SIZE == 0 && eaddr >= saddr + FLASH_BLK64K_SIZE)
        {
            blksiz = FLASH_BLK64K_SIZE;
        }
        else if (useErase32Block && saddr % FLASH_BLK32K_SIZE == 0 && eaddr >= saddr + FLASH_BLK32K_SIZE)
        {
            blksiz = FLASH_BLK32K_SIZE;
        }
        else if (eaddr > saddr)
        {
            blksiz = FLASH_BLK04K_SIZE;
        }
        else
        {
            break; // End
        }

        // Erase block
        if ((rc = sFlash_eraseBlock(saddr, blksiz)) != 0)
            return rc;

        // Write and Verify
        w = blksiz - addr % blksiz;
        w = w > n ? n : w;
        while (w)
        {
            r = w > BUF_SIZE ? BUF_SIZE : w;
            if ((rc = ufread(addr - s, wbuf, r)) != 0)
                goto exit;
            if ((rc = sFlash_Write(addr, wbuf, r)) != 0)
                goto exit;
            addr += r;
            w -= r;
            uprogress ? uprogress(addr - s) : 0;
        }
        n -= w;
        saddr += blksiz;
    }

exit:
    free(wbuf);
    return rc;
}

int sFlash_Erase(uint32_t addr, uint32_t n, progress_t uprogress)
{
    int rc = 0;
    uint32_t saddr = ALIGIN04K(addr);
    uint32_t eaddr = ALIGIN04K(addr + n - 1);
    uint32_t blksiz;

    while (1)
    {
        if (saddr % FLASH_BLK64K_SIZE == 0 && eaddr >= saddr + FLASH_BLK64K_SIZE)
        {
            blksiz = FLASH_BLK64K_SIZE;
        }
        else if (saddr % FLASH_BLK32K_SIZE == 0 && eaddr >= saddr + FLASH_BLK32K_SIZE)
        {
            blksiz = FLASH_BLK32K_SIZE;
        }
        else if (eaddr >= saddr)
        {
            blksiz = FLASH_BLK04K_SIZE;
        }
        else
        {
            break; // End
        }

        // Erase block
        if ((rc = sFlash_eraseBlock(saddr, blksiz)) != 0)
            return rc;

        uprogress ? uprogress(saddr - addr) : 0;

        saddr += blksiz;
    }

    return rc;
}

int sFlash_Write(uint32_t addr, uint8_t *buf, uint32_t n)
{
    int rc;
    uint16_t w, r;

    while (n > 0)
    {
        r = PAGE_SIZE - addr % PAGE_SIZE;
        w = n > r ? r : n;
        rc = sFlash_pageWrite(addr, buf, w);
        if (rc != 0)
            return rc;
        n -= w;
        buf += w;
        addr += w;
    }

    return n;
}

void sFlash_Read(uint32_t addr, uint8_t *buf, uint32_t n)
{
    sFlash_fastread(addr, buf, n);
}

int sFlash_writeSR(uint8_t reg1, uint8_t reg2)
{
    uint8_t arg[2];
    arg[0] = reg1;
    arg[1] = reg2;

    sFlash_WriteEnable();
    sFlash_Command(CMD_WRITE_STATUS_R, arg, sizeof(arg), NULL, 0, DIR_R);

    return sFlash_WaitBusy(0);
}

// Static Functions
static int sFlash_WaitBusy(uint32_t interval)
{
    uint8_t r;
    while (1)
    {
        r = sFlash_readStatusReg1();
        if (r == 0xFF)
            return EIO;
        if (!(r & SR1_BUSY_MASK))
            break;
        interval > 0 ? sFlash_DelayMs(interval) : 0;
    }
    return 0;
}

static void sFlash_WriteEnable(void)
{
    sFlash_Command(CMD_WRIRE_ENABLE, NULL, 0, NULL, 0, DIR_R);
}

static void sFlash_WriteDisable(void)
{
    sFlash_Command(CMD_WRITE_DISABLE, NULL, 0, NULL, 0, DIR_R);
}

static void sFlash_read(uint32_t addr, uint8_t *buf, uint16_t n)
{
    uint8_t arg[3];
    arg[0] = (addr >> 16) & 0xFF; // A23-A16
    arg[1] = (addr >> 8) & 0xFF;  // A15-A08
    arg[2] = addr & 0xFF;         // A07-A00

    sFlash_Command(CMD_READ_DATA, arg, sizeof(arg), buf, n, DIR_R);
}

static void sFlash_fastread(uint32_t addr, uint8_t *buf, uint16_t n)
{
    uint8_t arg[4];
    arg[0] = (addr >> 16) & 0xFF; // A23-A16
    arg[1] = (addr >> 8) & 0xFF;  // A15-A08
    arg[2] = addr & 0xFF;         // A07-A00
    arg[3] = 0x00;

    sFlash_Command(CMD_FAST_READ, arg, sizeof(arg), buf, n, DIR_R);
}

static int sFlash_eraseBlock(uint32_t addr, uint32_t blksiz)
{
    int rc = 0;
    switch (blksiz)
    {
    case FLASH_BLK04K_SIZE:
        rc = sFlash_eraseSector(addr);
        break;
    case FLASH_BLK32K_SIZE:
        rc = sFlash_erase32Block(addr);
        break;
    case FLASH_BLK64K_SIZE:
        rc = sFlash_erase64Block(addr);
        break;
    }
    return rc;
}

static int sFlash_eraseSector(uint32_t addr)
{
    uint8_t arg[3];

    sFlash_WriteEnable();
    arg[0] = (addr >> 16) & 0xff;
    arg[1] = (addr >> 8) & 0xff;
    arg[2] = addr & 0xff;

    sFlash_Command(CMD_SECTOR_ERASE, arg, sizeof(arg), NULL, 0, DIR_R);

    return sFlash_WaitBusy(10);
}

static int sFlash_erase64Block(uint32_t addr)
{
    uint8_t arg[3];

    sFlash_WriteEnable();
    arg[0] = (addr >> 16) & 0xff;
    arg[1] = (addr >> 8) & 0xff;
    arg[2] = addr & 0xff;

    sFlash_Command(CMD_BLOCK_ERASE64KB, arg, sizeof(arg), NULL, 0, DIR_R);

    return sFlash_WaitBusy(10);
}

static int sFlash_erase32Block(uint32_t addr)
{
    uint8_t arg[3];

    sFlash_WriteEnable();
    arg[0] = (addr >> 16) & 0xff;
    arg[1] = (addr >> 8) & 0xff;
    arg[2] = addr & 0xff;

    sFlash_Command(CMD_BLOCK_ERASE32KB, arg, sizeof(arg), NULL, 0, DIR_R);

    return sFlash_WaitBusy(10);
}

static int sFlash_eraseAll(void)
{
    sFlash_Command(CMD_CHIP_ERASE, NULL, 0, NULL, 0, DIR_R);

    return sFlash_WaitBusy(10);
}

static int sFlash_pageWrite(uint32_t addr, uint8_t *buf, uint16_t n)
{
    uint8_t arg[3];

    sFlash_WriteEnable();
    arg[0] = (addr >> 16) & 0xff;
    arg[1] = (addr >> 8) & 0xff;
    arg[2] = addr & 0xff;

    sFlash_Command(CMD_PAGE_PROGRAM, arg, sizeof(arg), buf, n, DIR_W);

    return sFlash_WaitBusy(0);
}