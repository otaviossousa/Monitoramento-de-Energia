#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <ESP8266WiFi.h>
#include "config.h"

/**
 * Gerencia conexão WiFi do ESP8266.
 * Responsabilidade única: Conectar e manter conexão com rede WiFi.
 */
class WiFiManager
{
public:
  /**
   * Conecta ao WiFi usando credenciais da configuração.
   * Motivo: Centralizar lógica de conexão WiFi.
   */
  static void connect()
  {
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    waitForConnection();
  }

  /**
   * Verifica se está conectado ao WiFi.
   */
  static bool isConnected()
  {
    return WiFi.status() == WL_CONNECTED;
  }

  /**
   * Retorna o SSID atual.
   */
  static String getSSID()
  {
    return WiFi.SSID();
  }

  /**
   * Retorna o IP local.
   */
  static String getLocalIP()
  {
    return WiFi.localIP().toString();
  }

private:
  /**
   * Aguarda conexão com WiFi.
   * Motivo: Não bloquear boot se WiFi não estiver disponível
   *         (timeout após 20 tentativas).
   */
  static void waitForConnection()
  {
    int attempts = 0;
    const int MAX_ATTEMPTS = 20;

    while (WiFi.status() != WL_CONNECTED && attempts < MAX_ATTEMPTS)
    {
      delay(500);
      attempts++;
    }
  }
};

#endif // WIFI_MANAGER_H
