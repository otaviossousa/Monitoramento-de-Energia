#include "data_storage.h"
#include <LittleFS.h>
#include <time.h>

// ==========================================
// CONSTRUCTOR / CONSTRUTOR
// ==========================================

DataStorage::DataStorage()
    : bufferIndex(0), bufferCount(0), aggregateCurrentSum(0), aggregatePowerSum(0),
      aggregateEnergyLast(0), aggregateSampleCount(0), lastSaveTime(0),
      dataPointCounter(0), lastFileCheckTime(0)
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
   * Otimização: Retorna apenas últimos 288 pontos (~24h com granularidade de 5min)
   * Se buffer tem mais pontos, usa decimação para manter tamanho do JSON pequeno
   * Isso reduz payload de ~50KB para ~15KB, permitindo carregamento rápido do gráfico
   * ts=timestamp, i=current, p=power, e=energy
   */
  const uint16_t MAX_HISTORY_POINTS = 288;
  const uint16_t DECIMATION_FACTOR = (bufferCount > MAX_HISTORY_POINTS)
                                         ? (bufferCount / MAX_HISTORY_POINTS)
                                         : 1;

  String json = "{\"history\":[";
  uint16_t pointsAdded = 0;

  for (uint16_t i = 0; i < bufferCount; i += DECIMATION_FACTOR)
  {
    if (pointsAdded > 0)
      json += ",";

    DataPoint point = getDataPoint(i);
    json += "{\"ts\":" + String(point.timestamp);
    json += ",\"i\":" + String(point.current, 2);
    json += ",\"p\":" + String(point.power, 1);
    json += ",\"e\":" + String(point.energy, 2) + "}";
    pointsAdded++;

    if (pointsAdded >= MAX_HISTORY_POINTS)
      break;
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
    return "{\"peak\":0,\"peak_current\":0,\"avg\":0,\"avg_current\":0,\"total\":0,\"last\":0}";
  }

  float peakPower = 0;
  float peakCurrent = 0;
  float sumPower = 0;
  float sumCurrent = 0;
  float totalEnergy = 0;
  float lastPower = 0;

  // Passagem única: calcula tudo de uma vez
  for (uint16_t i = 0; i < bufferCount; i++)
  {
    const DataPoint &point = getDataPoint(i);

    // Atualiza picos
    if (point.power > peakPower)
      peakPower = point.power;
    if (point.current > peakCurrent)
      peakCurrent = point.current;

    // Acumula para médias
    sumPower += point.power;
    sumCurrent += point.current;

    // Captura último ponto (primeira iteração, i==0)
    if (i == 0)
    {
      lastPower = point.power;
      totalEnergy = point.energy;
    }
  }

  float avgPower = sumPower / bufferCount;
  float avgCurrent = sumCurrent / bufferCount;

  String json = "{";
  json += "\"peak\":" + String(peakPower, 1);
  json += ",\"peak_current\":" + String(peakCurrent, 2);
  json += ",\"avg\":" + String(avgPower, 1);
  json += ",\"avg_current\":" + String(avgCurrent, 2);
  json += ",\"total\":" + String(totalEnergy, 2);
  json += ",\"last\":" + String(lastPower, 1);
  json += "}";

  return json;
}

void DataStorage::saveToCSV()
{
  /**
   * Salva todos os pontos do buffer em arquivo CSV
   * Motivo: Persistência em LittleFS garante dados não perdidos em reset
   */
  File file = LittleFS.open(STORAGE_CSV_FILENAME, "w");

  if (!file)
  {
    // Falha ao abrir arquivo - espera próxima tentativa
    return;
  }

  // Escreve cabeçalho CSV
  file.println("timestamp,current,power,energy");

  // Escreve todos os pontos do buffer (ordem cronológica)
  for (uint16_t i = bufferCount; i > 0; i--)
  {
    DataPoint point = getDataPoint(i - 1);
    file.println(formatCSVLine(point));
  }

  file.close();
}

void DataStorage::clearHistory()
{
  bufferIndex = 0;
  bufferCount = 0;
  dataPointCounter = 0;

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

  // Salva em arquivo
  unsigned long now = millis();
  saveToCSV();
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
   * Formata linha CSV com valores separados por vírgula
   * Formato: timestamp,current,power,energy
   */
  String line = String(point.timestamp) + ",";
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
