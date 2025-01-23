//
// HUB75 LED array driver firmware
//

#include <WiFi.h>
//#include <WiFiUDP.h>
#include "common.h"

void setup() {
  // set the led pin as output:
  // set the digital pin as output:
  pinMode(ledPin, OUTPUT);

  // initial image data move to display buffer
  memcpy(disp_data[1], thanks, sizeof(thanks));
  memcpy(disp_data[0], pika, sizeof(pika)); // pow-on initial data

  tcp_setup();

  gpio_put(oe, HIGH);
  }

void loop() {
  client = server.accept();
  // if network is active
  if (client) {
    //Serial.println("New client connected");
    
    while (client.connected()) {
      if (client.available()) {
        // receive binary data
        int bytesRead = 0;
        unsigned long startTime = millis();
        
        // Read all available data within a 500ms window
        while (client.available() && (millis() - startTime < 500) && bytesRead < MAX_BUFFER) {
          receivedData[bytesRead] = client.read();
          bytesRead++;
        }
        
        // print out received data
        //Serial.print("Received data: ");
        if (bytesRead > 0) {
          for (int i = 0; i < bytesRead; i++) {
            //Serial.print(receivedData[i], HEX);
            //Serial.print(" ");
          }
          //Serial.println();
        }
        
        // response to the server
        //
        const char* response = "Resp from Pico W!";
        client.write(response, strlen(response));
      }
    }
    if (plane == 0){
      message_s = 1;
    } else{
      message_s = 0;
    }
    int p = 2;
    for (int k = 0; k < 3; k++){
      for (int j = 0; j < 32; j++){
        for (int i = 0; i < 64; i++){
          disp_data[message_s][k][j][i] = receivedData[p++];
          //Serial.print(disp_data[1][k][j][i], HEX);
        }
      }
    }
    rp2040.fifo.push(message_s);  // send message to core1
    //Serial.print("data prepared");
  }
  //udp();  // in case of using UDP

  // board led indication
  //
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }
    // set the LED with the ledState of the variable:
    digitalWrite(ledPin, ledState);
  }
}

void setup1(){
  // set the gpio pins direction
  pinMode(r1, OUTPUT);
  pinMode(r2, OUTPUT);    
  pinMode(g1, OUTPUT);
  pinMode(g2, OUTPUT);    
  pinMode(b1, OUTPUT);
  pinMode(b2, OUTPUT);    
  pinMode(clk, OUTPUT);    
  pinMode(lat, OUTPUT);
  pinMode(oe, OUTPUT);    
  pinMode(a, OUTPUT);
  pinMode(b, OUTPUT);    
  pinMode(c, OUTPUT);
  pinMode(d, OUTPUT);
  pinMode(e, OUTPUT);

  gpio_put(oe, HIGH);
  gpio_put(lat, LOW);
  gpio_put(clk, LOW);
}

void loop1(){
  if (init_disp == true){
    init_disp = false;
    delay(1000);
  }
  // message check from core0
  //
  while (rp2040.fifo.available()>0){
    // bool rp2040.fifo.pop_nb(uint32_t *dest)
    message = rp2040.fifo.pop();
  }
 
  if (message == 0 || message == 1){
      plane = message;
      refresh();
    } else {
      gpio_put(oe, HIGH);
  }
}