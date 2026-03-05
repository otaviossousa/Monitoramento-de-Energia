# Armazenamento e Persistência

Este documento detalha como o sistema gerencia a aquisição, agregação e persistência dos dados de energia.

## Estratégia de Dados

O sistema utiliza uma abordagem híbrida para equilibrar resolução temporal e uso de memória:

1.  **Amostragem (1s):** O sensor é lido a cada segundo para capturar transientes e picos em tempo real.
2.  **Agregação (60s):** As leituras de 1 segundo são acumuladas. A cada 60 amostras, calcula-se a **média** de corrente e potência.
3.  **Persistência:** Apenas o dado agregado (média do minuto) é salvo no Buffer RAM e no arquivo CSV.

### Por que isso é feito?
- **Economia de Flash:** Gravar no sistema de arquivos a cada segundo desgastaria a memória Flash rapidamente.
- **Histórico Longo:** Permite armazenar 24 horas de dados na RAM (1440 minutos) com baixo consumo de memória.
- **Suavização:** Elimina ruídos pontuais na visualização histórica.

---

## Estruturas de Armazenamento

### 1. Buffer Circular (RAM)
Mantém os dados mais recentes para acesso rápido pela interface web.

- **Capacidade:** 1440 registros.
- **Cobertura Temporal:** 24 horas (1 registro/minuto).
- **Comportamento:** Quando cheio, sobrescreve o dado mais antigo (FIFO).

### 2. Arquivo CSV (Flash / LittleFS)
Armazenamento persistente que sobrevive ao reinício do ESP8266.

- **Arquivo Principal:** `/data.csv`
- **Backup Automático:** `/data_backup.csv`
- **Rotação:** Quando o arquivo principal atinge **150KB** (aprox. 7 dias de dados), ele é renomeado para backup e um novo é iniciado.
- **Formato:** `data_hora,corrente,potencia,energia`
  - Exemplo: `2026-02-25T14:30:00,2.500,550.0,1250.50`
- **Exemplo Real**: Arquivo com 3h de registro de monitoramento. `docs/csv/data_energy.csv`

---

### 3. **GET /api/csv** (Download de Arquivo)
Faz download do arquivo `data_energy.csv` com todo histórico.
```csv
data_hora,corrente,potencia,energia
2026-02-25T17:13:20,12.500,4800.0,125.340
2026-02-25T17:14:20,12.400,4700.0,126.100
```

### 4. **POST /api/clear** (Limpar Histórico)
Deleta todos os dados armazenados.
```json
{"status": "cleared"}
```

---

## Armazenamento no ESP8266

### Espaço Disponível
- **LittleFS**: ~1MB (configurável no platformio.ini)
- **Tamanho máximo CSV**: 150KB (rotação automática)
- **Backup**: Mantém arquivo anterior quando rotaciona

### Arquivo CSV
Localização: `/data.csv` (na flash do ESP8266)

Estrutura:
```csv
data_hora,corrente,potencia,energia
2026-02-25T17:13:20,12.500,4800.0,125.340
2026-02-25T17:14:20,12.400,4700.0,126.100
```

## 🔧 Troubleshooting

### "Arquivo não encontrado" em /api/csv
- CSV é criado apenas na primeira gravação (1 min)
