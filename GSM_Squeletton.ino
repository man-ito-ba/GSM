// GitHub : https://github.com/man-ito-ba/projet_GSM
/* Abbréviations
	#MU : élément à inclure dans le Manuel Utilisateur
	*/

// Numéro de la SIM : 0688364880

// Initialisation de la librairie GSM
#include <GSM.h>

// Création des éléments de la librairie
GSM gsmAccess;
GSM_SMS sms;

// Code PIN à indiquer rentre les guillemets si nécessaire
#define CodePIN ""

// Tableau destiné à conserver le numéro de l'expéditeur d'un SMS reçu par le shield
char Num_ExpediteurSMS[20];
char Num_Utilisateur[] = "+33603353147";

bool SMS_LED;			// variable qui permet l'allumage / extinction de la led par texto
bool SMS_PrecedentLED;	// variable est destiné à enregistrer l'état précédent de la LED

// Gestion du temps
unsigned long MillisPrecedent = 0;
unsigned long MillisInterval  = 1000;
unsigned long MillisActuel;

// ======== Différents contenus de SMS
char Message_Transmis;

// Identifiant du système
char SystemeID[] = "--Lampe UV de Brignoles--";

// Message de démarrage
char TextSMS_MiseEnMarche[] = "Systeme actif. Acces au reseau GSM : OK.";

// Instructions utilisateur
char InstructionsUtilisateur[] = "Instructions :\n0 : Recevoir l'ensemble des instructions ;\n1 : Allumer/Eteindre la LED ;\n2 : Obtenir l'etat de la LED ;\n3 : Verifier le temps d'utilisation de l'Arduino.";

// Messages de fonctionnement
char TextSMS_SmsLedOff[]        = "Commande SMS : LED OFF";
char TextSMS_SmsLedOn[]         = "Commande SMS : LED ON";
char TextSMS_BoutonLedOff[]     = "Appui sur bouton : LED OFF";
char TextSMS_BoutonLedOn[]      = "Appui sur bouton : LED ON";
char TextSMS_EtatLedOn[]        = "Controle d'etat : la LED est ON";
char TextSMS_EtatLedOff[]       = "Controle d'etat : la LED est OFF";
char TextSMS_TempsUtilisation[] = "L'arduino tourne depuis ";

// Message d'alerte
char TextSMS_ErreurInstruction[]    = "Attention, votre instruction n'est pas correcte. Envoyez '0' pour obtenir les instructions valides.";
char TextSMS_AlertePotentiometre[]  = "Attention, le niveau du potentiometre vient d'attendre 75 pour cent.";

// ==== Matériel
// ======== LED & variables nécessaires à son fonctionnement
const int LED = 12;
bool LED_Etat;

// ======== Bouton & variables associées à son bon fonctionnement
const int Bouton = 4;			 	// Bouton branché au pin 3
bool Bouton_Etat;				 	// Booléenne enregistrant l'état du bouton par la fonction digitalRead()
bool Bouton_EtatPrecedent = HIGH;	// Booléenne enregistrant l'état précédant du bouton
					// En l'occurence il est "HIGH" car, branché en INPUT_PULLUP, c'est ce qu'il renvoi en digitalRead
long Bouton_Temps    = 0;			// La dernière fois que le bouton a été pressé
long Bouton_Debounce = 200;			// Laps de temps que l'on estime nécessaire pour effectuer l'enregistrement du nouvel état du bouton (lorsqu'il est pressé). Après avec les bons composants électronique, on peut éviter les effets de rebonds du signal.

int Potentiometre;					// Variable servant à compter (simu à voir plus bas)


// ****************************************************************************
// *                                   Setup                                  *
// ****************************************************************************

void setup()
{
	// ======= Mise à zéro des variabes
	SMS_LED, SMS_PrecedentLED,
	Message_Transmis,
	Potentiometre,
	Bouton_Etat,
	LED_Etat,
	MillisActuel
	= 0;

	// ======= INITIALISATION MATÉRIEL
	pinMode(Bouton, INPUT_PULLUP);

	pinMode(LED, OUTPUT);
	digitalWrite(LED, HIGH);							// La LED est ON, marquant le début du setup*()
	
	// ======= Initialisation moniteur série
	Serial.begin(9600);
	while (!Serial) {
		; 												// On attend que le port Serial se connecte, ce qui est surtout indispensable seulement pour le port USB natif
	}

	bool PasDeConnection = true;						// Bool d'état de la connection

	while (PasDeConnection) {							// Démarrage du shield GSM
		Serial.println(F("-- Recherche du reseau..."));
		delay(500);										// Est-ce que je peux pas allonger pour laisser du temps en modem ?
		if (gsmAccess.begin(CodePIN) == GSM_READY) {	// Si la SIM a un code PIN, le passer en tant que paramètre de begin(), entre guillemets
			delay(500);
			PasDeConnection = false;
		} else {
			Serial.println(F("-- Pas de connection"));
			delay(1000);
		}
	}

	// Au démarrage, les textos en attente dans le modem sont toujours effacés pour éviter les conflits. #MU
	EffacementDesSMS();

	delay(1000);

	// ======= LANCEMENT PROGRAMME
	Serial.println(F("**************************"));
	Serial.println(F("** Programme SMS in/out **"));
	Serial.println(F("**************************"));

	digitalWrite(LED, LOW);								// la led s'éteint et marque la fin du setup()
	Envoi(TextSMS_MiseEnMarche);						// On envoi un texto au client une fois le système activé.
}


// ****************************************************************************
// *                                   Loop                                   *
// ****************************************************************************

void loop()
{
	EliminationMauvaiseInstructions();

	InstructionSMS();

	InstructionBouton();
	
	AllumageExtinctionLED();

	delay(10);				// Délai nécessaire pour que le programme tourne sans souci
}

// ****************************************************************************
// *                                 Reception                                *
// ****************************************************************************

void EliminationMauvaiseInstructions(){
	if(sms.available()){									// Avec cette boucle, s'il y a quoi que ce soit de dispo dans le buffer du modem, je le lis
		Serial.println(F("\nSms recu du "));
		sms.remoteNumber(Num_ExpediteurSMS, 20);			// On stock le numéro de l'expéditeur (il y a de grandes chances que ce soit le numéro de l'utilisateur préenregistré)
		Serial.println(Num_ExpediteurSMS);

		// if(Num_ExpediteurSMS == Num_Utilisateur){
			// 	Serial.println(F("Numero Compatible !"));
			// }
			// else{
			// 	Serial.println(F("Numero pas compatible"));
			// }

		// Première lettre du texto reçu = lettre, symbole...
		if (sms.peek() < 48 || sms.peek() > 57)
		{													// On utilise les codes ASCII des caractères
			Serial.print(F("Instruction erronee : "));
			Envoi(TextSMS_ErreurInstruction);
			EffacementDesSMS();
		}
	}
}

void InstructionSMS(){
	// ======= Si on a reçu un texto
	if(sms.available()){									// Avec cette boucle, s'il y a quoi que ce soit de dispo dans le buffer du modem, je le lis
		// ======= Lecture du choix de l'expéditeur
		int Choix_Action = sms.parseInt();					// on cherche un entier ("Int")
		Serial.println(Choix_Action);

		// ======= Instructions
		if(Choix_Action == 0){
			Envoi(InstructionsUtilisateur);					// 0 : instructions utilisateurs
		}
		else if(Choix_Action == 1){							// 1 : Allumer / éteindre LED
			SMS_LED = HIGH;									// On passe la booléenne de la LED en high
			LEDallumageSMS(SMS_LED);
		}
		else if(Choix_Action == 2){
			EtatDeLaLED();
		}

		EffacementDesSMS();
	}
}

void EffacementDesSMS(){
	while(sms.available() > 0)
	{
	    sms.flush();			// avec while(), on tourne tant que le ou les SMS reçus n'ont pas tous été effacés
	}
	Serial.println(F("(Message Efface !)"));
}


// ****************************************************************************
// *                              LED                                         *
// ****************************************************************************

void InstructionBouton(){
	Bouton_Etat = digitalRead(Bouton);			// Cette fonction permet à un bouton d'allumer la LED

	if(Bouton_Etat == LOW && Bouton_EtatPrecedent == HIGH && millis() - Bouton_Temps > Bouton_Debounce){	// Si on appuie sur le bouton, et que son état précédent était "pas appuyé", et qu'on ai laissé suffisamment de temps
		if(LED_Etat == HIGH){		 			// et que la LED est déjà allumée
			LED_Etat = LOW;			 			// on passe l'état (et non pas a led) à "éteint"
			delay(100);
			Envoi(TextSMS_BoutonLedOff);
		}
		else {
			LED_Etat = HIGH;					// et vice-versa
			delay(100);
			Envoi(TextSMS_BoutonLedOn);
		}
	}
}

void LEDallumageSMS(bool SMS_LED){
		if(LED_Etat == HIGH){
			LED_Etat = LOW;
			delay(100);
		    Envoi(TextSMS_SmsLedOff);
		}
		else {
			LED_Etat = HIGH;
			delay(100);
			Envoi(TextSMS_SmsLedOn);
		}
}

void AllumageExtinctionLED(){
	digitalWrite(LED, LED_Etat);				// On attribue à la LED a valeur de État_led
	Bouton_EtatPrecedent = Bouton_Etat; 		// sans oublier d'enregistrer l'état du bouton
}

void EtatDeLaLED(){
	if(LED_Etat){					// Cette fonction sert à indiquer à l'utilisateur l'état actuel de la LED
	    Envoi(TextSMS_EtatLedOn);
	}
	else{
		Envoi(TextSMS_EtatLedOff);
	}
}


void TempsUtilisationArduino(){
	return;
	MillisActuel = millis();
	// int Hours   =   Milliseconds / (1000*60*60);
	// int Minutes =  (Milliseconds % (1000*60*60)) / (1000*60);
	// int Seconds = ((Milliseconds % (1000*60*60)) % (1000*60)) / 1000;
	//Envoi(TextSMS_TempsUtilisation, MillisActuel);
}

// ****************************************************************************
// *                                   Envoi                                  *
// ****************************************************************************

void Envoi(char Message[]){
	Serial.print(F("[Envoi du message] "));
	Serial.println(SystemeID);
	Serial.print(Message);

	sms.beginSMS(Num_Utilisateur);
	sms.print(SystemeID);
	sms.print(Message);

	sms.endSMS();
	Serial.println(F(" [X]"));
}

void ComparaisonNumExpediteurNumUtilisateur(char Num_ExpediteurSMS){
	return;
	for(int i = 0; i < 11; i++){
		// Num_Utilisateur_Index = Num_Utilisateur[i];
		// Num_ExpediteurIndex   = Num_ExpediteurSMS[i];
	}
}

// void Reception(){
	// 	char Texto_LPL;		// Ici, en fait il s'agit du texto "lettre par lettre" ("LPL") qui est affiché

	// 	// Si un SMS est reçu()
	// 	if (sms.available()) {
	// 		Seria.println("Message en cours de reception...");
	// 		delay(1000);
		
	// 		Serial.println("Expediteur du SMS :");
		
	// 		// Obtention et affichage du numéro de l'expéditeur du SMS
	// 		sms.remoteNumber(Num_ExpediteurSMS, 20);
	// 		Serial.println(Num_ExpediteurSMS);
		
	// 		/* Messages qui seront automatiquement supprimés */
	// 			// Un exemple de supression de message
	// 			// Ex. : tout message commençant par un "#" devrait être effacé
	// 			if (sms.peek() == '#') {
	// 				Serial.println("Elimination du SMS");
	// 				sms.flush();
	// 			}

	// 		// Lecture des bytes du message et affichage sur le moniteur série
	// 		while (Texto_LPL = sms.read()) {		// Jusqu'à ce que les bytes lu par la fonction read() correspondent à la variable "c"
	// 			Serial.print(Texto_LPL);
	// 		}
			
	// 		Serial.println("\nFin du message");
			
	// 		// Suppression des messages de la memoire du modem
	// 		sms.flush();
	// 		Serial.println("Suppression du message OK");
			
	// 		delay(1000);
	// 	}

	// 	// S'il n'y a aucun SMS
	// 	if(!sms.available()){
	// 	    Serial.println("Pas de nouveau message");
	// 	}

	// 	delay(1000);
	// }