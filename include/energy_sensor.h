#ifndef ENERGY_SENSOR_H
#define ENERGY_SENSOR_H

#include <EmonLib.h>
#include "config.h"

/**
 * Gerencia a leitura e cálculo de energia do sensor SCT013.
 * Responsabilidade única: Interagir com o sensor e fornecer dados calibrados.
 */
class EnergySensor
{
private:
  EnergyMonitor monitor;
  float current;
  float power;
  float energy;

public:
  EnergySensor() : current(0), power(0), energy(0) {}

  /**
   * Inicializa o sensor na porta especificada.
   */
  void init()
  {
    monitor.current(SENSOR_PIN, SENSOR_CALIBRATION);
  }

  /**
   * Realiza leitura do sensor e atualiza os valores internos.
   * Filtra leituras abaixo do threshold para evitar ruído.
   */
  void sample()
  {
    current = monitor.calcIrms(SENSOR_SAMPLES);

    // Filtra ruído elétrico em baixas correntes
    if (current < SENSOR_CURRENT_THRESHOLD)
    {
      current = 0;
    }

    calculatePower();
    accumulateEnergy();
  }

  // ========== Getters ==========
  float getCurrent() const { return current; }
  float getPower() const { return power; }
  float getEnergy() const { return energy; }

private:
  /**
   * Calcula potência instantânea (P = V × I).
   * Motivo: Centralizar lógica de cálculo de potência.
   */
  void calculatePower()
  {
    power = current * GRID_VOLTAGE;
  }

  /**
   * Integra potência para obter energia acumulada (Wh).
   * Motivo: Discretizar energia em intervalos uniformes.
   */
  void accumulateEnergy()
  {
    // Converte potência (W) × intervalo (ms) para energia (Wh)
    energy += power * (SAMPLE_INTERVAL_MS / ENERGY_CONVERSION_FACTOR);
  }
};

#endif // ENERGY_SENSOR_H
