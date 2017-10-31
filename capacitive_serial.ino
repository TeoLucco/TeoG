void capacitiveSerialLoop() {
  reciveSerial();
  sendSerial();
}


void reciveSerial() {
  resetPatCountTimer();
  resetHitCountTimer();
  resetHugsCountTimer();
  if (pressedButton != -1) pressedButton = -1;
  if (touched != noWhere) touched = noWhere;
  if (touch_type != nothing) touch_type = nothing;
  if (Serial.available()) {
    int b = Serial.read();
    Serial3.print("FROM NANO TO MEGA:'");
    Serial3.print(b);
    Serial3.println("'");
    switch (b) {
      case 1: hugRecived();
        break;
      case 2: patRecived();
        break;
      case 3: hitRecived();
        break;
      case 4: pressedButton = Serial.read(); Serial3.print("Pressed button n "); Serial3.println(pressedButton); break;
      default : Serial3.println("Stringa ricevuta non coincide!"); break;
    }
  }
}

void hugRecived() {
  abbraccioN++;
  lastHugRecivedTime=millis();
  touch_type = hugT;
  Serial3.print("Abbraccio durata "); Serial3.println(abbraccioN);
  //playS(23);
  setLedTimer(4000);
  bodyLedUpdate(color_pulse,redC);
  resetPats();
  resetHits();
}

void patRecived(){
  resetHits();
  touch_type = patT;
  int i = Serial.read();
  //if (i == -1) break;
  lastPatTime[i] = millis();
  Serial3.print("pat at "); Serial3.println(i);
  setTouched(i);
  if (pat[i] == N_PATS) {
    resetPats();
    //playS(23);  
    setLedTimer(2000);
    bodyLedUpdate(color_wipe,lightBlueC);
  } else {
    pat[i]++;
    pats++;
    setLedTimer(2000);
    bodyLedUpdate(color_wipe,lightBlueC);
    //playS(23); 
  }
}

void hitRecived(){
  resetPats();
  touch_type = hitT;
  int i = Serial.read();
  //if (i == -1) break;
  lastHitTime[i] = millis();
  Serial3.print("hit at "); Serial3.println(i);
  setTouched(i);
  if (hit[i] == N_HITS) {
    resetHits();
    setLedTimer(2000);
    bodyLedUpdate(color_wipe,blueC);
    //nhits(i);
  } else {
    hit[i]++;
    hits++;
    setLedTimer(2000);
    bodyLedUpdate(color_wipe,orangeC);
    //hitRecived(i);
  }
}

void setTouched(int i) {
  switch (i) {
    case 0: touched = leftT; break;
    case 1: touched = rightT; break;
    case 2: touched = frontT; break;
    default: touched = noWhere; break;
  }
}

void sendSerial() {
  if (workingCapacitives != previousWorkingCapacitives) {
    switch (workingCapacitives) {
      case noOne: Serial.write(0); break;
      case head: Serial.write(1); break;
      case body: Serial.write(2); break;
      case both: Serial.write(3); break;
    }
    Serial3.print("Stato capacitivi cambiato da '"); Serial3.print(previousWorkingCapacitives); Serial3.print("' a '"); Serial3.print(workingCapacitives); Serial3.println("'");
    previousWorkingCapacitives = workingCapacitives;
  }
}

void nhits(int i) {
  if (!(testState == test_exe && testType == body_capacitives_t)) {
    switch (i) {
      case 0: startMovement(make_sad2L, redC, color_pulse, 33); break;
      case 1: startMovement(make_sad2R, redC, color_pulse, 33); break;
      case 2: startMovement(make_sad2,  redC, color_pulse, 33); break;
    }
  }
}

void hitRecived(int i) {
  if (!(testState == test_exe && testType == body_capacitives_t)) {
    switch (i) {
      case 0: startMovement(scared_hitL, orangeC, color_pulse, 34); break;
      case 1: startMovement(scared_hitR, orangeC, color_pulse, 34); break;
      case 2: startMovement(scared_hit,  orangeC, color_pulse, 34); break;
    }
  }
}

void resetPatCountTimer() {
  pats = 0;
  for (int i = 0; i < N_BODY_SENSORS; i++) {
    if (millis() - lastPatTime[i] > RESET_PAT_TIME /*&& capacitiveState[i] == no_touch*/) {
      pat[i] = 0;
    }
    pats += pat[i];
  }
}

void resetHitCountTimer() {
  hits = 0;
  for (int i = 0; i < N_BODY_SENSORS; i++) {
    if (millis() - lastHitTime[i] > RESET_HIT_TIME /*&& capacitiveState[i] == no_touch*/) {
      hit[i] = 0;
    }
    hits = hit[i];
  }
}

void resetHugsCountTimer(){
  if(millis() - lastHugRecivedTime>RESET_HUG_TIME){
       abbraccioN=0;
  }
}

void resetPats() {
  for (int j = 0; j < N_BODY_SENSORS; j++) pat[j] = 0;
  pats = 0;
}
void resetHits() {
  for (int j = 0; j < N_BODY_SENSORS; j++) hit[j] = 0;
  hits = 0;
}
void CapacitivesUpdate(warKingsCapacitives c) {
  previousWorkingCapacitives = workingCapacitives;
  workingCapacitives = c;
}

