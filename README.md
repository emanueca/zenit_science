# ZENIT — PID Balance (Gangorra)

> **Slogan:** *Código vs. Gravidade*
> **Resumo:** Protótipo didático que estabiliza uma **bolinha de pingue-pongue** em uma **gangorra 1D** usando **Arduino UNO**, **sensor Sharp GP2Y0A21** (distância) e **servo**. O Arduino mede a distância, calcula o **erro** e aplica **PID** para inclinar a barra e manter a bola em equilíbrio.

---

## 1) Código principal (Arduino UNO)

> Fonte e créditos do código base: **ELECTRONOOBS** (links no cabeçalho).
> Use este sketch como referência inicial e ajuste **Kp/Ki/Kd**, **setpoint** e o **mapeamento** conforme sua mecânica.

```cpp
/* PID balance code with ping pong ball and distance sensor sharp 2y0a21
 *  by ELECTRONOOBS: https://www.youtube.com/channel/UCjiVhIvGmRZixSzupD0sS9Q
 *  Tutorial: http://electronoobs.com/eng_arduino_tut100.php
 *  Code: http://electronoobs.com/eng_arduino_tut100_code1.php
 *  Scheamtic: http://electronoobs.com/eng_arduino_tut100_sch1.php 
 *  3D parts: http://electronoobs.com/eng_arduino_tut100_stl1.php   
 */
#include <Wire.h>
#include <Servo.h>

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
  //analogReference(EXTERNAL);
  Serial.begin(9600);  
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object
  myservo.write(125); //Put the servco at angle 125, so the balance is in the middle
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
    float dist_diference = distance_error - distance_previous_error;     
    PID_d = kd*((distance_error - distance_previous_error)/period);
      
    if(-3 < distance_error && distance_error < 3)
    {
      PID_i = PID_i + (ki * distance_error);
    }
    else
    {
      PID_i = 0;
    }
  
    PID_total = PID_p + PID_i + PID_d;  
    PID_total = map(PID_total, -150, 150, 0, 150);
  
    if(PID_total < 20){PID_total = 20;}
    if(PID_total > 160) {PID_total = 160; } 
  
    myservo.write(PID_total+30);  
    distance_previous_error = distance_error;
  }
}

float get_dist(int n)
{
  long sum=0;
  for(int i=0;i<n;i++)
  {
    sum=sum+analogRead(Analog_in);
  }  
  float adc=sum/n;
  //float volts = analogRead(adc)*0.0048828125;  // value from sensor * (5/1024)
  //float volts = sum*0.003222656;  // value from sensor * (3.3/1024) EXTERNAL analog refference

  float distance_cm = 17569.7 * pow(adc, -1.2062);
  //float distance_cm = 13*pow(volts, -1); 
  return(distance_cm);
}
```

> **Dica:** se a gangorra **inclina para o lado errado**, inverta o sinal (troque `distance_setpoint - distance` por `distance - distance_setpoint`) **ou** ajuste o lado mecânico do servo.

---

## 2) Materiais (BOM) — incluindo as **peças 3D** do protótipo

| Grupo                    | Item                                     | Qtd. | Observações                                                             |
| ------------------------ | ---------------------------------------- | ---: | ----------------------------------------------------------------------- |
| **Controle**             | Arduino **UNO** (ou compatível)          |    1 | 16 MHz; USB para programação                                            |
| **Sensor**               | **Sharp GP2Y0A21** (10–80 cm)            |    1 | Apontado para a bola; fixar firme; cabo 3 vias                          |
| **Atuação**              | **Servo** padrão (ex.: Futaba **S3003**) |    1 | 4,8–6 V; torque típico 3–4 kg·cm (para 1D é suficiente)                 |
| **Energia**              | **Fonte 5 V** externa (≥ 2 A)            |    1 | Não alimente servo pela USB                                             |
|                          | Capacitor **100–470 µF** (5 V)           |    1 | Próximo ao servo (anti-ruído)                                           |
| **Conexões**             | Protoboard + jumpers                     |    — | GND comum entre Arduino e fonte dos servos                              |
| **Bola**                 | **Pingue-pongue**                        |    1 | Superfície fosca ajuda o IR                                             |
| **Estrutura (3D/laser)** | **Base inferior**                        |    1 | Chapa 200×200 mm (MDF 3 mm / acrílico)                                  |
|                          | **Suporte do servo** (3D)                |    1 | Suporte em L com furação padrão de servo                                |
|                          | **Braço/berço da gangorra** (3D)         |    1 | Peça central que apoia a barra; encaixe no pivô                         |
|                          | **Pivô/fulcro** (3D ou parafuso + bucha) |    1 | Pode usar **parafuso M4** + porcas/arruelas; opcional **rolamento 608** |
|                          | **Barra da gangorra**                    |    1 | 200–250 mm de comprimento; canal raso central para a bola               |
|                          | **Suporte do Sharp** (3D)                |    1 | Mantém o sensor apontado e na altura correta                            |
|                          | **Adaptador para horn** (3D)             |    1 | Acopla horn do servo à barra (ou bieleta curta)                         |
|                          | **Espaçadores/colunas** (3D)             |    4 | Fixação entre base e nível da barra                                     |
|                          | **Abas/fixadores** (3D)                  |  2–4 | Para prender cabos/sensor na base                                       |

> As **peças 3D** acima correspondem ao que se vê no protótipo: suporte do servo, berço/pivô da barra, suporte do sensor, adaptador pro horn e espaçadores. Se quiser, eu te entrego os **STLs** com furação já casada (servo padrão + M3/M4).

---

## 3) Ligações elétricas (UNO)

**Sharp GP2Y0A21**

* **Vcc** → **5 V**
* **GND** → **GND**
* **OUT** → **A0** (igual ao `Analog_in` no código)

**Servo (S3003)**

* **Vermelho** → **5 V da fonte externa**
* **Marrom/Preto** → **GND** (comum ao GND do Arduino)
* **Laranja/Branco** (sinal) → **D9**

> **Crítico:** **GND comum** entre a fonte dos servos e o Arduino. Sem isso, o servo “pira” ou nem mexe.

---

## 4) Montagem mecânica (1D)

1. **Base** fixa (MDF/acrílico).
2. **Suporte do servo** centralizado, horn alinhado ao **centro da barra**.
3. **Pivô/fulcro** no centro da barra (reduz esforço do servo).
4. **Bola** rola sobre a barra; **sensor Sharp** posicionado lateralmente apontando para a bola (ou para a região central onde a bola passa).
5. Curso mecânico pequeno (±10–15°) evita saturação e melhora estabilidade.

---

## 5) Calibração do **Sharp GP2Y0A21** (10–80 cm)

* **Faixa doce para o projeto:** \~**15–40 cm** da bola ao sensor.
* Faça uma **curva experimental**: marque 15, 20, 25, 30, 35 cm; leia `analogRead()` e confira a curva `dist = 17569.7 * adc^-1.2062`.
* Se a leitura **satura** muito perto/longe, **realoque o sensor** ou ajuste o **setpoint** (`distance_setpoint`).
* **Luz ambiente** e superfícies brilhantes atrapalham: use **anteparo** (tubinho preto) no sensor e **bola fosca**.

---

## 6) *Tuning* do PID (prático)

1. **Ki = 0**, **Kd = 0** → suba o **Kp** até começar a oscilar (margem).
2. Adicione **Kd** para **amortecer** (reduz overshoot).
3. Introduza **Ki** aos poucos para remover **erro em regime** (não exagere para não “embalar”).
4. Use o **Serial Plotter** para enxergar **erro** e **resposta**.
5. Dê pequenos **toques** na barra/bola e observe o **tempo de acomodação**.

---

## 7) Métricas que impressionam a banca

* **Overshoot (%)** — pico inicial acima do setpoint.
* **Tempo de acomodação (s)** — até ficar estável (±5%).
* **Erro estacionário** — desvio final médio.
* **Taxa do loop (Hz)** — `period=50 ms` ≈ **20 Hz** (pode reduzir para 20–30 ms se a mecânica permitir).

---

## 8) Roadmap (opcional)

* **2D** (plataforma com 2 servos) usando **4 Sharp** ou **touch resistivo 4-fios**.
* **Telemetria** (plot x tempo) via Serial/Processing/Excel.
* **Modo competição** (tempo no centro / checkpoints).

---

## 9) Como rodar

1. Arduino IDE → selecione **Arduino UNO**.
2. Cole o **código principal** acima.
3. Ligue o hardware (servo em fonte **5 V externa**, **GND comum**).
4. Faça a **calibração** do Sharp e ajuste `distance_setpoint`.
5. Ajuste **Kp/Ki/Kd** até estabilizar.

---

## 10) Segurança

* Nunca alimente **servo** pesado pela **USB**.
* Use **capacitor** próximo ao servo.
* **Chave liga/desliga** e cabos bem fixos.

---

## 11) Licença & Créditos

* **Código base e ideia do sketch:** ELECTRONOOBS (links no cabeçalho do código).
* **Adaptações e documentação:** Equipe **ZENIT (IFFar)**.
* **Uso educacional**.

