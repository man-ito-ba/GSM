// ****************************************************************************
// *									GSM								   *
// ****************************************************************************

// Initialisation de la librairie GSM
#include <GSM.h>
// Création des éléments de la librairie
GSM gsmAccess;
GSM_SMS sms;
// Code PIN à indiquer rentre les guillemets si nécessaire
#define CodePIN ""
// Tableau destiné à conserver le numéro de l'expéditeur d'un SMS reçu par le shield
char Num_Expediteur[20];

bool SMS_LED;			// variable qui permet l'allumage extinction de la led par texto
bool SMS_LEDprecedent;	// variable est destiné à l'état précédent de la LED


// ****************************************************************************
// *								 Matériel								 *
// ****************************************************************************

// ======== LED & variables nécessaires à son fonctionnement
const int LED = 12;
bool LED_Etat;

// ======== Bouton & variables associées à son bon fonctionnement
const int Bouton = 4;				// Bouton branché au pin 3
bool Bouton_Etat;					// Booléenne enregistrant l'état du bouton par la fonction digitalRead()
bool Bouton_EtatPrecedent = HIGH;	// Booléenne enregistrant l'état précédant du bouton
									// En l'occurence il est "HIGH" car, branché en INPUT_PULLUP, c'est ce qu'il renvoi en digitalRead
long Bouton_Temps = 0;				// La dernière fois que le bouton a été pressé
long Bouton_Debounce = 200;			// Laps de temps que l'on estime nécessaire pour effectuer l'enregistrement du nouvel état du bouton (lorsqu'il est pressé). Après avec les bons composants électronique, on peut éviter les effets de rebonds du signal.



void setup()
{
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

	Reception();										// Avant de commencer le programme, au depart, on vériie s'il y a déjà des messages en attente

	// la led s'éteint et marque la fin du setup()
	digitalWrite(LED, LOW);
}

void loop()
{
	Reception();
	EtatDuBouton();
	LEDallumageBouton();

	// Délai nécessaire pour que le programme tourne sans souci
	delay(10);
}

void Recapitulatif_Instructions(){
	// Une fonction appelée par la réception de la suite "1234567890" ou "Instructions" 
	char InstructionsUtilisateur[] = 
		"Instructions";
	// Envoi effectif des instructions
	sms.beginSMS(Num_Expediteur);
	sms.print(InstructionsUtilisateur);
	sms.endSMS();
}

void Reception(){
	char Texto_LPL;		// Ici, en fait il s'agit du texto "lettre par lettre" ("LPL") qui est affiché

	// Si un SMS est reçu()
	if (sms.available()) {
		Serial.println("Message en cours de reception...");
		delay(1000);
		// Numéro expéditeur (Ø nécessaire ici)
			// Serial.println("Expediteur du SMS :");
			// Obtention et affichage du numéro de l'expéditeur du SMS
			// sms.remoteNumber(Num_Expediteur, 20);
			// Serial.println(Num_Expediteur);
	
		/* Messages qui seront automatiquement supprimés */
			// Un exemple de supression de message
			// Ex. : tout message commençant par un "#" devrait être effacé
			if (sms.peek() == '#') {
				Serial.println("Elimination du SMS");
				sms.flush();
			}
		// Lecture des bytes du message et affichage sur le moniteur série
		while (Texto_LPL = sms.read()) {		// Jusqu'à ce que les bytes lu par la fonction read() correspondent à la variable "Texto_LPL"
			Serial.print(Texto_LPL);
		}
	
		Serial.println("\nFin du message");
		delay(1000);

		// Suppression des messages de la memoire du modem
		sms.flush();
		Serial.println("Suppression du message OK");
	}

	// S'il n'y a aucun SMS
	if(!sms.available()){
		Serial.println("Pas de nouveau message");
	}
	delay(1000);
}

void EtatDuBouton(){
	Bouton_Etat = digitalRead(Bouton);	// Cette fonction permet à un bouton d'allumer la LED
}

void LEDallumageBouton(){
	// Si on appuie sur le bouton, et que son état précédent était "pas appuyé", et qu'on aisse suffisamment de temps
	if(Bouton_Etat == LOW && Bouton_EtatPrecedent == HIGH && millis() - Bouton_Temps > Bouton_Debounce){
		if(LED_Etat == HIGH){			// et que la LED est déjà allumée
			LED_Etat = LOW;				// on passe l'état (et non pas a led) à "éteint"
		}
		else {
			LED_Etat = HIGH;			// et vice-versa
		}
	}
	digitalWrite(LED, LED_Etat);		// et on attribue à la LED a valeur de État_led
	Bouton_EtatPrecedent = Bouton_Etat;	// sans oublier d'enregistrer l'état du bouton
}

void LEDallumageSMS(){

}

void LedAllumee(){
	// Cette fonction informe l'utilisateur que la Led a été allumée, que ce soit à distance ou par bouton
}