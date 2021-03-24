/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-03-24     peng       the first version
 */
#include "board.h"
#include <rtthread.h>
#include <rtdevice.h>
#include <string.h>
#include <stdlib.h>

//#define DBG_ENABLE
#define DBG_SECTION_NAME "tmc51xx"
#define DBG_LEVEL DBG_LOG
#define DBG_COLOR
#include <rtdbg.h>

#include "drv_spi.h"
#include "tmc51xx.h"


#ifdef PKG_USING_TMC51XX

static int tmc51xx_spi_transfer(struct rt_spi_device  *device, TMC51XX_SPI_DATA_st *pSD)
{
    uint32_t tmp;
    uint8_t send_buf[5];
    uint8_t receive_buf[5];
    struct rt_spi_message msg;

    send_buf[0] = (pSD->is_write << 7) | (pSD->addr & 0x7F);
    send_buf[1] = (uint8_t)(pSD->sd >> 24);
    send_buf[2] = (uint8_t)(pSD->sd >> 16);
    send_buf[3] = (uint8_t)(pSD->sd >> 8);
    send_buf[4] = (uint8_t)(pSD->sd);

    msg.send_buf   = send_buf;
    msg.recv_buf   = receive_buf;
    msg.length     = 5;
    msg.cs_take    = 1;
    msg.cs_release = 1;
    msg.next       = RT_NULL;

    rt_spi_transfer_message(device, &msg);

    pSD->status = receive_buf[0];
    tmp = receive_buf[1];
    tmp <<= 8;
    tmp |= receive_buf[2];
    tmp <<= 8;
    tmp |= receive_buf[3];
    tmp <<= 8;
    tmp |= receive_buf[4];
    pSD->rd = tmp;

    return RT_EOK;
}

/**
 * This function write a tmc51xx register
 *
 * @param dev the name of tmc51xx device
 * @param reg_addr register address
 * @param val The value to be written
 *
 * @return the operation status, RT_EOK on OK; RT_ERROR on error
 */
rt_err_t tmc51xx_write_reg(tmc51xx_device_t dev, uint8_t reg_addr, uint32_t val)
{
    rt_err_t result;
    TMC51XX_SPI_DATA_st tmc5160_sd;

    tmc5160_sd.addr = reg_addr;
    tmc5160_sd.is_write = 1;
    tmc5160_sd.sd = val;
    result = tmc51xx_spi_transfer(dev->spi, &tmc5160_sd);

    return result;
}

/**
 * This function read a tmc51xx register
 *
 * @param dev the name of tmc51xx device
 * @param reg_addr register address
 * @param pval The pointer to return register value
 *
 * @return the operation status, RT_EOK on OK; RT_ERROR on error
 */
rt_err_t tmc51xx_read_reg(tmc51xx_device_t dev, uint8_t reg_addr, uint32_t *pval)
{
    rt_err_t result;
    TMC51XX_SPI_DATA_st tmc5160_sd;

    tmc5160_sd.addr = reg_addr;
    tmc5160_sd.is_write = 0;
    tmc5160_sd.sd = 0;
    result = tmc51xx_spi_transfer(dev->spi, &tmc5160_sd);
    *pval = tmc5160_sd.rd;

    return result;
}

/**
 * This function initializes tmc51xx registered device driver
 *
 * @param i2c_bus_name the name of i2c bus
 *
 * @return the rda58cxx device.
 */
tmc51xx_device_t tmc51xx_init(tmc51xx_io_config_t io_config)
{
    tmc51xx_device_t dev;
    struct rt_spi_configuration cfg;

    dev = rt_calloc(1, sizeof(struct tmc51xx_device));
    if (dev == RT_NULL)
    {
        LOG_E("Can't allocate memory for tmc51xx device on '%s' ", io_config->spi_bus_name);
        return RT_NULL;
    }

    if (rt_hw_spi_device_attach(
                io_config->spi_bus_name,
                io_config->spi_device_name,
                io_config->cs_gpiox,
                io_config->cs_gpio_pin) != RT_EOK)
    {
        LOG_E("'%s' attach error", io_config->spi_bus_name);
        rt_free(dev);
        return RT_NULL;
    }

    dev->spi = (struct rt_spi_device *)rt_device_find(io_config->spi_device_name);
    if (dev->spi == RT_NULL)
    {
        LOG_E("Can't find tmc51xx device '%s' ", io_config->spi_device_name);
        rt_free(dev);
        return RT_NULL;
    }
    else
    {
        cfg.data_width = 8;
        cfg.mode = RT_SPI_MASTER | RT_SPI_MODE_0 | RT_SPI_MSB;
        cfg.max_hz = 2 * 1000 * 1000;                          /* 2M */

        rt_spi_configure(dev->spi, &cfg);

        rt_pin_mode(io_config->pin_en, PIN_MODE_OUTPUT);
        rt_pin_mode(io_config->pin_dir, PIN_MODE_OUTPUT);
        rt_pin_mode(io_config->pin_step, PIN_MODE_OUTPUT);
        rt_pin_write(io_config->pin_en, PIN_HIGH);
    }

    dev->lock = rt_mutex_create("mutex_tmc51xx", RT_IPC_FLAG_FIFO);
    if (dev->lock == RT_NULL)
    {
        LOG_E("Can't create mutex for tmc51xx device on '%s' ", io_config->spi_device_name);
        rt_free(dev);
        return RT_NULL;
    }

    tmc51xx_write_reg(dev, 0x10, 0x00080303);  // IHOLD_IRUN
    tmc51xx_write_reg(dev, 0x6C, 0x18410153);  // CHOPCONF

    return dev;
}

/**
 * This function releases memory and deletes mutex lock
 *
 * @param dev the pointer of device driver structure
 */
void tmc51xx_deinit(tmc51xx_device_t dev)
{
    RT_ASSERT(dev);

    rt_mutex_delete(dev->lock);

    rt_free(dev);
}

#endif  // PKG_USING_TMC51XX
