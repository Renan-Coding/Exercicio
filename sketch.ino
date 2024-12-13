#include <WiFi.h>
#include <HTTPClient.h>

#define led_green 2     // Pino utilizado para controle do led verde
#define led_red 16      // Pino utilizado para controle do led vermelho
#define led_yellow 5    // Pino utilizado para controle do led amarelo

const int buttonPin = 18;     // Pino utilizado para o botão
int buttonState = 0;          // Variável que lê o estado do botão
int lastButtonState = 0;      // Último estado do botão
unsigned long lastDebounceTime = 0; // Tempo de debounce
unsigned long debounceDelay = 50;   // Delay para debounce

const int ldrPin = 4;         // Número do pino do LDR
int threshold = 600;          // Limite de luminosidade

int buttonPressCount = 0;     // Contador de pressionamentos do botão

const char* ssid = "Wokwi-GUEST";
const char* password = "";

unsigned long buttonPressTime = 0;
bool openSemaphore = false;

void setup() {
  pinMode(led_yellow, OUTPUT); // Configura o pino do LED amarelo como saída
  pinMode(led_green, OUTPUT);  // Configura o pino do LED verde como saída
  pinMode(led_red, OUTPUT);    // Configura o pino do LED vermelho como saída
  pinMode(buttonPin, INPUT);   // Configura o pino do botão como entrada

  digitalWrite(led_yellow, LOW); // Inicializa o LED amarelo desligado
  digitalWrite(led_green, LOW);  // Inicializa o LED verde desligado
  digitalWrite(led_red, LOW);    // Inicializa o LED vermelho desligado

  Serial.begin(9600); // Inicializa a comunicação serial

  WiFi.begin(ssid, password); // Conecta ao WiFi
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println("\nConectado ao WiFi com sucesso!");
}

void loop() {
  int ldrstatus = analogRead(ldrPin); // Lê o valor do LDR

  if (ldrstatus <= threshold) {
    // Modo noturno
    Serial.print("Está escuro, pisque o LED amarelo: ");
    Serial.println(ldrstatus);
    digitalWrite(led_yellow, HIGH); // Liga o LED amarelo
    delay(500);
    digitalWrite(led_yellow, LOW);  // Desliga o LED amarelo
    delay(500);
  } else {
    // Modo claro
    Serial.print("Está claro, siga a temporização dos LEDs: ");
    Serial.println(ldrstatus);

    int reading = digitalRead(buttonPin); // Lê o estado do botão

    // Gerenciamento de debounce
    if (reading != lastButtonState) {
      lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > debounceDelay) {
      if (reading != buttonState) {
        buttonState = reading;

        if (buttonState == HIGH) {
          Serial.println("Botão pressionado!");

          // Se o semáforo estiver fechado (LED vermelho aceso)
          if (digitalRead(led_red) == HIGH) {
            buttonPressCount++;
            Serial.print("Contagem de pressionamentos: ");
            Serial.println(buttonPressCount);

            if (buttonPressCount >= 3) {
              // Envia alerta HTTP após 3 pressionamentos
              sendAlert();
              buttonPressCount = 0;
            } else {
              // Abre o semáforo após 1 segundo para pressionamento único
              buttonPressTime = millis();
              openSemaphore = true;
            }
          }
        }
      }
    }

    lastButtonState = reading;
    delay(50);

    if (openSemaphore && (millis() - buttonPressTime >= 1000)) {
      openSemaphore = false;
      Serial.println("Abrindo semáforo após 1 segundo do botão pressionado.");
      digitalWrite(led_red, LOW);   // Desliga o LED vermelho
      digitalWrite(led_green, HIGH); // Liga o LED verde
      delay(3000);
      digitalWrite(led_green, LOW);  // Desliga o LED verde
    } else if (!openSemaphore) {
      // Temporização convencional dos LEDs
      digitalWrite(led_green, HIGH); // Liga o LED verde
      delay(3000);
      digitalWrite(led_green, LOW);  // Desliga o LED verde
      
      digitalWrite(led_yellow, HIGH); // Liga o LED amarelo
      delay(2000);
      digitalWrite(led_yellow, LOW);  // Desliga o LED amarelo

      digitalWrite(led_red, HIGH);    // Liga o LED vermelho
      delay(5000);
      digitalWrite(led_red, LOW);     // Desliga o LED vermelho
    }
  }
}

void sendAlert() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin("http://www.google.com.br/"); // URL para enviar o alerta
    int httpCode = http.GET();

    if (httpCode > 0) {
      Serial.printf("Requisição enviada, código de resposta: %d\n", httpCode);
    } else {
      Serial.printf("Erro na requisição: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  } else {
    Serial.println("WiFi não está conectado.");
  }
}
