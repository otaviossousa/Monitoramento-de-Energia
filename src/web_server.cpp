#include "web_server.h"
#include "web_ui.h"
#include <ESP8266WiFi.h>
#include <LittleFS.h>

// ==========================================
// CONSTRUCTOR / CONSTRUTOR
// ==========================================

WebServer::WebServer(EnergySensor *sensor, DataStorage *storage)
    : server(HTTP_SERVER_PORT), sensorRef(sensor), storageRef(storage) {}

// ==========================================
// PUBLIC METHODS / MÉTODOS PÚBLICOS
// ==========================================

void WebServer::init()
{
  registerRoutes();
  startServer();
}

void WebServer::handleRequests()
{
  /**
   * Motivo: handleClient() processa uma requisição por loop.
   * Chamado frequentemente no loop principal para responsividade.
   */
  server.handleClient();
}

void WebServer::updateMDNS()
{
  /**
   * Motivo: MDNS.update() reenvia anúncios periódicos do serviço.
   * Necessário para manter dispositivo descobrível na rede.
   */
  MDNS.update();
}

// ==========================================
// PRIVATE METHODS / MÉTODOS PRIVADOS
// ==========================================

void WebServer::registerRoutes()
{
  /**
   * Motivo: Lambdas com captura [this] permitem acessar membros privados
   * enquanto delegam para métodos privados, mantendo encapsulamento.
   */
  server.on("/", [this]()
            { handleRoot(); });
  server.on("/api", [this]()
            { handleApi(); });
  server.on("/net", [this]()
            { handleNetwork(); });
  server.on("/api/history", [this]()
            { handleHistory(); });
  server.on("/api/stats", [this]()
            { handleStats(); });
  server.on("/api/csv", [this]()
            { handleCSVDownload(); });
  server.on("/api/clear", HTTP_POST, [this]()
            { handleClearHistory(); });
}

void WebServer::startServer()
{
  /**
   * Motivo: mDNS permite acesso via nome de host (monitoramentodeenergia.local)
   * em vez de IP, melhorando experiência do usuário em redes locais.
   */
  MDNS.begin(MDNS_SERVICE_NAME);
  server.begin();
}

void WebServer::handleRoot()
{
  /**
   * Motivo: send_P() usa PROGMEM para economizar RAM,
   * crítico em microcontroladores com memória limitada.
   */
  server.send_P(200, "text/html", INDEX_HTML);
}

void WebServer::handleApi()
{
  /**
   * Motivo: JSON construído manualmente é mais eficiente que bibliotecas
   * em ambientes com restrições de memória. Formatação com casas decimais
   * reduz tamanho de cada resposta HTTP.
   *
   * Campos retornados:
   * - i: corrente (A)
   * - p: potência (W)
   * - e: energia (Wh)
   * - s: status de estabilização (0=calibrando, 1=estabilizado)
   * - r: tempo restante de calibração (segundos, 0 se completo)
   */
  String json = "{";
  json += "\"i\":" + String(sensorRef->getCurrent(), 3) + ",";
  json += "\"p\":" + String(sensorRef->getPower(), 1) + ",";
  json += "\"e\":" + String(sensorRef->getEnergy(), 3) + ",";
  json += "\"s\":" + String(sensorRef->isSystemStabilized() ? 1 : 0) + ",";
  json += "\"r\":" + String(sensorRef->getStabilizationRemainingTime());
  json += "}";

  server.send(200, "application/json", json);
}

void WebServer::handleNetwork()
{
  /**
   * Motivo: Separar informações de rede em endpoint dedicado permite
   * que cliente atualize status WiFi independentemente de dados de energia.
   */
  String json = "{\"ssid\":\"" + WiFi.SSID() +
                "\",\"ip\":\"" + WiFi.localIP().toString() + "\"}";
  server.send(200, "application/json", json);
}

void WebServer::handleHistory()
{
  /**
   * Retorna histórico completo do buffer em formato JSON
   * Útil para inicializar gráficos históricos na interface web
   */
  server.send(200, "application/json", storageRef->getHistoryJSON());
}

void WebServer::handleStats()
{
  /**
   * Retorna estatísticas calculadas: pico e média (potência e corrente)
   * Útil para cards de resumo no painel
   */
  server.send(200, "application/json", storageRef->getStatisticsJSON());
}

void WebServer::handleCSVDownload()
{
  /**
   * Força geração de arquivo CSV e envia como download
   */

  // Lê arquivo e envia como anexo
  File file = LittleFS.open(STORAGE_CSV_FILENAME, "r");

  if (!file)
  {
    server.send(404, "text/plain", "Arquivo nao encontrado");
    return;
  }

  server.sendHeader("Content-Disposition", "attachment; filename=data_energy.csv");
  server.streamFile(file, "text/csv");
  file.close();
}

void WebServer::handleClearHistory()
{
  /**
   * Limpa todo o histórico de dados
   * Endpoint protegido por POST para evitar limpeza acidental
   */
  storageRef->clearHistory();
  sensorRef->init(); // Reinicia o sensor para zerar a energia acumulada
  server.send(200, "application/json", "{\"status\":\"cleared\"}");
}
