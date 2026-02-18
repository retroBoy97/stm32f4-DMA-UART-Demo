# stm32f4-DMA-UART-Demo

Low-level STM32F4 peripheral drivers implemented in C through direct register manipulation, without relying on STM32 HAL or CubeMX abstractions. This project demonstrates DMA-based memory-to-memory data transfer with interrupt handling, and UART2 serial transmission with `printf` retargeting.

---

## Features

- **DMA2 Memory-to-Memory Transfer** — Configures DMA2 Stream 0 in memory-to-memory mode using FIFO buffering and half-word (16-bit) data width
- **Interrupt-Driven DMA** — Handles Transfer Complete (TCIF) and Transfer Error (TEIF) interrupts via the NVIC
- **UART2 Transmit** — Initializes USART2 on PA2 with a 115200 baud rate for serial output
- **printf Retargeting** — Redirects standard `printf` output to UART2 by overriding `__io_putchar`
- **Bare-metal register access** — All peripheral configuration done through direct register reads/writes using CMSIS device headers, no HAL abstraction

---

## Hardware

| Item | Details |
|------|---------|
| Microcontroller | STM32F446RE (or compatible STM32F4 series) |
| Board | NUCLEO-F446RE |
| System Clock | 16 MHz (HSI) |
| UART TX Pin | PA2 (AF7 — USART2_TX) |
| Baud Rate | 115200 |
| DMA Controller | DMA2 Stream 0 |
| Transfer Type | Memory-to-Memory |
| Data Width | Half-Word (16-bit) |

---

## Project Structure

```
stm32f4-DMA-UART-Demo/
├── Inc/
│   ├── DMA.h          # DMA function prototypes and interrupt flag definitions
│   └── UART.h         # UART function prototypes
├── Src/
│   ├── main.c         # Application entry point, transfer trigger, result printing
│   ├── DMA.c          # DMA2 configuration and transfer start routines
│   └── UART.c         # UART2 GPIO and peripheral initialization, write routine
└── README.md
```

---

## How It Works

### UART Initialization

USART2 is initialized on pin PA2, configured in alternate function mode (AF7). The baud rate is computed manually using the formula:

```
BRR = (peripheral_clock + baudrate / 2) / baudrate
```

`printf` is retargeted to UART2 by implementing `__io_putchar`, which overrides the weak stub in the C standard library. The linker automatically resolves all `printf` calls through this function at link time.

### DMA Transfer

DMA2 Stream 0 is configured for memory-to-memory transfers with the following settings:

| Parameter | Value |
|-----------|-------|
| Direction | Memory-to-Memory |
| PSIZE (source) | Half-Word (16-bit) |
| MSIZE (destination) | Half-Word (16-bit) |
| Memory Increment | Enabled |
| Peripheral Increment | Enabled |
| FIFO Mode | Enabled |
| FIFO Threshold | Full (16 bytes) |
| Interrupts | Transfer Complete, Transfer Error |

The source address is loaded into PAR, the destination into M0AR, and the number of elements into NDTR. The DMA then autonomously transfers data while the CPU waits on the `global_transfer_complete` flag set inside the ISR.

### Interrupt Handling

The `DMA2_Stream0_IRQHandler` checks the Low Interrupt Status Register (LISR) for:
- **TCIF0** — Transfer complete: sets the global flag and clears the interrupt
- **TEIF0** — Transfer error: clears the interrupt (error handling can be extended here)

Flags are cleared by writing to the Low Interrupt Flag Clear Register (LIFCR) to prevent the ISR from re-firing continuously.

---

## Building and Flashing

This project was developed using **STM32CubeIDE**. To build and flash:

1. Clone the repository and open it as an existing STM32CubeIDE project
2. Build the project (`Project > Build All`)
3. Flash to the board via ST-Link (`Run > Debug` or `Run > Run`)

To view UART output, connect a serial terminal (e.g. PuTTY, Tera Term, or minicom) to the board's virtual COM port at **115200 baud, 8N1**.

---

## Expected Output

After the DMA transfer completes, the following is printed over UART2:

```
temp buffer[0]: 1
temp buffer[1]: 2
temp buffer[2]: 3
temp buffer[3]: 4
temp buffer[4]: 5
```

---

## Key Concepts Demonstrated

- **Bare-metal peripheral configuration** without STM32 HAL
- **DMA FIFO mode** and how threshold affects bus efficiency
- **PSIZE/MSIZE** and how they control both data width and address increment step
- **Interrupt flag lifecycle** — setting, detecting, and clearing flags in an ISR
- **printf retargeting** on bare-metal via `__io_putchar`
- **CMSIS register access** vs HAL abstraction

---

## Author

**retroBoy97**  
Embedded Systems enthusiast — building up from the registers.
