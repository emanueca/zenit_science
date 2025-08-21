# ZENIT — PID Balance

> **Slogan**: _Código vs. Gravidade._  
> **Resumo**: Sistema didático de **controle PID** (Proporcional–Integral–Derivativo) que mantém uma **bolinha** estável sobre uma base inclinável. Integra **física** (gravidade, dinâmica, atrito) e **programação** (amostragem, filtragem, controle) com **Arduino**, **sensores infravermelhos Sharp** e **servos**.

---

## 1) Objetivos
- Mostrar, de forma interativa, como um **PID** estabiliza um sistema **instável**.
- Medir **posição** da bola, calcular **erro** e atuar nos **servos** para corrigir a inclinação.
- Exibir **overshoot**, **tempo de acomodação** e **erro estacionário**.

## 2) Protótipos
- **V1 — 1D (Gangorra)**: 2 sensores Sharp (extremidades) → 1 servo (ex.: Futaba S3003).
- **V2 — 2D (Plataforma)**: 4 sensores Sharp (cantos) → 2 servos (pitch/roll).

> Para feira, a V1 já impressiona e facilita o _tuning_. A V2 é mais "wow" e exige mais ajuste.

## 3) Componentes (BOM)
| Item | Qtd. | Notas |
| --- | ---: | --- |
| Arduino Uno (ou compatível) | 1 | Controle em tempo real |
| Sensores Sharp IR Infravermelho (20-80cm) (GP2Y0A21 / GP2Y0A02) | 2 (V1) / 4 (V2) | Posição relativa |
| Servo Futaba S3003 | 1 (V1) / 2 (V2) | Inclinação da base |
| Fonte 5V externa | 1 | ≥ 1A (V1) / 2–3A (V2); **recom.: UBEC 5V 3A** |
| Protoboard e jumpers | — | Conexões |
| Capacitores e resustores | 1–5 | Perto do(s) servo(s) e ao lado do acrílico |
| Base de acrílico (ou outro) + dobradiças | — | Estrutura |
| Bola de pingue-pongue | 1 | Leve, boa resposta |

## 4) Ligações elétricas (V1 — 1D)
**Sensores Sharp (2x)**
- VCC → 5V  
- GND → GND  
- OUT → **A0 (esquerda)**, **A1 (direita)**

**Servo Futaba S3003 (1x)**
- Vermelho → **5V da fonte externa**  
- Marrom/Preto → **GND comum**  
- Laranja/Branco → **D9** (sinal PWM)

> **Importante**: alimente **servos** com **fonte externa**; una os **GNDs** (Arduino e fonte) e coloque **100–470 µF** entre 5V e GND perto do(s) servo(s).

## 5) Arquitetura de controle
```
Sensores (tensão analógica) → Leitura (ADC) → Filtragem → Estimativa de posição
→ Erro (setpoint - posição) → PID → Ângulo(s) do(s) servo(s) → Inclinação da base
→ Bola move → novos sensores → (loop a cada Δt)
```

**Estimativa de posição (V1)**  
Use a **diferença normalizada**:
```
pos ≈ (Right - Left) / (Right + Left)   // ~intervalo [-1, +1]
```
Aplique média móvel/mediana para reduzir ruído.

## 6) Código de exemplo (V1 — 1D, Arduino)
```cpp
#include <Servo.h>

// Pinos
const int PIN_LEFT  = A0;
const int PIN_RIGHT = A1;
const int PIN_SERVO = 9;

// PID
double Kp = 22.0, Ki = 2.0, Kd = 4.0;  // ponto de partida; ajuste em teste
double setpoint = 0.0;                 // centro
double integral = 0.0, lastError = 0.0;
unsigned long lastMicros = 0;

// Servo
Servo servo;
const int SERVO_CENTER = 90;    // neutro do mecanismo
const int SERVO_RANGE  = 20;    // ± amplitude (graus)

// Filtro exponencial simples
static double Lf=0, Rf=0;
static double ema(double prev, double value, double alpha){
  return alpha*value + (1.0 - alpha)*prev;
}

void setup(){
  servo.attach(PIN_SERVO);
  servo.write(SERVO_CENTER);
  lastMicros = micros();
  Serial.begin(115200);
}

void loop(){
  // dt
  unsigned long now = micros();
  double dt = (now - lastMicros) / 1e6; // s
  if(dt < 0.005) return;                // ~200 Hz máx
  lastMicros = now;

  // Leitura e filtragem
  int L = analogRead(PIN_LEFT);
  int R = analogRead(PIN_RIGHT);
  Lf = ema(Lf, (double)L, 0.3);
  Rf = ema(Rf, (double)R, 0.3);

  // Posição normalizada
  double sum = max(1.0, Lf + Rf);
  double pos = (Rf - Lf) / sum;   // ~[-1,+1]
  double error = setpoint - pos;

  // PID
  integral += error * dt;
  integral = constrain(integral, -0.5, 0.5); // anti-windup simples
  double derivative = (error - lastError) / dt;
  double u = Kp*error + Ki*integral + Kd*derivative;
  lastError = error;

  // Saída
  int angle = SERVO_CENTER + (int)constrain(u * SERVO_RANGE, -SERVO_RANGE, SERVO_RANGE);
  servo.write(angle);

  // Debug
  Serial.print("pos:"); Serial.print(pos, 3);
  Serial.print(", err:"); Serial.print(error, 3);
  Serial.print(", u:"); Serial.print(u, 3);
  Serial.print(", ang:"); Serial.println(angle);
}
```
> **Se inverter** (piorar o erro), troque o sinal de `pos` ou inverta `Kp` (ou troque LEFT/RIGHT).

## 7) Afinar o PID (_tuning_)
1. **Comece com Ki = 0, Kd = 0** e **aumente Kp** até oscilar ao redor do centro.  
2. **Adicione Kd** para amortecer (reduz overshoot).  
3. **Adicione Ki** para remover o erro em regime (devagar para não oscilar).  
4. Use o **Serial Plotter** para ver `pos`, `error`, `u` e medir **tempo de acomodação**.  
5. Dê pequenos toques na bola e observe a recuperação; ajuste **Kp/Kd**.

## 8) Como rodar
1. Instale **Arduino IDE** (a lib `Servo.h` já vem).  
2. Carregue o código no **Uno**.  
3. Alimente **servos** com **fonte 5V externa** e **GND comum**.  
4. Abra o **Serial Plotter** (115200 baud).  
5. Ajuste **Kp/Ki/Kd** e recompile.

## 9) Dicas mecânicas
- **Fricção baixa** e **plataforma rígida** melhoram a resposta.  
- Use **curso angular pequeno** (±10–20°); grandes inclinações saturam.  
- Centralize pivô/servos para reduzir esforço.  
- **S3003** aguenta V1; para V2, avalie **MG996R** (mais torque).

## 10) Segurança
- **Não** alimente servos pesados pela USB.  
- Coloque **capacitor** perto do(s) servo(s).  
- Prenda fios, isole contatos, tenha **chave liga/desliga** (e **porta-fusível** se usar baterias).

## 11) Métricas
- **Overshoot (%):** quanto passa do centro.  
- **Tempo de acomodação (s):** até estabilizar (±5%).  
- **Erro estacionário:** desvio final.  
- **Frequência de amostragem (Hz)** e **latência** do loop.

## 12) Estrutura de pastas (sugestão)
```
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
```

## 13) FAQ
<details>
<summary><strong>Posso usar só 1 sensor?</strong></summary>
Funciona, mas com 2 (ou 4) a posição fica mais robusta.
</details>

<details>
<summary><strong>Preciso converter tensão em distância exata?</strong></summary>
Para controle relativo em 1D, a <em>diferença normalizada</em> já funciona muito bem.
</details>

<details>
<summary><strong>O servo treme parado.</strong></summary>
Reduza <strong>Kp</strong> ou aplique uma <em>zona morta</em> (deadband) em torno do zero.
</details>

## 14) Licença
Distribuído sob a **MIT License**.

## 15) Créditos
- **Autor:** Emanuel (IFFar)  
- **Projeto e software:** ZENIT — PID Balance  
- **Agradecimentos:** colegas, orientação e banca avaliadora

## 16) Pitch (30–40 s)
> “O ZENIT conecta **física** e **programação**: medimos a posição da bola com sensores IR, rodamos um **PID** em tempo real no Arduino e inclinamos a base com servo(s) para compensar a **gravidade**. Otimizamos **Kp/Ki/Kd** para reduzir **overshoot** e **tempo de acomodação**, como fazem **drones** e **robôs**. A demonstração mostra a recuperação após perturbações, provando controle **algorítmico** do equilíbrio.”

---

### Como usar este arquivo
- Salve este conteúdo como **`README.md`** na raiz do repositório.  
- O GitHub renderiza **Markdown + HTML básico** (sem `<style>`/CSS).  
- Imagens opcionais podem ser vinculadas: `![esquema](wiring/esquema_v1.png)`.
