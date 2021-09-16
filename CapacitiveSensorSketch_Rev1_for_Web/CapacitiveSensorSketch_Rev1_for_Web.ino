
/*
 * Uses a high value resistor e.g. 10M between send pin and receive pin
 * Resistor effects sensitivity, experiment with values, 50K - 50M. Larger resistor values yield larger sensor values.
 * Receive pin is the sensor pin - try different amounts of foil/metal on this pin
 */

//NODEMCU board.
#define SEND_PIN  D2
#define RECEIVE_PIN D6

//328 based.  Note that only some pins can have interrupts attached
//#define SEND_PIN  4
//#define RECEIVE_PIN 2

/* capTimer: time of changed state.  Set to 0 to take a new reading.
 * capLast: the last recorded capacitance
 */
volatile unsigned long capTimer = 0;
volatile unsigned long capLast = 0;

void setup() {
  Serial.begin(115200);
  Serial.print("Booted");

  // Set LED to off (internal LED is inverted)
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  //capacitive input setup
  pinMode(SEND_PIN, OUTPUT);
  digitalWrite(SEND_PIN, LOW);
  pinMode(RECEIVE_PIN, INPUT);
  delay(100);
  // Set receive pin as interrupt, assign interrupt function and set RISING mode
  attachInterrupt(digitalPinToInterrupt(RECEIVE_PIN), receivePinISR, RISING);
  capSensorSetup();

}//end setup

void loop() {

  if (capTimer == 0) {
    //discard very small values, as we are interested in large fluctuations caused
    //by EMI from the AC sine wave
    if (capLast > 50) {
      digitalWrite(LED_BUILTIN, HIGH);
      Serial.println(capLast);
    }
    capSensorSetup();
  }
  else if (micros() - capTimer > 10000) {
    //check to see if we missed the interrupt for some reason
    if (digitalRead(RECEIVE_PIN) == HIGH) {
      digitalWrite(LED_BUILTIN, LOW);
      Serial.println("Aborting sensor read");
      capSensorSetup();
    }
  }
  

}//end loop


/*
 * set up the capacitive sensor pin to take a reading
 */
void capSensorSetup() {
  //DEBUG_PRINT("capSensorSetup()");
  digitalWrite(SEND_PIN, LOW);
  delayMicroseconds(10);
  /*  Here's a hacky solution for taking a reading at the same point 
   *  in the AC sine wave (at 50Hz) - use this if you actually want to 
   *  measure capacitance, as opposed to EMI, which is more stable
   *  in my testing.  See blog post.
  if (capLast < 20000) {
    delayMicroseconds(20000-capLast);
    }
  else {
    delayMicroseconds(20000);
    }*/
  capTimer = micros();
  digitalWrite(SEND_PIN, HIGH);
}


// Interrupt Service Routine for capacitive sensor
ICACHE_RAM_ATTR void receivePinISR() {  //NODEMCU
//void receivePinISR() {                //328 based arduino
    //if capTimer has already been set to 0 by this routine, don't run it twice
    if (capTimer != 0) {
        capLast = micros() - capTimer;
        capTimer = 0;
    }
} //End ISR
  
