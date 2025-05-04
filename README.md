# RTD_Pt100-Pt100-
A temperature sensor module using ads high resolution amplifier 
# 📘 RTD Input Module implementation 

Ongoing ...

## 🧪 Overview
This project implements a 4-wire RTD signal conditioning and acquisition system using an **ESP32** and the **ADS1220** precision ADC. The design supports both **Pt100** and **Pt1000** RTDs, with analog output provided for SCADA interfacing and serial output for debugging or monitoring.

---

## 🔧 Features
- ✅ Supports 4-wire RTD configuration (Pt100 & Pt1000)
- 🎯 High-accuracy temperature measurement using ADS1220 (24-bit)
- ⚡ Vref generation using resistor network: `Vref = 1.65V`
- 📈 Analog output signal compatible with SCADA systems
- 🖥️ Serial monitor output for debugging ,verification and HMI output
- 🔧 Easy RTD type configuration in code

---

## 🧩 Hardware Components
- ESP32 Node mcu-32s
- ADS1220 ADC
- RTD Sensor (Pt100 or Pt1000)
- TLV2372 or equivalent Op-Amps
- Precision resistors:
  - R1 = 1.62kΩ
  - R2 = R3 = 3.9kΩ
- Filter capacitors (e.g., 0.1µF, 47µF)
- 3.3V Regulated Power Supply

---

## 🧮 Vref Calculation

The reference voltage for the op-amp stages is generated using the following resistor network:


## 🔧 To Do
- [ ] Add OTA update capability
- [ ] Support for more RTD types (e.g., Ni120)
- [ ] Include temperature calibration offset
