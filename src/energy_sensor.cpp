#include "energy_sensor.h"

// ==========================================
// CONSTRUCTOR / CONSTRUTOR
// ==========================================

EnergySensor::EnergySensor() : current(0), power(0), energy(0) {}

// ==========================================
// PUBLIC METHODS / MÉTODOS PÚBLICOS
// ==========================================

void EnergySensor::init()
{
  /**
   * Motivo: Valor 84.8 foi calibrado empiricamente para maximizar precisão
   * da leitura de corrente do sensor SCT013.
   */
  monitor.current(SENSOR_PIN, SENSOR_CALIBRATION);
}

void EnergySensor::sample()
{
  // Lê a corrente RMS do sensor
  current = monitor.calcIrms(SENSOR_SAMPLES);

  // Filtra ruído elétrico em baixas correntes
  if (current < SENSOR_CURRENT_THRESHOLD)
  {
    current = 0;
  }

  calculatePower();
  accumulateEnergy();
}

// ==========================================
// PRIVATE METHODS / MÉTODOS PRIVADOS
// ==========================================

void EnergySensor::calculatePower()
{
  /**
   * Motivo: Potência instantânea em sistemas trifásicos é calculada por P = V × I.
   * Centralizar evita cálculos duplicados e facilita testes.
   */
  power = current * GRID_VOLTAGE;
}

void EnergySensor::accumulateEnergy()
{
  /**
   * Motivo: Integração numérica de potência em intervalos uniformes.
   * Converte ms para horas: (W × ms) / 3600000 = Wh
   */
  energy += power * (SAMPLE_INTERVAL_MS / ENERGY_CONVERSION_FACTOR);
}
