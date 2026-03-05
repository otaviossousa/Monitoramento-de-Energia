#include "data_storage.h"
#include <LittleFS.h>
#include <time.h>

// ==========================================
// CONSTRUCTOR / CONSTRUTOR
// ==========================================

DataStorage::DataStorage()
    : bufferIndex(0), bufferCount(0), aggregateCurrentSum(0), aggregatePowerSum(0),
      aggregateEnergyLast(0), aggregateSampleCount(0), lastSaveTime(0),
      dataPointCounter(0), lastFileCheckTime(0),
      globalPeakCurrent(0.0), globalPeakPower(0.0)
{
  // Inicializa buffer com zeros
  for (int i = 0; i < STORAGE_BUFFER_SIZE; i++)
  {
    buffer[i] = {0, 0, 0, 0};
  }
}

// ==========================================
// PUBLIC METHODS / MÉTODOS PÚBLICOS
// ==========================================

void DataStorage::init()
{
  initFileSystem();

  // Configura o fuso horário para o Horário de Brasília (BRT, UTC-3)
  // e sincroniza o tempo com servidores NTP.
  // Isso é crucial para que a função localtime() funcione corretamente.
  configTime(-3 * 3600, 0, "pool.ntp.org", "a.st1.ntp.br");

  // Arquivo CSV será criado na primeira gravação
  lastSaveTime = millis();
  lastFileCheckTime = millis();
}

void DataStorage::addDataPoint(float current, float power, float energy, bool forceWrite)
{
  /**
   * Motivo: Agregar 60 amostras (1 minuto) antes de gravar no CSV
   * Primeiro acumula dados, depois calcula médias quando completar 1 minuto
   * Assim o histórico fica compacto (1440 pontos/dia) e valores são médias suaves
   */

  // Acumula dados do período de 1 minuto
  aggregateCurrentSum += current;
  aggregatePowerSum += power;
  aggregateEnergyLast = energy; // Último valor de energia (não é média, é acumulado)
  aggregateSampleCount++;

  // Incrementa contador de pontos para controle de gravação
  dataPointCounter++;

  // ATUALIZAÇÃO DE PICO EM TEMPO REAL (Resolução de 1s)
  // Verifica picos antes da agregação para não perder transientes
  if (current > globalPeakCurrent)
    globalPeakCurrent = current;
  if (power > globalPeakPower)
    globalPeakPower = power;

  // Flush quando atingir 60 amostras (1 minuto com intervalo de 1s)
  // OU quando forçado (ex: antes de desligar)
  if (aggregateSampleCount >= 60 || forceWrite)
  {
    flushAggregateData();
  }
}

DataPoint DataStorage::getDataPoint(uint16_t index) const
{
  if (index >= bufferCount)
  {
    return {0, 0, 0, 0};
  }

  // Índice 0 é o mais recente, então inverte a busca no buffer circular
  int actualIndex = (bufferIndex - 1 - index + STORAGE_BUFFER_SIZE) % STORAGE_BUFFER_SIZE;
  return buffer[actualIndex];
}

String DataStorage::getHistoryJSON() const
{
  /**
   * Alteração: Retorna apenas os últimos 10 pontos (10 minutos)
   * Cria uma janela deslizante do tempo real no gráfico.
   * ts=timestamp, i=current, p=power, e=energy
   */
  const uint16_t POINTS_TO_SHOW = 10;

  // Define quantos pontos pegar: o que tiver no buffer ou o limite de 10
  uint16_t limit = (bufferCount < POINTS_TO_SHOW) ? bufferCount : POINTS_TO_SHOW;

  String json = "{\"history\":[";

  // Itera apenas sobre os 'limit' pontos mais recentes (índice 0 é o mais novo)
  for (uint16_t i = 0; i < limit; i++)
  {
    if (i > 0)
      json += ",";

    DataPoint point = getDataPoint(i);
    json += "{\"ts\":" + String(point.timestamp);
    json += ",\"i\":" + String(point.current, 3);
    json += ",\"p\":" + String(point.power, 1);
    json += ",\"e\":" + String(point.energy, 2) + "}";
  }

  json += "]}";
  return json;
}

String DataStorage::getStatisticsJSON() const
{
  /**
   * Otimização: Calcula estatísticas em passagem única pelo buffer
   * Evita múltiplas iterações e chamadas getDataPoint() redundantes
   */
  if (bufferCount == 0)
  {
    return "{\"peak\":0,\"peak_current\":0,\"avg\":0,\"avg_current\":0}";
  }

  float sumPower = 0;
  float sumCurrent = 0;

  // Passagem única: calcula tudo de uma vez
  for (uint16_t i = 0; i < bufferCount; i++)
  {
    const DataPoint &point = getDataPoint(i);

    // Acumula para médias
    sumPower += point.power;
    sumCurrent += point.current;
  }

  float avgPower = sumPower / bufferCount;
  float avgCurrent = sumCurrent / bufferCount;

  String json = "{";
  // Usa os picos globais capturados em tempo real, não a média do buffer
  json += "\"peak\":" + String(globalPeakPower, 1);
  json += ",\"peak_current\":" + String(globalPeakCurrent, 3);
  json += ",\"avg\":" + String(avgPower, 1);
  json += ",\"avg_current\":" + String(avgCurrent, 3);
  json += "}";

  return json;
}

void DataStorage::clearHistory()
{
  bufferIndex = 0;
  bufferCount = 0;
  dataPointCounter = 0;

  // Reseta os picos globais
  globalPeakCurrent = 0;
  globalPeakPower = 0;

  // Remove arquivo CSV
  LittleFS.remove(STORAGE_CSV_FILENAME);
  LittleFS.remove(STORAGE_CSV_BACKUP_FILENAME);
}

long DataStorage::getCSVFileSize() const
{
  File file = LittleFS.open(STORAGE_CSV_FILENAME, "r");
  if (!file)
  {
    return 0;
  }

  long size = file.size();
  file.close();
  return size;
}

// ==========================================
// PRIVATE METHODS / MÉTODOS PRIVADOS
// ==========================================

void DataStorage::advanceBufferIndex()
{
  bufferIndex = (bufferIndex + 1) % STORAGE_BUFFER_SIZE;
}

void DataStorage::flushAggregateData()
{
  /**
   * Processa os dados agregados de 1 minuto
   * Calcula médias e adiciona um ponto ao buffer principal
   * Depois persiste no CSV
   */
  if (aggregateSampleCount == 0)
    return;

  // Calcula médias
  float avgCurrent = aggregateCurrentSum / aggregateSampleCount;
  float avgPower = aggregatePowerSum / aggregateSampleCount;

  // Cria novo ponto de dados com MÉDIA de 1 minuto
  DataPoint point;
  point.timestamp = time(nullptr); // Timestamp do momento do flush
  point.current = avgCurrent;
  point.power = avgPower;
  point.energy = aggregateEnergyLast; // Energia acumulada total

  // Adiciona ao buffer circular
  buffer[bufferIndex] = point;
  bufferCount = min((uint16_t)(bufferCount + 1), (uint16_t)STORAGE_BUFFER_SIZE);
  advanceBufferIndex();

  // Salva em arquivo usando modo APPEND (Adição)
  // Isso é muito mais rápido do que reescrever o arquivo inteiro com saveToCSV()
  File file = LittleFS.open(STORAGE_CSV_FILENAME, "a");
  if (file)
  {
    // Se arquivo novo/vazio, escreve cabeçalho
    if (file.size() == 0) {
      file.println("data_hora,corrente,potencia,energia");
    }
    file.println(formatCSVLine(point));
    file.close();
  }

  unsigned long now = millis();
  lastSaveTime = now;

  // Verifica se arquivo precisa rotacionar
  if (now - lastFileCheckTime >= FILE_SIZE_CHECK_INTERVAL)
  {
    rotateCSVIfNeeded();
    lastFileCheckTime = now;
  }

  // Reset do buffer de agregação
  aggregateCurrentSum = 0;
  aggregatePowerSum = 0;
  aggregateSampleCount = 0;
}

void DataStorage::rotateCSVIfNeeded()
{
  long fileSize = getCSVFileSize();

  if (fileSize > (long)MAX_CSV_SIZE)
  {
    // Remove backup antigo
    LittleFS.remove(STORAGE_CSV_BACKUP_FILENAME);

    // Move arquivo atual para backup
    LittleFS.rename(STORAGE_CSV_FILENAME, STORAGE_CSV_BACKUP_FILENAME);

    // Novo arquivo será criado na próxima escrita
  }
}

String DataStorage::formatCSVLine(const DataPoint &point)
{
  /**
   * Formata linha CSV com valores separados por vírgula.
   * Formato: data_hora (ISO 8601), corrente, potencia, energia.
   * O formato ISO 8601 (YYYY-MM-DDTHH:MM:SS) é facilmente reconhecido por Excel.
   */
  char timeStr[20]; // Buffer para "YYYY-MM-DDTHH:MM:SS" + null
  time_t rawtime = point.timestamp;
  struct tm *timeinfo;
  timeinfo = localtime(&rawtime);
  strftime(timeStr, sizeof(timeStr), "%Y-%m-%dT%H:%M:%S", timeinfo);

  String line = String(timeStr) + ",";
  line += String(point.current, 3) + ",";
  line += String(point.power, 1) + ",";
  line += String(point.energy, 3);

  return line;
}

void DataStorage::initFileSystem()
{
  /**
   * Motivo: LittleFS oferece melhor performance e gerenciamento de espaço
   * comparado a SPIFFS em dispositivos ESP8266.
   * Tenta montar, se falhar, formata e monta novamente.
   */
  if (!LittleFS.begin())
  {
    // Format LittleFS no primeiro uso
    LittleFS.format();
    LittleFS.begin();
  }
}
