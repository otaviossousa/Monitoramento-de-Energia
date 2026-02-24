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
}

/* Celulares pequenos */
@media (max-width: 480px) {
  body{
    padding:12px;
  }

  h1{
    font-size:20px;
  }

  .grid{
    grid-template-columns:1fr;
  }

  .value{
    font-size:28px;
  }

  .card{
    padding:20px;
  }
}
</style>
</head>

<body>
<div class="wrap">

<div class="header">
  <h1>Painel de Energia</h1>
  <div class="pill" id="net">Conectando...</div>
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
const cvs={i:ci,p:cp,e:ce};                  // Referências aos canvas

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
Object.values(cvs).forEach(fit);
addEventListener('resize',()=>Object.values(cvs).forEach(fit));

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
 * Executa a cada 1 segundo
 */
async function fetchAPI(){
  try{
    const r=await fetch('/api',{cache:'no-store'});
    const j=await r.json();

    // Atualiza valores na tela
    i.firstChild.nodeValue=j.i.toFixed(3)+" "; bump(i);
    p.firstChild.nodeValue=j.p.toFixed(1)+" "; bump(p);
    e.firstChild.nodeValue=j.e.toFixed(3)+" "; bump(e);

    // Atualiza buffers de dados
    push(buf.i, j.i);
    push(buf.p, j.p);
    push(buf.e, Math.max(j.e, buf.e.at(-1)));

    // Redesenha os gráficos
    draw(cvs.i,buf.i,"#06b6d4");
    draw(cvs.p,buf.p,"#22c55e");
    draw(cvs.e,buf.e,"#a855f7");

    // Atualiza status
    status.textContent="OK"; status.className="ok";
    stamp();
  }catch{
    status.textContent="Error"; status.className="warn";
  }
}

/**
 * Busca informações da rede WiFi
 * Executa a cada 5 segundos
 */
async function fetchNet(){
  try{
    const r=await fetch('/net');
    const j=await r.json();
    net.textContent=`${j.ssid} @ ${j.ip}`;
  }catch{
    net.textContent="WiFi?";
  }
}

// ==========================================
// INITIALIZATION / INICIALIZAÇÃO
// ==========================================

setInterval(fetchAPI,1000);  // Atualiza dados a cada 1s
setInterval(fetchNet,5000);  // Atualiza WiFi a cada 5s
fetchAPI(); fetchNet();       // Primeira execução imediata
</script>
</body>
</html>
)HTML";

#endif // WEB_UI_H
