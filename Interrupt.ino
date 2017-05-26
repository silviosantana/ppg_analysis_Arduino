void interruptSetup(){     
  // Initializes Timer1 to throw an interrupt every 20mS.
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;

  OCR1A = 1249;            // compare match register 16MHz/256/50Hz
  TCCR1B |= (1 << WGM12);   // CTC mode
  TCCR1B |= (1 << CS12);    // 256 prescaler 
  TIMSK1 |= (1 << OCIE1A);  // enable timer compare interrupt 
  sei();         // MAKE SURE GLOBAL INTERRUPTS ARE ENABLED     
} 

// THIS IS THE TIMER 1 INTERRUPT SERVICE ROUTINE.
// Timer 1 makes sure that we take a reading every x miliseconds
//ISR(TIMER2_COMPA_vect){                         // triggered when Timer2 counts to 124
ISR(TIMER1_COMPA_vect){                        // triggered when Timer1 counts to ORC1A
  cli();                                      // disable interrupts while we do this
  signalA_in = analogRead(SENSOR_A_PIN);
  //Serial.println(signal_in);

  if (cTime > 500){//count 10s before start reading
    if (data_counter < LENGTH){
      ledFlag = HIGH;
      data_b1[data_counter] = signalA_in;
      data_b2[data_counter] = signalA_in;
      //v_data[data_counter] = (float) signal_in * VOLTAGE_STEP;
      temp = data_b1[data_counter];
      
      data_counter++;
    }else{
      if (!cap_flag){
        write_flag = true;
        cap_flag = HIGH;
      }
      cap_flag = HIGH;
      ledFlag = LOW;
    }
  }else{
    cTime = cTime + 1;
      
  }
  
  sei();                                   // enable interrupts when youre done!
}// end isr
