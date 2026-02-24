#include "wifi_manager.h"

// ==========================================
// PUBLIC STATIC METHODS / MÉTODOS ESTÁTICOS PÚBLICOS
// ==========================================

void WiFiManager::connect()
{
  /**
   * Motivo: Inicia processo de conexão com timeout para evitar
   * travamento indefinido em ambiente sem WiFi disponível.
   */
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  waitForConnection();
}

bool WiFiManager::isConnected()
{
  return WiFi.status() == WL_CONNECTED;
}

String WiFiManager::getSSID()
{
  return WiFi.SSID();
}

String WiFiManager::getLocalIP()
{
  return WiFi.localIP().toString();
}

// ==========================================
// PRIVATE STATIC METHODS / MÉTODOS ESTÁTICOS PRIVADOS
// ==========================================

void WiFiManager::waitForConnection()
{
  /**
   * Motivo: Timeout de 10 segundos (20 × 500ms) previne travamento.
   * Permite firmware continuar mesmo sem WiFi, facilitando debug.
   * Veja rule.md #4 - Fail Fast: Validação precoce evita problemas.
   */
  int attempts = 0;
  const int MAX_ATTEMPTS = 20;
  const int DELAY_MS = 500;

  while (WiFi.status() != WL_CONNECTED && attempts < MAX_ATTEMPTS)
  {
    delay(DELAY_MS);
    attempts++;
  }
}
