# Backend e Firmware

O firmware é escrito em C++ utilizando o framework Arduino e bibliotecas padrão do ESP8266.

## Organização do Código

O código é orientado a objetos para facilitar a manutenção e testes.

### Classes Principais

#### `EnergySensor` (`src/energy_sensor.cpp`)
Encapsula a biblioteca `EmonLib`.
- **Método `sample()`:** Chamado no loop principal. Lê o sensor, filtra ruídos (threshold) e acumula energia.
- **Cálculo de Potência:** `P = V * I` (Considerando tensão fixa definida em `config.h`).
- **Cálculo de Energia:** Integração da potência no tempo (Wh).

---
#### `DataStorage` (`src/data_storage.cpp`)
Gerencia a memória RAM e Flash.
- **Buffer Circular:** Array de structs `DataPoint` que sobrescreve os dados mais antigos quando cheio.
- **Agregação:** O sistema não grava no disco a cada segundo. Ele acumula 60 amostras e grava a média a cada minuto.
- **LittleFS:** Utilizado para salvar o arquivo `data_energ.csv`.
---
#### `WebServer` (`src/web_server.cpp`)
Configura as rotas HTTP e serve a interface.
- Usa **Lambdas** (`this{...}`) para roteamento, permitindo acesso aos métodos da classe.
- Serve o HTML comprimido armazenado em `web_ui.h`.
---
#### `WiFiManager` (`src/wifi_manager.cpp`)
Singleton para gerenciar conexão.
- Implementa timeout na conexão para evitar Watchdog Timer Reset se o roteador estiver desligado.
---
## Fluxo de Execução (`main.cpp`)

1. **Setup:**
   - Conecta WiFi.
   - Inicializa Sensor (calibração).
   - Monta Sistema de Arquivos.
   - Inicia WebServer e mDNS.

2. **Loop:**
   - `webServer.handleRequests()`: Processa HTTP.
   - Verifica se passou `SAMPLE_INTERVAL_MS` (1000ms).
   - Se sim:
     - `sensor.sample()`
     - `storage.addDataPoint()`

## Boas Práticas Implementadas

- **Non-blocking Delay:** Uso de `millis()` em vez de `delay()` no loop principal para manter o servidor web responsivo.
- **PROGMEM:** Strings longas (HTML) armazenadas na Flash para economizar RAM.
- **Fail-Safe:** Verificações de `nullptr` e timeouts de rede.
