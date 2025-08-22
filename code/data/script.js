// ===== Tema claro/escuro =====
const toggle = document.getElementById('themeToggle');
toggle?.addEventListener('click', () => {
  const isDark = document.documentElement.style.colorScheme === 'dark';
  document.documentElement.style.colorScheme = isDark ? 'light' : 'dark';
});

// ===== Simulador PID 1D =====
const simCanvas = document.getElementById('simCanvas');
const simCtx = simCanvas.getContext('2d');
const plotCanvas = document.getElementById('plotCanvas');
const plotCtx = plotCanvas.getContext('2d');

const kpEl = document.getElementById('kp');
const kiEl = document.getElementById('ki');
const kdEl = document.getElementById('kd');
const kpv = document.getElementById('kpv');
const kiv = document.getElementById('kiv');
const kdv = document.getElementById('kdv');
const disturbBtn = document.getElementById('disturb');
const resetBtn = document.getElementById('reset');

let x = 0, v = 0; // posição e velocidade
let integ = 0, lastErr = 0;
const setpoint = 0; // centro
let lastTime = performance.now();

// Para o gráfico
let history = [];
const historyLength = 400; // número de pontos a guardar

function resetSimulation() {
  x = 0;
  v = 0;
  integ = 0;
  lastErr = 0;
  history = []; // Limpa o histórico do gráfico
  plotCtx.clearRect(0, 0, plotCanvas.width, plotCanvas.height);
}

function step() {
  const now = performance.now();
  const dt = Math.min(0.05, (now - lastTime) / 1000); // s, cap a 50 ms
  lastTime = now;

  const Kp = parseFloat(kpEl.value);
  const Ki = parseFloat(kiEl.value);
  const Kd = parseFloat(kdEl.value);
  kpv.textContent = Kp.toFixed(2);
  kiv.textContent = Ki.toFixed(2);
  kdv.textContent = Kd.toFixed(2);

  // Erro e PID
  const err = setpoint - x;
  integ += err * dt;
  integ = Math.max(-3, Math.min(3, integ)); // anti-windup
  const deriv = (err - lastErr) / Math.max(1e-4, dt);
  lastErr = err;
  const u = Kp * err + Ki * integ + Kd * deriv; // força de controle

  // Dinâmica simples m=1 com atrito leve
  const friction = 0.6; // amortecimento
  v += (u - friction * v) * dt;
  x += v * dt; // integra posição
  x = Math.max(-1.2, Math.min(1.2, x));

  // Adiciona ao histórico para o gráfico
  history.push(x);
  if (history.length > historyLength) {
    history.shift();
  }

  // Desenho
  drawSimulation();
  drawPlot();
  requestAnimationFrame(step);
}

function drawSimulation() {
  const w = simCanvas.width, h = simCanvas.height;
  simCtx.clearRect(0, 0, w, h);
  // Trilha
  const midY = h * 0.5;
  simCtx.lineWidth = 6;
  simCtx.strokeStyle = 'rgba(99,102,241,.35)';
  simCtx.beginPath();
  simCtx.moveTo(20, midY);
  simCtx.lineTo(w - 20, midY);
  simCtx.stroke();
  // Centro
  simCtx.strokeStyle = 'rgba(37,99,235,.8)';
  simCtx.setLineDash([6, 6]);
  const cx = w / 2;
  simCtx.beginPath();
  simCtx.moveTo(cx, midY - 30);
  simCtx.lineTo(cx, midY + 30);
  simCtx.stroke();
  simCtx.setLineDash([]);
  // Bola
  const px = w / 2 + x * (w * 0.4);
  const r = 12;
  const grad = simCtx.createRadialGradient(px - 4, midY - 4, 2, px, midY, 18);
  grad.addColorStop(0, '#ffffff');
  grad.addColorStop(1, '#93c5fd');
  simCtx.fillStyle = grad;
  simCtx.strokeStyle = 'rgba(15,23,42,.3)';
  simCtx.beginPath();
  simCtx.arc(px, midY, r, 0, Math.PI * 2);
  simCtx.fill();
  simCtx.stroke();
}

function drawPlot() {
    const w = plotCanvas.width, h = plotCanvas.height;
    plotCtx.clearRect(0,0,w,h);
    const midY = h / 2;

    // Linha do setpoint (centro)
    plotCtx.strokeStyle = 'rgba(255, 82, 82, 0.5)';
    plotCtx.lineWidth = 1;
    plotCtx.setLineDash([4, 4]);
    plotCtx.beginPath();
    plotCtx.moveTo(0, midY);
    plotCtx.lineTo(w, midY);
    plotCtx.stroke();
    plotCtx.setLineDash([]);

    // Linha da posição
    plotCtx.strokeStyle = '#2563eb';
    plotCtx.lineWidth = 2;
    plotCtx.beginPath();
    history.forEach((pos, i) => {
        const plotX = (i / (historyLength - 1)) * w;
        const plotY = midY - pos * (h * 0.4); // Escala para a altura
        if (i === 0) {
            plotCtx.moveTo(plotX, plotY);
        } else {
            plotCtx.lineTo(plotX, plotY);
        }
    });
    plotCtx.stroke();
}

disturbBtn?.addEventListener('click', () => {
  v += (Math.random() > .5 ? 1 : -1) * (0.8 + Math.random() * 0.8);
});
resetBtn?.addEventListener('click', resetSimulation);
step();

// ===== Galeria local (client-side) =====
const input = document.getElementById('fileInput');
const gallery = document.getElementById('gallery');
const lightbox = document.getElementById('lightbox');
const lightImg = document.getElementById('lightImg');
const clearBtn = document.getElementById('clearGallery');

input?.addEventListener('change', (e) => {
  const files = Array.from(e.target.files || []);
  files.forEach(file => {
    if (!file.type.startsWith('image/')) return;
    const url = URL.createObjectURL(file);
    const fig = document.createElement('figure');
    const img = document.createElement('img');
    img.src = url;
    img.alt = file.name;
    img.addEventListener('click', () => {
      lightImg.src = url;
      lightbox.style.display = 'flex';
    });
    fig.appendChild(img);
    gallery.appendChild(fig);
  });
  input.value = '';
});
lightbox?.addEventListener('click', () => {
  lightbox.style.display = 'none';
  lightImg.src = '';
});
clearBtn?.addEventListener('click', () => {
  gallery.innerHTML = '';
});