#ifndef DATA_STORAGE_H
#define DATA_STORAGE_H

#include <Arduino.h>
#include "config.h"

/**
 * Estrutura para representar um ponto de dados histórico.
 * Contém: timestamp, corrente, potência e energia acumulada.
 */
struct DataPoint
{
  unsigned long timestamp; // Unix timestamp (segundos)
  float current;           // Amperes
  float power;             // Watts
  float energy;            // Watt-horas acumuladas
};

/**
 * Gerencia armazenamento e persistência de dados de energia.
 * Estratégia:
 * 1. Buffer circular em RAM (1440 pontos = 24h com intervalo de 1min)
 * 2. Arquivo CSV em LittleFS para histórico persistente
 * 3. Salva novos dados a cada intervalo configurável
 *
 * Responsabilidade: Centralizar lógica de armazenamento de dados
 */
class DataStorage
{
private:
  // Buffer circular em RAM
  DataPoint buffer[STORAGE_BUFFER_SIZE];
  uint16_t bufferIndex;
  uint16_t bufferCount;

  // Buffer intermediário para agregar dados de 1 minuto (60 amostras de 1 segundo)
  float aggregateCurrentSum;
  float aggregatePowerSum;
  float aggregateEnergyLast;
  uint16_t aggregateSampleCount;

  // Controle de gravação em arquivo
  unsigned long lastSaveTime;
  unsigned long dataPointCounter;

  // Controle de tamanho de arquivo
  unsigned long lastFileCheckTime;
  const unsigned long FILE_SIZE_CHECK_INTERVAL = 300000; // 5 minutos
  const unsigned long MAX_CSV_SIZE = 150000;             // 150KB máximo

  // Variáveis para rastrear picos absolutos (resolução de 1s)
  float globalPeakCurrent;
  float globalPeakPower;

  /**
   * Implementa buffer circular: move para próximo índice
   */
  void advanceBufferIndex();

  /**
   * Processa agregação de dados: calcula médias e salva no buffer principal
   * Chamado quando atingir 60 amostras (1 minuto com intervalo de 1s)
   */
  void flushAggregateData();

  /**
   * Rotaciona arquivo CSV quando excede tamanho máximo
   * Mantém últimos 2 arquivos (atual + backup)
   */
  void rotateCSVIfNeeded();

  /**
   * Formata linha CSV com os dados do ponto
   */
  String formatCSVLine(const DataPoint &point);

  /**
   * Inicializa o sistema de arquivos LittleFS
   */
  void initFileSystem();

public:
  /**
   * Construtor - inicializa buffer e controles
   */
  DataStorage();

  /**
   * Inicializa sistema de armazenamento
   * Checa integridade do arquivo CSV e inicia buffer
   */
  void init();

  /**
   * Adiciona nova amostra de dados (chamado a cada 1 segundo).
   * Acumula em buffer intermediário por 1 minuto (60 amostras).
   * Quando atingir 1 min, calcula médias e persiste no CSV.
   *
   * @param current Corrente em amperes
   * @param power Potência em watts
   * @param energy Energia acumulada em Wh
   * @param forceWrite Force gravação em arquivo imediatamente
   */
  void addDataPoint(float current, float power, float energy, bool forceWrite = false);

  /**
   * Retorna um ponto específico do buffer
   * @param index Índice (0 = mais recente, STORAGE_BUFFER_SIZE-1 = mais antigo)
   */
  DataPoint getDataPoint(uint16_t index) const;

  /**
   * Retorna quantidade de pontos válidos no buffer
   */
  uint16_t getBufferCount() const { return bufferCount; }

  /**
   * Gera JSON com histórico completo do buffer
   * Útil para atualizar gráficos na interface web
   */
  String getHistoryJSON() const;

  /**
   * Gera arquivo CSV completo para download
   * Formato: timestamp,current,power,energy
   */
  void serveCSV();

  /**
   * Salva buffer em arquivo CSV
   * Usado periodicamente para persistência
   */
  void saveToCSV();

  /**
   * Apaga histórico (limpa arquivo e buffer)
   */
  void clearHistory();

  /**
   * Retorna tamanho do arquivo CSV em bytes
   */
  long getCSVFileSize() const;

  /**
   * Retorna estatísticas para exibição no painel
   * @return JSON com consumo, pico, média, etc
   */
  String getStatisticsJSON() const;
};

#endif // DATA_STORAGE_H
