// ==========================================
// LIBRARIES / BIBLIOTECAS
// ==========================================

#include "config.h"
#include "energy_sensor.h"
#include "web_server.h"
#include "wifi_manager.h"

// ==========================================
// GLOBAL OBJECTS / OBJETOS GLOBAIS
// ==========================================

EnergySensor sensor;
WebServer webServer(&sensor);

// ==========================================
// TIMING / CONTROLE DE TEMPO
// ==========================================

unsigned long lastSampleTime = 0;

// ==========================================
// SETUP / INICIALIZAÇÃO
// ==========================================

/**
 * Inicializa todos os módulos do sistema.
 * Sequência: WiFi → Sensor → Servidor Web
 */
void setup()
{
  WiFiManager::connect();
  sensor.init();
  webServer.init();
}

// ==========================================
// MAIN LOOP / LOOP PRINCIPAL
// ==========================================

/**
 * Loop principal do microcontrolador.
 * Responsabilidades:
 * 1. Amostragem de dados do sensor (a cada SAMPLE_INTERVAL_MS)
 * 2. Processamento de requisições HTTP
 * 3. Manutenção do serviço mDNS
 */
void loop()
{
  // Processa requisições HTTP
  webServer.handleRequests();

  // Mantém anúncio mDNS ativo
  webServer.updateMDNS();

  // Coleta dados do sensor em intervalos regulares
  unsigned long now = millis();
  if (now - lastSampleTime >= SAMPLE_INTERVAL_MS)
  {
    lastSampleTime = now;
    sensor.sample();
  }
}
