# Project Enceladus

## Overview

**Project Enceladus** is an ESP32-based cooling-system monitor, alarm controller, display, and lighting controller for a **Monport Reno 45 Pro CO₂ laser**.

A 20-gallon glass aquarium will serve as the coolant reservoir and will be styled as a modern industrial / high-end PC water-cooling installation with addressable LEDs and a decorative bubble reactor.

The project will use a **single ESP32 running WLED plus custom Enceladus functionality**.

Development will be performed incrementally with **Codex**, with each phase ending in a working and independently testable system.

---

# 1. Version 1 Requirements

Enceladus Version 1 shall:

1. Monitor coolant **temperature**.
2. Monitor coolant **flow rate**.
3. Monitor coolant **level**.
4. Display live readings on a local OLED.
5. Determine an overall system status.
6. Provide an audible alarm using a buzzer.
7. Provide a local visual status indicator using one RGB/status LED.
8. Run **WLED on the same ESP32**.
9. Control decorative tank lighting according to cooling-system status.
10. Continue monitoring locally without requiring Wi-Fi.
11. Remain electrically independent of the Reno 45 internal electronics.
12. Use modular/removable sensors and connectors where practical.
13. Be designed for future expansion.

---

# 2. Reservoir

## Selected Reservoir

- 20-gallon glass aquarium
- Approximately 16 inches tall
- Distilled water
- Coolant level visible at all times
- No drilling of aquarium glass

## Visual Design

The reservoir itself will be part of the finished installation.

Desired appearance:

- Modern industrial
- High-end PC water cooling
- Laboratory/reactor aesthetic
- Black structural accents
- Clear acrylic components
- Cyan/ice-blue illumination
- Addressable LEDs
- Bubbling acrylic reactor
- Clean tubing and cable management

---

# 3. Existing Reno Cooling System

The original Monport submersible coolant pump will remain in use.

It will:

- Sit at the bottom of the aquarium
- Remain continuously submerged
- Supply coolant to the Reno 45 laser tube
- Remain independent of the decorative bubble system

## Existing Hose Measurements

Measured with calipers:

- Outside diameter: approximately **10 mm**
- Wall thickness: approximately **2 mm**
- Inside diameter: approximately **6 mm**

## Measured Actual Coolant Flow

500 mL fill tests:

| Test | Time |
|---|---:|
| 1 | 21.32 sec |
| 2 | 20.10 sec |
| 3 | 20.46 sec |

Measured normal system flow:

**Approximately 1.45 L/min**

This is the baseline flow through the actual Reno cooling circuit.

---

# 4. ESP32 Controller

## Selected Hardware

**MELIFE ESP32 ESP-32S Development Board**

Amazon ASIN:

`B07Q576VWZ`

Already owned.

One ESP32 will handle the entire project.

Responsibilities:

- Temperature acquisition
- Flow pulse counting
- Float-switch monitoring
- OLED display
- Alarm logic
- Buzzer
- Status LED
- System state machine
- WLED
- Addressable tank lighting

A second microcontroller is not currently required.

Exact GPIO assignments will be determined during circuit development.

---

# 5. Temperature Sensor

## Selected Hardware

**Waterproof DS18B20 temperature probe**

Amazon ASIN:

`B0C8J77NJR`

Already owned.

The kit also includes the required **4.7 kΩ pull-up resistors**.

## Interface

DS18B20 1-Wire digital interface.

Typical circuit:

```text
3.3V -------- DS18B20 VCC
 |
4.7k
 |
GPIO -------- DS18B20 DATA

GND ---------- DS18B20 GND
```

The sensor will preferably operate from 3.3 V.

## Installation

Probe enters from the top of the aquarium.

Placement should be:

- Fully submerged
- In circulating bulk coolant
- Away from the water surface
- Away from direct pump discharge
- Away from direct contact with aquarium glass

## Measurement

Primary value:

`Coolant Temperature`

Example:

```text
TEMP  21.8 C
```

Warning and critical thresholds will be determined experimentally.

---

# 6. Coolant Level Sensor

## Selected Hardware

**Piutouyar vertical float switch**

Amazon ASIN:

`B0F7K9CXGC`

Already owned.

Two switches are available, but Version 1 will use only **one**.

The second will remain a spare.

## Decision: One Float Switch

Two switches were originally considered:

- LOW
- CRITICAL LOW

Because the final reservoir is transparent glass and immediately visible whenever approaching the laser, the early LOW indication provides limited additional value.

The single electronic float will therefore indicate:

**LOW / UNSAFE COOLANT LEVEL**

## Position

The float must activate while:

- The coolant pump is still completely submerged
- Significant coolant remains
- Cooling is still functioning

Example:

```text
Normal Fill Level
~~~~~~~~~~~~~~~~~~~~~~~~


      Normal reserve


------------------------  Float threshold

      LOW COOLANT


      Safety reserve


      [ PUMP ]


________________________
Tank bottom
```

## Electrical Interface

The float behaves as a simple switch.

Likely configuration:

```text
ESP32 GPIO
    |
 Pull-up
    |
 Float Switch
    |
   GND
```

Firmware shall debounce the input.

The switch's normal-open/normal-closed behavior will be bench-tested before implementation.

---

# 7. Coolant Flow Sensor

## Selected Candidate

**Chiffonade 6 mm Hall-effect water-flow sensor**

Amazon ASIN:

`B0FZVK9WB5`

This is the Amazon flow sensor previously identified.

## Mechanical Compatibility

Existing Reno hose:

**6 mm inside diameter**

The sensor appears mechanically appropriate for the existing tubing.

## Target Flow

Normal measured flow:

**~1.45 L/min**

The sensor must:

- Reliably detect approximately 1.45 L/min
- Detect reduced flow
- Detect zero flow
- Introduce minimal restriction

## Installation

```text
20G Reservoir
      |
Monport Pump
      |
6 mm Hose
      |
FLOW SENSOR
      |
6 mm Hose
      |
Reno 45
      |
Laser Tube
      |
Return Hose
      |
20G Reservoir
```

## Flow Sensor Purpose

The Hall sensor provides direct confirmation that coolant is actually moving.

It can detect:

- Dead pump
- Unplugged pump
- Jammed impeller
- Kinked hose
- Blocked tubing
- Restricted cooling circuit
- Other loss-of-circulation conditions

Flow monitoring is therefore more useful than simply determining whether the pump has electrical power.

## Electrical Verification Required

Before connecting to ESP32:

1. Identify VCC.
2. Identify GND.
3. Identify pulse signal.
4. Determine required supply voltage.
5. Measure pulse HIGH voltage.
6. Determine whether output is open collector/open drain.
7. Determine pulse calibration constant.

**ESP32 GPIO pins are 3.3 V logic.**

A 5 V signal must not be connected directly to an ESP32 input.

Possible interface solutions:

- 3.3 V pull-up for open-collector output
- Resistor divider
- Logic-level converter

---

# 8. OLED Display

## Selected Hardware

Existing approximately **1-inch I²C OLED**.

Already owned.

Pins:

```text
GND
VCC
SCL
SDA
```

The display appears capable of roughly four useful text rows.

Likely controller:

- SSD1306
- SSD1305
- Similar compatible controller

Exact controller has not yet been confirmed.

## Verification

During Phase 1 determine:

- Controller IC
- Resolution
- I²C address
- Operating voltage

Likely resolution:

```text
128 x 64
```

or:

```text
128 x 32
```

Common I²C address:

```text
0x3C
```

but the address shall be detected rather than assumed.

## Proposed Normal Display

```text
ENCELADUS
T 21.8C
F 1.45L
L OK
```

Alternative:

```text
TEMP  21.8C
FLOW  1.45L
LEVEL OK
NORMAL
```

Readability is more important than graphics.

---

# 9. Audible Alarm

## Selected Hardware

**UMLIFE mini active buzzer**

Amazon ASIN:

`B0F1KFHSNK`

Already owned.

Characteristics:

- Two terminals
- Active buzzer
- Fixed tone
- Approximately 5 V nominal operation
- Approximately 2 kHz tone
- Approximately 85 dB

Because the buzzer is a two-terminal load rather than a logic-input module, the ESP32 should **not directly supply its operating current**.

## Proposed Driver

Use:

- Small NPN transistor, or preferably
- Small logic-level N-channel MOSFET

Concept:

```text
               +5V
                |
             BUZZER
                |
                |
             MOSFET
                |
               GND

ESP32 GPIO ---> Gate
```

The exact transistor/MOSFET and supporting resistor values will be selected during circuit design.

## Alarm Uses

The buzzer may indicate:

- Low coolant
- Low coolant flow
- No coolant flow
- High temperature
- Critical temperature
- Sensor failure

Different beep patterns can communicate different severity levels even though the buzzer has a fixed audio frequency.

For example:

```text
WARNING:
beep ... beep ... beep

CRITICAL:
BEEP-BEEP-BEEP-BEEP
```

A future alarm acknowledge/mute button may be added.

---

# 10. Local Status LED

## Requirement

One local visual status indicator.

Preferred hardware:

**Single addressable RGB LED/pixel**

This is preferable to a conventional three-channel RGB LED because it:

- Requires one GPIO
- Requires no three-channel PWM
- Supports arbitrary colors
- Supports blinking/pulsing
- Fits naturally with WLED-style control

Proposed colors:

| Color | Meaning |
|---|---|
| Green | Normal |
| Amber | Warning |
| Red | Critical |
| Off | Controller unavailable/off |

Exact device to be selected.

---

# 11. WLED Architecture

## Requirement

WLED and Enceladus shall operate on the **same ESP32**.

We will not attempt to run two independent firmware applications simultaneously.

Instead:

**Enceladus will be implemented as custom functionality within WLED, most likely as a WLED usermod.**

Concept:

```text
                 ESP32
                   |
        +----------+----------+
        |                     |
     WLED Core          ENCELADUS
                         Usermod
                            |
        +-------------------+-------------------+
        |                   |                   |
       TEMP                FLOW               LEVEL
    DS18B20            Hall Sensor            Float
                            |
                      SYSTEM STATE
                            |
              +-------------+-------------+
              |             |             |
             OLED         BUZZER       STATUS LED
                                           |
                                           |
                                    WLED Lighting
```

## Enceladus Usermod Responsibilities

- DS18B20 acquisition
- Hall pulse counting
- L/min calculation
- Float monitoring
- OLED updates
- Buzzer control
- Local LED control
- Alarm handling
- Sensor validation
- System state calculation
- WLED status/effect selection

## WLED Responsibilities

- Addressable LED control
- Effects
- Palettes
- Presets
- Segments
- Brightness
- Optional Wi-Fi UI

---

# 12. System State Model

All sensors feed one central Enceladus state machine.

Initial states:

```text
STARTUP
NORMAL
WARNING
CRITICAL
SENSOR_FAULT
```

## NORMAL

Example:

```text
Temp   21.8 C
Flow   1.45 L/min
Level  OK
```

Outputs:

- Green status LED
- Buzzer silent
- Normal OLED
- Normal reactor lighting

## WARNING

Potential causes:

- Temperature approaching limit
- Flow somewhat below normal
- Recoverable sensor anomaly

Outputs:

- Amber LED
- OLED warning
- Optional intermittent buzzer
- Amber/caution tank lighting

## CRITICAL

Potential causes:

- No coolant flow
- Severely reduced coolant flow
- Low coolant level
- Excessive temperature

Outputs:

- Red status LED
- Critical OLED message
- Audible alarm
- Red WLED alarm effect

## SENSOR_FAULT

Examples:

- DS18B20 disconnected
- Flow sensor produces implausible values
- Other input failure

The system should preferably **fail visibly rather than silently**.

---

# 13. Flow Processing

Normal measured reference:

**1.45 L/min**

Instantaneous Hall pulses shall not directly trigger alarms.

Firmware should:

1. Count pulses continuously.
2. Calculate flow over a defined interval.
3. Apply filtering/moving averaging.
4. Compare filtered flow against thresholds.
5. Require persistence before declaring a fault.

Possible classifications:

```text
NORMAL FLOW
REDUCED FLOW
CRITICAL FLOW
NO FLOW
```

Exact thresholds will be established after installing and calibrating the sensor.

---

# 14. WLED Reservoir Lighting

Addressable LEDs will illuminate:

- Aquarium
- Bubble reactor
- Possibly controller/enclosure accents

Possible LED technologies:

- WS2812B
- WS2811
- SK6812

Exact hardware remains to be selected.

## Normal Appearance

Industrial Enceladus theme:

- Cyan
- Ice blue
- Cool white
- Slow movement/pulse

## Status Integration

### Normal

```text
Cool cyan / ice blue
Slow reactor animation
```

### Temperature Rising

```text
Cyan -> Blue -> Amber
```

### Reduced Flow

```text
Amber pulse
```

### Flow Failure

```text
Red pulse / flash
```

### Low Coolant

```text
Red warning
```

The entire aquarium can therefore act as a very large status indicator.

---

# 15. Bubble Reactor

## Target Design

- Approximately 2-inch clear acrylic tube
- Approximately 12 inches tall
- Aquarium height approximately 16 inches
- Black acrylic base
- Black top collar
- Fine bubble effect
- LED illumination

Exact tube dimensions can change based on readily available material.

## Existing Air Source

Old aquarium air pump already owned.

System:

```text
Aquarium Air Pump
       |
     Airline
       |
    Air Stone
       |
  Acrylic Reactor
```

The bubble reactor is primarily decorative.

---

# 16. Separation of Systems

The decorative reactor and laser coolant plumbing shall remain independent.

## Laser Cooling

```text
Monport Pump
     |
Flow Sensor
     |
Reno 45
     |
Return
     |
Reservoir
```

## Bubble Reactor

```text
Air Pump
   |
Airline
   |
Air Stone
   |
Bubble Column
```

This prevents the decorative system from:

- Restricting laser coolant flow
- Introducing air into the laser tube
- Affecting pump reliability
- Becoming a dependency for laser cooling

---

# 17. Power Architecture

Enceladus shall initially use its own external low-voltage power supply.

Do not use the Reno 45 internal electronics as the project's power source.

Likely required rails:

```text
5V
3.3V
GND
```

Final supply size depends primarily on the addressable LED quantity.

The LED strip shall **not** be powered through the ESP32 development board.

All interconnected low-voltage electronics shall have an appropriate common reference/ground.

---

# 18. LED Logic-Level Conversion

ESP32 logic output:

**3.3 V**

Many 5 V addressable LEDs ideally expect a higher data signal.

The final circuit should therefore provision a proper:

**3.3 V -> 5 V logic buffer**

for the main WLED output.

The exact part will be selected during circuit development.

---

# 19. Physical Sensor Mounting

The aquarium glass shall not be drilled.

A removable top bracket should support:

- Float switch
- DS18B20
- Sensor wiring

Possible fabrication methods:

- Laser-cut acrylic
- 3D printing
- Combination of both

All sensors should remain removable for maintenance.

---

# 20. Current Hardware Inventory

## Owned / Selected

| Component | Status |
|---|---|
| MELIFE ESP32 ESP-32S — `B07Q576VWZ` | **Owned** |
| ~1-inch 4-pin I²C OLED | **Owned** |
| Waterproof DS18B20 — `B0C8J77NJR` | **Owned** |
| 4.7 kΩ DS18B20 resistors | **Owned** |
| Piutouyar float switches — `B0F7K9CXGC` | **Owned** |
| UMLIFE active buzzers — `B0F1KFHSNK` | **Owned** |
| Chiffonade 6 mm Hall sensor — `B0FZVK9WB5` | **Selected candidate** |
| 20-gallon glass aquarium | **Acquired/selected** |
| Original Monport coolant pump | **Owned** |
| Aquarium air pump | **Owned** |

## Still Needed / To Select

| Component | Status |
|---|---|
| Hall flow sensor | Purchase/test candidate |
| Addressable RGB status pixel | TBD |
| Addressable tank LEDs | TBD |
| 3.3V -> 5V LED data buffer | TBD |
| Buzzer MOSFET/transistor | TBD |
| Power supply | TBD |
| JST/screw connectors | TBD |
| Air stone | TBD |
| Clear acrylic reactor tube | TBD |
| Electronics enclosure | TBD |

---

# 21. Development Strategy

## Tooling

**Codex will be used for firmware development.**

The project shall be developed in small, independently testable phases.

Codex should not initially be asked to generate the complete Enceladus system.

Each phase should:

1. Have a clearly defined objective.
2. Introduce minimal new hardware.
3. Produce testable firmware.
4. Be verified on actual hardware.
5. Be committed/stabilized before beginning the next phase.

---

# 22. Phase 0 — Development Environment

### Goal

Establish a clean working firmware project.

Tasks:

- Identify exact ESP32 board variant.
- Establish Git repository.
- Configure VS Code/Codex environment.
- Select PlatformIO or Arduino build environment.
- Verify USB programming.
- Compile and flash basic test firmware.
- Establish project directory structure.

### Success Criteria

ESP32 reliably:

```text
Builds
Flashes
Boots
Outputs serial diagnostics
```

No sensors required yet.

---

# 23. Phase 1 — OLED

### Goal

Get the local display working.

Tasks:

- Connect OLED.
- Run I²C scanner.
- Determine address.
- Determine OLED controller.
- Determine resolution.
- Initialize display.
- Render four lines of text.

### Test Screen

```text
ENCELADUS
PHASE 1
OLED OK
READY
```

### Success Criteria

Display operates reliably through repeated power cycles.

---

# 24. Phase 2 — Temperature

### Goal

Integrate DS18B20.

Tasks:

- Install 4.7 kΩ pull-up.
- Detect DS18B20.
- Read temperature.
- Detect disconnected sensor.
- Display temperature.

### Display

```text
ENCELADUS
TEMP 21.8C
SENSOR OK
PHASE 2
```

### Success Criteria

Temperature is stable, plausible, and updates continuously.

---

# 25. Phase 3 — Water Level

### Goal

Integrate float switch.

Tasks:

- Determine NO/NC orientation.
- Choose GPIO.
- Implement pull-up.
- Implement debounce.
- Detect LOW/OK.
- Display level.

### Display

```text
TEMP  21.8C
LEVEL OK
FLOAT OK
PHASE 3
```

### Success Criteria

Changing float position reliably changes displayed state without false triggering.

---

# 26. Phase 4 — Coolant Flow

### Goal

Integrate Hall-effect flow sensor.

Before connecting signal to ESP32:

- Verify wiring.
- Measure supply requirement.
- Measure pulse-output voltage.
- Determine safe interface circuit.

Firmware tasks:

- Interrupt/pulse counting
- Flow conversion
- L/min calculation
- Filtering
- Sensor diagnostics

### Calibration Reference

Known actual flow:

**~1.45 L/min**

Compare Hall reading against manual timed-container measurements.

### Display

```text
TEMP 21.8C
FLOW 1.45L
LEVEL OK
PHASE 4
```

### Success Criteria

Displayed flow reasonably agrees with manual measurement and reliably detects stopped flow.

---

# 27. Phase 5 — State Machine

### Goal

Combine the three sensors into one coherent system.

Implement:

```text
STARTUP
NORMAL
WARNING
CRITICAL
SENSOR_FAULT
```

Tasks:

- Define preliminary temperature limits.
- Define preliminary flow limits.
- Add persistence timers.
- Add sensor validation.
- Prevent brief transients from generating false alarms.

### Success Criteria

Simulated sensor problems produce the correct state.

---

# 28. Phase 6 — Audible and Local Visual Alarms

### Goal

Add buzzer and RGB status LED.

Hardware:

- Active buzzer
- MOSFET/transistor driver
- Addressable RGB status pixel

Implement:

```text
NORMAL   = Green / silent

WARNING  = Amber / intermittent beep

CRITICAL = Red / urgent alarm
```

### Success Criteria

All state transitions produce correct visual and audible behavior.

---

# 29. Phase 7 — WLED Integration

### Goal

Move Enceladus functionality into WLED.

Tasks:

- Establish WLED source build.
- Confirm basic WLED operation on existing ESP32.
- Create Enceladus usermod.
- Migrate sensor components incrementally.
- Preserve OLED operation.
- Preserve alarm logic.
- Verify Wi-Fi independence of monitoring.

### Important Rule

Do not migrate all functionality simultaneously.

Suggested migration:

```text
WLED
 ↓
OLED
 ↓
Temperature
 ↓
Level
 ↓
Flow
 ↓
State Machine
 ↓
Alarm Outputs
```

### Success Criteria

Enceladus monitoring operates correctly while WLED simultaneously controls LEDs.

---

# 30. Phase 8 — Reservoir Lighting

### Goal

Add addressable tank lighting.

Tasks:

- Install logic-level buffer.
- Connect LEDs.
- Configure WLED segments.
- Create normal Enceladus preset.
- Create warning preset.
- Create critical preset.

### Initial Effects

```text
NORMAL
Ice-blue reactor

WARNING
Amber pulse

CRITICAL
Red alarm
```

### Success Criteria

Sensor state automatically influences tank lighting without interfering with monitoring.

---

# 31. Phase 9 — Physical Integration

### Goal

Turn the breadboard prototype into the finished Enceladus controller.

Tasks:

- Final connectors
- Sensor bracket
- Controller enclosure
- Cable management
- LED installation
- Bubble reactor
- Power distribution
- Strain relief
- Labels

### Success Criteria

The system is:

- Serviceable
- Neat
- Safe
- Removable
- Reliable

---

# 32. Codex Development Rules

When using Codex for Enceladus:

### Keep Phases Separate

Do not ask Codex to implement future-phase functionality prematurely.

### Preserve Known-Working Code

Once a phase works:

- Commit it
- Tag/checkpoint it
- Do not casually refactor it while adding unrelated functionality

### Hardware First

For each sensor:

1. Verify electrical interface.
2. Write minimal diagnostic code.
3. Test hardware.
4. Only then integrate into system architecture.

### Avoid Blocking Code

Firmware should avoid long `delay()` calls.

Sensor acquisition, display updates, alarms, and WLED must coexist without blocking each other.

### Separate Responsibilities

Prefer components/modules such as:

```text
TemperatureSensor
FlowSensor
LevelSensor
DisplayManager
AlarmManager
SystemState
```

rather than putting everything into one source file.

### Diagnostics

Serial logging should remain available throughout development.

Useful startup output:

```text
ENCELADUS
Firmware: x.x.x

OLED: OK
Temperature: OK
Flow: OK
Level: OK

System: NORMAL
```

---

# 33. Future Enhancements

Not part of Version 1:

## Laser Safety Interlock

Potentially inhibit laser firing for:

- No coolant flow
- Unsafe temperature
- Low coolant

Only after the Reno 45 interlock circuitry is fully understood.

## Second Temperature Sensor

Measure:

- Reservoir temperature
- Laser return temperature

Calculate coolant ΔT.

## Pump Current Monitoring

Monitor electrical pump operation in addition to actual flow.

## Data History

Track:

- Min/max temperature
- Average temperature
- Min/max flow
- Average flow

## Runtime Tracking

Track:

- Cooling-system runtime
- Pump runtime
- Laser-session runtime

## Maintenance Reminders

Examples:

- Coolant service
- Hose inspection
- Pump inspection
- Optics inspection

## Alarm History

Record recent:

```text
LOW FLOW
NO FLOW
LOW WATER
HIGH TEMP
SENSOR FAULT
```

## Physical Controls

Possible future buttons:

- Alarm acknowledge
- Silence
- Menu
- Display page
- Lighting mode

## Bubble-Pump Control

ESP32 could eventually control the aquarium air pump through isolated switching hardware.

## Additional Cooling

Only if measured coolant temperature demonstrates a need:

- Fan
- Radiator
- Heat exchanger

## Custom Enceladus WLED Effects

Possible effects:

- Reactor Core
- Enceladus Geyser
- Ice Flow
- Cryogenic
- Coolant Flow
- Thermal Warning
- Flow Failure
- Reactor Alarm

Actual sensor values could eventually influence animation speed, color, brightness, and pulse frequency.

---

# 34. Design Philosophy

Priorities, in order:

1. **Reliable coolant monitoring**
2. **Clear failure indication**
3. **Simple troubleshooting**
4. **Electrical safety**
5. **Modularity**
6. **Serviceability**
7. **Expandability**
8. **Industrial visual design**

Decorative lighting and the bubble reactor must never compromise coolant monitoring.

Where practical, Enceladus should **fail visibly rather than silently**.

---

# 35. Project Name

## Project Enceladus

Named after **Enceladus**, Saturn's icy moon famous for its subsurface ocean and water-rich geysers.

The name reflects:

- Water
- Cooling
- Flow
- Ice
- Geysers
- Reactor-style bubbling
- Scientific/industrial design