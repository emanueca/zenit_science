/*
 * ===========================================/ ZENIT - Mostra IFfar 2k25
 * Código com a direção de controle CORRIGIDA
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
const int SERVO_CENTRO     = 72;
const int SERVO_MIN_ANGULO = 55;
const int SERVO_MAX_ANGULO = 96;

// === Controle ===
float distance = 0.0;
unsigned long time_prev;
float distance_previous_error = 0.0, distance_error = 0.0;
const int period = 50;
float toleranciaCentro = 1.0; 

// === PID ===
// Valores RECOMENDADOS para iniciar a sintonia
float kp = 8.0f;  // <<< ALTERADO: Um ponto de partida que causa movimento
float ki = 0.0f;  // SEMPRE comece com 0
float kd = 0.0f;  // SEMPRE comece com 0

float distance_setpoint = 14.5;
float PID_p, PID_i, PID_d;

// === Limites ===
const int MAX_CONTROL_CM = 20.0;
const int NO_BALL_CM = 40.0;


void setup() {
  Serial.begin(9600);
  Serial.println("Sistema PID pronto para sintonia FINAL.");
  Serial.println("Comece enviando I0 e D0. Depois, ajuste o Kp.");
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

    distance = get_dist_manual(50);

    if (distance >= NO_BALL_CM) {
      myservo.write(SERVO_CENTRO);
      digitalWrite(ledPin, LOW);
      noTone(buzzerPin);
      PID_i = 0;
      distance_previous_error = 0;
      return;
    }

    float distance_corrigida = distance;
    if (distance_corrigida > MAX_CONTROL_CM) {
      distance_corrigida = MAX_CONTROL_CM;
    }

    distance_error = distance_setpoint - distance_corrigida;

    if (abs(distance_error) <= toleranciaCentro) { 
      myservo.write(SERVO_CENTRO);
      PID_i = 0;
      distance_previous_error = 0;
      digitalWrite(ledPin, HIGH);
      tone(buzzerPin, 1500, 100); 
      return;
    }

    PID_p = kp * distance_error;
    PID_d = kd * ((distance_error - distance_previous_error) / dt);

    if (abs(distance_error) < 3) {
      PID_i = PID_i + (ki * distance_error * dt);
    } else {
      PID_i = 0;
    }

    // 4. Soma e aplica a correção
    float correcao_pid = PID_p + PID_i + PID_d;
    // <<< ALTERAÇÃO CRÍTICA AQUI: Sinal invertido para '+' >>>
    int angulo_final = SERVO_CENTRO + correcao_pid; 
    
    angulo_final = constrain(angulo_final, SERVO_MIN_ANGULO, SERVO_MAX_ANGULO);
    myservo.write(angulo_final);

    distance_previous_error = distance_error;

    digitalWrite(ledPin, LOW);
    noTone(buzzerPin);

    // Debug
    Serial.print("Dist: "); Serial.print(distance, 1);
    Serial.print(" | Erro: "); Serial.print(distance_error, 1);
    Serial.print(" | Ang: "); Serial.print(angulo_final);
    Serial.print(" | Kp: "); Serial.print(kp, 2);
    Serial.print(" | Kd: "); Serial.print(kd, 2);
    Serial.print(" | Ki: "); Serial.print(ki, 2);
    Serial.print(" | Tol: "); Serial.println(toleranciaCentro, 2);
  }
}

// Suas outras funções (handleSerialInput e get_dist_manual) continuam as mesmas
void handleSerialInput() {
  if (Serial.available() > 0) {
    char command = Serial.read();
    float value = Serial.parseFloat();

    if (command == 'P' || command == 'p') {
      kp = value;
      Serial.print(">>> Kp = "); Serial.println(kp);
    } else if (command == 'D' || command == 'd') {
      kd = value;
      Serial.print(">>> Kd = "); Serial.println(kd);
    } else if (command == 'I' || command == 'i') {
      ki = value;
      Serial.print(">>> Ki = "); Serial.println(ki);
    } else if (command == 'T' || command == 't') { 
      toleranciaCentro = value;
      Serial.print(">>> Tolerancia = "); Serial.println(toleranciaCentro);
    }
    
    while (Serial.available() > 0) {
      Serial.read();
    }
  }
}

float get_dist_manual(int num_leituras) {
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
