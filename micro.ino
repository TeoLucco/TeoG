<<<<<<< HEAD

=======
void getDownMicroFilter(){
  for(int i=0;i<1500;i++) //metto 0 per n volte al fine di abbassare il valore del filtro
      microLowpassFilter.input(0);
}
>>>>>>> parent of 699a0ee... securitycommit

void microLoop(){
  if( (millis()-movementFinishTime>2000) && (actual_movement==no_movement) &&(gameState==no_game) && (interpreterState==fam_modality) && digitalRead(BUSY_PIN)==HIGH && triskar.isStopped()){
    int wave = analogRead(soundPin);//read the value of A0. range 0-1024
    int value= abs(wave-512); //range 0-512
    microLowpassFilter.input(value);

<<<<<<< HEAD
  int wave = analogRead(soundPin);//read the value of A0. range 0-1024
  int value = abs(wave - 512); //range 0-512
  microLowpassFilter.input(value);
  if ( (millis() - movementFinishTime > 2000) && ((actual_movement==no_movement || actual_movement==idle)) && (gameState == no_game) && (interpreterState == fam_modality) && digitalRead(BUSY_PIN) == HIGH && triskar.isStopped()) {
    if (microLowpassFilter.output() > microSoglia) { //if the value is greater than 384 on 512
=======
    if(microLowpassFilter.output() > microSoglia){ //if the value is greater than 384 on 512
>>>>>>> parent of 699a0ee... securitycommit
      microI++;
      lastadd=millis();
    }
    if(microI>=microISequence){ //if the value is greater than 384 on 512
      startMovement(scared_behind);
      microLowpassFilter.setToNewValue(0.0);
<<<<<<< HEAD
      microI = 0;
    } else if (microLowpassFilter.output() < microSoglia && (microI >= microISequenceShortMin && microI <= microISequenceShortMax)) {
=======
      //getDownMicroFilter();
      microI=0;
    }else
    if(microLowpassFilter.output() < microSoglia && (microI>=microISequenceShortMin && microI<=microISequenceShortMax)){
>>>>>>> parent of 699a0ee... securitycommit
      startMovement(scared_round);
      microLowpassFilter.setToNewValue(0.0);
      microI=0;
    } 
  }else
  microLowpassFilter.setToNewValue(0.0);
  if(millis()-lastadd>500){
      microI=0;
  }
}
