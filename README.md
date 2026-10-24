# Smart Temperature Monitoring and Alarm System

**LPC1768 · LM35 · 16×2 LCD · LED Bar Graph · On-board Buzzer**

![Platform](https://img.shields.io/badge/MCU-NXP%20LPC1768-blue)
![Language](https://img.shields.io/badge/Language-Embedded%20C-informational)
![IDE](https://img.shields.io/badge/IDE-Keil%20uVision4-orange)
![Board](https://img.shields.io/badge/Board-ALS--SDA--ARMCTXM3--01-green)

> ESD Lab Mini-Project (ICT 3143) · V Semester B.Tech (CCE) · Manipal Institute of Technology, MAHE · 2026

A real-time temperature monitor built on the NXP LPC1768 (ARM Cortex-M3). An LM35 sensor is read through the on-chip 12-bit ADC and the temperature is shown on a 16×2 LCD. An 8-LED bar graph shows the temperature range at a glance, and the on-board buzzer beeps when the temperature rises above **40 °C**.

---

## Table of Contents

1. [Features](#features)
2. [Behaviour at a glance](#behaviour-at-a-glance)
3. [Hardware](#hardware)
4. [Pin map](#pin-map)
5. [Wiring](#wiring)
6. [How it works](#how-it-works)
7. [Configuration](#configuration)
8. [Build and flash](#build-and-flash)
9. [Testing and demo](#testing-and-demo)
10. [Troubleshooting](#troubleshooting)
11. [Limitations and future scope](#limitations-and-future-scope)
12. [Project structure](#project-structure)
13. [Team](#team)
14. [References](#references)

---

## Features

- Continuous temperature measurement with an **LM35** (10 mV per °C) on ADC channel AD0.0.
- **12-bit ADC**, with the average of 16 conversions for a steadier reading.
- **16×2 LCD** showing the temperature, the sensor voltage, the raw ADC count and a status word.
- **LED bar graph** (LED1 to LED8): one more LED lights for every 2 °C, and the current step blinks.
- **Buzzer alarm** above 40 °C, beeping in time with the blinking LED, with **1 °C hysteresis**.
- **Sensor-fault detection**: if the ADC reading is stuck at either end, all LEDs blink and the buzzer stays silent.
- **Start-up self test**: a short buzzer beep when the system starts.

---

## Behaviour at a glance

### Status and alarm

| Temperature | LCD status | Buzzer |
|---|---|---|
| Below 33.0 °C | `NORMAL` | off |
| 33.0 °C to 40.0 °C | `WARM` | off |
| Above 40.0 °C | `HIGH!` | **beeps** with the blinking LED |
| ADC stuck at 0 or full scale | `FAULT` | off, all 8 LEDs blink |

Once the alarm has started, it stops only when the temperature falls to **39.0 °C or below**. Between 39.0 °C and 40.0 °C the buzzer can still be beeping while the LCD already reads `WARM`. This is the 1 °C hysteresis.

### LED bar graph

LEDs below the current step stay on; the current step blinks.

| Temperature | Steady LEDs | Blinking LED |
|---|---|---|
| Below 28 °C | none | LED1 |
| 28 – 30 °C | LED1 | LED2 |
| 30 – 32 °C | LED1–2 | LED3 |
| 32 – 34 °C | LED1–3 | LED4 |
| 34 – 36 °C | LED1–4 | LED5 |
| 36 – 38 °C | LED1–5 | LED6 |
| 38 – 40 °C | LED1–6 | LED7 |
| Above 40 °C | LED1–7 | LED8 |
| Sensor fault | none | all 8 together |

### LCD screen

```
T:29.5 C  295mV
Raw: 366 NORMAL
```

Line 1 shows the temperature and the voltage at the ADC pin. Line 2 shows the raw 12-bit ADC count and the status.

---

## Hardware

| Component | Quantity | Purpose |
|---|---|---|
| ALS-SDA-ARMCTXM3-01 board (NXP LPC1768) | 1 | Controller, 16×2 LCD, LEDs and buzzer BZ1 |
| LM35 temperature sensor | 1 | Analog temperature sensing (10 mV/°C) |
| Breadboard | 1 | LM35 connections |
| Jumper wires | several | Sensor wiring, plus one wire to the buzzer input |
| 10-core FRC cables | 2 | CNA to CNA1 (LEDs), CNC to CNAD (LCD) |
| USB or serial cable and PC | 1 | Programming with Flash Magic |
| Hair dryer, reference thermometer or multimeter | optional | Demonstration and checking readings |

---

## Pin map

| Function | LPC1768 pin | Board connection |
|---|---|---|
| LM35 Vout (ADC input AD0.0) | P0.23 | CND pin 1 |
| LM35 +Vs | 5 V | Test point TP1 |
| LM35 GND | GND | Test point TP3 and CND pin 10 |
| LEDs LED1 to LED8 | P0.4 to P0.11 | CNA to CNA1 (ribbon) |
| LCD D4 to D7 | P0.15 to P0.18 | CNC to CNAD (ribbon) |
| LCD RS | P0.19 | CNC to CNAD |
| LCD EN | P0.20 | CNC to CNAD |
| Buzzer control output | P2.0 | CND pin 7, jumper to **CNA5 pin 8** |

> The buzzer is driven from P2.0 and not from a CNA line, because the CNA lines already drive the LEDs. On our kit, the buzzer's signal input was found on **CNA5 pin 8**.

---

## Wiring

**Switch the board off before changing any wire.**

```
LM35  +Vs  ─────────────►  5 V  (TP1)
LM35  Vout ─────────────►  CND pin 1   (P0.23 / AD0.0)
LM35  GND  ─────────────►  GND (TP3)  and  CND pin 10

CND pin 7 (P2.0) ── jumper wire ──►  CNA5 pin 8   (on-board buzzer input)

CNA ══ ribbon ══►  CNA1   (8 LEDs, P0.4 – P0.11)
CNC ══ ribbon ══►  CNAD   (LCD, P0.15 – P0.20)
```

### Step by step

1. **LM35.** Hold it with the flat face toward you and the legs pointing down. Left leg: 5 V. Middle leg: CND pin 1. Right leg: ground. Put each leg in its own breadboard row.
2. **Ground.** Connect the right leg's row to TP3 (GND) and to CND pin 10.
3. **Buzzer.** Connect one jumper wire from CND pin 7 to CNA5 pin 8. Plug **no ribbon cable** into CNA5.
4. **LEDs.** Ribbon cable from CNA to CNA1.
5. **LCD.** Ribbon cable from CNC to CNAD. Short jumpers JP16 and JP5 and adjust the contrast with POT3 (as in Lab 9).
6. Connect the 5 V last.

### Notes

- The CND, CNA and CNA5 headers are 2×5 headers. Pin 1 is the corner where a ribbon cable's red stripe sits. Odd pins run along one row, even pins along the other, and pin 10 is the corner diagonally opposite pin 1.
- CNA5 also feeds the relay, DC-motor and stepper driver circuits. Only the one buzzer jumper wire is connected there. Keep all the other CNA5 pins unconnected and nothing attached to the relay or motor terminals.
- The LM35 must be powered from **5 V**, not 3.3 V. Its output stays well below the ADC limit of 3.3 V.

---

## How it works

1. **Sense.** The LM35 outputs 10 mV for each 1 °C, so the voltage in millivolts equals the temperature in tenths of a degree. For example, 405 mV is 40.5 °C.
2. **Convert.** The 12-bit ADC (reference 3.3 V) gives a count from 0 to 4095:

   ```
   V (mV) = ADC × 3300 / 4095
   T (°C) = V (mV) / 10
   ```

   One ADC count is about 0.8 mV, which is roughly 0.08 °C.
3. **Average.** The program discards the first conversion, then averages 16 more. The ADC clock is set to stay below the 4.5 MHz limit.
4. **Classify.** The temperature sets the LED step, the status text and the alarm state.
5. **Alarm.** The alarm turns on above 40 °C and off again at 39 °C or below. While it is on, the buzzer follows the blinking LED, which gives a beep-beep pattern.
6. **Fault check.** A count below 10 or above 4085 means a disconnected or shorted sensor. All LEDs then blink and the buzzer is held off.

### Code overview

| Function | Role |
|---|---|
| `lcd_nibble`, `lcd_cmd`, `lcd_data`, `lcd_puts`, `lcd_init` | 4-bit LCD driver |
| `adc_init`, `adc_read`, `read_mv` | ADC setup, one conversion, and the 16-sample average in millivolts |
| `led_level` | Converts the temperature to a bar-graph step (0 to 7) |
| `main` | Start-up self test, then the measure, classify, display and indicate loop |

---

## Configuration

These constants are at the top of the program.

| Constant | Default | Meaning |
|---|---|---|
| `HIGH_T` | `400u` | Alarm threshold, in tenths of a degree (40.0 °C) |
| `HYST` | `10u` | The alarm clears this far below `HIGH_T` (1.0 °C) |
| `BASE_T` | `260u` | Start of the LED steps (26.0 °C) |
| `STEP_T` | `20u` | Temperature per LED step (2.0 °C) |
| `330u` (in `main`) | `330u` | Temperature where the status changes from NORMAL to WARM (33.0 °C) |

To test the alarm without a heat source, set `HIGH_T` to `340u` and warm the sensor with your fingers. Put it back to `400u` for the final demo.

---

## Build and flash

### Keil uVision4

1. Create a new project in its own folder and choose **NXP, LPC1768**.
2. Click **Yes** when asked to add the startup file.
3. Copy `system_LPC17xx.c` from `C:\Keil\ARM\Startup\NXP\LPC17xx\` into the project folder and add it to the source group.
4. Add `main.c` (the project code).
5. Open **Project, Options for Target, Output** and tick **Create HEX File**.
6. Build with **F7**. You need 0 errors.

### Flash Magic

Settings as in the lab manual (Lab 6):

| Setting | Value |
|---|---|
| Device | LPC1768 |
| COM port | the port your cable uses |
| Baud rate | 9600 |
| Interface | None (ISP) |
| Oscillator | 12 MHz |
| Advanced options | Use DTR and RTS to control RST and ISP pin; keep RTS asserted; T1 = 50 ms, T2 = 100 ms |

1. Select the generated `.hex` file.
2. Short jumper JP3 and set the SW21 DIP switch as your lab instructs.
3. Click **Start**, then press the reset switch SW1.

---

## Testing and demo

### Quick checks

1. **Power on.** The LCD shows `LM35 Monitor` and `System starting`, and the buzzer gives one short beep.
2. **Room temperature.** The LCD shows about 28 to 30 °C and `NORMAL`. One or two LEDs are lit.
3. **Warm the sensor.** Pinch it with your fingers, or aim a hair dryer at it from 25 to 30 cm. The LED bar grows step by step.
4. **Above 33 °C.** The status changes to `WARM`.
5. **Above 40 °C.** The status changes to `HIGH!`, LED7 stays lit, LED8 blinks and the buzzer beeps.
6. **Cool down.** The buzzer stops when the temperature falls to 39 °C, then the status returns to `WARM` and `NORMAL`.

### Safety

- Keep the hair dryer **25 to 30 cm** away and aim it at the sensor only, never at the board or the LCD.
- Many LM35 variants are rated up to 100 °C. Stop heating if the reading passes about **80 °C**.
- Do not hold a flame near the sensor.

### Checking accuracy

Compare the LCD reading with a reference thermometer at room temperature, with finger warmth and with the hair dryer, and note the difference for each. The result depends on the LM35 variant, the ADC reference and the wiring.

---

## Troubleshooting

| Symptom | Likely cause and fix |
|---|---|
| LCD blank or only solid blocks | Adjust POT3 for contrast. Check JP16 and JP5 are shorted and that the CNC to CNAD ribbon is seated the right way round. |
| `FAULT` and all LEDs blinking | The yellow (Vout) or ground wire is loose, or the LM35 is not powered. Check CND pin 1 and the ground connections. |
| Reads about 330.0 °C and 3300 mV | The ADC pin is at 3.3 V. Vout is not on CND pin 1, or 5 V has reached the pin. Unplug and recheck the legs. |
| Reading does not change when touched | A wire is loose, or the LM35 legs share a breadboard row. |
| No start-up beep | Check the jumper from CND pin 7 to CNA5 pin 8 and that no ribbon cable is on CNA5. |
| Buzzer sounds all the time and goes quiet during the alarm | The buzzer input is active-low. Swap the `FIOSET` and `FIOCLR` lines for `BUZZ_MASK` in the program. |
| Relay clicks | CNA5 also feeds the relay driver. Make sure only the one buzzer jumper is connected to CNA5. |
| LEDs do not light | Check the CNA to CNA1 ribbon, and that nothing else is plugged into CNA. |

---

## Limitations and future scope

**Limitations**

- Single sensor with fixed thresholds set in the program.
- The accuracy depends on the LM35 variant, the ADC reference and the wiring.
- The buzzer control pin is specific to our kit and must be re-checked on another board.

**Future scope**

- Adjustable alarm limits with a potentiometer or the matrix keypad.
- A relay or MOSFET driver for automatic fan control.
- PWM output for fan speed or LED brightness in proportion to temperature.
- Logging the readings to a PC over UART.
- Wireless alerts over Bluetooth, Wi-Fi or GSM.
- Several sensors for different points in an enclosure.

---

## Project structure

```
.
├── main.c        # complete program (LCD, ADC, LED bar graph, buzzer logic)
└── README.md     # this file
```

---

## Team

| Name | Registration No. |
|---|---|
| Shivam Shandilya | 240953622 |
| Ishita Padhi | 240953630 |
| Soukaryo Chandra | 240953644 |

School of Computer Engineering, Manipal Institute of Technology, Manipal Academy of Higher Education.

---

## References

1. NXP Semiconductors, *LPC176x/5x User Manual*, UM10360.
2. Texas Instruments, *LM35 Precision Centigrade Temperature Sensors*, datasheet.
3. *Embedded Systems Lab Manual (ICT 3143)*, School of Computer Engineering, MIT Manipal.
4. Keil, *µVision IDE documentation*.
