#include "web_server.h"
#include "web_ui.h"
#include <ESP8266WiFi.h>

// ==========================================
// CONSTRUCTOR / CONSTRUTOR
// ==========================================

WebServer::WebServer(EnergySensor *sensor)
    : server(HTTP_SERVER_PORT), sensorRef(sensor) {}

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
}

void WebServer::startServer()
{
  /**
   * Motivo: mDNS permite acesso via nome de host (dashboardpvp.local)
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
   */
  String json = "{";
  json += "\"i\":" + String(sensorRef->getCurrent(), 3) + ",";
  json += "\"p\":" + String(sensorRef->getPower(), 1) + ",";
  json += "\"e\":" + String(sensorRef->getEnergy(), 3);
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
