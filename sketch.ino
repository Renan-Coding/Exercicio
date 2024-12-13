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
  pinMode(led_yellow, OUTPUT);
  pinMode(led_green, OUTPUT);
  pinMode(led_red, OUTPUT);
  pinMode(buttonPin, INPUT);

  digitalWrite(led_yellow, LOW);
  digitalWrite(led_green, LOW);
  digitalWrite(led_red, LOW);

  Serial.begin(9600);

  // WiFi.begin(ssid, password);
  // while (WiFi.status() != WL_CONNECTED) {
  //   delay(100);
  //   Serial.print(".");
  // }
  // Serial.println("\nConectado ao WiFi com sucesso!");
}

void loop() {
  int ldrstatus = analogRead(ldrPin);

  if (ldrstatus <= threshold) {
    // Modo noturno
    Serial.print("Está escuro, pisque o LED amarelo: ");
    Serial.println(ldrstatus);
    digitalWrite(led_yellow, HIGH);
    delay(500);
    digitalWrite(led_yellow, LOW);
    delay(500);
  } else {
    // Modo claro
    Serial.print("Está claro, siga a temporização dos LEDs: ");
    Serial.println(ldrstatus);

    int reading = digitalRead(buttonPin);

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
      digitalWrite(led_red, LOW);
      digitalWrite(led_green, HIGH);
      delay(3000);
      digitalWrite(led_green, LOW);
    } else if (!openSemaphore) {
      // Temporização convencional dos LEDs
      digitalWrite(led_green, HIGH);
      delay(3000);
      digitalWrite(led_green, LOW);
      
      digitalWrite(led_yellow, HIGH);
      delay(2000);
      digitalWrite(led_yellow, LOW);

      digitalWrite(led_red, HIGH);
      delay(5000);
      digitalWrite(led_red, LOW);
    }
  }
}

void sendAlert() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin("http://www.google.com.br/");
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
