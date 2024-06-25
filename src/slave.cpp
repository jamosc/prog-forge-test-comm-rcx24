//bibliotecas
#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <HardwareSerial.h>

//classes
class LineFollower
{
public:
  int ir1, ir2, ir3, ir4, ir5;
  int values[5];

  LineFollower(int port1, int port2, int port3, int port4, int port5)
  {
    ir1 = port1;
    ir2 = port2;
    ir3 = port3;
    ir4 = port4;
    ir5 = port5;
    pinMode(ir1, INPUT);
    pinMode(ir2, INPUT);
    pinMode(ir3, INPUT);
    pinMode(ir4, INPUT);
    pinMode(ir5, INPUT);
  }

  void refresh_values()
  {
    int ports[] = {ir1, ir2, ir3, ir4, ir5};
    for (int i = 0; i <= 4; i++)
    {
      values[i] = digitalRead(ports[i]);
    }
  }
};

//constantes
#define ESP_TX2 17
#define ESP_RX2 16
#define BAUD_RATE 115200

//objetos
HardwareSerial espSlave(2); // uart2
LineFollower line_follower1(26, 27, 14, 12, 13);
LineFollower line_follower2(34, 35, 32, 33, 25);

//variaveis
int readings[10];
char send_data[11];

//funcoes
void ready_state(){
  const char* ready_message = "RDY";
  const char* acknowledge_message = "ACK\n";

  bool rdy_received = false;
  char message_received[sizeof(ready_message) + 1] = {'\0'};
  size_t bytes_received_rdy;

  while(rdy_received == false){
    if(espSlave.available() > 2){ // verifica se existem bytes disponiveis para serem lidos
      bytes_received_rdy = espSlave.readBytesUntil('\n', message_received, strlen(ready_message));
      if(bytes_received_rdy == strlen(ready_message)){ // verifica se a mensagem recebida foi o reconhecimento
        if(strcmp(message_received, ready_message) == 0){
            rdy_received = true; // conexao reconhecida
            espSlave.write(acknowledge_message, sizeof(acknowledge_message));
        }
      }
    }
  }
}

void int_array_to_string(int int_array[], char string_data[], int array_lenght){
    for(int i = 0; i < array_lenght; i++){
        if(int_array[i] == 0){
            string_data[i] = '0';
        }
        else{
            string_data[i] = '1';
        }
    }
    string_data[array_lenght] = '\n';
}

void setup() {
    init();
    espSlave.begin(BAUD_RATE, SERIAL_8N1, ESP_RX2, ESP_TX2); // 8 bits no parity 1 stop bit
    ready_state();
    // Serial.begin(BAUD_RATE);
    delay(2000);
}

void loop() {
    line_follower1.refresh_values();
    line_follower2.refresh_values();

    for(int i = 0; i < 5; i++){
        readings[i] = line_follower2.values[i];
    }
    for(int i = 0; i < 5; i++){
        readings[5+i] = line_follower1.values[i];
    }

    int lenght = sizeof(readings)/sizeof(readings[0]);
    int_array_to_string(readings, send_data, lenght);

    // for(int i = 0; i < lenght + 1; i++){
    //     Serial.print(send_data[i]);
    // }
    // Serial.println("");

    espSlave.write(send_data,lenght+1);

    memset(readings, 0, sizeof(readings));
    memset(send_data, 0, sizeof(send_data));
    delay(200);
}