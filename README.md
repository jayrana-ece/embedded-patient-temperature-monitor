# Embedded Patient Temperature Monitor

An embedded temperature monitoring system designed using the LPC2138 ARM7 microcontroller, LM35 temperature sensor, ADC0804 analog-to-digital converter, and a 16×2 LCD.

The project is developed in Embedded C using Keil µVision and simulated in Proteus.

## Project Overview

The system monitors temperature using an LM35 sensor. Since the sensor produces an analog voltage, the ADC0804 converts this voltage into an 8-bit digital value that can be processed by the LPC2138.

The microcontroller calculates the temperature, displays the reading on the LCD, and activates a buzzer when the temperature enters a configured alert condition.

## Features

- Temperature sensing using the LM35.
- Analog-to-digital conversion using the ADC0804.
- LPC2138-based temperature processing.
- Real-time temperature display on a 16×2 LCD.
- Buzzer alert for high or low temperature conditions.
- UART output for serial monitoring at 9600 baud.
- Circuit design and simulation using Proteus.
- Embedded C firmware developed in Keil µVision.

## Components Used

| Component | Purpose |
|---|---|
| LPC2138 | Main microcontroller |
| LM35 | Analog temperature sensor |
| ADC0804 | 8-bit analog-to-digital converter |
| 16×2 LCD | Temperature and status display |
| Buzzer | Temperature alert indication |
| Resistors and supporting components | Circuit interfacing |
| Virtual terminal | UART output monitoring in simulation |

## Software and Tools

- Keil µVision — Embedded C development and compilation.
- Proteus — Circuit design and simulation.
- Embedded C — Microcontroller programming.

## Pin Connections

### 1. LM35 and ADC0804

The LM35 generates an analog voltage proportional to temperature. This signal is connected to the ADC0804 analog input.

The ADC0804 digital output is connected to the LPC2138 as follows:

| ADC0804 signal | LPC2138 connection |
|---|---|
| D0–D7 | P1.16–P1.23 |
| WR | P0.6 |
| RD | P0.8 |
| INTR | P0.9 |
| VIN+ | LM35 output |
| VIN− | Ground, as configured in the circuit |

### 2. LCD Connections

The 16×2 LCD is interfaced in 4-bit mode.

| LCD signal | LPC2138 pin |
|---|---|
| RS | P0.16 |
| EN | P0.17 |
| D4 | P0.18 |
| D5 | P0.19 |
| D6 | P0.20 |
| D7 | P0.21 |
| RW | Ground |

### 3. Buzzer and UART

| Function | LPC2138 connection |
|---|---|
| Buzzer control | P0.22 |
| UART0 TXD | P0.0 |
| UART0 RXD | P0.1 |

The buzzer and other loads must be driven within the microcontroller's electrical limits. An appropriate driver circuit should be used where necessary.

## Working Principle

1. The LM35 senses the temperature and produces an analog voltage.
2. The ADC0804 converts the analog voltage into an 8-bit digital value.
3. The LPC2138 reads the ADC output and calculates the corresponding temperature.
4. The LCD displays the temperature and its status.
5. The controller compares the reading against predefined temperature thresholds.
6. The buzzer provides an alert when a configured threshold is crossed.
7. UART0 transmits monitoring information at 9600 baud.

## Temperature Status Logic

The following thresholds are configured in the project firmware.

| Temperature | Status | Alert |
|---|---|---|
| Below 35°C | LOW | Buzzer alert |
| 35°C to below 38°C | NORMAL | No alert |
| 38°C and above | HIGH | Buzzer alert |

These are project-defined demonstration thresholds, not clinical diagnostic limits. The system is an educational prototype and is not intended for medical diagnosis or patient-care decisions.

## Temperature Calculation

The ADC0804 produces an 8-bit digital output with values from 0 to 255.

For an ADC reference range of 0–5 V, the approximate input voltage is:

\[
V_{\text{in}}=\frac{\text{ADC value}\times 5}{255}
\]

For the LM35, which has a nominal sensitivity of 10 mV/°C:

\[
T(^{\circ}\mathrm{C})=\frac{V_{\text{in}}}{0.01}
\]

Therefore, the approximate temperature calculation is:

\[
T(^{\circ}\mathrm{C})=
\frac{\text{ADC value}\times 500}{255}
\]

This formula assumes the ADC input range is 0–5 V and the LM35 output is connected directly to the ADC input. Actual accuracy depends on the circuit configuration and component characteristics.

## Project Structure

```text
embedded-patient-temperature-monitor/
├── README.md
├── KEIL/
│   ├── Keil project files
│   └── Embedded C source
├── PROTEUS/
│   └── Proteus simulation files
├── Images/
│   ├── circuit.png
│   ├── keil.png
│   └── output.png
└── docs/
    └── project-report.pdf
```

Replace the example names with your actual files. Include only folders and files that are present in your repository.

## Simulation and Verification

The project is designed and evaluated in the Proteus simulation environment.

Suggested simulation checks:

- Verify that the LCD initializes correctly.
- Vary the simulated LM35 temperature input.
- Observe changes in the ADC output and displayed temperature.
- Check the LOW, NORMAL, and HIGH status transitions.
- Verify buzzer behavior for each configured threshold.
- Inspect UART output at 9600 baud, if enabled in the simulation.

Add screenshots showing the actual circuit, successful Keil build, and running Proteus simulation.

## Limitations

- The project has been evaluated through software simulation.
- Physical hardware implementation and testing have not been claimed.
- Temperature accuracy depends on the ADC reference, sensor characteristics, and firmware calculations.
- The project is intended for educational demonstration, not clinical use.

## Future Improvements

- Add temperature logging.
- Implement configurable alert thresholds.
- Add a graphical temperature trend display.
- Introduce calibration and error handling.
- Test the system on physical hardware in a controlled environment.

## Author

Jay Rana
