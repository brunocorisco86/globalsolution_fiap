// Economizador de Água e Energia para o Império
// Desenvolvido para a manutenção da ordem e economia na galáxia

// Definição dos pinos
const int trigPin = 23;
const int echoPin = 22;
const int ledVerde = 18;
const int ledVermelho = 17;
const int buzzer = 16;

// Definição das notas musicais para a Marcha Imperial
#define NOTE_A4  440
#define NOTE_F4  349
#define NOTE_C5  523
#define NOTE_G5  784
#define NOTE_F5  698
#define NOTE_C5  523
#define NOTE_A5  880
#define NOTE_GS5 831
#define NOTE_G5  784
#define NOTE_DS5 622
#define NOTE_D5  587
#define NOTE_CS5 554
#define NOTE_C5  523
#define NOTE_B4  494

// Configurações de tempo
const unsigned long TEMPO_LIMITE_BANHO = 5 * 60 * 1000; // 5 minutos
const unsigned long TEMPO_LIMITE_AUSENCIA = 1 * 60 * 1000; // 1 minuto

// Variáveis de controle
bool pessoaPresente = false;
unsigned long tempoInicio = 0;
unsigned long tempoSemPresenca = 0;

// Melodia da Marcha Imperial
int melodia[] = {
  NOTE_A4, NOTE_A4, NOTE_A4, NOTE_F4, NOTE_C5,
  NOTE_A4, NOTE_F4, NOTE_C5, NOTE_A4
};

int duracoes[] = {
  4, 4, 4, 8, 16,
  4, 8, 16, 2
};

// Função para tocar a Marcha Imperial
void tocarMarchaimperial() {
  int tamanhoMelodia = sizeof(melodia) / sizeof(melodia[0]);
  
  for (int i = 0; i < tamanhoMelodia; i++) {
    int duracaoNota = 1000 / duracoes[i];
    tone(buzzer, melodia[i], duracaoNota);
    delay(duracaoNota * 1.3);
    noTone(buzzer);
  }
}

void setup() {
  // Inicialização da comunicação serial
  Serial.begin(9600);
  
  // Configuração dos pinos
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ledVerde, OUTPUT);
  pinMode(ledVermelho, OUTPUT);
  pinMode(buzzer, OUTPUT);

  // Estado inicial dos componentes
  digitalWrite(ledVerde, LOW);
  digitalWrite(ledVermelho, LOW);
  digitalWrite(buzzer, LOW);

  Serial.println("Sistema iniciado - aguardando presença.");
}

void loop() {
  int distancia = calcularDistancia();

  if (distancia > 0 && distancia <= 100) {
    tratarPresenca();
  } else {
    tratarAusencia();
  }

  delay(500);
}

// Função para calcular a distância usando o sensor ultrassônico
int calcularDistancia() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  return duration * 0.034 / 2;
}

// Função para tratar a presença detectada
void tratarPresenca() {
  if (!pessoaPresente) {
    pessoaPresente = true;
    tempoInicio = millis();
    Serial.println("Pessoa detectada no banho. Timer iniciado.");
  }

  unsigned long tempoAtual = millis() - tempoInicio;
  Serial.print("Tempo de banho: ");
  Serial.print(tempoAtual / 1000);
  Serial.println(" segundos");

  if (tempoAtual >= TEMPO_LIMITE_BANHO) {
    digitalWrite(ledVerde, LOW);
    digitalWrite(ledVermelho, HIGH);
    tocarMarchaimperial(); // Toca a melodia da Marcha Imperial
    Serial.println("Tempo excedido! Aviso acionado.");
  } else {
    digitalWrite(ledVerde, HIGH);
    digitalWrite(ledVermelho, LOW);
  }

  tempoSemPresenca = 0;
}

// Função para tratar a ausência detectada
void tratarAusencia() {
  if (pessoaPresente) {
    if (tempoSemPresenca == 0) {
      tempoSemPresenca = millis();
      Serial.println("Pessoa saiu do alcance do sensor.");
    } else if (millis() - tempoSemPresenca >= TEMPO_LIMITE_AUSENCIA) {
      pessoaPresente = false;
      tempoInicio = 0;
      digitalWrite(ledVerde, LOW);
      digitalWrite(ledVermelho, LOW);
      Serial.println("Timer resetado. Sistema aguardando nova detecção.");
    }
  }
}