# 📊 Guia do Sistema de Armazenamento de Dados

## 🎯 Resumo da Implementação

Seu projeto agora possui um **sistema completo de persistência de dados** com:

- ✅ **Buffer Circular em RAM**: 288 pontos (24 horas com intervalo de 5 minutos)
- ✅ **Armazenamento em Arquivo CSV**: Persistência em LittleFS do ESP8266
- ✅ **5 Novos Endpoints REST**: Para consulta, histórico e download
- ✅ **Estatísticas Automáticas**: Pico, média, total e último valor
- ✅ **Limite Automático**: Rotação de arquivo quando ultrapassa 150KB

---

## 📝 Novos Arquivos

### 1. `include/data_storage.h` e `src/data_storage.cpp`
Classe central de gerenciamento de dados com métodos:

```cpp
// Adicionar novo ponto (chamado automaticamente no loop)
storage.addDataPoint(current, power, energy);

// Obter histórico em JSON para dashboard
storage.getHistoryJSON();

// Obter estatísticas calculadas
storage.getStatisticsJSON();

// Salvar CSV manualmente
storage.saveToCSV();

// Limpar histórico
storage.clearHistory();
```

---

## 🔌 Novos Endpoints HTTP

### 1. **GET /api** (Dados Instantâneos)
```json
{
  "i": 12.5,      // Corrente em Amperes
  "p": 4800.0,    // Potência em Watts
  "e": 125.34,    // Energia acumulada em Wh
  "s": 1,         // Status (0=calibrando, 1=estabilizado)
  "r": 0          // Tempo restante calibração (segundos)
}
```

### 2. **GET /api/history** (Histórico Completo)
```json
{
  "history": [
    {"ts": 1708881200, "i": 12.5, "p": 4800.0, "e": 125.34},
    {"ts": 1708881300, "i": 12.4, "p": 4700.0, "e": 126.10},
    ...
  ]
}
```
**Uso**: Carregar gráfico de histórico no dashboard (últimas 24h)

### 3. **GET /api/stats** (Estatísticas)
```json
{
  "peak": 5200.5,    // Pico de potência (W)
  "avg": 3400.2,     // Média de potência (W)
  "total": 1850.75,  // Consumo total acumulado (Wh)
  "last": 4800.0     // Último valor registrado (W)
}
```
**Uso**: Cards de resumo no painel

### 4. **GET /api/csv** (Download de Arquivo)
Faz download do arquivo `data_energy.csv` com todo histórico
```csv
timestamp,current,power,energy
1708881200,12.5,4800.0,125.34
1708881300,12.4,4700.0,126.10
```

### 5. **POST /api/clear** (Limpar Histórico)
Deleta todos os dados armazenados
```json
{"status": "cleared"}
```

---

## ⚙️ Configurações (em `config.h`)

```cpp
#define STORAGE_BUFFER_SIZE 288              // Pontos em RAM (24h)
#define STORAGE_SAVE_INTERVAL_MS 300000      // Salva CSV a cada 5 minutos
#define STORAGE_CSV_FILENAME "/data.csv"     // Arquivo principal
#define STORAGE_CSV_BACKUP_FILENAME "/data_backup.csv"  // Backup
```

**Explicação do intervalo**:
- Amostragem: **1 segundo** (SAMPLE_INTERVAL_MS)
- Armazenamento: **5 minutos** (STORAGE_SAVE_INTERVAL_MS)
- Total em RAM: 288 × 5min = 1440min = **24 horas**

---

## 🔄 Fluxo de Dados

```
┌─────────────────────┐
│ Sensor SCT013       │
│ (a cada 1seg)       │
└──────────┬──────────┘
           │
           ▼
┌─────────────────────┐
│ EnergySensor.sample()│
│ Lê corrente/potência│
└──────────┬──────────┘
           │
           ▼
┌─────────────────────┐
│ DataStorage.add()   │
│ Buffer + Arquivo CSV│
└──────────┬──────────┘
           │
           ▼
┌─────────────────────┐
│ WebServer API       │
│ Endpoints JSON      │
└─────────────────────┘
```

---

## 📊 Uso no Frontend

### Exemplo 1: Carregar Histórico ao Iniciar
```javascript
fetch('/api/history')
  .then(r => r.json())
  .then(data => {
    // data.history contém últimas 24h
    atualizarGrafico(data.history);
  });
```

### Exemplo 2: Mostrar Estatísticas
```javascript
fetch('/api/stats')
  .then(r => r.json())
  .then(data => {
    document.getElementById('pico').textContent = data.peak.toFixed(1) + ' W';
    document.getElementById('media').textContent = data.avg.toFixed(1) + ' W';
    document.getElementById('total').textContent = data.total.toFixed(2) + ' Wh';
  });
```

### Exemplo 3: Download de Arquivo CSV
```javascript
// Cria link de download
const link = document.createElement('a');
link.href = '/api/csv';
link.download = 'energy_data.csv';
link.click();
```

---

## 💾 Armazenamento no ESP8266

### Espaço Disponível
- **LittleFS**: ~1MB (configurável no platformio.ini)
- **Tamanho máximo CSV**: 150KB (rotação automática)
- **Backup**: Mantém arquivo anterior quando rotaciona

### Arquivo CSV
Localização: `/data.csv` (na flash do ESP8266)

Estrutura:
```
timestamp,current,power,energy
1708881200,12.50,4800.0,125.34
1708881300,12.40,4700.0,126.10
```

---

## 🚀 Próximos Passos Recomendados

1. **Atualizar Dashboard (`web_ui.h`)**
   - Adicionar gráfico de histórico com `/api/history`
   - Mostrar cards de estatísticas com `/api/stats`
   - Botão para download CSV
   - Botão para limpar histórico

2. **Visualização em Tempo Real**
   - Gráfico atualizado a cada 1 segundo com `/api`
   - Histórico carregado ao iniciar página

3. **Análise Avançada** (Opcional)
   - Detectar picos anormais
   - Alertas por limite de consumo
   - Comparação com períodos anteriores

---

## 🔧 Troubleshooting

### "Arquivo não encontrado" em /api/csv
- CSV é criado apenas na primeira gravação (5 min após inicialização)
- Aguarde 5 minutos

### Memória RAM baixa
Se atingir 75%+ de RAM:
- Reduzir STORAGE_BUFFER_SIZE de 288 para 144 (12h)
- Aumentar STORAGE_SAVE_INTERVAL_MS para 600000 (10 min)

### Flash cheia
Arquivo roda automaticamente quando ultrapassa 150KB
- Pode ser alterado modificando `MAX_CSV_SIZE` em `data_storage.cpp`

---

## 📌 Resumo de Modificações

| Arquivo | Modificação |
|---------|------------|
| `config.h` | +4 linhas (configurações storage) |
| `web_server.h` | +1 membro, +5 endpoints |
| `web_server.cpp` | +60 linhas (handlers) |
| `main.cpp` | +2 linhas (inicialização) |
| `platformio.ini` | +1 dependência (LittleFS) |
| **Novos** | `data_storage.h` + `data_storage.cpp` |

**Status de Compilação**: ✅ **OK** (sem erros ou warnings)

---

Última atualização: 25 de Fevereiro de 2026
