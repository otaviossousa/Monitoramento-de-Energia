# Documentação da API REST

O dispositivo expõe uma API JSON para consumo pelo frontend ou sistemas de terceiros (Home Assistant, Node-RED, etc).

## Endpoints

### 1. Dados em Tempo Real
Retorna os valores instantâneos lidos pelo sensor.

- **URL:** `/api`
- **Método:** `GET`
- **Resposta (JSON):**
```json
{
  "i": 2.450,    // Corrente (Amperes)
  "p": 539.0,    // Potência (Watts)
  "e": 12.500,   // Energia Acumulada (Wh)
  "s": 1,        // Status (1=Estabilizado, 0=Calibrando)
  "r": 0,        // Tempo restante de calibração (s)
  "u": 3600      // Uptime (segundos)
}
```

### 2. Histórico Recente
Retorna os dados armazenados no buffer circular (RAM) para construção de gráficos.

- **URL:** `/api/history`
- **Método:** `GET`
- **Resposta (JSON):**
```json
{
  "history": [
    {
      "ts": 1709230000, // Timestamp Unix
      "i": 2.4,         // Corrente
      "p": 528.0,       // Potência
      "e": 12.4         // Energia
    },
    ...
  ]
}
```

### 3. Estatísticas
Retorna dados calculados sobre o período monitorado no buffer.

- **URL:** `/api/stats`
- **Método:** `GET`
- **Resposta (JSON):**
```json
{
  "peak": 1200.5,        // Pico de Potência (W)
  "peak_current": 5.4,   // Pico de Corrente (A)
  "avg": 450.2,          // Média de Potência (W)
  "avg_current": 2.1     // Média de Corrente (A)
}
```

### 4. Informações de Rede
Retorna status da conexão.

- **URL:** `/net`
- **Método:** `GET`
- **Resposta (JSON):**
```json
{
  "ssid": "NomeDaRede",
  "ip": "192.168.1.105"
}
```

### 5. Download de Dados (CSV)
Baixa o arquivo completo de histórico persistente.

- **URL:** `/api/csv`
- **Método:** `GET`
- **Content-Type:** `text/csv`

### 6. Limpar Histórico
- **URL:** `/api/clear`
- **Método:** `POST`
- **Descrição:** Apaga todos os dados da memória RAM e o arquivo CSV da Flash.
