// ****************************************************************************
// *									GSM									*
// ****************************************************************************

// Initialisation de la librairie GSM
#include <GSM.h>

// Création des éléments de la librairie
GSM gsmAccess;
GSM_SMS sms;

// Code PIN à indiquer rentre les guillemets si nécessaire
#define CodePIN ""

// Tableau destiné à conserver le numéro de l'expéditeur d'un SMS reçu par le shield
char Num_ExpediteurSMS[20];
char Num_Utilisateur[] = "0603353147";

bool SMS_LED;			// variable qui permet l'allumage extinction de la led par texto
bool SMS_PrecedentLED;	// variable est destiné à l'état précédent de la LED

// ======== Différents contenus de SMS

// Identifiant du système
char SystemeID[] = "Lampe UV de Brignoles : ";

// Message de démarrage
char TextSMS_MiseEnMarche[] = "le systeme est actif. Acces au reseau GSM : OK.";

// Messages de fonctionnement
char TextSMS_ActivationLED_OFF[] = "La LED est OFF";
char TextSMS_ActivationLED_ON[]  = "La LED est ON";

// Message d'alerte
char TextSMS_ErreurInstruction[]    = "Attention, votre instruction n'est pas correcte. Envoyez '0' pour obtenir les instructions valides.";
char TextSMS_AlertePotentiometre[]  = "Attention, le niveau du potentiometre vient d'attendre 75 pour cent.";

// ****************************************************************************
// *								 Matériel								*
// ****************************************************************************

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


void setup()
{
	// ======= Mise à zéro des variabes
	Potentiometre,
	LED_Etat
	= 0;

	// ======= INITIALISATION MATÉRIEL
	pinMode(Bouton, INPUT_PULLUP);

	pinMode(LED, OUTPUT);
	digitalWrite(LED, HIGH);
	
	// ======= INITIALISATION GSM
	// Initialisation des communication avec le moniteur serie et attente de l'ouverture du port
	Serial.begin(9600);
	while (!Serial) {
	; // On attend que le port Serial se connecte, ce qui est surtout indispensable seulement pour le port USB natif
	}

	// Bool d'état de la connection
	bool PasDeConnection = true;

	// Démarrage du shield GSM
	// Si la SIM a un code PIN, le passer en tant que paramètre de begin(), entre guillemets
	while (PasDeConnection) {
	Serial.println("-- Recherche du reseau...");
	if (gsmAccess.begin(CodePIN) == GSM_READY) {
		PasDeConnection = false;
	} else {
		Serial.println("-- Pas de connection");
		delay(1000);
	}
	}

	// ======= LANCEMENT PROGRAMME
	Serial.println("**************************");
	Serial.println("** Programme SMS in/out **");
	Serial.println("**************************");

	delay(1000);

	// la led s'éteint et marque la fin du setup()
	digitalWrite(LED, LOW);
	// On envoi un texto au client une fois le système activé.
	Envoi(TextSMS_MiseEnMarche);
}

void loop()
{
	SimuationPotentiometreParBouton();
	Reception();
	EtatDuBouton();
	LEDallumageBouton();
	ChangementEtatLED();	// Une fonction pour vérifier comment si la LED change d'état afin d'activer un envoi de texto ?

	// Délai nécessaire pour que le programme tourne sans souci
	delay(10);
}

void Recapitulatif_Instructions(){
	// Une fonction appelée par la réception de la suite "1234567890" ou "Instructions" 
	char InstructionsUtilisateur[] = 
	"Instructions";
	// Envoi effectif des instructions
	sms.beginSMS(Num_ExpediteurSMS);
	sms.print(InstructionsUtilisateur);
	sms.endSMS();
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

void SimuationPotentiometreParBouton(){
	// // Une fonction d'incrémentation d'un bouton, qui tourne en boucle, juste pour simuler un potentiometre que j'ai pas...
	// // La boucle va jusqu'à un niveau requis
	// Potentiometre = Potentiometre + 1;
	// if(Potentiometre > 10000 && Potentiometre < 10002){
	//     Envoi(TextSMS_AlertePotentiometre);
	// }
}

void Reception(){
	// ======= Si on a reçu un texto
	// if ou while > 0 ?
	if(sms.available()){							// Avec cette boucle, s'il y a quoi que ce soit de dispo dans le buffer du modem, je le lis
		sms.remoteNumber(Num_ExpediteurSMS, 20);		// On stock le numéro de l'expéditeur (il y a de grandes chances que ce soit le numéro de l'utilisateur préenregistré)
		Serial.println("Sms recu du ");
		Serial.println(Num_ExpediteurSMS);

		if (sms.peek() < 48 || sms.peek() > 57){ 	// Si la première lettre du texto reçu est autre chose qu'un chiffre (lettre, symbole, etc.)
			Serial.print("Erreur dans l'instruction recue : ");
			Serial.println(TextSMS_ErreurInstruction);
			Envoi(TextSMS_ErreurInstruction);
		}

		// ======= On lit le choix fait par l'expéditeur du message
		int Choix_Action = sms.parseInt();			// on cherche un entier ("Int")
		Serial.println(Choix_Action);

		// ======= Instructions
		if(Choix_Action == 0){
			Recapitulatif_Instructions();			// On envoie le récapitulatif des instructions
		}
		else if(Choix_Action == 1){
			SMS_LED = HIGH;				// On passe la booléenne de la LED en high
			LEDallumageSMS(SMS_LED);
		}
		else{
			Envoi(TextSMS_ErreurInstruction);
		}

		sms.flush();							// On efface le message

		delay(10);
	}
}

void EtatDuBouton(){
	Bouton_Etat = digitalRead(Bouton);	// Cette fonction permet à un bouton d'allumer la LED
}

void LEDallumageBouton(){
	// Si on appuie sur le bouton, et que son état précédent était "pas appuyé", et qu'on ai laissé suffisamment de temps
	if(Bouton_Etat == LOW && Bouton_EtatPrecedent == HIGH && millis() - Bouton_Temps > Bouton_Debounce){
		if(LED_Etat == HIGH){		 // et que la LED est déjà allumée
			LED_Etat = LOW;			 // on passe l'état (et non pas a led) à "éteint"
			Envoi(TextSMS_ActivationLED_OFF);
		}
		else {
			LED_Etat = HIGH;			// et vice-versa
			Envoi(TextSMS_ActivationLED_ON);
		}
	}
	digitalWrite(LED, LED_Etat);		// et on attribue à la LED a valeur de État_led
	Bouton_EtatPrecedent = Bouton_Etat; // sans oublier d'enregistrer l'état du bouton
}

void LEDallumageSMS(bool SMS_LED){
	//if(SMS_LED == HIGH && SMS_PrecedentLED == LOW){
		if(LED_Etat == HIGH){
		    LED_Etat = LOW;
		    Envoi(TextSMS_ActivationLED_OFF);
		    SMS_PrecedentLED = !SMS_LED;
		}
		else {
			LED_Etat = HIGH;
			Envoi(TextSMS_ActivationLED_ON);
			SMS_PrecedentLED = !SMS_LED;
		}
	//}
	digitalWrite(LED, LED_Etat);
	//SMS_PrecedentLED = SMS_LED;
}

void ChangementEtatLED(){

}

void Envoi(char Message[]){
	Serial.print("Envoi du message : ");
	Serial.println(Message);

	sms.beginSMS(Num_Utilisateur);
	sms.print(SystemeID);
	sms.print(Message);

	sms.endSMS();
	Serial.println("Envoi OK !\n");
}