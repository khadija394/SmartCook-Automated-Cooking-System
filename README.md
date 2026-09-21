# SmartCook — Automated Cooking & Safety System

An Arduino-based smart cooking prototype designed to assist users with recipe guidance, real-time ingredient measurement, and cooking safety monitoring.

<p align="center">
  <img src="https://img.shields.io/badge/Arduino-Mega%202560-00979D?logo=arduino&logoColor=white" alt="Arduino Mega 2560">
  <img src="https://img.shields.io/badge/Language-C%2B%2B-blue?logo=cplusplus&logoColor=white" alt="C++">
  <img src="https://img.shields.io/badge/Simulation-Wokwi-orange" alt="Wokwi">
  <img src="https://img.shields.io/badge/Project-BSIT-purple" alt="BSIT Project">
</p>

<p align="center">
  <img src="https://raw.githubusercontent.com/khadija394/SmartCook-Automated-Cooking-System/refs/heads/main/src/simulation/screenshots/circuit.png" alt="SmartCook Complete Circuit" width="800">
</p>

## Project Overview

SmartCook is an embedded-system prototype that combines recipe management, ingredient weight monitoring, cooking guidance, user notifications, and thermal safety protection.

The system was developed as a BSIT semester project and tested using the Wokwi virtual prototyping platform.

## Key Features

* Recipe management
* Step-by-step cooking guidance
* Real-time ingredient weight monitoring
* HX711 load-cell integration
* LCD-based user feedback
* Buzzer notifications
* Temperature monitoring
* Automatic overheat protection
* Relay-based heater shutdown
* Wokwi hardware simulation

## How It Works

1. The user selects a recipe.
2. The system loads the required ingredient quantity.
3. The HX711 load-cell system monitors the ingredient weight.
4. The current weight is compared with the target quantity.
5. The system p
