AVR128DB48 Curiosity Nano QPC port
==================================

[QuantumLeaps/qpc](https://github.com/QuantumLeaps/qp-arduino ) port for
[AVR128DB48 CURIOSITY NANO EVALUATION KIT](https://www.microchip.com/en-us/development-tool/EV35L43A) (Part Number: EV35L43A)

QPC compiled as static library.

Custom port for AVR128. Now avr not supported by the [QuantumLeaps](https://www.state-machine.com/)

BSP
---

Boards have different pin configuration. Direct move not available.

| pin          | AVR128DA48  | AVR128DB48  |
|--------------|-------------|-------------|
| Yellow LED0  | PC6         | PB3         | 
| User switch (SW0)  | PC7   | PB2         | 
| UART         | USART1      | USART3      | 
| UART RX      | PC0         | PB0         | 
| UART TX      | PC1         | PB1         |

blinker
-------

```bash
QPN version:
Program Memory Usage 	:	3752 bytes   2,9 % Full
Data Memory Usage 		:	194 bytes   1,2 % Full
QPC version:
Program Memory Usage 	:	4258 bytes   3,2 % Full
Data Memory Usage 		:	368 bytes   2,2 % Full
```

