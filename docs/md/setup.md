# Instalação e Configuração

Este guia cobre os passos necessários para compilar e carregar o firmware no ESP8266.

## Pré-requisitos

- **VS Code** instalado.
- Extensão **PlatformIO IDE** instalada no VS Code.
- Drivers USB para o seu modelo de placa ESP8266 (ex: CH340 ou CP2102).

## 1. Clonar o Repositório

```bash
git clone https://github.com/otaviossousa/energy-monitor.git
cd energy-monitor
```

## 2. Configuração do Projeto

Antes de compilar, é **essencial** configurar as credenciais de rede e parâmetros do sensor.

Abra o arquivo `include/config.h` e edite as definições conforme seu ambiente:

### Configuração WiFi
```cpp
#define WIFI_SSID "SuaRedeWiFi"      // Nome da sua rede
#define WIFI_PASSWORD "SuaSenha123"  // Senha da rede
```

### Calibração do Sensor e Rede
```cpp
#define SENSOR_CALIBRATION 84.8      // Valor de calibração (ajuste com multímetro)
#define GRID_VOLTAGE 220.0           // Tensão da sua rede (127.0, 220.0 ou 380.0)
```
> **Nota:** O valor `84.8` é empírico para o sensor SCT-013-000 com resistor de carga padrão. Se a leitura estiver imprecisa, ajuste este valor proporcionalmente.

### Outras Configurações
- `SAMPLE_INTERVAL_MS`: Intervalo de leitura (padrão 1000ms).
- `STORAGE_SAVE_INTERVAL_MS`: Frequência de gravação no CSV.

## 3. Compilação e Upload

1. Conecte o ESP8266 ao computador via USB.
2. No PlatformIO (barra lateral esquerda do VS Code), clique no ícone do Alien.
3. Em **Project Tasks**, selecione seu ambiente (ex: `nodemcuv2`).
4. Clique em **Build** para compilar e verificar erros.
5. Clique em **Upload** para gravar o firmware na placa.
6. (Opcional) Clique em **Monitor** para ver os logs na Serial (Baudrate 115200).

## 4. Sistema de Arquivos (LittleFS)

O projeto utiliza LittleFS para salvar o histórico. O código formata automaticamente a memória na primeira inicialização se não conseguir montar o sistema de arquivos.

Se desejar limpar a memória manualmente ou fazer upload de arquivos iniciais:
- Use a tarefa **PlatformIO: Upload Filesystem Image**.

## 5. Verificação

Após o upload:
1. Abra o Monitor Serial.
2. O ESP8266 tentará conectar ao WiFi.
3. Ao conectar, ele exibirá o IP atribuído (ex: `192.168.1.100`).
4. Acesse esse IP no navegador ou tente `http://monitoramentodeenergia.local`.
