#include "SIM900.h"
#include <SoftwareSerial.h>
#include "call.h"


//Classes for the gsm libs
CallGSM call;


char phone_number[20];
char user_phone_number[] = "XXXXXXXXXXXXX"; //DEFINE YOUR USER CELLPHONE NUMBER

//Flag to just inform the user one time
boolean user_informed_movement = false;


byte trigPin = 3; //Pin from HC-SR04 "Trig"
byte echoPin = 4; //Pin from HC-SR04 "Echo"


void setup() {


  //Define HC-SR04 pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT); 
  pinMode(GSM_ON, OUTPUT); //Pinmode for Digital Pin 9

  Serial.begin(9600); //Start a Serial COM
  
  Serial.println(F("GSM ALARM V1.0"));
  
  Serial.print(F("Starting GSM COM..."));

  if (gsm.begin(9600)) //Start the GSM COM
  {
    Serial.println(F("ready"));
  }
  else
  {
    Serial.println(F("Connexion au modem GSM impossible"));
  }


  //Wait for the GSM to register in an network
  while (gsm.CheckRegistration() != 1) {

    Serial.println(F("En attente de l'enregistrement reseau"));
    delay(5000);
  }

}

void loop() {

  //First lets see if we have a call to reset the "already informed user"
  if (user_informed_movement==true) check_call();

  //Check for movement
  //If we have movement and the user is still not informed, lets do a call.
  if (movement() && user_informed_movement == false)
  {
    Serial.println(F("Movement"));

    //If there is no active call, we can do a call
    if (call.CallStatus() != CALL_ACTIVE_VOICE) {

      call.Call(user_phone_number);

      delay(15000); //Give some time for the call to take place
      call.HangUp();

      //User is already informed of movement
      user_informed_movement = true;

    }
  }

}

//******************************************************************************
//***************************    CHECK CALL    *********************************
//******************************************************************************

void check_call()
{

  //Check call from an any number
  byte stat = call.CallStatusWithAuth(phone_number, 0, 0);


  //A call if being placed
  if (stat == CALL_INCOM_VOICE_AUTH) {
    //Hang up the call.
    call.HangUp();

    delay(1000);

    //Compare the phone number from the call placer to the user number
    if (strcmp(phone_number, user_phone_number) == 0)
    {
      //Reset warning flag
      user_informed_movement = false;
    }
  }

}

//******************************************************************************
//****************************    MOVEMENT    **********************************
//******************************************************************************

byte movement()
{

  long duration, distance;

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);

  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);

  //Calculate the distance (in cm) based on the speed of sound.
  distance = duration / 58.2;

  Serial.print(F("Dist:"));
  Serial.println(distance);

  delay(50);

  if (distance >= 80 || distance <= 0)  return 0; //Ajust this values according to your needs

  else
  {
    return 1;
  }

}