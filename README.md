# IoT Smart Alarm Clock (Lab 4)

## Overview
This project implements a smart IoT-enabled alarm clock using a TM4C123 LaunchPad and an ESP8266 Wi-Fi module. The system connects to a web-based MQTT application, allowing real-time monitoring and remote control of the clock through a publish–subscribe interface.

## System Architecture
The system consists of three primary components:
- **TM4C123 LaunchPad** running the clock logic, display driver, and UART interface
- **ESP8266 (ESP-01)** acting as a Wi-Fi bridge between the TM4C and the MQTT broker
- **Web Application** that publishes commands and subscribes to clock state updates

Data flows as follows:
TM4C ↔ UART ↔ ESP8266 ↔ MQTT Broker ↔ Web Application

## Features
- Real-time display of hour, minute, second, and 12/24-hour mode
- Remote control via MQTT (hour/minute/second increment and decrement)
- Bi-directional MQTT communication using structured topics
- ST7735 color LCD output
- Bidirectional UART between TM4C and ESP8266 Wi-Fi module
- Mode button on web interface to change between 12/24 hour time on the ST7735 LCD
- Color button on web interface to toggle between different colors for the clock on the ST7735 LCD
- Toggle AM/PM button on web interface to toggle between AM and PM for the clock on the ST7735 LCD


## MQTT Topics
All MQTT topics are prefixed with the student EID.

### Publish (Board → Web)
- `<EID>/b2w/mode`
- `<EID>/b2w/hour`
- `<EID>/b2w/min`
- `<EID>/b2w/sec`

### Subscribe (Web → Board)
- `<EID>/w2b`

## Hardware Used
- TM4C123GXL LaunchPad
- ESP8266-ESP01 Wi-Fi module
- LM2937 3.3V voltage regulator
- ST7735R TFT LCD
- External switches and passive components

## Software Components
- TM4C firmware written in C
- ESP8266 firmware written using Arduino framework
- Web interface using HTML, JavaScript, and MQTT over WebSockets

## Setup Notes
- ESP8266 requires a dedicated 3.3V regulator
- All grounds must be connected
- UART5 is used for TM4C–ESP bidirectional communication
- MQTT broker in this case is Dr. McDermott's own local broker

## Deliverables
- KiCad schematic of external hardware
- System call graph
- Debug dumps and jitter measurements
- Photos demonstrating synchronized operation
- Power and current characterization

## Team
Team size: 2

## Course
Embedded Systems Laboratory (ECE 445L)
