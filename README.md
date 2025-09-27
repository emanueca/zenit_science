<!DOCTYPE html>
<html lang="pt-BR">
<head>
  <meta charset="UTF-8">
  <title>ZENIT — PID Balance (Gangorra)</title>
  <!-- A estilização foi removida para compatibilidade com a visualização do GitHub. 
       GitHub aplica seu próprio CSS, portanto é suficiente usar tags HTML básicas
       como h1, h2, p, pre, code e tabelas para obter um resultado agradável. -->
</head>
<body>

<h1>ZENIT — PID Balance (Gangorra)</h1>

<blockquote>
  <p><strong>Slogan:</strong> <em>Código vs. Gravidade</em></p>
  <p><strong>Resumo:</strong> Este protótipo didático estabiliza uma <strong>bolinha de pingue‑pongue</strong> em uma <strong>gangorra 1D</strong> usando <strong>Arduino UNO</strong>, um sensor de distância <strong>Sharp GP2Y0A21</strong> e um <strong>servo padrão</strong>. O Arduino mede a distância da bola, calcula o <em>erro</em> e aplica um controlador <strong>PID</strong> para inclinar a barra, mantendo a esfera em equilíbrio.</p>
</blockquote>

<hr />

<h2>1) Código principal (Arduino UNO)</h2>

<p>O código a seguir é baseado no tutorial do canal <strong>ELECTRONOOBS</strong>; use‑o como referência inicial. Ajuste os ganhos <code>Kp</code>, <code>Ki</code> e <code>Kd</code>, o <code>setpoint</code> e o mapeamento conforme a sua mecânica. O firmware oficial e aprimorado que usamos no projeto está disponível em <code>assets/code/index/pid_balance</code> no repositório — esta página apresenta apenas o código de base de terceiros.</p>

<pre><code>/* PID balance code with ping pong ball and distance sensor sharp 2y0a21
 *  by ELECTRONOOBS: https://www.youtube.com/channel/UCjiVhIvGmRZixSzupD0sS9Q
 *  Tutorial: http://electronoobs.com/eng_arduino_tut100.php
 *  Code: http://electronoobs.com/eng_arduino_tut100_code1.php
 *  Scheamtic: http://electronoobs.com/eng_arduino_tut100_sch1.php
 *  3D parts: http://electronoobs.com/eng_arduino_tut100_stl1.php   
 */
#include &lt;Wire.h&gt;
#include &lt;Servo.h&gt;

///////////////////////Inputs/outputs///////////////////////
int Analog_in = A0;
Servo myservo;  // create servo object to control a servo, later attatched to D9
///////////////////////////////////////////////////////

////////////////////////Variables///////////////////////
int Read = 0;
float distance = 0.0;
float elapsedTime, time, timePrev;        //Variables for time control
float distance_previous_error, distance_error;
int period = 50;  //Refresh rate period of the loop is 50ms
///////////////////////////////////////////////////////

///////////////////PID constants///////////////////////
float kp=8; //Mine was 8
float ki=0.2; //Mine was 0.2
float kd=3100; //Mine was 3100
float distance_setpoint = 21;           //Should be the distance from sensor to the middle of the bar in mm
float PID_p, PID_i, PID_d, PID_total;
///////////////////////////////////////////////////////

void setup() {
  Serial.begin(9600);
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object
  myservo.write(125); //Put the servo at angle 125, so the balance is in the middle
  pinMode(Analog_in,INPUT);
  time = millis();
}

void loop() {
  if (millis() > time+period)
  {
    time = millis();
    distance = get_dist(100);
    distance_error = distance_setpoint - distance;
    PID_p = kp * distance_error;
    float dist_difference = distance_error - distance_previous_error;
    PID_d = kd*((distance_error - distance_previous_error)/period);

    if(-3 &lt; distance_error && distance_error &lt; 3)
    {
      PID_i = PID_i + (ki * distance_error);
    }
    else
    {
      PID_i = 0;
    }

    PID_total = PID_p + PID_i + PID_d;
    PID_total = map(PID_total, -150, 150, 0, 150);

    if(PID_total &lt; 20){PID_total = 20;}
    if(PID_total &gt; 160) {PID_total = 160; }

    myservo.write(PID_total+30);
    distance_previous_error = distance_error;
  }
}

float get_dist(int n)
{
  long sum=0;
  for(int i=0;i&lt;n;i++)
  {
    sum=sum+analogRead(Analog_in);
  }
  float adc=sum/n;
  float distance_cm = 17569.7 * pow(adc, -1.2062);
  return(distance_cm);
}
</code></pre>

<p><strong>Dica:</strong> se a gangorra se inclinar para o lado errado, inverta o sinal (troque <code>distance_setpoint - distance</code> por <code>distance - distance_setpoint</code>) ou ajuste o lado mecânico do servo.</p>

<hr />

<h2>2) Materiais (BOM) — incluindo as peças 3D e opcionais</h2>

<p>Além dos itens essenciais, listamos algumas peças opcionais que podem aprimorar a experiência, como fonte de 7 V (para motores mais robustos), <em>buzzer</em> e <em>LED</em> com resistores, e um capacitor extra para filtrar o sensor.</p>

<table>
  <thead>
    <tr><th>Grupo</th><th>Item</th><th>Qtd.</th><th>Observações</th></tr>
  </thead>
  <tbody>
    <tr><td><strong>Controle</strong></td><td>Arduino <strong>UNO</strong> (ou compatível)</td><td>1</td><td>16 MHz; USB para programação</td></tr>
    <tr><td><strong>Sensor</strong></td><td><strong>Sharp GP2Y0A21</strong> (10–80 cm)</td><td>1</td><td>Apontado para a bola; fixar firme; cabo 3 vias</td></tr>
    <tr><td><strong>Atuação</strong></td><td><strong>Servo</strong> padrão (ex.: Futaba <strong>S3003</strong>)</td><td>1</td><td>4,8–6 V; torque típico 3–4 kg·cm (para 1D é suficiente)</td></tr>
    <tr><td rowspan="3"><strong>Energia</strong></td><td><strong>Fonte 5 V</strong> externa (≥ 2 A)</td><td>1</td><td>Não alimente servo pela USB</td></tr>
    <tr><td><strong>Fonte 7 V</strong> (opcional)</td><td>1</td><td>Para quem deseja explorar servos de maior torque; use com regulador adequado</td></tr>
    <tr><td>Capacitor <strong>100–470 µF</strong> (5 V)</td><td>1</td><td>Próximo ao servo (anti‑ruído) e outro opcional junto ao sensor para filtrar picos</td></tr>
    <tr><td rowspan="3"><strong>Conexões</strong></td><td>Protoboard + jumpers</td><td>—</td><td>GND comum entre Arduino e fonte dos servos; inclua resistores (220–330 Ω) para LED e buzzer</td></tr>
    <tr><td><strong>LED</strong> (opcional)</td><td>1</td><td>Indicador visual de centragem; usar com resistor</td></tr>
    <tr><td><strong>Buzzer</strong> (opcional)</td><td>1</td><td>Alarme sonoro para quando a bola estiver equilibrada; usar com resistor</td></tr>
    <tr><td><strong>Bola</strong></td><td>Bolinha de pingue‑pongue</td><td>1</td><td>Superfície fosca ajuda o IR</td></tr>
    <tr><td rowspan="9"><strong>Estrutura (3D / laser)</strong></td><td><strong>Base inferior</strong></td><td>1</td><td>Chapa 200×200 mm (MDF 3 mm / acrílico)</td></tr>
    <tr><td><strong>Suporte do servo</strong> (3D)</td><td>1</td><td>Suporte em L com furação padrão do servo</td></tr>
    <tr><td><strong>Braço / berço da gangorra</strong> (3D)</td><td>1</td><td>Peça central que apoia a barra; encaixe no pivô</td></tr>
    <tr><td><strong>Pivô / fulcro</strong> (3D ou parafuso + bucha)</td><td>1</td><td>Pode usar parafuso M4 + porcas/arruelas; opcionalmente rolamento 608</td></tr>
    <tr><td><strong>Barra da gangorra</strong></td><td>1</td><td>200–250 mm de comprimento; canal raso central para a bola</td></tr>
    <tr><td><strong>Suporte do Sharp</strong> (3D)</td><td>1</td><td>Mantém o sensor apontado e na altura correta</td></tr>
    <tr><td><strong>Adaptador para horn</strong> (3D)</td><td>1</td><td>Acopla o horn do servo à barra (ou bieleta curta)</td></tr>
    <tr><td><strong>Espaçadores / colunas</strong> (3D)</td><td>4</td><td>Fixação entre base e nível da barra</td></tr>
    <tr><td><strong>Abas / fixadores</strong> (3D)</td><td>2–4</td><td>Para prender cabos/sensor na base</td></tr>
    <tr><td><strong>Protoboard</strong> (opcional)</td><td>1</td><td>Facilita a montagem provisória; inclua resistores para LED e buzzer</td></tr>
  </tbody>
</table>

<p>As peças <em>impressas em 3D</em> correspondem ao que se vê no protótipo: suporte do servo, berço/pivô da barra, suporte do sensor, adaptador para horn e espaçadores. Todos os arquivos STL estão em <code>assets/files/3dFiles</code> do repositório.</p>

<hr />

<h2>3) Ligações elétricas (UNO)</h2>

<p>As ligações básicas permanecem as mesmas; caso opte por incluir LED e buzzer, conecte‑os aos pinos indicados no firmware oficial (<code>ledPin = 6</code>, <code>buzzerPin = 3</code>) com resistores em série (~220 Ω para o LED e ~100 Ω para o buzzer). Não se esqueça de usar capacitores próximos ao servo e ao sensor para filtrar ruídos.</p>

<h3>Sensor Sharp GP2Y0A21</h3>
<ul>
  <li><strong>Vcc</strong> → 5 V</li>
  <li><strong>GND</strong> → GND</li>
  <li><strong>OUT</strong> → A0 (igual ao <code>Analog_in</code> no código)</li>
</ul>

<h3>Servo (S3003)</h3>
<ul>
  <li><strong>Vermelho</strong> → 5 V da fonte externa</li>
  <li><strong>Marrom/Preto</strong> → GND (comum ao GND do Arduino)</li>
  <li><strong>Laranja/Branco</strong> (sinal) → D9</li>
</ul>

<!-- Seção de materiais recomendados e comparações -->
<hr />

<h2>13) Materiais recomendados e comparações</h2>

<p>Durante o desenvolvimento do projeto, testamos diferentes variantes de hardware. A tabela a seguir resume opções de baixo custo, as peças que utilizamos e alternativas recomendadas para maior precisão ou robustez. Os valores são aproximados e servem apenas para comparação.</p>

<table>
  <thead>
    <tr>
      <th>Componente</th>
      <th>Básico (baixo custo)</th>
      <th>Usado no protótipo</th>
      <th>Recomendado (alto desempenho)</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>Sensor de distância</td>
      <td>Sharp 2Y0A21 (10–80 cm) — ruído elevado e tendência a saturar</td>
      <td>Sharp GP2Y0A21YK0F — nosso sensor, moderada precisão</td>
      <td>Sensor <em>time‑of‑flight</em> (VL53L0X/VL53L1X) — melhor linearidade e menos ruído</td>
    </tr>
    <tr>
      <td>Servo</td>
      <td>Servo analógico padrão (Futaba S3003) — engrenagens plásticas</td>
      <td>Servo digital MG995 — torque maior e resposta mais rápida</td>
      <td>Servo digital de engrenagem metálica (MG996R ou DS3218) — torque e precisão elevados</td>
    </tr>
    <tr>
      <td>Placa microcontrolador</td>
      <td>UNO clone genérico — baixo custo, confiável</td>
      <td>Arduino UNO original — usado no projeto, suporte oficial</td>
      <td>Teensy 4.x / STM32 — processadores mais rápidos e mais portas</td>
    </tr>
    <tr>
      <td>Fonte de alimentação</td>
      <td>5 V / 1 A — suficiente apenas para servo leve</td>
      <td>5 V / 2 A — utilizado no protótipo, alimenta servo padrão com folga</td>
      <td>5 V–7 V / 3 A com regulador DC‑DC — para servos de torque elevado e múltiplos periféricos</td>
    </tr>
    <tr>
      <td>Estrutura</td>
      <td>MDF 3 mm cortado a laser — economia, mas menos durável</td>
      <td>Impressão 3D PLA — nosso compromisso entre custo e personalização</td>
      <td>Acrílico ou alumínio usinado — maior rigidez e estética profissional</td>
    </tr>
  </tbody>
</table>

<p>Escolher componentes mais avançados pode reduzir ruídos, aumentar a estabilidade do controle e simplificar a calibração. Entretanto, o custo sobe e a montagem pode exigir conhecimentos adicionais.</p>

<!-- Seção de datasheets -->
<hr />

<h2>14) Datasheets</h2>

<p>Os <em>datasheets</em> de todos os componentes utilizados (sensor de distância, servos, microcontroladores e outros periféricos) estão disponíveis na raiz do repositório em <code>zenit_science/datasheets</code>. Consulte esses documentos para detalhes técnicos, pinagens, curvas de resposta e limites de operação. Ter acesso aos datasheets facilita a compreensão do hardware e auxilia na resolução de problemas.</p>

<p><strong>Crítico:</strong> GND deve ser comum entre a fonte dos servos e o Arduino. Sem isso, o servo poderá se comportar de forma errática ou nem se mover.</p>

<hr />

<h2>4) Montagem mecânica (1D)</h2>

<ol>
  <li><strong>Base</strong> fixa (MDF/acrílico).</li>
  <li><strong>Suporte do servo</strong> centralizado, horn alinhado ao centro da barra.</li>
  <li><strong>Pivô/fulcro</strong> posicionado no centro da barra (reduz esforço do servo).</li>
  <li><strong>Bola</strong> rola sobre a barra; o sensor Sharp deve ficar lateralmente apontando para a esfera (ou para a região central onde ela passa).</li>
  <li>Curso mecânico pequeno (±10–15°) evita saturação e melhora a estabilidade.</li>
</ol>

<hr />

<h2>5) Tutorial interativo e calibrações</h2>

<p>Este projeto vai além do sketch básico. Dentro do diretório <code>assets</code> do repositório GitHub você encontra todos os arquivos necessários para experimentar, calibrar e entender o sistema:</p>

<ul>
  <li><strong>Firmware oficial:</strong> em <code>assets/code/index/pid_balance</code> está a versão que utilizamos na exposição. Ela inclui melhorias como filtro exponencial, zona morta (“deadband”), beep/LED quando a bola está estabilizada e histerese no servo. Leia o código e adapte os parâmetros conforme a sua mecânica.</li>
  <li><strong>Jogo PID:</strong> em <code>assets/code/game/index</code> há um pequeno jogo/programa interativo em que o usuário pode “brincar” de equilibrar a bola ajustando os ganhos PID. Ele registra placar e sons para tornar o experimento mais lúdico; embora seja um protótipo e não funcione tão bem quanto o firmware oficial, é útil para aprender como as constantes influenciam o comportamento.</li>
  <li><strong>Calibrações:</strong> nas pastas <code>assets/calibra_buzzer</code>, <code>assets/calibra_sensor</code> e <code>assets/calibra_servo</code> há sketches simples para testar o buzzer/LED, estimar a curva de distância do sensor e determinar o ângulo central do servo. Esses arquivos não são 100 % precisos, mas servem como base de calibração — ajuste os valores conforme o seu hardware e anote os resultados para usar no firmware principal.</li>
  <li><strong>Arquivos de impressão 3D e mídia:</strong> em <code>assets/files/3dFiles</code> estão os STL das peças do protótipo. O subdiretório <code>assets/files</code> também hospedará futuramente um <em>banner</em> e slides de apresentação para auxiliar na explicação do projeto.</li>
</ul>

<p>Para utilizar os sketches de calibração:</p>
<ol>
  <li>Abra o arquivo adequado (<code>buzzer_centro</code>, <code>sensor_curve</code> ou <code>servo_zero</code>, por exemplo) na IDE Arduino.</li>
  <li>Carregue no Arduino e siga as instruções do Serial Monitor para identificar o ponto em que o buzzer/LED deve acender, a curva real do sensor de distância e o ângulo central do servo.</li>
  <li>Anote os valores medidos e atualize os parâmetros no firmware oficial.</li>
</ol>

<hr />

<h2>6) Calibração do Sharp GP2Y0A21 (10–80 cm)</h2>

<p>A faixa doce para este sensor no contexto do projeto é entre <strong>15 e 40 cm</strong>. Faça uma curva experimental: marque 15, 20, 25, 30 e 35 cm; leia <code>analogRead()</code> e confira a curva <code>dist = 17569.7 × adc<sup>−1.2062</sup></code>. Se a leitura saturar muito perto ou muito longe, realoque o sensor ou ajuste o <code>distance_setpoint</code>. Luz ambiente e superfícies brilhantes podem atrapalhar — use anteparo (tubinho preto) no sensor e uma bola fosca.</p>

<hr />

<h2>7) <em>Tuning</em> do PID (prático)</h2>

<ol>
  <li>Comece com <code>Ki = 0</code>, <code>Kd = 0</code>; suba o <code>Kp</code> até começar a oscilar (margem).</li>
  <li>Adicione <code>Kd</code> para amortecer, reduzindo o <em>overshoot</em>.</li>
  <li>Introduza <code>Ki</code> aos poucos para remover o erro em regime; não exagere para não “embalar”.</li>
  <li>Use o <em>Serial Plotter</em> da IDE para enxergar o erro e a resposta.</li>
  <li>Dê pequenos toques na barra/bola e observe o tempo de acomodação.</li>
</ol>

<hr />

<h2>8) Métricas que impressionam a banca e observações sobre o sensor</h2>

<p>Algumas grandezas úteis para demonstrar o desempenho:</p>

<ul>
  <li><strong>Overshoot (%)</strong> — pico inicial acima do setpoint.</li>
  <li><strong>Tempo de acomodação (s)</strong> — tempo até ficar estável (±5 %).</li>
  <li><strong>Erro estacionário</strong> — desvio final médio.</li>
  <li><strong>Taxa do loop (Hz)</strong> — com <code>period = 50 ms</code> a taxa é ≈ 20 Hz, mas pode ser reduzida para 20–30 ms se a mecânica permitir.</li>
  <li><strong>Filtros</strong> — o firmware oficial emprega filtro exponencial na leitura, histerese no servo e zona morta para evitar jitter.</li>
</ul>

<p><strong>Atenção ao comportamento do sensor:</strong> o Sharp GP2Y0A21 é modulado internamente. Como explica o artigo da Robot Research Lab, a modulação produz pulsos na saída que podem gerar ruído inesperado; se você ler o valor enquanto o sensor está atualizando, pode capturar picos de tensão até <em>1 V</em> acima do valor real, dificultando leituras consistentes【910885255550240†L108-L121】. Além disso, o sensor passa 32 ms transmitindo e apenas ~9 ms descansando, e não há maneira de saber exatamente quando o período de repouso ocorre【910885255550240†L108-L131】. Quando a bola fica fora da faixa de detecção (longe do sensor), esses picos fazem a saída “ficar louca” — o famoso <em>glitch de modulação</em>. Adicionamos filtros e limitação de distância no firmware oficial para mitigar esse efeito, mas tenha ciência de que o bug é inerente à física do sensor.</p>

<hr />

<h2>9) Roadmap (opcional)</h2>

<ul>
  <li>Expandir para <strong>2D</strong> (plataforma com 2 servos) usando 4 sensores Sharp ou touch resistivo de 4 fios.</li>
  <li>Implementar telemetria (plotagem x tempo) via Serial/Processing/Excel ou web.</li>
  <li>Modo competição: medir o tempo em que a bola permanece no centro ou em checkpoints definidos.</li>
</ul>

<hr />

<h2>10) Como rodar</h2>

<ol>
  <li>Abra a IDE Arduino e selecione <strong>Arduino UNO</strong> como placa.</li>
  <li>Se quiser testar o código base, cole o sketch apresentado no item 1. Para melhor desempenho, abra o arquivo <code>pid_balance</code> em <code>assets/code/index</code> e carregue‑o no Arduino.</li>
  <li>Ligue o hardware: servo alimentado por fonte 5 V externa, LED e buzzer com resistores, GND comum.</li>
  <li>Faça a calibração do sensor e ajuste <code>distance_setpoint</code>; se necessário, use os sketches de calibração para estimar os valores corretos.</li>
  <li>Tune os ganhos <code>Kp</code>, <code>Ki</code> e <code>Kd</code> até estabilizar.</li>
  <li>Caso deseje, experimente o “jogo” PID em <code>assets/code/game/index</code>.</li>
</ol>

<hr />

<h2>11) Segurança</h2>

<ul>
  <li>Nunca alimente servos pesados pela porta USB.</li>
  <li>Use capacitores próximos ao servo e ao sensor para atenuar ruído e picos.</li>
  <li>Instale uma chave liga/desliga e mantenha os cabos bem fixos.</li>
</ul>

<hr />

<h2>12) Licença &amp; Créditos</h2>

<p>Este projeto é de uso <strong>educacional</strong>. O sketch original foi criado por <strong>ELECTRONOOBS</strong> (links no cabeçalho do código). A equipe <strong>ZENIT (IFFar)</strong> adaptou, aprimorou o firmware e desenvolveu toda a documentação, peças 3D e material de apoio.</p>

<p>Quem quiser replicar ou derivar este projeto está livre para fazê‑lo, desde que:</p>
<ul>
  <li>Mantenha os créditos a <strong>ELECTRONOOBS</strong> pelo código base e à equipe <strong>ZENIT (IFFar)</strong> pelas adaptações.</li>
  <li>Cite o repositório oficial (<a href="https://github.com/emanueca/zenit_science">github.com/emanueca/zenit_science</a>) em qualquer material de divulgação.</li>
  <li>Não remova as notas de fonte e créditos presentes no código.</li>
</ul>

</body>
</html>
