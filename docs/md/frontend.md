# Frontend e Interface Web

A interface do usuário é uma Single Page Application (SPA) leve, projetada para ser servida diretamente pelo microcontrolador.

## Estrutura do Arquivo `web_ui.h`

Todo o código HTML, CSS e JavaScript está contido na variável `INDEX_HTML` dentro de `include/web_ui.h`, utilizando a macro `PROGMEM`.

### Tecnologias
- **HTML5 / CSS3:** Layout responsivo usando CSS Grid e Flexbox. Variáveis CSS (`:root`) para temas.
- **JavaScript (Vanilla):** Lógica de atualização do DOM sem frameworks pesados.
- **Chart.js:** Biblioteca externa (CDN) para renderização dos gráficos.

## Fluxo de Dados no Frontend

1. **Inicialização:**
   - Carrega a página.
   - Chama `loadHistory()` para buscar dados passados (`/api/history`).
   - Chama `loadStats()` para preencher os cards de estatísticas.

2. **Loop de Tempo Real (1s):**
   - Função `fetchAPI()` chama `/api`.
   - Atualiza os números grandes (Corrente, Potência).
   - Adiciona o ponto ao gráfico "Tempo-real" (canvas pequeno nos cards).
   - Verifica flag de calibração.

3. **Loop de Histórico (15s):**
   - Recarrega o gráfico principal para manter sincronia com o backend.

## Como Modificar a Interface

Como o HTML está dentro de uma string C++ (`R"HTML(...)HTML"`), a edição direta pode ser trabalhosa.

**Fluxo recomendado para desenvolvimento:**
1. Copie o conteúdo de `INDEX_HTML` para um arquivo `index.html` local.
2. Edite e teste no navegador (você precisará "mockar" as chamadas de API ou rodar um servidor local simples).
3. Após finalizar, copie o conteúdo de volta para `include/web_ui.h`.

### Personalização de Cores
No bloco `<style>`, altere as variáveis em `:root`:
```css
:root {
  --accent: #0ea5e9; /* Cor principal (Azul) */
  --bg1: #f7f9fc;    /* Fundo */
}
```

> **Atenção:** Ao editar `web_ui.h`, cuidado para não remover os delimitadores `R"HTML(` e `)HTML";`.
