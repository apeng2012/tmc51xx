# TMC51xx

## 1、介绍

这是一个 RT-Thread 的软件包，用于驱动步进电机芯片TMC5160。

### 1.1 许可证

tmc51xx 遵循 MIT 许可。

### 1.3 依赖

- RT-Thread 3.0+
- SPI 设备驱动程序

## 2、如何打开 tmc51xx

使用 tmc51xx package 需要在 RT-Thread 的包管理器中选择它，具体路径如下：

```
RT-Thread online packages
    peripheral libraries and drivers --->
        [*] TMC51xx power driver for stepper motors
```

然后让 RT-Thread 的包管理器自动更新，或者使用 `pkgs --update` 命令更新包到 BSP 中。

## 3、使用 tmc51xx

请参考 examples 目录中的例子。

## 4、注意事项

- tmc5160有3种操作模式，目前只测试了脉冲和方向的驱动方式。
- tmc5160控制端口采用SPI接口。

## 5、联系方式 & 感谢

* 维护：apeng2012
* 主页：https://github.com/apeng2012/tmc51xx
