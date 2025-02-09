// to make clock pulse
void clock_func(byte port){
  //digitalWrite(0, HIGH);
  gpio_put(port, 1);  // in case of wite once, data wouldn't be captured correctly
  gpio_put(port, 1);  // due to the cable bandwidth issue
  gpio_put(port, 1);
  //digitalWrite(0, LOW);
  gpio_put(port, 0);
}

void refresh(){
  for (row_c = 0; row_c < 16; row_c++){
    // select row address
    gpio_put(oe, HIGH);
    gpio_put(a, a_sel[row_c]);
    gpio_put(b, b_sel[row_c]);
    gpio_put(c, c_sel[row_c]);
    gpio_put(d, d_sel[row_c]);
    gpio_put(e, e_sel[row_c]);    
// led display refreah cycle
//
// display row data with shade control
// due to the processing time, we prepare two similar code
// (if we set disp_data plate dimension as a variable, it will causes thirty percent processing time increasing)
  if (plane == 0){ 
    for (byte j = 0; j < 16; j++){
      row_set_0(j);
      gpio_put(oe, HIGH);
      clock_func(lat);
      gpio_put(oe, LOW);
    }
  } else {
    for (byte j = 0; j < 16; j++){
      row_set_1(j);
      gpio_put(oe, HIGH);
      clock_func(lat);
      gpio_put(oe, LOW);
    }
    }}}

// row data buffer write
//
void row_set_0(byte shade){
    for (byte i = 0; i < 64; i++){
      // color order b/g/r
      // red
      if (disp_data[0][2][row_c][i] > shade){
        gpio_put(r1, 1);        
      }
      else{
        gpio_put(r1, 0);
      }
      if (disp_data[0][2][row_c + 16][i] > shade){
        gpio_put(r2, 1);
      }
      else{
        gpio_put(r2, 0);
      }
      // green
      if (disp_data[0][1][row_c][i] > shade){
        gpio_put(g1, 1);
      }
      else{
        gpio_put(g1, 0);
      }
      if (disp_data[0][1][row_c + 16][i] > shade){
        gpio_put(g2, 1);
      }
      else{
        gpio_put(g2, 0);
      }
      //blue
      if (disp_data[0][0][row_c][i] > shade){
        gpio_put(b1, 1);
      }
      else{
        gpio_put(b1, 0);
      }
      if (disp_data[0][0][row_c + 16][i] > shade){
        gpio_put(b2, 1);
      }
      else{
        gpio_put(b2, 0);
      }
      // write to row buffer
      clock_func(clk);
    }
}

void row_set_1(byte shade){
    for (byte i = 0; i < 64; i++){
      // thanks color order b/g/r
      // red
      if (disp_data[1][2][row_c][i] > shade){
        gpio_put(r1, 1);        
      }
      else{
        gpio_put(r1, 0);
      }
      if (disp_data[1][2][row_c + 16][i] > shade){
        gpio_put(r2, 1);
      }
      else{
        gpio_put(r2, 0);
      }
      // green
      if (disp_data[1][1][row_c][i] > shade){
        gpio_put(g1, 1);
      }
      else{
        gpio_put(g1, 0);
      }
      if (disp_data[1][1][row_c + 16][i] > shade){
        gpio_put(g2, 1);
      }
      else{
        gpio_put(g2, 0);
      }
      //blue
      if (disp_data[1][0][row_c][i] > shade){
        gpio_put(b1, 1);
      }
      else{
        gpio_put(b1, 0);
      }
      if (disp_data[1][0][row_c + 16][i] > shade){
        gpio_put(b2, 1);
      }
      else{
        gpio_put(b2, 0);
      }
      // write to row buffer
      clock_func(clk);
    }
}

// TCP setup
void tcp_setup(){
  Serial.begin(115200);
  
  pinMode(nw, INPUT_PULLUP);

// Wi-Fi address switch
// 0 : home
// 1 : support center
  int sw_stat = digitalRead(nw);
  if (sw_stat == 0){
    staticIP = IPAddress(192, 168, 1, 200);
    gateway = IPAddress(192, 168, 1, 1);
  }else{
    staticIP = IPAddress(192, 168, 11, 200);
    gateway = IPAddress(192, 168, 11, 1);
  }

  WiFi.config(staticIP, gateway, subnet);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi...");
  }
  
  Serial.println("Connected to WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  server.begin();
}

// UDP setup(reserve)
//
void udp_setup(){
  // reset the connection
  WiFi.disconnect(true);
  delay(500);

  WiFi.mode(WIFI_STA);
  WiFi.config(myIP);

  WiFi.begin(ssid, password);

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
  }

  Udp.begin(5000);
}

// UDP receive(reserve)
//
void udp(){
    // udp data check
  int packetSize = Udp.parsePacket();
  if (packetSize){  
    memset(packetBuffer,0,sizeof(packetBuffer));
    Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
    // message to specify display data panel to core1
    if (packetBuffer[0] == 'A'){
      if (init_disp == true){
        message_s = 1;
      } else {
        if (img_pointer == 0){
          if (plane == 0){
            memcpy(disp_data[1], thanks, sizeof(thanks));
            message_s = 1;

          } else{
            memcpy(disp_data[0], thanks, sizeof(thanks));
            message_s = 0;
            }
          img_pointer += 1;
        } else{
          if (img_pointer == 1){
            if (plane == 0){
              memcpy(disp_data[1], akafuji, sizeof(akafuji));
              message_s = 1;
            } else{
              memcpy(disp_data[0], akafuji, sizeof(akafuji));
              message_s = 0;
              }
          img_pointer += 1;
          } else{
            if (img_pointer == 2){
            if (plane == 0){
              memcpy(disp_data[1], pika, sizeof(pika));
              message_s = 1;
            } else{
            memcpy(disp_data[0], pika, sizeof(pika));
            message_s = 0;
            }
            img_pointer = 0;
            }
          }}
      }
    } else {
      if (packetBuffer[0] == 'C'){
        message_s = 9;
      }
    }
    rp2040.fifo.push(message_s);  // send message to core1
  }
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