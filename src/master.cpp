// bibliotecas
#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <HardwareSerial.h>

// constantes
#define VESPA_TX2 17
#define VESPA_RX2 16
#define BAUD_RATE 115200

// objetos
HardwareSerial vespaMaster(2); // uart2
LiquidCrystal_I2C lcd(0x27,16,2); // inicializa o display no endereco 0x27

// variaveis
char received_string[11];
size_t bytes_received;

// funcoes
void ready_state(){
  const char* ready_message = "RDY\n"; // trasmite um sinal de ready ate ser reconhecido
  const char* acknowledge_message = "ACK"; // frequencia de transmissao = 1 Hz
  const unsigned long transmitInterval = 1e6; // 1.000.000 us = 1 s, vamos usar a funcao micros() para contabilizar o tempo
    
  unsigned long start = micros();
  bool ack_received = false;
  char message_received[sizeof(acknowledge_message) + 1] = {'\0'};
  size_t bytes_received_rdy;

  while(ack_received == false){
    if(micros() - start >= transmitInterval){ // tempo decorrido (a cada 1s transmite o sinal de rdy)
      vespaMaster.write(ready_message, sizeof(ready_message));
      start = micros(); // reseta o timer
    }
    if(vespaMaster.available() > 2){ // verifica se existem bytes disponiveis para serem lidos
      bytes_received_rdy = vespaMaster.readBytesUntil('\n', message_received, strlen(acknowledge_message));
      if(bytes_received_rdy == strlen(acknowledge_message)){ // verifica se a mensagem recebida foi o reconhecimento
        if(strcmp(message_received, acknowledge_message) == 0){
          ack_received = true; // conexao reconhecida
        }
      }
    }
  }
}

void setup()
{
  init(); // inicializa timers e outras ferramentas
  lcd.init();
  lcd.setBacklight(HIGH);
  vespaMaster.begin(BAUD_RATE, SERIAL_8N1, VESPA_RX2, VESPA_TX2); // 8 bits no parity 1 stop bit
  // Serial.begin(BAUD_RATE);
  ready_state();
  lcd.setCursor(0,0);
  lcd.print("SERIAL");
  lcd.setCursor(0,1);
  lcd.print("CONECTADO");
  delay(2000);
}

void loop()
{
  bytes_received = vespaMaster.readBytesUntil('\n', received_string, 10);

  lcd.setCursor(0,0);
  lcd.print("LEITURA SENSORES");
  if(bytes_received > 0){
    // Serial.println(received_string);
    lcd.setCursor(0,1);
    lcd.print(received_string);
  }
  delay(20);
}

