/*
 * ===========================================/ ZENIT - Mostra IFfar 2k25
 * ... (resto dos seus comentários iniciais) ...
 */
#include <Servo.h>

// Protótipos
float get_dist_manual(int n);
void handleSerialInput();

// === Conexões da placa ===
const int SENSOR_PIN = A0;
const int ledPin     = 6;
const int buzzerPin  = 3;
Servo myservo;

// === Servo ===
const int SERVO_CENTRO     = 82;
const int SERVO_MIN_ANGULO = 65;
const int SERVO_MAX_ANGULO = 100;

// === Controle ===
float distance = 0.0;
unsigned long time_prev;
float distance_previous_error = 0.0, distance_error = 0.0;
const int period = 50;

// === PID ===
// Coloque aqui os valores base para começar a sintonia
float kp = 7.0f; // Um bom ponto de partida
float ki = 0.3f; // Comece sempre com 0
float kd = 3900.0f; // Comece sempre com 0

float distance_setpoint = 15.5;
float PID_p, PID_i, PID_d;

// === Limites ===
const float MAX_CONTROL_CM = 20.0;
const float NO_BALL_CM = 40.0;


void setup() {
  Serial.begin(9600);
  Serial.println("Sistema PID pronto para sintonia.");
  myservo.attach(9);
  myservo.write(SERVO_CENTRO);
  pinMode(SENSOR_PIN, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  time_prev = millis();
  delay(1000);
}

void loop() {
  handleSerialInput();

  if (millis() - time_prev >= period) {
    float dt = (millis() - time_prev) / 1000.0;
    time_prev = millis();

    // 1. Leitura do sensor
    distance = get_dist_manual(50);

    // === MODO SEM BOLA ===
    if (distance >= NO_BALL_CM) {
      myservo.write(SERVO_CENTRO);
      digitalWrite(ledPin, LOW);
      noTone(buzzerPin);
      PID_i = 0;
      distance_previous_error = 0;
      Serial.println("MODO ESPERA - Sem bola detectada.");
      return;
    }

    // === SATURAÇÃO DO SENSOR ===
    float distance_corrigida = distance;
    if (distance_corrigida > MAX_CONTROL_CM) {
      distance_corrigida = MAX_CONTROL_CM;
    }

    // 2. Calcular erro
    distance_error = distance_setpoint - distance_corrigida;

    // === Zona morta no centro ===
    if (abs(distance_error) <= 0.3) {
      myservo.write(SERVO_CENTRO);
      PID_i = 0;
      distance_previous_error = 0;
      digitalWrite(ledPin, HIGH);
      tone(buzzerPin, 1500, 100);
      Serial.println("Equilibrado - Zona morta");
      return;
    }

    // --- CORREÇÃO APLICADA AQUI ---
    // 3. PID com ganho padrão (sem ganho dinâmico para a sintonia)
    PID_p = kp * distance_error;
    PID_d = kd * ((distance_error - distance_previous_error) / dt);

    // Cálculo do Integral (será usado depois da sintonia de P e D)
    if (abs(distance_error) < 3) {
      PID_i = PID_i + (ki * distance_error * dt);
    } else {
      PID_i = 0;
    }

    // 4. Soma e aplica a correção
    float correcao_pid = PID_p + PID_i + PID_d;
    int angulo_final = SERVO_CENTRO + correcao_pid;
    angulo_final = constrain(angulo_final, SERVO_MIN_ANGULO, SERVO_MAX_ANGULO);
    myservo.write(angulo_final);

    distance_previous_error = distance_error;

    // === Indicador (fora do equilíbrio) ===
    digitalWrite(ledPin, LOW);
    noTone(buzzerPin);

    // Debug
    Serial.print("Dist: "); Serial.print(distance, 1);
    Serial.print(" | Erro: "); Serial.print(distance_error, 1);
    Serial.print(" | Angulo: "); Serial.print(angulo_final);
    Serial.print(" | Kp: "); Serial.print(kp, 2);
    Serial.print(" | Kd: "); Serial.println(kd, 2);
  }
}

// === Ajuste em tempo real dos ganhos ===
void handleSerialInput() {
    // ... (sua função handleSerialInput continua a mesma) ...
  if (Serial.available() > 0) {
    char command = Serial.read();
    float value = Serial.parseFloat();

    if (command == 'P' || command == 'p') {
      kp = value;
      Serial.print(">>> Kp base = "); Serial.println(kp);
    } else if (command == 'D' || command == 'd') {
      kd = value;
      Serial.print(">>> Kd = "); Serial.println(kd);
    } else if (command == 'I' || command == 'i') {
      ki = value;
      Serial.print(">>> Ki = "); Serial.println(ki);
    }
    while (Serial.available() > 0) {
      Serial.read();
    }
  }
}

// === Função do sensor ===
float get_dist_manual(int num_leituras) {
    // ... (sua função get_dist_manual continua a mesma) ...
  long sum = 0;
  if (num_leituras <= 1) {
    sum = analogRead(SENSOR_PIN);
  } else {
    for (int i = 0; i < num_leituras; i++) {
      sum += analogRead(SENSOR_PIN);
    }
  }
  
  float adc = (float)sum / (float)num_leituras;
  
  float distance_cm = 6787.0 / (adc - 3.0) - 4.0;
  
  if (distance_cm < 0 || distance_cm > 100) {
    return NO_BALL_CM;
  }
  
  return distance_cm;
}