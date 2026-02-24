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

  /**
   * Calcula potência instantânea (P = V × I).
   * Motivo: Centralizar lógica de cálculo de potência.
   */
  void calculatePower();

  /**
   * Integra potência para obter energia acumulada (Wh).
   * Motivo: Discretizar energia em intervalos uniformes.
   */
  void accumulateEnergy();

public:
  /**
   * Construtor - inicializa valores de leitura em zero.
   */
  EnergySensor();

  /**
   * Inicializa o sensor na porta especificada.
   */
  void init();

  /**
   * Realiza leitura do sensor e atualiza os valores internos.
   * Filtra leituras abaixo do threshold para evitar ruído.
   */
  void sample();

  // ========== Getters ==========
  /**
   * Retorna a corrente RMS atual em amperes.
   */
  float getCurrent() const { return current; }

  /**
   * Retorna a potência instantânea em watts.
   */
  float getPower() const { return power; }

  /**
   * Retorna a energia acumulada em watt-horas.
   */
  float getEnergy() const { return energy; }
};

#endif // ENERGY_SENSOR_H
