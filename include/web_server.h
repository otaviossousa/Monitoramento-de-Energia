#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include "config.h"
#include "energy_sensor.h"
#include "data_storage.h"

/**
 * Gerencia o servidor web e comunicação HTTP.
 * Responsabilidade única: Disponibilizar APIs REST e servir interface web.
 */
class WebServer
{
private:
  ESP8266WebServer server;
  EnergySensor *sensorRef;
  DataStorage *storageRef;

  /**
   * Registra todas as rotas HTTP.
   * Motivo: Centralizar definição de rotas.
   */
  void registerRoutes();

  /**
   * Inicia o servidor web e mDNS.
   * Motivo: Separar lógica de inicialização.
   */
  void startServer();

  /**
   * Rota GET / - Retorna a interface web.
   */
  void handleRoot();

  /**
   * Rota GET /api - Retorna dados de energia em JSON.
   * Formato: {"i": <current>, "p": <power>, "e": <energy>}
   */
  void handleApi();

  /**
   * Rota GET /net - Retorna informações de rede WiFi.
   * Formato: {"ssid": <network_name>, "ip": <device_ip>}
   */
  void handleNetwork();

  /**
   * Rota GET /api/history - Retorna histórico de dados.
   * Formato: {"history":[{ts,i,p,e},...]}
   */
  void handleHistory();

  /**
   * Rota GET /api/stats - Retorna estatísticas calculadas.
   * Formato: {"peak":<pico>, "avg":<média>, "total":<total>, "last":<último>}
   */
  void handleStats();

  /**
   * Rota GET /api/csv - Faz download do arquivo CSV com histórico completo.
   */
  void handleCSVDownload();

  /**
   * Rota POST /api/clear - Limpa histórico de dados.
   */
  void handleClearHistory();

public:
  /**
   * Construtor - inicializa servidor na porta HTTP_SERVER_PORT.
   * @param sensor Referência ao objeto EnergySensor para leitura de dados.
   * @param storage Referência ao objeto DataStorage para histórico de dados.
   */
  WebServer(EnergySensor *sensor, DataStorage *storage);

  /**
   * Inicializa o servidor web e registra rotas HTTP.
   */
  void init();

  /**
   * Processa requisições HTTP pendentes.
   * Motivo: Manter loop principal limpo e responsivo.
   */
  void handleRequests();

  /**
   * Mantém mDNS ativo e anunciando o serviço.
   * Motivo: Permitir acesso por nome de host (dashboardpvp.local).
   */
  void updateMDNS();
};

#endif // WEB_SERVER_H
