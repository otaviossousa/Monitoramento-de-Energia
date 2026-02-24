#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <ESP8266WiFi.h>
#include "config.h"

/**
 * Gerencia conexão WiFi do ESP8266.
 * Responsabilidade única: Conectar e manter conexão com rede WiFi.
 * Implementação: Padrão Singleton com métodos estáticos.
 */
class WiFiManager
{
public:
  /**
   * Conecta ao WiFi usando credenciais da configuração.
   * Motivo: Centralizar lógica de conexão com timeout seguro.
   */
  static void connect();

  /**
   * Verifica se está conectado ao WiFi.
   */
  static bool isConnected();

  /**
   * Retorna o SSID da rede WiFi conectada.
   */
  static String getSSID();

  /**
   * Retorna o endereço IP local do dispositivo.
   */
  static String getLocalIP();

private:
  /**
   * Aguarda conexão com WiFi.
   * Motivo: Timeout de 10 segundos previne travamento indefinido.
   *         Permite firmware continuar mesmo sem WiFi para debug.
   */
  static void waitForConnection();
};

#endif // WIFI_MANAGER_H
