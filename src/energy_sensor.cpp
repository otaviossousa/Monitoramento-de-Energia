#include "energy_sensor.h"

// ==========================================
// CONSTRUCTOR / CONSTRUTOR
// ==========================================

EnergySensor::EnergySensor() : current(0), power(0), energy(0), startupTime(0), isStabilized(false) {}

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

  // Inicia o período de estabilização
  startupTime = millis();
  isStabilized = false;
  energy = 0;
}

void EnergySensor::sample()
{
  // Atualiza o status de estabilização
  unsigned long elapsedTime = millis() - startupTime;
  if (!isStabilized && elapsedTime >= STABILIZATION_TIME_MS)
  {
    isStabilized = true;
  }

  // Lê a corrente RMS do sensor
  // Usamos uma variável temporária para não afetar a leitura pública se não estiver estabilizado
  float rawCurrent = monitor.calcIrms(SENSOR_SAMPLES);

  // Se o sistema ainda está calibrando/estabilizando, ignoramos a leitura
  // Isso evita o pico inicial (ex: 79A) causado pela carga do capacitor/filtros
  if (!isStabilized)
  {
    current = 0;
    power = 0;
    return;
  }

  current = rawCurrent;

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

uint16_t EnergySensor::getStabilizationRemainingTime() const
{
  /**
   * Retorna quanto tempo falta para o sistema estabilizar.
   * Se já estabilizado ou em fase inicial, retorna 0.
   */
  if (isStabilized || startupTime == 0)
  {
    return 0;
  }

  unsigned long elapsedTime = millis() - startupTime;
  if (elapsedTime >= STABILIZATION_TIME_MS)
  {
    return 0;
  }

  // Converte para segundos (arredonda para cima)
  uint16_t remainingMs = STABILIZATION_TIME_MS - elapsedTime;
  return (remainingMs + 999) / 1000; // Arredonda para cima
}
