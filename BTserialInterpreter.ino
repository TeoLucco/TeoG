//void resetButtons() {
//  triangolo = false;
//  quadrato = false;
//  cerchio = false;
//  croce = false;
//  startbutton = false;
//  select = false;
//}
//


#define SENDSTATETIME 1000
unsigned long int lastSendState = 0;
double prev_posX = 0;
double actual_posX = 0;
double prev_posY = 0;
double actual_posY = 0;
double prev_posTh = 0;
double actual_posTh = 0;
void sendState() {
  if (millis() - lastSendState > SENDSTATETIME) {
    Serial3.print("*A" + String(interpreterState) + "*");
    lastSendState = millis();
  }
  prev_posX = actual_posX;
  actual_posX = triskar.getPosX();
  prev_posY = actual_posY;
  actual_posY = triskar.getPosY();
  prev_posTh = actual_posTh;
  actual_posTh = triskar.getPosTh();
  if (actual_posX != prev_posX) {
    Serial3.print("*X" + String(actual_posX) + "*");
  }
  if (actual_posY != prev_posY) {
    Serial3.print("*Y" + String(actual_posY) + "*");
  }
  if (actual_posTh != prev_posTh) {
    Serial3.print("*T" + String(actual_posTh) + "*");
  }
  if (millis() - lastBatteryUpdate > BATTERY_UPDATE_TIME) {
    battery_indicator = constrain(mapfloat(voltage, MIN_INDICATOR_VOLTAGE, MAX_INDICATOR_VOLTAGE, MIN_INDICATOR_VALUE, MAX_INDICATOR_VALUE), MIN_INDICATOR_VALUE, MAX_INDICATOR_VALUE);
    Serial3.print("*B" + String(battery_indicator) + "*");
    lastBatteryUpdate = millis();
  }
  Serial3.print("*D" + String(f_front) + "*");
  Serial3.print("*L" + String(f_left) + "*");
  Serial3.print("*R" + String(f_right) + "*");
  Serial3.print("*S" + String(f_back) + "*");
  Serial3.print("*F" + String(fotores_value) + "*");
  Serial3.print("*M" + String(micro_f) + "*");
}

void famMod() {
  if (Serial3.available()) {
    b = Serial3.read();
    movementPanel();
    startMovementBT();
    settings();
    ledControl();
    playAudio();
    headButtonsControl();
  }
  //sendState();
}

void headButtonsControl() {
  if (b == '*') {
    buttonToTouch = Serial.parseInt();
    Serial3.println(Serial3.read());
    CapacitivesUpdate(head);
    Serial3.print("buttonToTouch: ");Serial3.println(buttonToTouch);
  }
}

void movementPanel() {
  switch (b) {
    case '0':
      //resetButtons();
      if (actual_movement == no_movement) {
        if (prev_movement != idle && prev_movement != follow && prev_movement != autonomous_movement) {
          triskar.stop2();
          move = false;
        } else if (prev_movement == follow) {
          actual_movement = follow;
          prev_movement = no_movement;
        }
        else if (prev_movement == autonomous_movement) {
          actual_movement = autonomous_movement;
          prev_movement = no_movement;
        }
        else if (prev_movement == idle) {
          //startMovement(idle, rainbow_cycle);
          actual_movement = idle;
          prev_movement = no_movement;
        }
      }
      //}
      break;

    case '1':
      //Serial3.println("1-SU");
      if (!veryclose_front_obstacle && actual_movement != dontwonna) {
        if (actual_movement == idle) {
          prev_movement = idle;
          actual_movement = no_movement;
        }
        move = true;
        movementFinishTime = millis();
        triskar.run(speed_trg, 0.0);
      } else if (veryclose_front_obstacle && actual_movement != dontwonna)  startMovement(dontwonna, yellowC, color_pulse, DONT_WONNA_AUDIO);
      break;
    case '2':
      //Serial3.println("2-GIU'");
      if (!veryclose_back_obstacle && actual_movement != dontwonna) {
        if (actual_movement == idle) {
          prev_movement = idle;
          actual_movement = no_movement;
        }
        move = true;
        triskar.run(-speed_trg, 0.0);
        movementFinishTime = millis();
      } else if (veryclose_back_obstacle && actual_movement != dontwonna)  startMovement(dontwonna, yellowC, color_pulse, DONT_WONNA_AUDIO);
      break;

    case '3':
      //Serial3.println("3-SINISTRA");
      move = true;
      if (actual_movement == no_movement) {
        triskar.run(0.0, speed_trg / robot_radius);
        movementFinishTime = millis();
      } else if (actual_movement == autonomous_movement) {
        prev_movement = autonomous_movement;
        actual_movement = no_movement;
        movementFinishTime = millis();
        triskar.run(0.0, speed_trg / robot_radius);
      } else if (actual_movement == follow) {
        prev_movement = follow;
        actual_movement = no_movement;
        movementFinishTime = millis();
        triskar.run(0.0, speed_trg / robot_radius);
      } else if (actual_movement == idle) {
        prev_movement = idle;
        actual_movement = no_movement;
        movementFinishTime = millis();
        triskar.run(0.0, speed_trg / robot_radius);
      }

      break;

    case '4':
      //Serial3.println("4-DESTRA");
      move = true;
      if (actual_movement == no_movement) {
        triskar.run(0.0, -speed_trg / robot_radius);
        movementFinishTime = millis();
      } else if (actual_movement == autonomous_movement) {
        prev_movement = autonomous_movement;
        actual_movement = no_movement;
        movementFinishTime = millis();
        triskar.run(0.0, -speed_trg / robot_radius);
      } else if (actual_movement == follow) {
        prev_movement = follow;
        actual_movement = no_movement;
        movementFinishTime = millis();
        triskar.run(0.0, -speed_trg / robot_radius);
      } else if (actual_movement == idle) {
        prev_movement = idle;
        actual_movement = no_movement;
        movementFinishTime = millis();
        triskar.run(0.0, -speed_trg / robot_radius);
      }
      break;
    //Code when RIGHT key is pressed

    case '5':
      Serial3.println("5-SELECT");
      if (!select) {

        select = true;
        speed_trg -= 3;
        if (speed_trg < 0)     speed_trg = 0;
        Serial3.print("*C" + String(speed_trg) + "*");
        //          Serial3.print("Speed trg:  ");
        //          Serial3.println(speed_trg);
      }
      break;

    case '6':
      Serial3.println("6-START");
      if (!startbutton) {

        startbutton = true;
        speed_trg += 3;
        if (speed_trg > 35)   speed_trg = 35;
        Serial3.print("*C" + String(speed_trg) + "*");
        //          Serial3.print("Speed trg:  ");
        //          Serial3.println(speed_trg);
      }
      break;
  }
}

void settings() {
  switch (b) {
    case 'J': micro = true; break;
    case 'K': micro = false; break;
    case 'L': previousWorkingCapacitives = workingCapacitives; workingCapacitives = body; break;
    case 'M': previousWorkingCapacitives = workingCapacitives; workingCapacitives = noOne; break;
    case 'N': speakers = true; break;
    case 'O': speakers = false; break;
    case '7': interpreterState = choose_modality; break;
  }
}


void ledControl() {
  switch (b) {
    case 'P': headLedSetColor(red); break;
    case 'Q': headLedSetColor(yellow); break;
    case 'R': headLedSetColor(blue); break;
    case 'S': headLedSetColor(green); break;
    case 'T': headLedUpdate(led_off); head_leds = false; break;
    case 'U': head_leds = true; headLedUpdate(rainbow_cycle); break;
    case 'V': head_leds = true; headLedUpdate(color_wipe);  break;
    case 'W': head_leds = true; headLedUpdate(color_pulse); break;
    case 'X': bodyLedUpdate(redC); break;
    case 'Y': bodyLedUpdate(yellowC); break;
    case 'Z': bodyLedUpdate(blueC); break;
    case '[': bodyLedUpdate(greenC); break;
    case ']': bodyLedUpdate(led_off); body_leds = false; break;
    case '^': body_leds = true; bodyLedUpdate(rainbow_cycle); break;
    case '_': body_leds = true; bodyLedUpdate(color_wipe); break;
    case 'a': body_leds = true; bodyLedUpdate(color_pulse); break;
  }
}

void startMovementBT() {
  switch (b) {
    case '9': stopMovement(); break;
    case ':': startMovement(11); break;
    case ';': startMovement(12); break;
    case '<': startMovement(13); break;
    case '=': startMovement(14); break;
    case '>': startMovement(15); break;
    case '?': startMovement(16); break;
    case '@': startMovement(17); break;
    case 'A': startMovement(18); break;
    case 'B': startMovement(19); break;
    case 'C': startMovement(20); break;
    case 'D': startMovement(21); break;
    case 'E': startMovement(22); break;
    case 'F': startMovement(29); break;
    case 'G': stopMovement(); startMovement(follow); actual_obstacle = none; last_obstacle = none; break;
    case 'H': stopMovement(); startMovement(autonomous_movement); break;
    case 'I': startMovement(autonomous_capa); break; //START DI MOSCACIECA
    case ',': startMovement(dance); break;
    case '8': startMovement(brokeIce); break;
    case '-': startMovement(colorGame); break;  
  }
}


void playAudio() {
  switch (b) {
    case 'b': playS(9); break;
    case 'c': playS(10); break;
    case 'd': playS(11); break;
    case 'e': playS(12); break;
    case 'f': playS(13); break;
    case 'g': playS(14); break;
    case 'h': playS(15); break;
    case 'i': playS(16); break;
  }

  if (b == '%') {
    playS(Serial.parseInt());
  }
}
//  Serial3.println("7-TRIANGOLO");
//        if (!triangolo) {
//          startMovementBT();
//          triangolo = true;
//          //startMovement(make_happy[2]);
//
//          //CODICE PER ATTIVARE FOLLOWING
//          //startMovement(follow);
//          //actual_obstacle=none;
//          //last_obstacle=none;
//
//          //CODICE PER ATTIVARE AUTONOMOUS MOVEMENT
//          btMov = true; //mi segno che il comando è stato dato da bluetooth, quindi la fotoresistenza non deve intervenire.
//          if (actual_movement == no_movement || actual_movement == follow || actual_movement == idle) {
//            Serial3.println("start Autonomous Movement");
//            startMovement(autonomous_movement);
//
//          } else {
//            Serial3.println("start Following");
//            stopMovement();
//            startMovement(follow);
//            actual_obstacle = none;
//            last_obstacle = none;
//          }
//
//        }
//}
//
//        Serial3.println("8-QUADRATO");
//        if (!quadrato) {
//
//          quadrato = true;
//          startMovement(next_movement);
//          if (next_movement < make_sad2)
//            next_movement++;
//          else next_movement = make_circle;
//        }

//
//      case '9':
//        Serial3.println("9-X");
//        if (!croce) {
//
//          croce = true;
//          startMovement(make_happy2);
//          //startMovement(makeCircle);
//          //startMovement(angrymov);
//        }
//        break;
//      case 'A':
//        Serial3.println("A-CERCHIO");
//        if (!cerchio) {
//          cerchio = true;
//
//          //startMovement(make_happy[3]);
//          //CODICE PER DISATTIVARE FOLLOWING E AUTONOMOUS MOVEMENT
//          btMov = false;
//          stopAutFollow();
//
//          //CODICE PER DISATTIVARE AUTONOMOUS MOVEMENT
//          /* autonomous_movement=false;
//            move=false;
//            aut=false;*/
//        }
//        break;
//      default:
//        // default code (should never run)
//        break;
void chooseModality() {
  if (Serial3.available()) {
    b = Serial3.read();
    switch (b) {
      //      case '0':
      //        resetButtons();
      //        break;

      case '8':
        //        Serial3.println("7-TRIANGOLO");
        //        if (!triangolo) {
        //          triangolo = true;
        interpreterState = choose_game;
        Serial3.println();
        Serial3.println("Selezionare Gioco");
        playS(02);
        //        }
        break;
      case '7':
        //        Serial3.println("8-QUADRATO");
        //        if (!quadrato) {
        //          quadrato = true;
        interpreterState = fam_modality;
        previousWorkingCapacitives = workingCapacitives;
        workingCapacitives = body;
        movementFinishTime = millis();
        //        }
        break;
      case 'A':
        //        Serial3.println("A-CERCHIO");
        //        if (!cerchio) {
        //          cerchio = true;
        previousWorkingCapacitives = workingCapacitives;
        workingCapacitives = noOne;
        interpreterState = test_modality;
        //        }
        break;

      default:
        break;
    }

  }
  //sendState();
}
void chooseGame() {
  if (Serial3.available()) {
    b = Serial3.read();
    switch (b) {
      case '0':
        interpreterState = choose_scenario;
        currentGameI = 0;
        playS(firstGameAudioNumber + currentGameI);
        Serial3.print("Selezionato Gioco: "); Serial3.println(currentGameI);
        Serial3.println("Selezionare Scenario");
        //        }
        break;
      case '1':
        interpreterState = choose_scenario;
        currentGameI = 1;
        playS(firstGameAudioNumber + currentGameI);
        //        }
        break;

      case '2':
        //        Serial3.println("9-X");
        //        if (!croce) {
        //          croce = true;
        //interpreterState = sg_waiting;
        //        }
        break;
      case '3':
        //        Serial3.println("A-CERCHIO");
        //        if (!cerchio) {
        //          cerchio = true;
        //interpreterState = sg_waiting;
        //        }
        break;
      default:
        // default code (should never run)
        break;
    }

  }
  //sendState();
}

void chooseScenario() {
  if (Serial3.available()) {
    b = Serial3.read();
    switch (b) {
      case '0':
        interpreterState = sg_waiting;
        currentScenarioI = 0;
        playS(firstScenarioAudioNumber + currentScenarioI);
        Serial3.print("Selezionato Scenario:  "); Serial3.println(currentScenarioI);
        Serial3.println("Premere '1' per iniziare Gioco");
        //        }
        break;
      case '1':
        interpreterState = sg_waiting;
        currentScenarioI = 1;
        playS(firstScenarioAudioNumber + currentGameI);
        //        }
        break;

      case '2':
        //        Serial3.println("9-X");
        //        if (!croce) {
        //          croce = true;
        interpreterState = sg_waiting;
        //        }
        break;
      case '3':
        //        Serial3.println("A-CERCHIO");
        //        if (!cerchio) {
        //          cerchio = true;
        interpreterState = sg_waiting;
        //        }
        break;
      default:
        // default code (should never run)
        break;
    }
  }
  //sendState();
}

void sgWaiting() {
  if (Serial3.available()) {
    b = Serial3.read();
    switch (b) {
      //      case '0':
      //        resetButtons();
      //        break;

      case '1':
        //        Serial3.println("9-X");
        //        if (!croce) {
        //          croce = true;
        interpreterState = game_modality;
        gameState = setting;
        previousWorkingCapacitives = workingCapacitives;
        workingCapacitives = head;
        //        }
        break;

      default:
        // default code (should never run)
        break;
    }

  }
  //sendState();
}

void gameMod() {
  if (Serial3.available()) {
    b = Serial3.read();
    switch (b) {
      //      case '0':
      //        resetButtons();
      //        break;

      default:
        // default code (should never run)
        break;
    }

  }
  //sendState();
}

void disCharge() {
  //sendState();
}

void btInterpreter() {
  switch (interpreterState) {
    case choose_modality: chooseModality(); break;
    case choose_game:     chooseGame(); break;
    case choose_scenario: chooseScenario(); break;
    case sg_waiting:      sgWaiting(); break;
    case game_modality:   gameMod(); break;
    case fam_modality:    famMod(); break;
    case discharge:       disCharge(); break;
    case test_modality:   hardwareTests(); break;
  }
}


