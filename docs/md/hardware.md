# Hardware e Conexões

Este documento descreve os componentes físicos e o esquema de ligação necessário para o funcionamento do monitor de energia.

## Lista de Componentes

1. **Microcontrolador:** ESP8266 (NodeMCU v3, Wemos D1 Mini ou similar).
2. **Sensor de Corrente:** SCT-013-000 (Não invasivo, 100A/50mA).
3. **Componentes para Circuito de Condicionamento (Bias):**
   - 2x Resistores de 10/100Ω  para o divisor de tensão.
   - 1x Capacitor de 10/100µF (Eletrolítico) para desacoplamento/bypass.
   - 1x Resistor de Carga (Burden Resistor): **Necessário apenas se o sensor não tiver interno**. O SCT-013-000 geralmente precisa de um resistor externo (ex: 33Ω ou 18Ω). Para este projeto está sendo utilizado o de 22Ω.
   - 1x Jack J2 fêmea (Opcional, para conectar o plug do sensor).

## Esquema de Ligação

O ESP8266 possui um ADC que lê de 0 a 1V (ou 0 a 3.3V dependendo da placa, como o NodeMCU que já tem divisor interno). Como o sensor gera uma corrente alternada (AC), precisamos criar um "terra virtual" no meio da tensão de referência para ler a onda completa (ciclos positivos e negativos).

### Circuito Divisor de Tensão (Offset)
![Diagrama de Fluxo de Dados](img/circuito_divisor_de_tensao.png)

### Conexão do Sensor SCT-013

O sensor é conectado em paralelo com o circuito acima:
1. Um fio do sensor vai ao **GND**.
2. O outro fio do sensor vai ao ponto central **A0** (onde os resistores se encontram).
3. **Importante:** Se usar o SCT-013-000, coloque o **Resistor de Carga (Burden)** em paralelo com os dois fios do sensor.

## Instalação no Quadro Elétrico

1. **Segurança Primeiro:** Desligue o disjuntor geral antes de instalar se não tiver experiência.
2. Abra o clipe do sensor SCT-013.
3. Abrace **apenas um** dos fios da fase do circuito que deseja medir.
   - ❌ Se abraçar o cabo inteiro (Fase + Neutro), a leitura será ZERO (cancelamento de fluxo).
   - ✅ Abrace apenas o fio da Fase.
4. Feche o clipe até ouvir o clique.

## Configuração no Código

No arquivo `include/config.h`:
```cpp
#define SENSOR_PIN A0
#define GRID_VOLTAGE 220.0  // Ajuste para a tensão da sua rede
```
