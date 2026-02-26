#ifndef WEB_UI_H
#define WEB_UI_H

/**
 * HTML e JavaScript para dashboard de monitoramento de energia.
 * Separado em arquivo header para melhorar legibilidade e manutenção.
 */
const char INDEX_HTML[] PROGMEM = R"HTML(
<!doctype html>
<html lang="en">
<head>
<meta charset="utf-8"/>
<meta name="viewport" content="width=device-width,initial-scale=1"/>
<title>Painel de Energia</title>

<!-- Chart.js para gráficos históricos -->
<script src="https://cdn.jsdelivr.net/npm/chart.js@4.4.0/dist/chart.umd.min.js"></script>

<style>
/* ==========================================
   DESIGN SYSTEM / TOKENS DE DESIGN
   ========================================== */
:root{
  --bg1:#f7f9fc; --bg2:#eef2f7;
  --card:#ffffff; --border:#e6e9ef;
  --text:#1f2430; --muted:#6b7280;
  --accent:#0ea5e9;
  --shadow:0 12px 30px rgba(14,53,108,.10);
}

/* ==========================================
   RESET / ESTILOS BASE
   ========================================== */
*{box-sizing:border-box}
body{
  margin:0;
  font-family:Inter,system-ui;
  background:radial-gradient(1200px 900px at 20% 10%,var(--bg2),var(--bg1));
  display:flex; justify-content:center; padding:22px;
}
.wrap{
  width:1100px;
  display:flex;
  flex-direction:column;
  gap:18px;
}

/* ==========================================
   HEADER / CABEÇALHO
   ========================================== */
.header{
  display:flex;
  align-items:center;
  justify-content:space-between;
}
h1{
  margin:0;
  display:flex;
  gap:10px;
  align-items:center;
}
.pill{
  padding:8px 12px;
  border-radius:999px;
  font-size:12px;
  color:var(--muted);
  background:#ffffffaa;
}

/* ==========================================
   GRID LAYOUT / LAYOUT DO DASHBOARD
   ========================================== */
.grid{
  display:grid;
  grid-template-columns:repeat(3,1fr);
  gap:16px;
}
.grid-full{
  grid-column:1/-1;
}

/* ==========================================
   BUTTON / BOTÕES
   ========================================== */
.button{
  padding:10px 16px;
  border:none;
  border-radius:10px;
  font-size:14px;
  cursor:pointer;
  transition:.2s;
  font-weight:600;
  display:inline-flex;
  align-items:center;
  gap:8px;
}
.button-primary{
  background:var(--accent);
  color:white;
}
.button-primary:hover{
  background:#0284c7;
  transform:translateY(-2px);
  box-shadow:0 8px 16px rgba(14,165,233,.3);
}
.button-secondary{
  background:#f3f4f6;
  color:var(--text);
}
.button-secondary:hover{
  background:#e5e7eb;
}
.button-danger{
  background:#fee2e2;
  color:#991b1b;
}
.button-danger:hover{
  background:#fecaca;
  transform:translateY(-2px);
}

/* ==========================================
   ACTIONS / AÇÕES
   ========================================== */
.actions{
  display:flex;
  gap:12px;
  margin-top:16px;
  padding-top:16px;
  border-top:1px solid var(--border);
}

/* ==========================================
   STATS GRID / GRID DE ESTATÍSTICAS
   ========================================== */
.stats-container{
  display:grid;
  grid-template-columns:repeat(4,1fr);
  gap:12px;
  margin-bottom:16px;
}
.stat-item{
  background:#f9fafb;
  border-radius:12px;
  padding:12px;
  text-align:center;
}
.stat-value{
  font-size:24px;
  font-weight:700;
  color:var(--accent);
  margin:4px 0;
}
.stat-label{
  font-size:11px;
  color:var(--muted);
  text-transform:uppercase;
  letter-spacing:.5px;
}

/* ==========================================
   CARD / CARTÃO DE MÉTRICA
   ========================================== */
.card{
  background:white;
  border-radius:18px;
  padding:16px;
  box-shadow:var(--shadow);
}
.label{
  font-size:12px;
  color:var(--muted);
  text-transform:uppercase;
  display:flex;
  gap:8px;
}
.chip{
  font-size:11px;
  padding:4px 8px;
  border-radius:999px;
  background:#dff3fc;
  color:#075985;
}
.chip.purple{
  background:#f3e8ff;
  color:#7e22ce;
}

.value{
  font-size:36px;
  font-weight:800;
  margin-top:6px;
  transition:.2s;
}
.unit{
  font-size:14px;
  color:var(--muted);
}
canvas{
  width:100%;
  height:28px;
  margin-top:8px;
}

/* ==========================================
   FOOTER / RODAPÉ
   ========================================== */
.footer{
  display:flex;
  justify-content:flex-end;
  gap:8px;
  font-size:12px;
  color:var(--muted);
}
.ok{ color:#16a34a; font-weight:600; }
.warn{ color:#ca8a04; font-weight:600; }
.bump{ transform:translateY(-2px) scale(1.01); }

/* ==========================================
   RESPONSIVENESS / RESPONSIVIDADE
   ========================================== */

/* Tablets e celulares médios */
@media (max-width: 900px) {
  .wrap{
    width:100%;
  }

  .grid{
    grid-template-columns:repeat(2, 1fr);
  }

  .stats-container{
    grid-template-columns:repeat(2, 1fr);
  }

  .grid{
    grid-template-columns:1fr;
  }

  .stats-container{
    grid-template-columns:1fr;
  }

  .actions{
    flex-direction:column;
  }

  .button{
    width:100%;
    justify-content:center;
  }

  .value{
    font-size:28px;
  }

  .card{
    padding:20px;
  }
}

/* Estilos para títulos dos gráficos */
h3 {
  font-family: inherit;
  margin:0 0 12px 0;
  font-size:14px;
  color:var(--muted);
  text-transform:uppercase;
  letter-spacing:0.5px;
}

/* ==========================================
   LAYOUT RESPONSIVO PARA ESTATÍSTICAS
   ========================================== */
.stats-layout {
  display:grid;
  grid-template-columns:1fr 1fr;
  gap:24px;
  margin-top:16px;
}

@media (max-width:1024px) {
  .stats-layout {
    grid-template-columns:1fr;
    gap:16px;
  }
}
</style>
</head>

<body>
<div class="wrap">

<div class="header">
  <h1>Painel de Energia</h1>
  <div style="display:flex; gap:8px;">
    <div class="pill" id="calib" style="display:none; background:#fef3c7; color:#875400;">⏳ Calibrando...</div>
    <div class="pill" id="net">Conectando...</div>
  </div>
</div>

<div class="grid">
  <div class="card">
    <div class="label">Current <span class="chip">Real-time</span></div>
    <div class="value" id="i">-- <span class="unit">A</span></div>
    <canvas id="ci"></canvas>
  </div>

  <div class="card">
    <div class="label">Power <span class="chip">Real-time</span></div>
    <div class="value" id="p">-- <span class="unit">W</span></div>
    <canvas id="cp"></canvas>
  </div>

  <div class="card">
    <div class="label">Energy <span class="chip purple">Cumulative</span></div>
    <div class="value" id="e">-- <span class="unit">Wh</span></div>
    <canvas id="ce"></canvas>
  </div>
</div>

<!-- HISTÓRICO E ESTATÍSTICAS -->
<div class="card grid-full">
  <div class="label">Estatísticas <span class="chip">24h Histórico</span></div>

  <!-- Layout com 2 Colunas: Corrente (Esquerda) e Potência (Direita) -->
  <div class="stats-layout">

    <!-- ==================== COLUNA ESQUERDA: CORRENTE ==================== -->
    <div>
      <!-- Estatísticas de Corrente -->
      <div class="stats-container" style="grid-template-columns:1fr 1fr; display:grid; gap:12px; margin-bottom:16px;">
        <div class="stat-item">
          <div class="stat-label">Pico Corrente</div>
          <div class="stat-value" id="stat-peak-current">-</div>
          <div class="stat-label">A</div>
        </div>
        <div class="stat-item">
          <div class="stat-label">Média Corrente</div>
          <div class="stat-value" id="stat-avg-current">-</div>
          <div class="stat-label">A</div>
        </div>
      </div>

      <!-- Gráfico de Corrente -->
      <div style="background:#f9fafb; border-radius:12px; padding:16px;">
        <h3 style="margin:0 0 12px 0; font-size:14px; color:var(--muted); text-transform:uppercase;">⚡ Histórico Corrente</h3>
        <canvas id="historyChartCurrent" style="height:300px;"></canvas>
      </div>
    </div>

    <!-- ==================== COLUNA DIREITA: POTÊNCIA ==================== -->
    <div>
      <!-- Estatísticas de Potência -->
      <div class="stats-container" style="grid-template-columns:1fr 1fr; display:grid; gap:12px; margin-bottom:16px;">
        <div class="stat-item">
          <div class="stat-label">Pico Potência</div>
          <div class="stat-value" id="stat-peak">-</div>
          <div class="stat-label">W</div>
        </div>
        <div class="stat-item">
          <div class="stat-label">Média Potência</div>
          <div class="stat-value" id="stat-avg">-</div>
          <div class="stat-label">W</div>
        </div>
      </div>

      <!-- Gráfico de Potência -->
      <div style="background:#f9fafb; border-radius:12px; padding:16px;">
        <h3 style="margin:0 0 12px 0; font-size:14px; color:var(--muted); text-transform:uppercase;">📊 Histórico Potência</h3>
        <canvas id="historyChartPower" style="height:300px;"></canvas>
      </div>
    </div>

  </div>

  <!-- Ações -->
  <div class="actions">
    <button class="button button-primary" id="btnDownload" onclick="downloadCSV()">
      📥 Baixar CSV
    </button>
    <button class="button button-secondary" id="btnRefresh" onclick="reloadHistory()">
      🔄 Recarregar
    </button>
    <button class="button button-danger" id="btnClear" onclick="clearHistory()">
      🗑️ Limpar Histórico
    </button>
  </div>
</div>

<div class="footer">
  <span>Status:</span><span id="status" class="ok">OK</span>
  <span>| Updated:</span><span id="ts">--:--:--</span>
</div>

</div>

<script>
// ==========================================
// CHART DATA BUFFERS / BUFFERS DE GRÁFICOS
// ==========================================

const N=60;                                    // Número de pontos no gráfico
const buf={i:Array(N).fill(0),p:Array(N).fill(0),e:Array(N).fill(0)}; // Histórico de dados

// Inicializa canvas refs depois que DOM carregar
let cvs = {i: null, p: null, e: null};

// ==========================================
// CANVAS INITIALIZATION / INICIALIZAÇÃO
// ==========================================

/**
 * Ajusta o tamanho do canvas para o DPI do dispositivo
 * @param {HTMLCanvasElement} c - Elemento canvas
 */
function fit(c){
  const r=devicePixelRatio||1,b=c.getBoundingClientRect();
  c.width=b.width*r;c.height=b.height*r;
}

/**
 * Inicializa referências aos canvas elementos
 */
function initCanvasRefs() {
  try {
    cvs.i = document.getElementById('ci');
    cvs.p = document.getElementById('cp');
    cvs.e = document.getElementById('ce');

    if(!cvs.i || !cvs.p || !cvs.e) {
      console.error('Canvas elements não encontrados');
      return false;
    }

    Object.values(cvs).forEach(fit);
    return true;
  } catch(e) {
    console.error('Erro ao inicializar canvas:', e);
    return false;
  }
}

addEventListener('resize',()=>{
  if(cvs.i && cvs.p && cvs.e) {
    Object.values(cvs).forEach(fit);
  }
});

// ==========================================
// CHART DRAWING / DESENHO DE GRÁFICOS
// ==========================================

/**
 * Desenha um gráfico de linha com gradiente no canvas
 * @param {HTMLCanvasElement} c - Canvas a desenhar
 * @param {Array<number>} data - Dados do gráfico
 * @param {string} color - Cor da linha (hex)
 */
function draw(c,data,color){
  // Verifica se canvas é válido
  if(!c || !c.getContext) {
    console.warn('Canvas inválido, pulando draw');
    return;
  }

  try {
    const ctx=c.getContext('2d'),W=c.width,H=c.height,p=6;
    ctx.clearRect(0,0,W,H);

    // Calcula min/max para escala
    const min=Math.min(...data),max=Math.max(...data);
    const lo=min==max?min-1:min, hi=min==max?max+1:max;
    const y=v=>H-p-(v-lo)*(H-2*p)/(hi-lo);

    // Desenha linha de referência
    ctx.strokeStyle="#e6e9ef";
    ctx.beginPath();ctx.moveTo(0,H-p);ctx.lineTo(W,H-p);ctx.stroke();

    // Desenha a linha do gráfico
    ctx.strokeStyle=color;ctx.lineWidth=2;
    ctx.beginPath();
    data.forEach((v,i)=>{
      const x=i*(W/(data.length-1));
      i?ctx.lineTo(x,y(v)):ctx.moveTo(x,y(v));
    });
    ctx.stroke();

    // Desenha gradiente de preenchimento
    const g=ctx.createLinearGradient(0,0,0,H);
    g.addColorStop(0,color+"88");
    g.addColorStop(1,color+"00");
    ctx.fillStyle=g;
    ctx.lineTo(W,H);ctx.lineTo(0,H);ctx.fill();
  } catch(e) {
    console.error('Erro ao desenhar gráfico:', e);
  }
}

// ==========================================
// UTILITY FUNCTIONS / FUNÇÕES UTILITÁRIAS
// ==========================================

/**
 * Adiciona um valor ao buffer, removendo o mais antigo se necessário
 * @param {Array<number>} b - Buffer
 * @param {number} v - Valor a adicionar
 */
function push(b,v){ b.push(v); if(b.length>N)b.shift(); }

/**
 * Anima um elemento com efeito bump (pulo)
 * @param {HTMLElement} el - Elemento a animar
 */
function bump(el){ el.classList.add('bump'); setTimeout(()=>el.classList.remove('bump'),200); }

/**
 * Atualiza o timestamp do último update
 */
function stamp(){
  const d=new Date();
  ts.textContent=d.toLocaleTimeString();
}

// ==========================================
// API COMMUNICATION / COMUNICAÇÃO COM API
// ==========================================

/**
 * Busca os dados de energia da API e atualiza o dashboard
 * Executa a cada 2 segundos
 */
async function fetchAPI(){
  try{
    console.debug('🔄 Buscando /api...');
    const r=await fetch('/api',{cache:'no-store'});
    if(!r.ok) throw new Error('API returned status ' + r.status);

    const j=await r.json();
    console.debug('📊 Dados recebidos:', j);

    // Valida dados recebidos
    if(typeof j.i !== 'number' || typeof j.p !== 'number' || typeof j.e !== 'number') {
      throw new Error('Dados inválidos recebidos da API: ' + JSON.stringify(j));
    }

    // Controla indicador de calibração
    const calib=document.getElementById('calib');
    if(calib) {
      if(j.s===0){
        // Sistema ainda está calibrando
        calib.style.display='block';
        calib.textContent=`⏳ Calibrando... ${j.r}s`;
      }else{
        // Sistema já está calibrado
        calib.style.display='none';
      }
    }

    // Atualiza valores na tela
    const iEl = document.getElementById('i');
    const pEl = document.getElementById('p');
    const eEl = document.getElementById('e');

    if(iEl && iEl.firstChild) {
      iEl.firstChild.nodeValue=j.i.toFixed(3)+" ";
      bump(iEl);
    }
    if(pEl && pEl.firstChild) {
      pEl.firstChild.nodeValue=j.p.toFixed(1)+" ";
      bump(pEl);
    }
    if(eEl && eEl.firstChild) {
      eEl.firstChild.nodeValue=j.e.toFixed(3)+" ";
      bump(eEl);
    }

    // Atualiza buffers de dados
    push(buf.i, j.i);
    push(buf.p, j.p);
    push(buf.e, Math.max(j.e, buf.e.at(-1)));

    // Redesenha os gráficos (com verificação de canvas)
    if(cvs.i) draw(cvs.i,buf.i,"#06b6d4");
    if(cvs.p) draw(cvs.p,buf.p,"#22c55e");
    if(cvs.e) draw(cvs.e,buf.e,"#a855f7");

    // Atualiza status
    const statusEl = document.getElementById('status');
    if(statusEl) { statusEl.textContent="OK"; statusEl.className="ok"; }
    stamp();
  }catch(e){
    console.error('❌ Erro em fetchAPI:', e.message);
    const statusEl = document.getElementById('status');
    if(statusEl) { statusEl.textContent="Error: " + e.message; statusEl.className="warn"; }
  }
}

/**
 * Busca informações da rede WiFi
 * Executa a cada 5 segundos
 */
async function fetchNet(){
  try{
    const r=await fetch('/net');
    if(!r.ok) throw new Error('API /net retornou status ' + r.status);

    const j=await r.json();
    const netEl = document.getElementById('net');
    if(netEl) {
      netEl.textContent=`${j.ssid || 'N/A'} @ ${j.ip || 'N/A'}`;
    }
  }catch(e){
    console.warn('Erro ao buscar dados de rede:', e);
    const netEl = document.getElementById('net');
    if(netEl) netEl.textContent='WiFi Offline';
  }
}

// ==========================================
// HISTORY CHART / GRÁFICOS DE HISTÓRICO
// ==========================================

let historyChartPower = null;
let historyChartCurrent = null;

/**
 * Cria/atualiza os gráficos de histórico com Chart.js (um para cada grandeza)
 * Separa Potência e Corrente em dois gráficos distintos para melhor visualização
 * @param {Array} history - Array de pontos com {ts, i, p, e}
 */
function updateHistoryChart(history){
  try {
    const ctxPower = document.getElementById('historyChartPower');
    const ctxCurrent = document.getElementById('historyChartCurrent');

    if(!ctxPower || !ctxCurrent) {
      console.error('Canvas #historyChartPower ou #historyChartCurrent não encontrado');
      return;
    }

    // Se histórico está vazio, mostra gráficos em branco (sem dados)
    if(!history || history.length === 0) {
      history = [];
    }

    // Formata dados para os gráficos
    const labels = history.map(point => {
      const date = new Date(point.ts * 1000);
      return date.toLocaleTimeString('pt-BR', {hour:'2-digit', minute:'2-digit'});
    }).reverse();

    const powerData = history.map(p => p.p).reverse();
    const currentData = history.map(p => p.i).reverse();

    // Destrói gráficos anteriores se existirem
    if(historyChartPower) {
      try {
        historyChartPower.destroy();
      } catch(e) {
        console.warn('Erro ao destruir gráfico de potência anterior:', e);
      }
    }

    if(historyChartCurrent) {
      try {
        historyChartCurrent.destroy();
      } catch(e) {
        console.warn('Erro ao destruir gráfico de corrente anterior:', e);
      }
    }

    // ==================== GRÁFICO DE POTÊNCIA ====================
    historyChartPower = new Chart(ctxPower.getContext('2d'), {
      type: 'line',
      data: {
        labels: labels,
        datasets: [
          {
            label: 'Potência (W)',
            data: powerData,
            borderColor: '#22c55e',
            backgroundColor: 'rgba(34, 197, 94, 0.1)',
            borderWidth: 2.5,
            fill: true,
            tension: 0.4,
            pointRadius: 0,
            pointHoverRadius: 8,
          }
        ]
      },
      options: {
        responsive: true,
        maintainAspectRatio: true,
        interaction: {
          mode: 'index',
          intersect: false
        },
        plugins: {
          legend: {
            display: false
          },
          tooltip: {
            backgroundColor: 'rgba(0, 0, 0, 0.8)',
            padding: 12,
            titleFont: { size: 13 },
            bodyFont: { size: 12 },
            callbacks: {
              label: function(context) {
                return 'Potência: ' + context.parsed.y.toFixed(2) + ' W';
              }
            }
          }
        },
        scales: {
          y: {
            type: 'linear',
            beginAtZero: true,
            title: { display: true, text: 'Potência (W)', font: { size: 12 } },
            grid: { drawBorder: false, color: 'rgba(0,0,0,0.05)' }
          },
          x: {
            grid: { display: false }
          }
        }
      }
    });

    // ==================== GRÁFICO DE CORRENTE ====================
    historyChartCurrent = new Chart(ctxCurrent.getContext('2d'), {
      type: 'line',
      data: {
        labels: labels,
        datasets: [
          {
            label: 'Corrente (A)',
            data: currentData,
            borderColor: '#06b6d4',
            backgroundColor: 'rgba(6, 182, 212, 0.1)',
            borderWidth: 2.5,
            fill: true,
            tension: 0.4,
            pointRadius: 0,
            pointHoverRadius: 8,
          }
        ]
      },
      options: {
        responsive: true,
        maintainAspectRatio: true,
        interaction: {
          mode: 'index',
          intersect: false
        },
        plugins: {
          legend: {
            display: false
          },
          tooltip: {
            backgroundColor: 'rgba(0, 0, 0, 0.8)',
            padding: 12,
            titleFont: { size: 13 },
            bodyFont: { size: 12 },
            callbacks: {
              label: function(context) {
                return 'Corrente: ' + context.parsed.y.toFixed(3) + ' A';
              }
            }
          }
        },
        scales: {
          y: {
            type: 'linear',
            beginAtZero: true,
            title: { display: true, text: 'Corrente (A)', font: { size: 12 } },
            grid: { drawBorder: false, color: 'rgba(0,0,0,0.05)' }
          },
          x: {
            grid: { display: false }
          }
        }
      }
    });

  } catch(e) {
    console.error('Erro ao criar gráficos de histórico:', e);
  }
}

/**
 * Carrega histórico de 24 horas da API
 */
async function loadHistory(){
  try{
    const r = await fetch('/api/history', {cache:'no-store'});
    const j = await r.json();

    // Sempre chama updateHistoryChart, mesmo se vazio
    const historyData = (j.history && j.history.length > 0) ? j.history : [];
    updateHistoryChart(historyData);
  }catch(e){
    console.error('Erro ao carregar histórico:', e);
    // Mesmo com erro, tenta mostrar gráfico vazio
    updateHistoryChart([]);
  }
}

/**
 * Recarrega o histórico manualmente
 */
async function reloadHistory(){
  try {
    const btn = document.getElementById('btnRefresh');
    if(btn) {
      btn.disabled = true;
      btn.textContent = '⏳ Carregando...';
    }

    await loadHistory();

    if(btn) {
      btn.disabled = false;
      btn.textContent = '🔄 Recarregar';
    }
  } catch(e) {
    console.error('Erro ao recarregar histórico:', e);
    const btn = document.getElementById('btnRefresh');
    if(btn) {
      btn.disabled = false;
      btn.textContent = '🔄 Recarregar';
    }
  }
}

/**
 * Carrega e exibe estatísticas
 */
async function loadStats(){
  try{
    const r = await fetch('/api/stats', {cache:'no-store'});
    if(!r.ok) throw new Error('API retornou status ' + r.status);

    const j = await r.json();

    // Garante que todos os elementos existem antes de atualizar
    const peakEl = document.getElementById('stat-peak');
    const peakCurrentEl = document.getElementById('stat-peak-current');
    const avgEl = document.getElementById('stat-avg');
    const avgCurrentEl = document.getElementById('stat-avg-current');

    if(peakEl) peakEl.textContent = j.peak.toFixed(1);
    if(peakCurrentEl) peakCurrentEl.textContent = j.peak_current.toFixed(2);
    if(avgEl) avgEl.textContent = j.avg.toFixed(1);
    if(avgCurrentEl) avgCurrentEl.textContent = j.avg_current.toFixed(2);
  }catch(e){
    console.error('Erro ao carregar estatísticas:', e);
    // Mostra valores padrão em caso de erro
    document.getElementById('stat-peak').textContent = '-';
    document.getElementById('stat-peak-current').textContent = '-';
    document.getElementById('stat-avg').textContent = '-';
    document.getElementById('stat-avg-current').textContent = '-';
  }
}

/**
 * Faz download do arquivo CSV com histórico completo
 */
async function downloadCSV(){
  try{
    const btn = document.getElementById('btnDownload');
    btn.disabled = true;
    btn.textContent = '⏳ Preparando...';

    // Aguarda um pouco para garantir que a API salvou os dados
    await new Promise(resolve => setTimeout(resolve, 500));

    // Abre link de download
    window.open('/api/csv', '_blank');

    btn.disabled = false;
    btn.textContent = '📥 Baixar CSV';
  }catch(e){
    console.error('Erro ao baixar CSV:', e);
    alert('Erro ao baixar arquivo!');
  }
}

/**
 * Limpa todo o histórico com confirmação
 */
async function clearHistory(){
  const confirmed = confirm('⚠️ Tem certeza que deseja limpar TODOS os dados de histórico?\n\nEsta ação não pode ser desfeita!');

  if(!confirmed) return;

  try{
    const btn = document.getElementById('btnClear');
    btn.disabled = true;
    btn.textContent = '🔄 Limpando...';

    const r = await fetch('/api/clear', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' }
    });

    const j = await r.json();

    if(r.ok) {
      alert('✅ Histórico limpo com sucesso!');

      // Recarrega histórico (que agora estará vazio)
      await loadHistory();
      await loadStats();
    }else{
      alert('❌ Erro ao limpar histórico');
    }

    btn.disabled = false;
    btn.textContent = '🗑️ Limpar Histórico';
  }catch(e){
    console.error('Erro ao limpar histórico:', e);
    alert('Erro ao limpar histórico!');
    const btn = document.getElementById('btnClear');
    btn.disabled = false;
    btn.textContent = '🗑️ Limpar Histórico';
  }
}

// ==========================================
// INITIALIZATION / INICIALIZAÇÃO
// ==========================================

/**
 * Inicializa dashboard: carrega dados iniciais de forma segura
 * Usa Promise.allSettled para garantir que erros em uma função não bloqueiam as outras
 */
async function initDashboard() {
  try {
    console.log('Iniciando dashboard...');

    // Primeiro: inicializa canvas (crítico para draw())
    const canvasOk = initCanvasRefs();
    if (!canvasOk) {
      console.warn('Canvas não inicializado, mas continuando...');
    }

    // Segundo: carrega dados em paralelo (não bloqueia um ao outro)
    // Usa allSettled em vez de all para garantir que erros não param o carregamento
    const results = await Promise.allSettled([
      fetchAPI(),
      fetchNet(),
      loadHistory(),
      loadStats()
    ]);

    results.forEach((result, index) => {
      if(result.status === 'rejected') {
        const names = ['fetchAPI', 'fetchNet', 'loadHistory', 'loadStats'];
        console.warn(`Falha em ${names[index]}:`, result.reason);
      }
    });

    console.log('Dashboard inicializado!');
  } catch(e) {
    console.error('Erro na inicialização do dashboard:', e);
    // Mesmo com erro, continua tentando atualizar periodicamente
  }
}

// Inicia dashboard quando página carrega completamente
// Tenta múltiplas estratégias de ativação para melhor compatibilidade
function tryInitDashboard() {
  console.log('🚀 Tentando iniciar dashboard...');
  initDashboard().then(() => {
    console.log('✅ Dashboard pronto!');
  }).catch(e => {
    console.error('❌ Erro ao inicializar:', e);
    // Retenta em 2 segundos
    setTimeout(tryInitDashboard, 2000);
  });
}

// Múltiplos gatilhos para garantir que inicializa
if(document.readyState === 'loading') {
  console.log('📄 DOM ainda carregando, aguardando...');
  document.addEventListener('DOMContentLoaded', tryInitDashboard);
  document.addEventListener('readystatechange', () => {
    if(document.readyState === 'complete') {
      console.log('✓ Document completo, inicializando...');
      tryInitDashboard();
    }
  });
} else {
  console.log('✓ DOM já pronto, inicializando agora...');
  tryInitDashboard();
}

// Fallback: garante inicialização mesmo se eventos não disparem
setTimeout(() => {
  if(!window.dashboardInitialized) {
    console.warn('⚠️ Dashboard não inicializado, forçando...');
    window.dashboardInitialized = true;
    tryInitDashboard();
  }
}, 1000);

// Configurar intervalos de atualização
setInterval(fetchAPI,2000);    // Atualiza dados a cada 2s (otimizado: reduzido de 1s)
setInterval(fetchNet,5000);    // Atualiza WiFi a cada 5s
setInterval(loadStats,10000);  // Atualiza estatísticas a cada 10s
setInterval(loadHistory,60000); // Recarrega histórico a cada 1min [com decimação de dados]
</script>
</body>
</html>
)HTML";

#endif // WEB_UI_H
