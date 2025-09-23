/*
 * ===================================================================================
 * ==                   ASSISTENTE INTERATIVO DE CALIBRAÇÃO DE SERVO                  ==
 * ===================================================================================
 * GitHub: zenit_science
 *
 * DESCRIÇÃO:
 * Este script interativo guia o usuário através de um processo passo a passo para
 * encontrar os valores de calibração perfeitos (Centro, Mínimo e Máximo) para um
 * servo motor em um projeto de plataforma de equilíbrio.
 *
 * -----------------------------------------------------------------------------------
 *
 * COMO USAR (TUTORIAL):
 * 1. CONEXÕES DE HARDWARE:
 * - Servo Motor: Conecte o pino de sinal no pino digital D9.
 * - LED (Opcional): Conecte o anodo (perna longa) no pino digital D6.
 * - Buzzer (Opcional): Conecte o pino positivo no pino digital D3.
 *
 * 2. CARREGUE O CÓDIGO: Envie este script para a sua placa Arduino.
 *
 * 3. ABRA O MONITOR SERIAL: Vá em Ferramentas > Monitor Serial (Ctrl+Shift+M) e
 * configure a velocidade no canto inferior direito para "9600 baud".
 *
 * 4. SIGA AS INSTRUÇÕES: O assistente irá guiá-lo com perguntas e comandos no
 * monitor serial.
 *
 * -----------------------------------------------------------------------------------
 *
 * REFERÊNCIA DE COMANDOS (Teclas a serem pressionadas no Monitor Serial):
 *
 * --- Na Etapa de Equilíbrio (SERVO_CENTRO) ---
 * '+' : Use se a bola rolou EM DIREÇÃO ao sensor. (Levanta o lado do sensor).
 * '-' : Use se a bola rolou PARA LONGE do sensor. (Abaixa o lado do sensor).
 * 's' : Use quando a bola ficar PERFEITAMENTE PARADA para salvar o valor.
 *
 * --- Nas Etapas de Limites (valores MIN e MAX) ---
 * '+' / '-' : Use para ajustar o ângulo manualmente até o limite desejado.
 * 's' : Use para DEFINIR o ângulo atual como o limite de segurança.
 *
 * -> FEEDBACK: Cada vez que '+' ou '-' for pressionado, o LED no pino D6
 * piscará e o buzzer no pino D3 emitirá um beep.
 *
 * -----------------------------------------------------------------------------------
 *
 * AVISOS IMPORTANTES:
 * - L-obs: Recomenda-se usar este script de calibração ANTES de carregar o
 * código principal de controle PID.
 * - L-obs: Seja cuidadoso e sincero ao definir os limites MÍNIMO e MÁXIMO.
 * Um valor incorreto pode forçar o servo e causar danos permanentes
 * ao seu dispositivo.
 *
* POSSÍVEIS ERROS E SOLUÇÕES:
 *
 * 1. PROBLEMA: Os comandos '+', '-' e 's' não funcionam. O programa não responde.
 * CAUSA: A configuração de "Final de Linha" (Line Ending) no Monitor Serial
 * está incorreta.
 * SOLUÇÃO: No Monitor Serial, vá no menu dropdown do canto inferior direito e
 * selecione a opção "Nova linha" (Newline).
 *
 * 2. PROBLEMA: O servo faz um zunido/barulho de esforço contínuo.
 * CAUSA: O ângulo de MÍNIMO ou MÁXIMO está forçando o servo contra um limite
 * físico.
 * SOLUÇÃO: Durante a calibração, ajuste o ângulo para um valor menos extremo
 * (diminua o máximo ou aumente o mínimo) até o barulho parar.
 *
 */

#include <Servo.h>

// === Configurações de Hardware ===
Servo myservo;
const int SERVO_PIN   = 9;  // Servo
const int LED_PIN     = 6;  // LED
const int BUZZER_PIN  = 3;  // Buzzer

// === Variáveis Globais ===
int modoPrincipal  = 0;  // 0 = menu inicial | 1 = automático | 2 = manual
int modoCalibracao = 0;
int anguloAtual    = 0;
int centroFinal    = -1;
int minFinal       = -1;
int maxFinal       = -1;

// === Modelos de Servo Suportados ===
struct ServoModelo {
  const char* nome;
  int graus;
};

ServoModelo modelos[] = {
  {"Futaba S3003", 180},
  {"TowerPro SG90", 180},
  {"MG996R", 180},
  {"Servo 270° Genérico", 270}
};
int modeloSelecionado = -1;

// ==================================================================
// SETUP
// ==================================================================
void setup() {
  Serial.begin(9600);
  while (!Serial) {}

  myservo.attach(SERVO_PIN);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  Serial.println(F("============================================="));
  Serial.println(F("== Assistente Interativo de Calibracao     =="));
  Serial.println(F("============================================="));
  Serial.println();
  Serial.println(F("Escolha o modo:"));
  Serial.println(F("1. Calibracao Automatica (assistente passo-a-passo)"));
  Serial.println(F("2. Ajuste Manual (digite valores diretamente)"));
  modoPrincipal = 0;
}

// ==================================================================
// LOOP PRINCIPAL
// ==================================================================
void loop() {
  if (Serial.available() > 0) {
    String linha = Serial.readStringUntil('\n');
    linha.trim();
    if (linha.length() == 0) return;

    if (modoPrincipal == 0) {
      if (linha == "1") {
        modoPrincipal = 1;
        mostrarListaModelosServo();
      } else if (linha == "2") {
        modoPrincipal = 2;
        iniciarModoManual();
      } else {
        Serial.println(F("Opcao invalida. Digite 1 ou 2."));
      }
      return;
    }

    // Rotas automáticas
    if (modoPrincipal == 1) {
      char comando = linha.charAt(0);
      switch (modoCalibracao) {
        case 1: handleSelecaoMotor(comando); break;
        case 2: handleCentro(comando); break;
        case 3: handlePromptMax(comando); break;
        case 4: handleMaximo(comando); break;
        case 5: handlePromptMin(comando); break;
        case 6: handleMinimo(comando); break;
        case 100: handleMenuFinal(comando); break;
      }
    }

    // Rotas manuais
    if (modoPrincipal == 2) {
      handleManual(linha);
    }
  }
}

// ==================================================================
// FUNÇÕES DE FEEDBACK
// ==================================================================
void feedbackAcao() {
  digitalWrite(LED_PIN, HIGH);
  tone(BUZZER_PIN, 1200, 50);
  delay(50);
  digitalWrite(LED_PIN, LOW);
  noTone(BUZZER_PIN);
}

// ==================================================================
// MENUS E MANUAL
// ==================================================================
void mostrarListaModelosServo() {
  Serial.println(F("Selecione o modelo de servo:"));
  for (int i = 0; i < (sizeof(modelos) / sizeof(modelos[0])); i++) {
    Serial.print(i + 1);
    Serial.print(F(". "));
    Serial.print(modelos[i].nome);
    Serial.print(F(" ("));
    Serial.print(modelos[i].graus);
    Serial.println(F(" graus)"));
  }
  modoCalibracao = 1;
}

void iniciarModoManual() {
  Serial.println(F("=== Ajuste Manual ==="));
  Serial.println(F("Digite no formato: c=72 m=65 x=100"));
  Serial.println(F("Pode digitar um de cada vez tambem (ex: c=75)."));
  Serial.println(F("c = Centro | m = Minimo | x = Maximo"));
}

void handleManual(String entrada) {
  if (entrada.startsWith("c=")) {
    centroFinal = entrada.substring(2).toInt();
    Serial.print(F("Centro definido manualmente: "));
    Serial.println(centroFinal);
  } else if (entrada.startsWith("m=")) {
    minFinal = entrada.substring(2).toInt();
    Serial.print(F("Minimo definido manualmente: "));
    Serial.println(minFinal);
  } else if (entrada.startsWith("x=")) {
    maxFinal = entrada.substring(2).toInt();
    Serial.print(F("Maximo definido manualmente: "));
    Serial.println(maxFinal);
  } else if (entrada == "l") {
    mostrarLista();
  } else if (entrada == "fim") {
    exibirResultadosFinais();
  } else {
    Serial.println(F("Comando invalido. Use c=, m=, x= ou fim."));
  }
}

// ==================================================================
// ETAPA 1: Seleção de Motor
// ==================================================================
void handleSelecaoMotor(char comando) {
  int indice = comando - '1';
  if (indice >= 0 && indice < (sizeof(modelos) / sizeof(modelos[0]))) {
    modeloSelecionado = indice;
    Serial.print(F("Modelo selecionado: "));
    Serial.println(modelos[indice].nome);
    iniciarCalibracaoCentro();
  } else {
    Serial.println(F("Opcao invalida."));
  }
}

// ==================================================================
// ETAPA 2: Calibracao Centro
// ==================================================================
void iniciarCalibracaoCentro() {
  anguloAtual = modelos[modeloSelecionado].graus / 2;
  myservo.write(anguloAtual);
  Serial.println(F("--- Etapa 2: Calibracao do Centro ---"));
  imprimirInstrucoesCentro();
  modoCalibracao = 2;
}

void imprimirInstrucoesCentro() {
  Serial.println(F(" '+' = Levantar lado do sensor"));
  Serial.println(F(" '-' = Abaixar lado do sensor"));
  Serial.println(F(" 's' = Salvar Centro"));
  Serial.println(F(" 'l' = Mostrar lista atual"));
  Serial.print(F("Angulo Central Atual: "));
  Serial.println(anguloAtual);
}

void handleCentro(char comando) {
  if (comando == '+') {
    anguloAtual++;
    myservo.write(anguloAtual);
    feedbackAcao();
    imprimirInstrucoesCentro();
  } else if (comando == '-') {
    anguloAtual--;
    myservo.write(anguloAtual);
    feedbackAcao();
    imprimirInstrucoesCentro();
  } else if (comando == 's' || comando == 'S') {
    centroFinal = anguloAtual;
    Serial.print(F(">>> Centro definido: "));
    Serial.println(centroFinal);
    promptCalibracaoMax();
  } else if (comando == 'l') {
    mostrarLista();
  } else {
    Serial.println(F("Comando invalido."));
  }
}

// ==================================================================
// ETAPA 3: Calibracao Max
// ==================================================================
void promptCalibracaoMax() {
  Serial.println(F("--- Etapa 3: Calibracao do Maximo ---"));
  Serial.println(F("Calibrar agora? (s/n)"));
  modoCalibracao = 3;
}

void handlePromptMax(char comando) {
  if (comando == 's') {
    anguloAtual = centroFinal + 15;
    myservo.write(anguloAtual);
    imprimirInstrucoesMaximo();
    modoCalibracao = 4;
  } else if (comando == 'n') {
    maxFinal = centroFinal + 20;
    promptCalibracaoMin();
  } else {
    Serial.println(F("Digite s ou n."));
  }
}

void imprimirInstrucoesMaximo() {
  Serial.println(F(" '+' = Mais inclinacao"));
  Serial.println(F(" '-' = Menos inclinacao"));
  Serial.println(F(" 's' = Salvar Maximo"));
  Serial.println(F(" 'l' = Mostrar lista atual"));
  Serial.print(F("Angulo Maximo Atual: "));
  Serial.println(anguloAtual);
}

void handleMaximo(char comando) {
  if (comando == '+') {
    anguloAtual++;
    myservo.write(anguloAtual);
    feedbackAcao();
    imprimirInstrucoesMaximo();
  } else if (comando == '-') {
    anguloAtual--;
    myservo.write(anguloAtual);
    feedbackAcao();
    imprimirInstrucoesMaximo();
  } else if (comando == 's') {
    maxFinal = anguloAtual;
    Serial.print(F(">>> Maximo definido: "));
    Serial.println(maxFinal);
    promptCalibracaoMin();
  } else if (comando == 'l') {
    mostrarLista();
  } else {
    Serial.println(F("Comando invalido."));
  }
}

// ==================================================================
// ETAPA 4: Calibracao Min
// ==================================================================
void promptCalibracaoMin() {
  Serial.println(F("--- Etapa 4: Calibracao do Minimo ---"));
  Serial.println(F("Calibrar agora? (s/n)"));
  modoCalibracao = 5;
}

void handlePromptMin(char comando) {
  if (comando == 's') {
    anguloAtual = centroFinal - 15;
    myservo.write(anguloAtual);
    imprimirInstrucoesMinimo();
    modoCalibracao = 6;
  } else if (comando == 'n') {
    minFinal = centroFinal - 20;
    exibirResultadosFinais();
  } else {
    Serial.println(F("Digite s ou n."));
  }
}

void imprimirInstrucoesMinimo() {
  Serial.println(F(" '+' = Menos inclinacao"));
  Serial.println(F(" '-' = Mais inclinacao"));
  Serial.println(F(" 's' = Salvar Minimo"));
  Serial.println(F(" 'l' = Mostrar lista atual"));
  Serial.print(F("Angulo Minimo Atual: "));
  Serial.println(anguloAtual);
}

void handleMinimo(char comando) {
  if (comando == '+') {
    anguloAtual++;
    myservo.write(anguloAtual);
    feedbackAcao();
    imprimirInstrucoesMinimo();
  } else if (comando == '-') {
    anguloAtual--;
    myservo.write(anguloAtual);
    feedbackAcao();
    imprimirInstrucoesMinimo();
  } else if (comando == 's') {
    minFinal = anguloAtual;
    Serial.print(F(">>> Minimo definido: "));
    Serial.println(minFinal);
    exibirResultadosFinais();
  } else if (comando == 'l') {
    mostrarLista();
  } else {
    Serial.println(F("Comando invalido."));
  }
}

// ==================================================================
// LISTA E RESULTADOS
// ==================================================================
void mostrarLista() {
  Serial.println(F("=== Valores Descobertos ==="));
  Serial.print(F("SERVO_CENTRO = ")); Serial.println(centroFinal);
  Serial.print(F("SERVO_MIN    = ")); Serial.println(minFinal);
  Serial.print(F("SERVO_MAX    = ")); Serial.println(maxFinal);
}

void exibirResultadosFinais() {
  myservo.write(centroFinal);
  Serial.println(F("============================================="));
  Serial.println(F("== CALIBRACAO COMPLETA!                    =="));
  Serial.println(F("============================================="));
  mostrarLista();
  Serial.println();
  Serial.println(F("Copie no seu codigo principal:"));
  Serial.print(F("const int SERVO_CENTRO     = ")); Serial.println(centroFinal);
  Serial.print(F("const int SERVO_MIN_ANGULO = ")); Serial.println(minFinal);
  Serial.print(F("const int SERVO_MAX_ANGULO = ")); Serial.println(maxFinal);
  Serial.println();
  menuFinal();
}

void menuFinal() {
  Serial.println(F("O que deseja fazer?"));
  Serial.println(F("1. Refazer Centro"));
  Serial.println(F("2. Refazer Maximo"));
  Serial.println(F("3. Refazer Minimo"));
  Serial.println(F("4. Reiniciar Tudo"));
  Serial.println(F("5. Sair"));
  modoCalibracao = 100;
}

void handleMenuFinal(char comando) {
  if (comando == '1') {
    iniciarCalibracaoCentro();
  } else if (comando == '2') {
    promptCalibracaoMax();
  } else if (comando == '3') {
    promptCalibracaoMin();
  } else if (comando == '4') {
    setup(); // reinicia fluxo
  } else if (comando == '5') {
    Serial.println(F("Encerrado."));
    while (1);
  } else {
    Serial.println(F("Opcao invalida."));
  }
}