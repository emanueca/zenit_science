<!DOCTYPE html>
<html lang="pt-br">
<head>
  <meta charset="utf-8" />
  <meta name="viewport" content="width=device-width, initial-scale=1" />
  <title>ZENIT — PID Balance | Leia‑me</title>
  <style>
    :root {
      --bg: #f3f8ff;
      --card: #ffffff;
      --primary: #2563eb; /* azul */
      --primary-600: #1d4ed8;
      --accent: #60a5fa; /* azul claro */
      --text: #0f172a; /* slate-900 */
      --muted: #475569; /* slate-600 */
      --border: #e2e8f0; /* slate-200 */
      --success: #16a34a;
      --warning: #d97706;
      --danger: #dc2626;
      --radius: 16px;
      --shadow: 0 10px 30px rgba(2, 6, 23, .08);
      --mono: ui-monospace, SFMono-Regular, Menlo, Monaco, Consolas, "Liberation Mono", "Courier New", monospace;
    }
    * { box-sizing: border-box; }
    html, body { height: 100%; }
    body {
      margin: 0;
      font-family: Inter, system-ui, -apple-system, Segoe UI, Roboto, Ubuntu, Cantarell, "Helvetica Neue", Arial, "Noto Sans", sans-serif;
      color: var(--text);
      background: radial-gradient(1200px 800px at 20% -10%, #e0f2fe 0%, transparent 60%),
                  radial-gradient(1000px 700px at 100% 0%, #dbeafe 0%, transparent 65%),
                  var(--bg);
      line-height: 1.6;
    }
    .wrap { max-width: 1100px; margin: 40px auto; padding: 0 20px; }
    .card { background: var(--card); border: 1px solid var(--border); border-radius: var(--radius); box-shadow: var(--shadow); overflow: hidden; }
    header.hero { padding: 28px 28px 22px; background: linear-gradient(180deg, #eff6ff 0%, #fff 60%); border-bottom: 1px solid var(--border); }
    .title { display: flex; align-items: center; gap: 16px; flex-wrap: wrap; }
    .logo { width: 44px; height: 44px; border-radius: 12px; background: linear-gradient(135deg, var(--primary), var(--accent)); box-shadow: 0 8px 20px rgba(37, 99, 235, .25); }
    h1 { margin: 0; font-size: 28px; letter-spacing: .2px; }
    .tagline { margin-top: 6px; color: var(--muted); font-weight: 500; }

    nav.toc { display: flex; gap: 12px; flex-wrap: wrap; margin-top: 16px; }
    nav.toc a { text-decoration: none; color: var(--primary-600); background: #eef2ff; padding: 8px 12px; border-radius: 999px; border: 1px solid #e0e7ff; font-weight: 600; font-size: 14px; }
    nav.toc a:hover { background: #e0e7ff; }

    .content { padding: 28px; }
    section { margin: 28px 0 36px; }
    h2 { font-size: 22px; margin: 0 0 12px; }
    h3 { font-size: 18px; margin: 18px 0 8px; }
    p { margin: 10px 0; }
    .muted { color: var(--muted); }

    .grid { display: grid; gap: 16px; }
    .grid-2 { grid-template-columns: repeat(auto-fit, minmax(260px, 1fr)); }
    .grid-3 { grid-template-columns: repeat(auto-fit, minmax(220px, 1fr)); }

    .note { padding: 12px 14px; border-radius: 12px; font-size: 14px; border: 1px solid var(--border); }
    .note.success { background: #f0fdf4; border-color: #dcfce7; }
    .note.warn { background: #fffbeb; border-color: #fef3c7; }
    .note.danger { background: #fef2f2; border-color: #fee2e2; }

    ul, ol { padding-left: 20px; }
    code, pre { font-family: var(--mono); }
    code.inline { background: #f1f5f9; padding: 2px 6px; border-radius: 6px; border: 1px solid var(--border); }

    pre {
      background: #0b1220; color: #e2e8f0; padding: 16px; border-radius: 12px; overflow: auto;
      border: 1px solid #0f172a; box-shadow: inset 0 0 0 1px rgba(255,255,255,.03);
    }
    pre .cm-key { color: #93c5fd; }
    pre .cm-str { color: #86efac; }
    pre .cm-num { color: #fca5a5; }
    pre .cm-com { color: #94a3b8; font-style: italic; }

    .kbd { font-family: var(--mono); padding: 2px 8px; border: 1px solid var(--border); border-bottom-width: 2px; border-radius: 8px; background: #f8fafc; font-size: 12px; }

    .table { width: 100%; border-collapse: collapse; font-size: 14px; }
    .table th, .table td { border: 1px solid var(--border); padding: 10px 12px; text-align: left; }
    .table th { background: #f8fafc; }

    .footer { padding: 18px 28px; border-top: 1px solid var(--border); background: #fafafa; font-size: 14px; color: var(--muted); display: flex; justify-content: space-between; gap: 8px; flex-wrap: wrap; }

    .chips { display: flex; gap: 8px; flex-wrap: wrap; }
    .chip { background: #ecfeff; color: #155e75; border: 1px solid #cffafe; padding: 4px 10px; border-radius: 999px; font-size: 12px; font-weight: 600; }

    @media print {
      body { background: #fff; }
      .card { box-shadow: none; border: none; }
      nav.toc { display: none; }
      .footer { display: none; }
    }
  </style>
</head>
<body>
  <div class="wrap">
    <div class="card">
      <header class="hero">
        <div class="title">
          <div class="logo" aria-hidden="true"></div>
          <div>
            <h1>ZENIT — PID Balance</h1>
            <div class="tagline"><strong>Slogan:</strong> <em>“Código vs. Gravidade.”</em></div>
            <p class="muted" style="margin-top:6px; max-width: 75ch;">
              O ZENIT é um sistema didático de <strong>controle de equilíbrio</strong> que usa <strong>PID</strong> (Proporcional–Integral–Derivativo) para manter uma bolinha estável em uma base inclinável. Integra <strong>física</strong> (gravidade, dinâmica, atrito) e <strong>programação</strong> (amostragem, filtragem, controle) com <strong>Arduino</strong>, sensores infravermelhos e servos.
            </p>
            <div class="chips" aria-label="palavras-chave">
              <span class="chip">PID</span>
              <span class="chip">Controle</span>
              <span class="chip">Arduino</span>
              <span class="chip">Robótica</span>
              <span class="chip">Física aplicada</span>
            </div>
          </div>
        </div>
        <nav class="toc" aria-label="Sumário">
          <a href="#objetivos">Objetivos</a>
          <a href="#versoes">Versões</a>
          <a href="#componentes">Componentes (BOM)</a>
          <a href="#ligacoes">Ligações</a>
          <a href="#controle">Arquitetura de controle</a>
          <a href="#codigo">Código (Arduino)</a>
          <a href="#tuning">Afinar PID</a>
          <a href="#rodar">Como rodar</a>
          <a href="#mecanica">Mecânica</a>
          <a href="#seguranca">Segurança</a>
          <a href="#metricas">Métricas</a>
          <a href="#estrutura">Estrutura</a>
          <a href="#faq">FAQ</a>
          <a href="#licenca">Licença</a>
          <a href="#pitch">Pitch</a>
        </nav>
      </header>

      <div class="content">
        <section id="objetivos">
          <h2>1) Objetivos</h2>
          <ul>
            <li>Demonstrar, de forma interativa, como um <strong>controle PID</strong> estabiliza um sistema <strong>instável</strong>.</li>
            <li>Medir a <strong>posição</strong> da bola, calcular o <strong>erro</strong> e atuar nos <strong>servos</strong> para corrigir a inclinação.</li>
            <li>Exibir métricas (overshoot, tempo de acomodação, erro estacionário) e a resposta em tempo real.</li>
          </ul>
        </section>

        <section id="versoes">
          <h2>2) Versões do protótipo</h2>
          <div class="grid grid-2">
            <div class="note">
              <h3>V1 — 1D (Gangorra)</h3>
              <p>2 sensores Sharp nas extremidades → 1 servo central (ex.: Futaba S3003).</p>
            </div>
            <div class="note">
              <h3>V2 — 2D (Plataforma)</h3>
              <p>4 sensores Sharp (cantos) → 2 servos (pitch/roll).</p>
            </div>
          </div>
          <p class="muted" style="margin-top:10px;">Para feira, a V1 já impressiona e facilita a afinação. A V2 é mais “wow”, mas exige mais tuning.</p>
        </section>

        <section id="componentes">
          <h2>3) Componentes (BOM)</h2>
          <table class="table" role="table">
            <thead>
              <tr><th>Item</th><th>Qtd.</th><th>Notas</th></tr>
            </thead>
            <tbody>
              <tr><td>Arduino Uno (ou compatível)</td><td>1</td><td>Controle em tempo real</td></tr>
              <tr><td>Sensores Sharp IR analógicos (GP2Y0A21 / GP2Y0A02)</td><td>2 (V1) / 4 (V2)</td><td>Leitura de posição relativa</td></tr>
              <tr><td>Servo Futaba S3003</td><td>1 (V1) / 2 (V2)</td><td>Atuação da inclinação</td></tr>
              <tr><td>Fonte 5V externa</td><td>1</td><td>≥ 1A (V1) / 2–3A (V2) • Recom.: UBEC 5V 3A</td></tr>
              <tr><td>Protoboard, jumpers</td><td>—</td><td>Conexões</td></tr>
              <tr><td>Capacitor eletrolítico 100–470 µF</td><td>1–2</td><td>Perto do(s) servo(s)</td></tr>
              <tr><td>Base (MDF/acrílico) + dobradiças</td><td>—</td><td>Estrutura mecânica</td></tr>
              <tr><td>Bola de pingue-pongue</td><td>1</td><td>Leve, boa resposta</td></tr>
            </tbody>
          </table>
        </section>

        <section id="ligacoes">
          <h2>4) Ligações elétricas (V1 — 1D)</h2>
          <div class="grid grid-2">
            <div class="note">
              <h3>Sensores Sharp (2x)</h3>
              <ul>
                <li>VCC → 5V</li>
                <li>GND → GND</li>
                <li>OUT → A0 (esquerda), A1 (direita)</li>
              </ul>
            </div>
            <div class="note">
              <h3>Servo Futaba S3003 (1x)</h3>
              <ul>
                <li>Vermelho → <strong>5V da fonte externa</strong></li>
                <li>Marrom/Preto → <strong>GND comum</strong></li>
                <li>Laranja/Branco → <strong>D9</strong> (sinal PWM)</li>
              </ul>
            </div>
          </div>
          <div class="note warn" style="margin-top:12px;">
            <strong>Muito importante:</strong> Alimente os <em>servos</em> com <strong>fonte externa</strong> (não pela USB). Una os <strong>GNDs</strong> (Arduino e fonte) e coloque <strong>100–470 µF</strong> entre 5V e GND perto do(s) servo(s).
          </div>
        </section>

        <section id="controle">
          <h2>5) Arquitetura de controle</h2>
<pre><code>
Sensores (tensão analógica) → Leitura (ADC) → Filtragem → Estimativa de posição
→ Erro (setpoint - posição) → PID → Ângulo(s) do(s) servo(s) → Inclinação da base
→ Bola move → novos sensores → (loop a cada Δt)
</code></pre>
          <h3>Estimativa de posição (V1)</h3>
          <p>Use a <strong>diferença normalizada</strong>:
            <code class="inline">pos ≈ (Right - Left) / (Right + Left)</code> → intervalo aproximado <code class="inline">[-1, +1]</code>.
            Filtre leituras com <em>média móvel</em> ou <em>mediana</em> para reduzir ruído.
          </p>
        </section>

        <section id="codigo">
          <h2>6) Código de exemplo (V1 — 1D, Arduino)</h2>
<pre><code class="language-cpp">
<span class="cm-com">#include &lt;Servo.h&gt;</span>

<span class="cm-com">// Pinos</span>
<span class="cm-key">const</span> <span class="cm-key">int</span> PIN_LEFT  = A0;
<span class="cm-key">const</span> <span class="cm-key">int</span> PIN_RIGHT = A1;
<span class="cm-key">const</span> <span class="cm-key">int</span> PIN_SERVO = 9;

<span class="cm-com">// PID</span>
<span class="cm-key">double</span> Kp = 22.0, Ki = 2.0, Kd = 4.0;  <span class="cm-com">// inicie por aqui; afine no teste</span>
<span class="cm-key">double</span> setpoint = 0.0;                 <span class="cm-com">// centro</span>
<span class="cm-key">double</span> integral = 0.0, lastError = 0.0;
<span class="cm-key">unsigned</span> <span class="cm-key">long</span> lastMicros = 0;

<span class="cm-com">// Servo</span>
Servo servo;
<span class="cm-key">const</span> <span class="cm-key">int</span> SERVO_CENTER = 90;    <span class="cm-com">// neutro do seu mecanismo</span>
<span class="cm-key">const</span> <span class="cm-key">int</span> SERVO_RANGE  = 20;    <span class="cm-com">// ± amplitude em graus (ajuste conforme mecânica)</span>

<span class="cm-com">// Filtro simples</span>
<span class="cm-key">double</span> ema(<span class="cm-key">double</span> prev, <span class="cm-key">double</span> value, <span class="cm-key">double</span> alpha) {
  <span class="cm-key">return</span> alpha*value + (1.0 - alpha)*prev;
}
<span class="cm-key">double</span> Lf=0, Rf=0; <span class="cm-com">// leituras filtradas</span>

<span class="cm-key">void</span> setup() {
  servo.attach(PIN_SERVO);
  servo.write(SERVO_CENTER);
  <span class="cm-com">// No Uno a resolução é 10 bits por padrão</span>
  lastMicros = micros();
  Serial.begin(115200);
}

<span class="cm-key">void</span> loop() {
  <span class="cm-com">// dt</span>
  <span class="cm-key">unsigned</span> <span class="cm-key">long</span> now = micros();
  <span class="cm-key">double</span> dt = (now - lastMicros) / 1e6; <span class="cm-com">// segundos</span>
  <span class="cm-key">if</span> (dt &lt; 0.005) <span class="cm-key">return</span>; <span class="cm-com">// ~200 Hz máx</span>
  lastMicros = now;

  <span class="cm-com">// Leitura e filtragem (EMA)</span>
  <span class="cm-key">int</span> L = analogRead(PIN_LEFT);
  <span class="cm-key">int</span> R = analogRead(PIN_RIGHT);
  Lf = ema(Lf, (<span class="cm-key">double</span>)L, 0.3);
  Rf = ema(Rf, (<span class="cm-key">double</span>)R, 0.3);

  <span class="cm-com">// Posição normalizada</span>
  <span class="cm-key">double</span> sum = max(1.0, Lf + Rf);
  <span class="cm-key">double</span> pos = (Rf - Lf) / sum;    <span class="cm-com">// ~[-1,+1]</span>
  <span class="cm-key">double</span> error = setpoint - pos;

  <span class="cm-com">// PID</span>
  integral += error * dt;
  <span class="cm-com">// anti-windup</span>
  integral = constrain(integral, -0.5, 0.5);
  <span class="cm-key">double</span> derivative = (error - lastError) / dt;
  <span class="cm-key">double</span> u = Kp*error + Ki*integral + Kd*derivative;
  lastError = error;

  <span class="cm-com">// Saída para servo</span>
  <span class="cm-key">int</span> angle = SERVO_CENTER + (<span class="cm-key">int</span>)constrain(u * SERVO_RANGE, -SERVO_RANGE, SERVO_RANGE);
  servo.write(angle);

  <span class="cm-com">// Debug</span>
  Serial.print(<span class="cm-str">"pos:"</span>); Serial.print(pos, 3);
  Serial.print(<span class="cm-str">", err:"</span>); Serial.print(error, 3);
  Serial.print(<span class="cm-str">", u:"</span>); Serial.print(u, 3);
  Serial.print(<span class="cm-str">", ang:"</span>); Serial.println(angle);
}
</code></pre>
          <p class="muted"><strong>Dica:</strong> se a correção vier “ao contrário” (piorando o erro), troque o sinal de <code class="inline">pos</code> ou inverta <code class="inline">Kp</code> (ou troque LEFT/RIGHT).</p>
        </section>

        <section id="tuning">
          <h2>7) Afinar o PID (tuning)</h2>
          <ol>
            <li><strong>Comece com Ki = 0, Kd = 0</strong> e <strong>aumente Kp</strong> até o sistema oscilar ao redor do centro.</li>
            <li><strong>Adicione Kd</strong> para amortecer e reduzir overshoot.</li>
            <li><strong>Adicione Ki</strong> para eliminar o erro em regime (devagar, para não oscilar).</li>
            <li>Use o <span class="kbd">Ferramentas → Serial Plotter</span> para enxergar <em>pos</em>, <em>error</em>, <em>u</em> e medir <em>tempo de acomodação</em>.</li>
            <li>Toque levemente a bola → observe recuperação. Ajuste <strong>Kp/Kd</strong> conforme necessário.</li>
          </ol>
          <p class="muted">Alternativa: método <strong>Ziegler–Nichols</strong> (ganho crítico e período crítico), mas com bola leve a abordagem manual costuma ser mais tranquila.</p>
        </section>

        <section id="rodar">
          <h2>8) Como rodar</h2>
          <ol>
            <li>Instale <strong>Arduino IDE</strong> (a lib <code class="inline">Servo.h</code> já vem).</li>
            <li>Carregue o código no Uno.</li>
            <li>Alimente <strong>servos</strong> com <strong>fonte 5V externa</strong> e <strong>GND comum</strong>.</li>
            <li>Abra o <strong>Serial Plotter</strong> (115200 baud) para visualizar a resposta.</li>
            <li>Afinar <strong>Kp/Ki/Kd</strong> recompilando quando alterar.</li>
          </ol>
        </section>

        <section id="mecanica">
          <h2>9) Dicas mecânicas</h2>
          <ul>
            <li><strong>Fricção baixa</strong> e <strong>plataforma rígida</strong> melhoram a resposta.</li>
            <li>Use <strong>curso angular pequeno</strong> (±10–20°); grandes inclinações geram saturação.</li>
            <li>Centralize o pivô/servos para reduzir esforço.</li>
            <li><strong>S3003</strong> aguenta bem V1; para V2 avalie servos com mais torque (ex.: MG996R).</li>
          </ul>
        </section>

        <section id="seguranca">
          <h2>10) Segurança e boas práticas</h2>
          <ul>
            <li><strong>Nunca</strong> alimente servos pesados pela USB.</li>
            <li>Coloque <strong>capacitor</strong> perto do(s) servo(s).</li>
            <li>Prenda fios e isole contatos na base.</li>
            <li>Tenha <strong>chave liga/desliga</strong> e <strong>porta-fusível</strong> se usar baterias.</li>
          </ul>
        </section>

        <section id="metricas">
          <h2>11) Métricas para apresentar</h2>
          <ul>
            <li><strong>Overshoot (%):</strong> quão longe passa do centro.</li>
            <li><strong>Tempo de acomodação (s):</strong> tempo até estabilizar (±5%).</li>
            <li><strong>Erro estacionário:</strong> desvio final do setpoint.</li>
            <li><strong>Frequência de amostragem (Hz)</strong> e <strong>latência</strong> do loop.</li>
          </ul>
        </section>

        <section id="estrutura">
          <h2>12) Estrutura de pastas sugerida</h2>
<pre><code>
/ZENIT
  ├─ firmware/
  │   └─ zenit_pid_1d.ino
  ├─ cad/                  # peças da base (opcional)
  ├─ docs/
  │   ├─ cartaz_a4.pdf
  │   └─ ensaio_metricas.csv
  ├─ wiring/
  │   └─ esquema_v1.png
  └─ README.md
</code></pre>
        </section>

        <section id="faq">
          <h2>13) FAQ</h2>
          <div class="grid grid-2">
            <div class="note">
              <h3>Posso usar só 1 sensor?</h3>
              <p>Funciona, mas com 2 (ou 4) a posição fica mais robusta.</p>
            </div>
            <div class="note">
              <h3>Preciso converter tensão em distância exata?</h3>
              <p>Para controle relativo em 1D, a <em>diferença normalizada</em> já funciona muito bem.</p>
            </div>
            <div class="note">
              <h3>O servo treme parado.</h3>
              <p>Reduza <strong>Kp</strong> ou aplique <em>zona morta</em> (deadband) em torno do zero.</p>
            </div>
          </div>
        </section>

        <section id="licenca">
          <h2>14) Licença</h2>
          <p>Este projeto é disponibilizado sob a <strong>MIT License</strong>. Use, modifique e apresente na sua feira livremente, mantendo créditos.</p>
        </section>

        <section id="creditos">
          <h2>15) Créditos</h2>
          <ul>
            <li><strong>Autor:</strong> Emanuel (IFFar)</li>
            <li><strong>Projeto e software:</strong> ZENIT – PID Balance</li>
            <li><strong>Agradecimentos:</strong> colegas, orientação e banca avaliadora.</li>
          </ul>
        </section>

        <section id="pitch">
          <h2>16) Pitch (30–40 s) para a banca</h2>
          <blockquote class="note" style="border-left: 4px solid var(--primary);">
            “O ZENIT conecta <strong>física</strong> e <strong>programação</strong>: medimos a posição da bola com sensores IR, rodamos um <strong>PID</strong> em tempo real no Arduino e inclinamos a base com servo(s) para compensar a <strong>gravidade</strong>. Otimizamos <strong>Kp/Ki/Kd</strong> para reduzir <strong>overshoot</strong> e <strong>tempo de acomodação</strong>, como fazem <strong>drones</strong> e <strong>robôs</strong>. A demonstração mostra a recuperação após perturbações, provando controle <strong>algorítmico</strong> do equilíbrio.”
          </blockquote>
        </section>
      </div>

      <div class="footer">
        <span>© ZENIT — PID Balance</span>
        <span>Versão do documento: 1.0</span>
      </div>
    </div>
  </div>
</body>
</html>
