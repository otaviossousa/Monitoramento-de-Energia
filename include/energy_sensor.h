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
  unsigned long startupTime;      // Marca o tempo de inicialização
  bool isStabilized;              // Indica se passou do período de estabilização

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
   * Nota: Dados não são acumulados durante o período de estabilização.
   */
  void sample();

  /**
   * Verifica se o sistema passou do período de estabilização.
   * Retorna true quando STABILIZATION_TIME_MS milissegundos passaram desde o init().
   */
  bool isSystemStabilized() const { return isStabilized; }

  /**
   * Retorna o tempo (em segundos) até o sistema estar totalmente estabilizado.
   * Retorna 0 se já está estabilizado.
   */
  uint16_t getStabilizationRemainingTime() const;

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
