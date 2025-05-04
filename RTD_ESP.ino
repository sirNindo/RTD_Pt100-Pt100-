#include <SPI.h>

#define CS_PIN     5     // Chip Select
#define DRDY_PIN   4     // Data Ready from ADS1220
#define DAC_PIN    25    // DAC1 output (GPIO25) to SCADA (analog voltage output)

// ADS1220 SPI Commands
#define CMD_RESET   0x06
#define CMD_START   0x08
#define CMD_RDATA   0x10
#define CMD_WREG    0x40

bool isPt1000 = false;  // Default is Pt100

void writeRegister(uint8_t reg, uint8_t value) {
  digitalWrite(CS_PIN, LOW);
  SPI.transfer(CMD_WREG | (reg & 0x03)); // Write 1 register
  SPI.transfer(0x00); // Number of registers to write - 1
  SPI.transfer(value);
  digitalWrite(CS_PIN, HIGH);
}

void sendCommand(uint8_t cmd) {
  digitalWrite(CS_PIN, LOW);
  SPI.transfer(cmd);
  digitalWrite(CS_PIN, HIGH);
}

void setupADS1220() {
  sendCommand(CMD_RESET);
  delay(100);

  // Config:
  // MUX = AIN0-AIN1, Gain = 1
  writeRegister(0x00, 0x08);
  // Data rate = 20SPS
  writeRegister(0x01, 0x04);
  // Internal reference, no burnout
  writeRegister(0x02, 0x40);
  writeRegister(0x03, 0x00); // Default settings

  sendCommand(CMD_START);
}

void setup() {
  Serial.begin(115200);
  pinMode(CS_PIN, OUTPUT);
  pinMode(DRDY_PIN, INPUT);
  digitalWrite(CS_PIN, HIGH);

  SPI.begin();
  delay(100);
  setupADS1220();

  // Initial auto-detection of RTD type
  long raw = readRawData();
  float resistance = convertToResistance(raw);
  isPt1000 = resistance > 200.0; // Threshold to differentiate Pt100/Pt1000
  Serial.print("RTD Type Detected: ");
  Serial.println(isPt1000 ? "Pt1000" : "Pt100");

  analogWriteResolution(8);  // Set resolution for DAC output
}

long readRawData() {
  while (digitalRead(DRDY_PIN) == HIGH); // Wait for DRDY = LOW

  digitalWrite(CS_PIN, LOW);
  SPI.transfer(CMD_RDATA);
  uint8_t b1 = SPI.transfer(0x00);
  uint8_t b2 = SPI.transfer(0x00);
  uint8_t b3 = SPI.transfer(0x00);
  digitalWrite(CS_PIN, HIGH);

  long raw = ((long)b1 << 16) | ((long)b2 << 8) | b3;
  if (raw & 0x800000) raw |= 0xFF000000; // Sign extend

  return raw;
}

float convertToResistance(long raw, float vref = 1.65, float gain = 1.0, float i_exc = 0.00025) {
  float voltage = (raw * vref) / (gain * 8388608.0); // 2^23
  return voltage / i_exc;
}

float resistanceToTemperature(float R) {
  float R0 = isPt1000 ? 1000.0 : 100.0;
  float alpha = 0.00385;

  return (R - R0) / (R0 * alpha);  // Approximate linear temperature value for 0–200°C
}

void outputToSCADA(float temperature) {
  // Scale temp to voltage: -200°C = 0V, 800°C = 3.3V pt100 operation range 
  float clampedTemp = constrain(temperature, 200.0, 800.0);
  float voltage = clampedTemp * (3.3 / 100.0);
  int dacValue = map(voltage * 1000, 0, 3300, 0, 255); // For 8-bit DAC
  Serial.print("Temperature:");
  Serial.print(dacValue)  ;

  dacWrite(DAC_PIN, dacValue);
}

void loop() {
  long raw = readRawData();
  float resistance = convertToResistance(raw);
  float temperature = resistanceToTemperature(resistance);

  // Print to Serial
  Serial.print("Raw ADC: "); Serial.print(raw);
  Serial.print(" | Resistance: "); Serial.print(resistance, 2); Serial.print(" Ω");
  Serial.print(" | Temp: "); Serial.print(temperature, 2); Serial.println(" °C");

  // Output to SCADA via DAC
  outputToSCADA(temperature);

  delay(1000);
}
