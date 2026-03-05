# Arquitetura do Sistema

O sistema segue uma arquitetura modular baseada em eventos de tempo (Polling) e interrupções de rede, projetada para rodar em um microcontrolador single-core (ESP8266).

## Diagrama de Fluxo de Dados

![Diagrama de Fluxo de Dados](https://github.com/otaviossousa/Monitoramento-de-Energia/blob/main/docs/img/diagrama_do_fluxo_de_dados.png?raw=true)


## Módulos Principais

### 1. Camada de Hardware (`EnergySensor`)
Responsável pela interação direta com a biblioteca `EmonLib`.
- **Função:** Lê o ADC, aplica calibração e calcula Corrente RMS e Potência Aparente.
- **Estabilização:** Implementa lógica (`STABILIZATION_TIME_MS`) para ignorar leituras instáveis logo após o boot.

### 2. Camada de Dados (`DataStorage`)
Gerencia a persistência e integridade dos dados.
- **Buffer em RAM:** Mantém os últimos dados para acesso rápido via API.
- **Agregação:** Acumula 60 leituras de 1 segundo para gravar apenas uma média por minuto no arquivo CSV, preservando a vida útil da memória Flash.
- **Rotação de Arquivo:** Monitora o tamanho do CSV (`MAX_CSV_SIZE`). Se exceder 150KB, realiza backup e inicia um novo arquivo.

### 3. Camada de Apresentação (`WebServer` & `WebUI`)
- **Servidor:** Baseado em `ESP8266WebServer`.
- **Frontend:** Single Page Application (SPA) embutida na Flash (PROGMEM) via `web_ui.h`.
- **API:** Exponibiliza dados em JSON para o frontend.

### 4. Conectividade (`WiFiManager`)
Gerencia a conexão WiFi.
- **Fail Fast:** Tenta conectar por um tempo limitado. Se falhar, o sistema continua operando offline (registrando dados), permitindo debug via Serial ou reconexão futura.

## Ciclo de Vida (Loop Principal)

O `loop()` no `main.cpp` é não-bloqueante:
1. **WebServer:** Atende requisições HTTP pendentes.
2. **mDNS:** Atualiza o serviço de descoberta.
3. **Timer (1s):**
   - Chama `sensor.sample()`.
   - Chama `storage.addDataPoint()`.
