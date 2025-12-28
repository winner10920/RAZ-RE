# Pinout


QFN32 (4x4) | Pin Name | Type | Alternate Function | Optional Function | XD0011_NTMB_V1_2
--- | --- | --- | --- | --- | --- 
1 | VDD | S |  |  | 
2 | PC14-OSC32_IN | I/O | OSC32_IN |  | 
3 | PC15-OSC32_OUT | I/O | OSC32_OUT |  | 
4 | NRST | I/O | Device reset input/internal reset output |  | 
5 | VDDA | S | Analog power supply |  | 
6 | PA0 | I/O | USART1_CTS, USART2_CTS, LPUART_TX, ADC_IN0, SPI1_SCK, RTC_TAMP2, WKUP0, I2S_CK | COMP_INM, COMP_OUT, OPAMP_VINP | NC?
7 | PA1 | I/O | USART1_RTS, USART2_RTS, EVENTOUT, SPI1_NSS, I2S_WS, LPTIM_IN2, TIM8_CH2, I2C1_SMBA, TIM3_ETR, LPUART_TX | ADC_IN1, COMP_INP, OPAMP_VINP | TV1 VOLTAGE SENSE
8 | PA2 | I/O | USART1_TX, USART2_TX, TIM8_CH3, SPI1_MOSI, I2S_SD, TIM1_BKIN | ADC_IN2, WKUP2, OPAMP_VINM | TV2 VOLTAGE SENSE
9 | PA3 | I/O | USART1_RX, USART2_RX, TIM8_CH4, TIM1_CH2, SPI1_MISO, I2S_MCK, LPUART_RX | ADC_IN3, COMP_INP | MIC ?
10 | PA4 | I/O | SPI1_NSS, I2S_WS, USART1_CK, USART2_CK, TIM3_CH1, TIM1_CH1, SPI1_NSS, I2S_WS, I2C1_SCL, TIM8_ETR, LPUART_TX | ADC_IN4, COMP_INM, OPAMP_VINP | Not Connected
11 | PA5 | I/O | SPI1_SCK, I2S_CK, TIM8_ETR, TIM1_CH2N, TIM1_CH3 | ADC_IN5, COMP_INM, OPAMP_VINM | COIL TV2
12 | PA6 | I/O | SPI1_MISO, I2S_MCK, TIM3_CH1, TIM1_BKIN, TIM8_CH1, EVENTOUT, LPUART_CTS, LPUART_TX, I2C2_SCL, LPTIM_ETR, BEEPER_OUT | ADC_IN6, COMP_OUT, OPAMP_VOUT | BACKLIGHT
13 | PA7 | I/O | SPI1_MOSI, SPI2_NSS, I2S_SD, TIM3_CH2, TIM1_CH1N, TIM8_CH2, EVENTOUT, LPUART_RX, I2C2_SDA, BEEPER_N_OUT, USART2_CTS | ADC_IN7, OPAMP_VINP, COMP_INP | BUTTON
14 | PB0 | I/O | TIM3_CH3, TIM1_CH2N, EVENTOUT, SPI2_SCK | ADC_IN8, OPAMP_VINP | LP4086 ISET<br> PULL LOW FOR FASTER CHARGE?
15 | PB1 | I/O | LPUART_RTS, I2S_SD, SPI2_MOSI, USART2_CK, SPI1_MOSI | ADC_IN9, OPAMP_VINM, ADC_IN10 | LP4086 CHRG <br> PULLED LOW ACTIVE
16 | PB2 | I/O | I2C1_SMBA, I2C2_SMBA, TIM3_CH4, LPTIM_OUT |  | R2?
17 | VDD | S | Digital power supply |  | GROUND
18 | PA8 | I/O | USART1_CK, TIM1_CH1, EVENTOUT, MCO, SPI2_NSS, TIM8_CH2N |  | GT25Q80A CS
19 | PA9 | I/O | I2C1_SCL, I2C2_SCL, SPI2_SCK, TIM8_CH1N, LPTIM_OUT, USART2_TX, MCO, USART1_TX, TIM1_CH2, TIM8_BKIN |  | GT25Q80A CLK
20 | PA10 | I/O | USART1_RX, TIM1_CH3, TIM8_BKIN, I2C1_SDA, I2C2_SDA, SPI2_MISO, USART2_RX, RTC_REFIN, USART1_CTS |  | GT25Q80A DO
21 | PA11 | I/O | TIM1_CH4, EVENTOUT, I2C2_SCL, SPI2_MOSI | RTC_REFIN, COMP_OUT | GT25Q80A DI
22 | PA12 | I/O | USART1_RTS, TIM1_ETR, EVENTOUT, I2C2_SDA, SPI2_MISO | COMP_OUT | 
23 | PA13 (SWDIO) | I/O | USART1_RX, USART2_RX, I2C1_SDA, SPI1_SCK, I2S_CK, SWDIO |  | SWDIO
24 | PA14 (SWCLK) | I/O | USART1_TX, USART2_TX, SWCLK, I2C1_SMBA, SPI1_MISO |  | SWCLK
25 | PA15 | I/O | SPI1_NSS, I2S_WS, USART1_RX, USART2_RX, LPUART_RTS, EVENTOUT |  | DISP CS
26 | PB3 | I/O | SPI1_SCK, I2S_CK, EVENTOUT, LPUART_TX, TIM3_ETR |  | DISP SCLK
27 | PB4 | I/O | SPI1_MISO, TIM3_CH1, EVENTOUT, TIM8_BKIN, LPUART_RX, LPTIM_OUT |  | COIL TV1
28 | PB5 | I/O | SPI1_MOSI,I2S_SD,I2C1_SMBA, TIM8_BKIN, TIM3_CH2, LPUART_TX, LPTIM_IN1, TIM8_CH3N, I2C1_SCL |  | DISP MOSI
29 | PB6 | I/O | USART1_TX, TIM8_CH1N, TIM8_CH3, LPTIM_ETR, I2C1_SDA, USART1_RX |  | DISP RST
30 | PB7 | I/O | TIM8_CH2N, LPUART_CTS, LPUART_RX, LPTIM_IN2, TIM8_CH4 |  | DISP RS
31 | PF2-BOOT0 | I | Boot memory selection |  | 
32 | PB8 | I/O | I2C1_SCL, TIM8_CH1 |  | COIL TV1?


| GPIOA |  |  |  |  |  |  |  |  |  |  |  |  |  |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
|  |  |  |  |  |  |  |  |  |  |  |  |  |  |
| PIN | PMODE |  | STATE |  | PUPD |  |  | POTYPE | SR | POD | PID | DS | AF |
| 15 | 0 | GENERAL OUTPUT | ON, HSR |  | 0 | NO PU/PD |  | 0 | 0 | 0 | 0 | 0 |  |
|  | 1 |  |  |  | 0 |  |  | 0 | 0 | 0 | 0 | 0 |  |
| 14 | 1 | ALTERNATE | SWDIO |  | 1 | PD ⬇ |  | 0 | 0 | 0 | 0 | 0 |  |
|  | 0 |  |  |  | 0 |  |  | 0 | 0 | 0 | 0 | 0 |  |
| 13 | 1 | ALTERNATE | SWCLK |  | 0 | PU ⬆ |  | 0 | 0 | 0 | 0 | 0 |  |
|  | 0 |  |  |  | 1 |  |  | 0 | 0 | 0 | 0 | 0 |  |
| 12 | 0 | GENERAL OUTPUT | ON |  | 0 | NO PU/PD |  | 0 | 0 | 0 | 0 | 0 |  |
|  | 1 |  |  |  | 0 |  |  | 0 | 0 | 0 | 0 | 0 |  |
| 11 | 1 | ALTERNATE | SPI2 MOSI, HSR |  | 0 | NO PU/PD |  | 0 | 0 | 0 | 0 | 0 |  |
|  | 0 |  |  |  | 0 |  |  | 0 | 0 | 0 | 0 | 0 |  |
| 10 | 1 | ALTERNATE | SPI2 MISO, HSR |  | 0 | NO PU/PD |  | 0 | 0 | 0 | 0 | 0 |  |
|  | 0 |  |  |  | 0 |  |  | 0 | 0 | 0 | 0 | 0 |  |
| 9 | 1 | ALTERNATE | SPI2 SCLK, HSR |  | 0 | NO PU/PD |  | 0 | 0 | 0 | 0 | 0 |  |
|  | 0 |  |  |  | 0 |  |  | 0 | 0 | 0 | 0 | 0 |  |
| 8 | 0 | GENERAL OUTPUT | ON, HSR |  | 0 | NO PU/PD |  | 0 | 0 | 0 | 0 | 0 |  |
|  | 1 |  |  |  | 0 |  |  | 0 | 0 | 0 | 0 | 0 |  |
| 7 | 0 | INPUT | HIGH |  | 0 | PU ⬆ | 15 | 0 | 1 | 1 | 1 | 0 | AF15 |
|  | 0 |  |  |  | 1 |  | 14 | 0 | 0 | 0 | 0 | 0 | AF0 |
| 6 | 1 | ALTERNATE | TIM3_CH1, HSR, HDS |  | 0 | NO PU/PD | 13 | 0 | 0 | 0 | 0 | 0 | AF0 |
|  | 0 |  |  |  | 0 |  | 12 | 0 | 0 | 1 | 1 | 0 | AF15 |
| 5 | 1 | ALTERNATE | TIM1_CH1 HSR, HDS |  | 0 | NO PU/PD | 11 | 0 | 1 | 0 | 1 | 0 | AF0 |
|  | 0 |  |  |  | 0 |  | 10 | 0 | 1 | 0 | 0 | 0 | AF0 |
| 4 | 0 | GENERAL OUTPUT | OFF |  | 0 | PU ⬆ | 9 | 0 | 1 | 0 | 1 | 0 | AF0 |
|  | 1 |  |  |  | 1 |  | 8 | 0 | 1 | 1 | 1 | 0 | AF15 |
| 3 | 0 | INPUT | LOW, HDS |  | 0 | NO PU/PD | 7 | 0 | 0 | 0 | 1 | 0 | AF15 |
|  | 0 |  |  |  | 0 |  | 6 | 0 | 1 | 0 | 0 | 1 | AF2 |
| 2 | 1 | ANALOG | HSR |  | 0 | NO PU/PD | 5 | 0 | 1 | 1 | 0 | 1 | AF3 |
|  | 1 |  |  |  | 0 |  | 4 | 0 | 0 | 0 | 0 | 0 | AF15 |
| 1 | 1 | ANALOG | HSR |  | 0 | NO PU/PD | 3 | 0 | 0 | 0 | 0 | 1 | AF15 |
|  | 1 |  |  |  | 0 |  | 2 | 0 | 1 | 0 | 0 | 0 | AF15 |
| 0 | 0 | INPUT | LOW |  | 0 | NO PU/PD | 1 | 0 | 1 | 0 | 0 | 0 | AF15 |
|  | 0 |  |  |  | 0 |  | 0 | 0 | 0 | 0 | 0 | 0 | AF15 |



| GPIOB |  |  |  |  |  |  |  |  |  |  |  |  |  |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
|  |  |  |  |  |  |  |  |  |  |  |  |  |  |
| PIN | PMODE |  | STATE |  | PUPD |  |  | POTYPE | SR | POD | PID | DS | AF |
| 15 | 1 | ANALOG |  |  | 0 | NO PU/PD |  | 0 | 0 | 0 | 0 | 0 |  |
|  | 1 |  |  |  | 0 |  |  | 0 | 0 | 0 | 0 | 0 |  |
| 14 | 1 | ANALOG |  |  | 0 | NO PU/PD |  | 0 | 0 | 0 | 0 | 0 |  |
|  | 1 |  |  |  | 0 |  |  | 0 | 0 | 0 | 0 | 0 |  |
| 13 | 1 | ANALOG |  |  | 0 | NO PU/PD |  | 0 | 0 | 0 | 0 | 0 |  |
|  | 1 |  |  |  | 0 |  |  | 0 | 0 | 0 | 0 | 0 |  |
| 12 | 1 | ANALOG |  |  | 0 | NO PU/PD |  | 0 | 0 | 0 | 0 | 0 |  |
|  | 1 |  |  |  | 0 |  |  | 0 | 0 | 0 | 0 | 0 |  |
| 11 | 1 | ANALOG |  |  | 0 | NO PU/PD |  | 0 | 0 | 0 | 0 | 0 |  |
|  | 1 |  |  |  | 0 |  |  | 0 | 0 | 0 | 0 | 0 |  |
| 10 | 1 | ANALOG |  |  | 0 | NO PU/PD |  | 0 | 0 | 0 | 0 | 0 |  |
|  | 1 |  |  |  | 0 |  |  | 0 | 0 | 0 | 0 | 0 |  |
| 9 | 1 | ANALOG |  |  | 0 | NO PU/PD |  | 0 | 0 | 0 | 0 | 0 |  |
|  | 1 |  |  |  | 0 |  |  | 0 | 0 | 0 | 0 | 0 |  |
| 8 | 1 | ALTERNATE | TIM8_CH1, HSR, HDS |  | 0 | NO PU/PD |  | 0 | 0 | 0 | 0 | 0 |  |
|  | 0 |  |  |  | 0 |  |  | 0 | 0 | 0 | 0 | 0 |  |
| 7 | 0 | GENERAL OUTPUT | HIGH, HSR |  | 0 | NO PU/PD | 15 | 0 | 1 | 0 | 0 | 0 | AF15 |
|  | 1 |  |  |  | 0 |  | 14 | 0 | 1 | 0 | 0 | 0 | AF15 |
| 6 | 0 | GENERAL OUTPUT | HIGH, HSR |  | 0 | NO PU/PD | 13 | 0 | 1 | 0 | 0 | 0 | AF15 |
|  | 1 |  |  |  | 0 |  | 12 | 0 | 1 | 0 | 0 | 0 | AF15 |
| 5 | 1 | ALTERNATE | SPI1_I2S_MOSI_SD, HSR |  | 0 | NO PU/PD | 11 | 0 | 1 | 0 | 0 | 0 | AF15 |
|  | 0 |  |  |  | 0 |  | 10 | 0 | 1 | 0 | 0 | 0 | AF15 |
| 4 | 0 | GENERAL OUTPUT | OPEN-DRAIN |  | 0 | NO PU/PD | 9 | 0 | 1 | 0 | 0 | 0 | AF15 |
|  | 1 |  |  |  | 0 |  | 8 | 0 | 1 | 0 | 0 | 1 | AF5 |
| 3 | 1 | ALTERNATE | SPI1_I2S_SCLK_CK, HSR, HIGH |  | 0 | NO PU/PD | 7 | 0 | 1 | 1 | 1 | 0 | AF15 |
|  | 0 |  |  |  | 0 |  | 6 | 0 | 1 | 1 | 1 | 0 | AF15 |
| 2 | 0 | INPUT | HIGH, PULLUP |  | 0 | PU ⬆ | 5 | 0 | 1 | 0 | 0 | 0 | AF0 |
|  | 0 |  |  |  | 1 |  | 4 | 1 | 0 | 0 | 0 | 0 | AF15 |
| 1 | 0 | INPUT | LOW, PULLUP |  | 0 | PU ⬆ | 3 | 0 | 1 | 1 | 1 | 0 | AF0 |
|  | 0 |  |  |  | 1 |  | 2 | 0 | 0 | 0 | 1 | 0 | AF15 |
| 0 | 0 | GENERAL OUTPUT | LOW |  | 0 | NO PU/PD | 1 | 0 | 0 | 0 | 0 | 0 | AF15 |
|  | 1 |  |  |  | 0 |  | 0 | 0 | 0 | 0 | 0 | 0 | AF15 |

| GPIOC |  |  |  |  |  |  |  |  |  |  |  |  |  |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
|  |  |  |  |  |  |  |  |  |  |  |  |  |  |
| PIN | PMODE |  | STATE |  | PUPD |  |  | POTYPE | SR | POD | PID | DS | AF |
| 15 | 0 | INPUT | HIGH, PULLUP |  | 0 | PU ⬆ |  | 0 | 0 | 0 | 0 | 0 |  |
|  | 0 |  |  |  | 1 |  |  | 0 | 0 | 0 | 0 | 0 |  |
| 14 | 0 | INPUT | HIGH, PULLUP |  | 0 | PU ⬆ |  | 0 | 0 | 0 | 0 | 0 |  |
|  | 0 |  |  |  | 1 |  |  | 0 | 0 | 0 | 0 | 0 |  |
| 13 | 1 | ANALOG | HSR |  | 0 | NO PU/PD |  | 0 | 0 | 0 | 0 | 0 |  |
|  | 1 |  |  |  | 0 |  |  | 0 | 0 | 0 | 0 | 0 |  |
| 12 | 0 | INPUT |  |  | 0 | NO PU/PD |  | 0 | 0 | 0 | 0 | 0 |  |
|  | 0 |  |  |  | 0 |  |  | 0 | 0 | 0 | 0 | 0 |  |
| 11 | 0 | INPUT |  |  | 0 | NO PU/PD |  | 0 | 0 | 0 | 0 | 0 |  |
|  | 0 |  |  |  | 0 |  |  | 0 | 0 | 0 | 0 | 0 |  |
| 10 | 0 | INPUT |  |  | 0 | NO PU/PD |  | 0 | 0 | 0 | 0 | 0 |  |
|  | 0 |  |  |  | 0 |  |  | 0 | 0 | 0 | 0 | 0 |  |
| 9 | 0 | INPUT |  |  | 0 | NO PU/PD |  | 0 | 0 | 0 | 0 | 0 |  |
|  | 0 |  |  |  | 0 |  |  | 0 | 0 | 0 | 0 | 0 |  |
| 8 | 0 | INPUT |  |  | 0 | NO PU/PD |  | 0 | 0 | 0 | 0 | 0 |  |
|  | 0 |  |  |  | 0 |  |  | 0 | 0 | 0 | 0 | 0 |  |
| 7 | 0 | INPUT |  |  | 0 | NO PU/PD | 15 | 0 | 0 | 0 | 1 | 0 | AF15 |
|  | 0 |  |  |  | 0 |  | 14 | 0 | 0 | 0 | 1 | 0 | AF15 |
| 6 | 0 | INPUT |  |  | 0 | NO PU/PD | 13 | 0 | 1 | 0 | 0 | 0 | AF15 |
|  | 0 |  |  |  | 0 |  | 12 | 0 | 0 | 0 | 0 | 0 | AF0 |
| 5 | 0 | INPUT |  |  | 0 | NO PU/PD | 11 | 0 | 0 | 0 | 0 | 0 | AF0 |
|  | 0 |  |  |  | 0 |  | 10 | 0 | 0 | 0 | 0 | 0 | AF0 |
| 4 | 0 | INPUT |  |  | 0 | NO PU/PD | 9 | 0 | 0 | 0 | 0 | 0 | AF0 |
|  | 0 |  |  |  | 0 |  | 8 | 0 | 0 | 0 | 0 | 0 | AF0 |
| 3 | 0 | INPUT |  |  | 0 | NO PU/PD | 7 | 0 | 0 | 0 | 0 | 0 | AF0 |
|  | 0 |  |  |  | 0 |  | 6 | 0 | 0 | 0 | 0 | 0 | AF0 |
| 2 | 0 | INPUT |  |  | 0 | NO PU/PD | 5 | 0 | 0 | 0 | 0 | 0 | AF0 |
|  | 0 |  |  |  | 0 |  | 4 | 0 | 0 | 0 | 0 | 0 | AF0 |
| 1 | 0 | INPUT |  |  | 0 | NO PU/PD | 3 | 0 | 0 | 0 | 0 | 0 | AF0 |
|  | 0 |  |  |  | 0 |  | 2 | 0 | 0 | 0 | 0 | 0 | AF0 |
| 0 | 0 | INPUT |  |  | 0 | NO PU/PD | 1 | 0 | 0 | 0 | 0 | 0 | AF0 |
|  | 0 |  |  |  | 0 |  | 0 | 0 | 0 | 0 | 0 | 0 | AF0 |


| GPIOF |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
|  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
| PIN | PMODE |  | STATE |  | PUPD |  |  | POTYPE | SR | POD | PID | DS | AF | AFL |
| 15 | 0 | INPUT |  |  | 0 | NO PU/PD |  | 0 | 0 | 0 | 0 | 0 |  | 1 |
|  | 0 |  |  |  | 0 |  |  | 0 | 0 | 0 | 0 | 0 |  | 1 |
| 14 | 0 | INPUT |  |  | 0 | NO PU/PD |  | 0 | 0 | 0 | 0 | 0 |  | 1 |
|  | 0 |  |  |  | 0 |  |  | 0 | 0 | 0 | 0 | 0 |  | 1 |
| 13 | 0 | INPUT |  |  | 0 | NO PU/PD |  | 0 | 0 | 0 | 0 | 0 |  | 1 |
|  | 0 |  |  |  | 0 |  |  | 0 | 0 | 0 | 0 | 0 |  | 1 |
| 12 | 0 | INPUT |  |  | 0 | NO PU/PD |  | 0 | 0 | 0 | 0 | 0 |  | 1 |
|  | 0 |  |  |  | 0 |  |  | 0 | 0 | 0 | 0 | 0 |  | 1 |
| 11 | 0 | INPUT |  |  | 0 | NO PU/PD |  | 0 | 0 | 0 | 0 | 0 |  | 0 |
|  | 0 |  |  |  | 0 |  |  | 0 | 0 | 0 | 0 | 0 |  | 0 |
| 10 | 0 | INPUT |  |  | 0 | NO PU/PD |  | 0 | 0 | 0 | 0 | 0 |  | 0 |
|  | 0 |  |  |  | 0 |  |  | 0 | 0 | 0 | 0 | 0 |  | 0 |
| 9 | 0 | INPUT |  |  | 0 | NO PU/PD |  | 0 | 0 | 0 | 0 | 0 |  | 0 |
|  | 0 |  |  |  | 0 |  |  | 0 | 0 | 0 | 0 | 0 |  | 0 |
| 8 | 0 | INPUT |  |  | 0 | NO PU/PD |  | 0 | 0 | 0 | 0 | 0 |  | 0 |
|  | 0 |  |  |  | 0 |  |  | 0 | 0 | 0 | 0 | 0 |  | 0 |
| 7 | 1 | ANALOG | HSR |  | 0 | NO PU/PD | 15 | 0 | 0 | 0 | 0 | 0 | AF0 | 0 |
|  | 1 |  |  |  | 0 |  | 14 | 0 | 0 | 0 | 0 | 0 | AF0 | 0 |
| 6 | 1 | ANALOG | HSR |  | 0 | NO PU/PD | 13 | 0 | 0 | 0 | 0 | 0 | AF0 | 0 |
|  | 1 |  |  |  | 0 |  | 12 | 0 | 0 | 0 | 0 | 0 | AF0 | 0 |
| 5 | 0 | INPUT |  |  | 0 | NO PU/PD | 11 | 0 | 0 | 0 | 0 | 0 | AF0 | 1 |
|  | 0 |  |  |  | 0 |  | 10 | 0 | 0 | 0 | 0 | 0 | AF0 | 1 |
| 4 | 0 | INPUT |  |  | 0 | NO PU/PD | 9 | 0 | 0 | 0 | 0 | 0 | AF0 | 1 |
|  | 0 |  |  |  | 0 |  | 8 | 0 | 0 | 0 | 0 | 0 | AF0 | 1 |
| 3 | 0 | INPUT |  |  | 0 | NO PU/PD | 7 | 0 | 1 | 0 | 0 | 0 | AF15 | 1 |
|  | 0 |  |  |  | 0 |  | 6 | 0 | 1 | 0 | 0 | 0 | AF15 | 1 |
| 2 | 1 | ANALOG | HSR |  | 0 | NO PU/PD | 5 | 0 | 0 | 0 | 0 | 0 | AF0 | 1 |
|  | 1 |  |  |  | 0 |  | 4 | 0 | 0 | 0 | 0 | 0 | AF0 | 1 |
| 1 | 1 | ANALOG | HSR |  | 0 | NO PU/PD | 3 | 0 | 0 | 0 | 0 | 0 | AF0 | 1 |
|  | 1 |  |  |  | 0 |  | 2 | 0 | 1 | 0 | 0 | 0 | AF15 | 1 |
| 0 | 1 | ANALOG | HSR |  | 0 | NO PU/PD | 1 | 0 | 1 | 0 | 0 | 0 | AF15 | 1 |
|  | 1 |  |  |  | 0 |  | 0 | 0 | 1 | 0 | 0 | 0 | AF15 | 1 |



original peripherals
GPIOA 0x40010800
GPIOx_PMODE @ 0x0 = 0x69a9293c
GPIOx_POTYPE @ 0x4 = 0x00000000
GPIOx_SR @ 0x8 = 0x00008f66
GPIOx_PUPD @ 0xc = 0x24004100
GPIOx_PID @ 0x10 = 0x00009b80
GPIOx_POD @ 0x14 = 0x00009120
GPIOx_PBSC @ 0x18 = 0x00000000
GPIOx_PLOCK @ 0x1c = 0x00000000
GPIOx_AFL @ 0x20 = 0xf23fffff
GPIOx_AFH @ 0x24 = 0xf00f000f
GPIOx_PBC @ 0x28 = 0x00000000
GPIOx_DS @ 0x2c = 0x00000068

GPIOB 0x40010c00
GPIOx_PMODE @ 0x0 = 0xfffe5981
GPIOx_POTYPE @ 0x4 = 0x00000010
GPIOx_SR @ 0x8 = 0x0000ffe8
GPIOx_PUPD @ 0xc = 0x00000014
GPIOx_PID @ 0x10 = 0x000000cc
GPIOx_POD @ 0x14 = 0x000000c8
GPIOx_PBSC @ 0x18 = 0x00000000
GPIOx_PLOCK @ 0x1c = 0x00000000
GPIOx_AFL @ 0x20 = 0xff0f0fff
GPIOx_AFH @ 0x24 = 0xfffffff5
GPIOx_PBC @ 0x28 = 0x00000000
GPIOx_DS @ 0x2c = 0x00000100

GPIOC 0x40011000
GPIOx_PMODE @ 0x0 = 0x0c000000
GPIOx_POTYPE @ 0x4 = 0x00000000
GPIOx_SR @ 0x8 = 0x00002000
GPIOx_PUPD @ 0xc = 0x50000000
GPIOx_PID @ 0x10 = 0x0000c000
GPIOx_POD @ 0x14 = 0x00000000
GPIOx_PBSC @ 0x18 = 0x00000000
GPIOx_PLOCK @ 0x1c = 0x00000000
GPIOx_AFL @ 0x20 = 0x00000000
GPIOx_AFH @ 0x24 = 0xfff00000
GPIOx_PBC @ 0x28 = 0x00000000
GPIOx_DS @ 0x2c = 0x00000000

GPIOF 0x40011c00
GPIOx_PMODE @ 0x0 = 0x0000f03f
GPIOx_POTYPE @ 0x4 = 0x00000000
GPIOx_SR @ 0x8 =  	
GPIOx_PUPD @ 0xc = 0x00000000
GPIOx_PID @ 0x10 = 0x00000000
GPIOx_POD @ 0x14 = 0x00000000
GPIOx_PBSC @ 0x18 = 0x00000000
GPIOx_PLOCK @ 0x1c = 0x00000000
GPIOx_AFL @ 0x20 = 0xff000fff
GPIOx_AFH @ 0x24 = 0x00000000
GPIOx_PBC @ 0x28 = 0x00000000
GPIOx_DS @ 0x2c = 0x00000000

