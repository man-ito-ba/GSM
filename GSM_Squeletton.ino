// ****************************************************************************
// *                                    GSM                                   *
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


// ****************************************************************************
// *                                 Matériel                                 *
// ****************************************************************************

// ======== LED & variables nécessaires à son fonctionnement
const int LED = 13;
bool LED_Etat;

// ======== Bouton & variables associées à son bon fonctionnement
const int Bouton = 3;				// Bouton branché au pin 3
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
	bool notConnected = true;

	// Démarrage du shield GSM
	// Si la SIM a un code PIN, le passer en tant que paramètre de begin(), entre guillemets
	while (notConnected) {
		Serial.println("-- Recherche du reseau...");
		if (gsmAccess.begin(CodePIN) == GSM_READY) {
			notConnected = false;
		} 
		else {
			Serial.println("-- Pas de connection");
			delay(1000);
		}
	}

	// ======= LANCEMENT PROGRAMME
	Serial.println("**************************");
	Serial.println("** Programme SMS in/out **");
	Serial.println("**************************");

	Reception();										// Avant de commencer le programme, au depart, on vériie s'il y a déjà des messages en attente

	// la led s'éteint et marque la fin du setup()
	digitalWrite(LED, LOW);
}

void loop()
{
	Reception();
	EtatDuBouton();
	AllumageDeLaLED();

	// Délai nécessaire pour que le programme tourne sans souci
	delay(10);
}

void Reception(){
	char Texto_LPL;		// Ici, en fait il s'agit du texto "lettre par lettre" ("LPL") qui est affiché

	// Si un SMS est reçu()
	if (sms.available()) {
		Seria.println("Message en cours de reception...");
		delay(1000);
		Serial.println("Expediteur du SMS :");
		// Obtention et affichage du numéro de l'expéditeur du SMS
		sms.remoteNumber(Num_ExpediteurSMS, 20);
		Serial.println(Num_ExpediteurSMS);
		/* Messages qui seront automatiquement supprimés */
			// Un exemple de supression de message
			// Ex. : tout message commençant par un "#" devrait être effacé
			if (sms.peek() == '#') {
				Serial.println("Elimination du SMS");
				sms.flush();
			}
		// Lecture des bytes du message et affichage sur le moniteur série
		while (Texto_LPL = sms.read()) {		// Jusqu'à ce que les bytes lu par la fonction read() correspondent à la variable "c"
			Serial.print(Texto_LPL);
		}
		Serial.println("\nFin du message");
		// Suppression des messages de la memoire du modem
		sms.flush();
		Serial.println("Suppression du message OK");
		delay(1000);
	}
	// S'il n'y a aucun SMS
	if(!sms.available()){
	    Serial.println("Pas de nouveau message");
	}
	delay(1000);
	Instructions();
}

void EtatDuBouton(){
	Bouton_Etat = digitalRead(Bouton);
}

void AllumageDeLaLED(){
	if(Bouton_Etat == LOW && Bouton_EtatPrecedent == HIGH && millis() - Bouton_Temps > Bouton_Debounce){
		if(LED_Etat == HIGH){
			LED_Etat = LOW;
		}
		else {
			LED_Etat = HIGH;
		}
	}
	digitalWrite(LED, LED_Etat);
	Bouton_EtatPrecedent = Bouton_Etat;
}