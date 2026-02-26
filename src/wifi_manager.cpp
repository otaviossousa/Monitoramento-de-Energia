#include "wifi_manager.h"
#include <time.h>

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

  // Sincroniza horário com servidor NTP se conectado
  if (isConnected())
  {
    /**
     * Motivo: NTP sincroniza o relógio interno do ESP8266 com servidor de horário
     * Necessário para timestamps válidos nos dados registrados
     * UTC timezone (0) - Pode ser ajustado para sua região
     */
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");

    // Aguarda sincronização (máximo 10 segundos)
    time_t now = time(nullptr);
    int syncAttempts = 0;
    while (now < 24 * 3600 && syncAttempts < 20)
    {
      delay(500);
      now = time(nullptr);
      syncAttempts++;
    }
  }
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
