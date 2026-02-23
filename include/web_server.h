#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include "config.h"
#include "web_ui.h"
#include "energy_sensor.h"

/**
 * Gerencia o servidor web e comunicação HTTP.
 * Responsabilidade única: Disponibilizar APIs REST e servir interface web.
 */
class WebServer
{
private:
  ESP8266WebServer server;
  EnergySensor *sensorRef;

public:
  WebServer(EnergySensor *sensor)
      : server(HTTP_SERVER_PORT), sensorRef(sensor) {}

  /**
   * Inicializa o servidor web e registra rotas.
   */
  void init()
  {
    registerRoutes();
    startServer();
  }

  /**
   * Processa requisições HTTP pendentes.
   * Motivo: Manter loop principal limpo.
   */
  void handleRequests()
  {
    server.handleClient();
  }

  /**
   * Mantém mDNS ativo e anunciando o serviço.
   * Motivo: Permitir acesso por nome (dashboardpvp.local).
   */
  void updateMDNS()
  {
    MDNS.update();
  }

private:
  /**
   * Registra todas as rotas HTTP.
   * Motivo: Centralizar definição de rotas.
   */
  void registerRoutes()
  {
    server.on("/", [this]()
              { handleRoot(); });
    server.on("/api", [this]()
              { handleApi(); });
    server.on("/net", [this]()
              { handleNetwork(); });
  }

  /**
   * Inicia o servidor web e mDNS.
   * Motivo: Separar lógica de inicialização.
   */
  void startServer()
  {
    MDNS.begin(MDNS_SERVICE_NAME);
    server.begin();
  }

  /**
   * Rota GET / - Retorna a interface web.
   */
  void handleRoot()
  {
    server.send_P(200, "text/html", INDEX_HTML);
  }

  /**
   * Rota GET /api - Retorna dados de energia em JSON.
   * Formato: {"i": <current>, "p": <power>, "e": <energy>}
   */
  void handleApi()
  {
    String json = "{";
    json += "\"i\":" + String(sensorRef->getCurrent(), 3) + ",";
    json += "\"p\":" + String(sensorRef->getPower(), 1) + ",";
    json += "\"e\":" + String(sensorRef->getEnergy(), 3);
    json += "}";

    server.send(200, "application/json", json);
  }

  /**
   * Rota GET /net - Retorna informações de rede WiFi.
   * Formato: {"ssid": <network_name>, "ip": <device_ip>}
   */
  void handleNetwork()
  {
    String json = "{\"ssid\":\"" + WiFi.SSID() +
                  "\",\"ip\":\"" + WiFi.localIP().toString() + "\"}";
    server.send(200, "application/json", json);
  }
};

#endif // WEB_SERVER_H
