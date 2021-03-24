/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-03-24     peng       the first version
 */
#ifndef PKG_TMC51XX_H__
#define PKG_TMC51XX_H__

#ifdef __cplusplus
extern "C" {
#endif


struct tmc51xx_io_config
{
    const char *spi_bus_name;
    const char *spi_device_name;
    rt_base_t pin_en;
    rt_base_t pin_dir;
    rt_base_t pin_step;
    rt_base_t pin_diga0;
    GPIO_TypeDef *cs_gpiox;
    uint16_t cs_gpio_pin;
};
typedef struct tmc51xx_io_config *tmc51xx_io_config_t;

struct tmc51xx_device
{
    struct tmc51xx_io_config ioc;
    struct rt_spi_device *spi;
    rt_mutex_t lock;
};
typedef struct tmc51xx_device *tmc51xx_device_t;

typedef struct
{
    uint32_t sd;
    uint32_t rd;
    uint8_t addr;
    uint8_t status;
    uint8_t is_write;
} TMC51XX_SPI_DATA_st;

tmc51xx_device_t tmc51xx_init(tmc51xx_io_config_t io_config);
void tmc51xx_deinit(tmc51xx_device_t dev);
rt_err_t tmc51xx_write_reg(tmc51xx_device_t dev, uint8_t reg_addr, uint32_t val);
rt_err_t tmc51xx_read_reg(tmc51xx_device_t dev, uint8_t reg_addr, uint32_t *pval);

#ifdef __cplusplus
}
#endif

#endif  // PKG_TMC51XX_H__
