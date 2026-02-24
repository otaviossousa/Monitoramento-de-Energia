#ifndef CONFIG_H
#define CONFIG_H

// ==========================================
// WIFI CONFIGURATION / CONFIGURAÇÃO WIFI
// ==========================================
#ifndef WIFI_SSID
#define WIFI_SSID "DEFINIR SSID" // Rede WiFi (defina antes de compilar)
#endif

#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD "DEFINIR PASSWORD" // Senha WiFi (defina antes de compilar)
#endif

// ==========================================
// SENSOR CONFIGURATION / CONFIGURAÇÃO SENSOR
// ==========================================
#define SENSOR_PIN A0                 // Pino analógico do sensor SCT013
#define SENSOR_CALIBRATION 84.8       // Calibração empírica para precisão
#define SENSOR_SAMPLES 1480           // Amostras para cálculo RMS
#define SENSOR_CURRENT_THRESHOLD 0.05 // Filtro de leitura (Amperes)

// ==========================================
// POWER CONFIGURATION / CONFIGURAÇÃO POTÊNCIA
// ==========================================
#define GRID_VOLTAGE 380.0                 // Tensão de rede (380V trifásico)
#define ENERGY_CONVERSION_FACTOR 3600000.0 // ms para horas

// ==========================================
// TIMING CONFIGURATION / CONFIGURAÇÃO TIMING
// ==========================================
#define SAMPLE_INTERVAL_MS 1000 // Intervalo de coleta (ms)

// ==========================================
// NETWORK CONFIGURATION / CONFIGURAÇÃO REDE
// ==========================================
#define MDNS_SERVICE_NAME "paineldeenergia" // Nome do serviço mDNS // (acessível via http://paineldeenergia.local)

#define HTTP_SERVER_PORT 80              // Porta do servidor web

// ==========================================
// UI CONFIGURATION / CONFIGURAÇÃO UI
// ==========================================
#define CHART_BUFFER_SIZE 60 // Pontos no gráfico histórico

#endif // CONFIG_H
