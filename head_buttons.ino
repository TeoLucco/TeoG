void checkBT() {
  if (millis() - firstSoundTime >= WAIT_BT_CONN && !Serial3.available() && interpreterState == choose_modality) {
    previousWorkingCapacitives=workingCapacitives;
    workingCapacitives=head;
    headInterpreter=true;
    //Serial3.println("ATTIVAZIONE CAPACITIVI TESTA");
    playS(17);
  } else if (Serial3.available()){ firstSoundTime = millis(); headInterpreter=false;
  } 
}
unsigned long int lastPressedButtonTime = 0;
boolean colorByButton=false;
#define WIPE_DURATION 2000
void headCapacitiveLoop(){
  if(pressedButton!=-1){
    lastPressedButtonTime=millis();
    colorByButton=true;
    switch(pressedButton){
      case 0: if(led_state!=led_off)headLedUpdate(red,color_wipe);break;
      case 1: if(led_state!=led_off)headLedUpdate(green,color_wipe);break;
      case 2: if(led_state!=led_off)headLedUpdate(blue,color_wipe);break;
      case 3: if(led_state!=led_off)headLedUpdate(yellow,color_wipe);break;
    }
  }
  else if(colorByButton && millis()-lastPressedButtonTime>WIPE_DURATION){
    colorByButton=false;
    if(led_state!=led_off)headLedUpdate(rainbow_cycle);
  }
}

void headCapacitiveInterpreter() {
  if (headInterpreter && gameState != wait_answer) {
    switch (interpreterState) {
      case choose_modality: chooseModCap(); break;
      case choose_game:     chooseGameCap(); break;
      case sg_waiting:      startGameWaitCap(); break;
      case game_modality:   break;
      case fam_modality:    break;
      case test_modality:   break;
      case discharge:       break;
    }
  }else checkBT();
}

void chooseModCap() {
  switch (pressedButton) {
    case -1: break;
    case 0: interpreterState = choose_game; break;
    case 1: interpreterState = fam_modality; break;
    case 2: interpreterState = test_modality; break;
    case 3: break;
  }
}
void chooseGameCap() {
  switch (pressedButton) {
    case -1: break;
    case 0:
      if (gameNumber > 1) gameNumber--;
      else gameNumber = 1;
      playS(19 + gameNumber);
      break;

    case 1:
      if (gameNumber < N_GAMES) gameNumber++;
      else gameNumber = N_GAMES;
      playS(19 + gameNumber);
      break;

    case 2:
      interpreterState = sg_waiting;
      playS(24 + gameNumber); //come posizionare patch
      break;

    case 3:
      interpreterState = choose_modality;
      break;
  }
}
void startGameWaitCap() {
  switch (pressedButton) {
    case -1:  break;
    case 0:   break;
    case 1:   break;
    case 2:   interpreterState = game_modality; break;
    case 3:   interpreterState = choose_game; break;
  }

}

