# Power_management

## 简介

这是一款以STM32F042为主控，INA226为ADC的功率控制模块，可以实时测量底盘的功率，超出设置的功率上限时开始扣除缓冲能量，缓冲能量清空后会使底盘断电五秒，之后重新恢复供电并重置缓冲能量。

在这份工程中INA226的驱动文件INA226.c和INA226.h实现了基于STM32 HAL库的多种功能，包括**初始化INA226设备、读取电流值、读取功率值、读取总线电压、读取分流电压、复位INA226设备、设置配置寄存器、设置校准寄存器、读取制造商ID（用于设备检测）等**，可以在其他型号的STM32设备上快捷移植，更换I2C收发函数后亦可在其他设备上移植。

## 使用说明

### 嵌入式工程部分

#### 修改功率上限和缓冲能量

在**UserTasks.h**文件中可以修改功率上限和缓冲能量：

```C
#define POWER_MAX (50.0f)
#define BufferEnergyMax (60.0f)
```

修改这两个宏定义的值即可。

#### STM32F042F6P6配置方法

此款单片机芯片引脚较少，体积小巧，很适合用在这样的小型模块中，但是这也带来了一些使用上的问题。一个很致命的问题是它**没有引出BOOT0和BOOT1引脚**，所以无法通过控制这两个引脚的电平来设置启动模式，更要命的是默认的启动模式无法正确地从我们的main函数启动，这就需要我们使用一些工具修改启动模式。

这里推荐使用意法半导体官方提供的工具**STM32CubeProgrammer**，下载地址是[STM32CubeProg | Software - STMicroelectronics](https://www.st.com/en/development-tools/stm32cubeprog.html)

安装成功后，启动软件，选择 JLink 连接，点击序列号框右侧的刷新按钮检测 JLink，然后点击右上角连接 JLink。

![连接JLink](./photo/image-20251217212722199.png)

然后点击左侧栏的“OB”选项。

![进入OB选项](./photo/image-20251217212820150.png)

在“User Configuration”中找到最下面的“BOOT_SEL”，将其取消勾选。

![取消勾选BOOT\_SEL](./photo/image-20251217213016839.png)

确保“nBOOT0”和“nBOOT1”已经勾选。

![勾选nBOOT0和nBOOT1](./photo/image-20251217213055850.png)
