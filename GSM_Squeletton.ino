// GitHub : https://github.com/man-ito-ba/projet_GSM
/* Abbréviations
	#MU : élément à inclure dans le Manuel Utilisateur
	*/

// Numéro de la SIM : 0688364880
// Étape importante : configurer la bande de fréquence utilisée par le modem, en lançant le croquis "BandManagement" #MU

// Librairie de watchdog, utilisée pour la fonction de reset
// #include <avr/wdt.h>

// Initialisation de la librairie GSM
#include <GSM.h>

// Création des éléments de la librairie
GSM gsmAccess;
GSM_SMS sms;

// Code PIN à indiquer rentre les guillemets si nécessaire
#define CodePIN ""

// Tableau destiné à conserver le numéro de l'expéditeur d'un SMS reçu par le shield
char Num_Expediteur[20];
char Num_Administrateur[] = "+33603353147";

bool SMS_LED;			// variable qui permet l'allumage / extinction de la led par texto
bool SMS_PrecedentLED;	// variable est destiné à enregistrer l'état précédent de la LED

// Gestion du temps
const int Sec = 1000;
unsigned long MillisPrecedent = 0;
unsigned long MillisInterval = 10000;
unsigned long MillisActuel;

// ======== Différents contenus de SMS
// La fonction Envoi comprend trois paramètres : un thème ("Theme_...", et le contenu divisé en deux messages "Text_...".)

// Identifiant du système
char SystemeID[] = "\n-Robot GSM de Brignoles-";

// Message de démarrage
char Theme_MiseEnMarche[]   = "\nActivation du systeme. ";
char Text_MiseEnMarche[] = "Acces au reseau GSM OK.";

// Rappel des instructions utilisateur
char Theme_Instructions[]           = "Instructions :";
char Text_InstructionsUtilisateur[] = "\n0 : Rappel des instructions ;\n1 : Allumer/Eteindre la LED ;\n2 : Obtenir l'etat de la LED ;\n3 : Verifier le temps d'utilisation de l'Arduino";

// Messages de fonctionnement LED
char Theme_LED[]      = "\nLED > ";
char Text_SmsLED[]    = "Commande SMS > ";
char Text_BoutonLED[] = "Appui sur bouton > ";
char Text_EtatLED[]   = "Controle d'etat > ";
char Text_LEDOff[]    = "OFF";
char Text_LEDOn[]     = "ON";

// Temps d'utilisation
char Theme_TempsUtilisation[] = "L'arduino tourne depuis ";
char Temps_UtilisationUnite[] = " au total.";

// Message d'alerte
char Theme_Alerte[]                   = "Attention, ";
char Text_AlerteInstruction[]         = "votre instruction n'est pas correcte. ";
char Text_AlerteInstructionCorrecte[] = "Envoyez '0' pour obtenir les instructions valides.";
char Text_AlertePotentiometre[]       = "le niveau du potentiometre vient d'attendre 75 pour cent.";
char Text_AlerteIntrusion[]           = "une personne non enregistree a envoye une instruction (qui a ete ignoree). Voici son numero : ";

// Texte vide, au cas où je ne souhaite pas envoyer un Message2, la troisième partie des textos.
char Text_Null[] = "";

// ==== Matériel
// ======== LED & variables nécessaires à son fonctionnement
const int LED = 12;
bool LED_Etat;

// ======== Bouton & variables associées à son bon fonctionnement
const int Bouton = 4;			 	// Bouton d'allumage de la LE
bool Bouton_Etat;				 	// Booléenne enregistrant l'état du bouton par la fonction digitalRead()
bool Bouton_EtatPrecedent;			// Booléenne enregistrant l'état précédant du bouton
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
	Potentiometre,
	Bouton_Etat,
	Bouton_EtatPrecedent,
	LED_Etat,
	MillisActuel,
	MillisInterval
	= 0;

	// ======= Initialisation matériel
	pinMode(Bouton, INPUT_PULLUP);

	pinMode(LED, OUTPUT);
	digitalWrite(LED, HIGH);									// La LED est ON, marquant le début du setup()
	
	// ======= Initialisation moniteur série
	Serial.begin(9600);
	while (!Serial) {
		; 														// On attend que le port Serial se connecte, ce qui est surtout indispensable seulement pour le port USB natif
	}

	// ======= Connection au réseau
	ConnectionReseau();										// Fonction de connection au réseau

	delay(1000);

	// ======= LANCEMENT PROGRAMME
	Serial.println(F("**************************"));
	Serial.println(F("** Programme SMS in/out **"));
	Serial.println(F("**************************"));

	digitalWrite(LED, LOW);										// la led s'éteint et marque la fin du setup()

	Envoi(Theme_MiseEnMarche, Text_MiseEnMarche, Text_Null);	// On envoi un texto au client une fois le système activé.
	
	SuppressionSMS();											// Au démarrage, les textos en attente dans le modem sont toujours effacés pour éviter les conflits. #MU
}


// ****************************************************************************
// *                                   LOOP                                   *
// ****************************************************************************

void loop()
{
	// Millis() ? Pour checker seulement toutes les 10 min ? :(

	ReceptionSMS();				// Instructions données par SMS

	InstructionBouton();		// Instructions données par le bouton physique
	
	AllumageExtinctionLED();	// Action sur la LED

	delay(10);					// Délai nécessaire pour que le programme tourne sans souci
}

// ****************************************************************************
// *                              Fonctionnement                              *
// ****************************************************************************


// void VerificationPlantage(void) {
	// wdt_enable(WDTO_1S);		// Fonction de reset : trop courte car reboot après 1 seconde. Or, il arrive pour des raisons valides que le programme ne repasse pas par cette boucle à temps...
	// for(;;);
// }

void ConnectionReseau(){
	bool PasDeConnection = true;						// Bool d'état de la connection
	while (PasDeConnection) {							// Démarrage du shield GSM
		Serial.println(F("[Recherche reseau]"));
		delay(500);										// Est-ce que je peux pas allonger pour laisser du temps en modem ?
		if (gsmAccess.begin(CodePIN) == GSM_READY) {	// Si la SIM a un code PIN, le passer en tant que paramètre de begin(), entre guillemets
			delay(500);
			PasDeConnection = false;
		} else {
			Serial.println(F("[Pas de connection]"));
			delay(1000);
		}
	}
}

// ****************************************************************************
// *                                 Reception                                *
// ****************************************************************************

void ReceptionSMS(){
	if(sms.available()){		// Avec cette boucle, s'il y a quoi que ce soit de dispo dans le buffer du modem, je le lis
		NumeroExpediteur();		// Relevé du numéro de l'expéditeur
		MauvaisNumero();		// En cas de mauvais numéro
		MauvaiseInstruction();	// Si le premier caractère n'est pas un chiffre entre 1 et 3, ou 9
		// LectureSMS();		// Impression du message sur le moniteur série (pour débugger)
		InstructionSMS();		// Actions à entreprendre en fonction des instructions
		SortieReception:
		SuppressionSMS();
	}
}

void NumeroExpediteur(){
	Serial.print(F("\n[SMS recu du "));
	sms.remoteNumber(Num_Expediteur, 20);	// On stock le numéro de l'expéditeur (il y a de grandes chances que ce soit le numéro de l'utilisateur préenregistré)
	Serial.print(Num_Expediteur);
	Serial.println(F("]"));
}

void MauvaisNumero(){
	for(int i = 0; i < 12; i++){											// La boucle va en 12 parce que le numéro est composé de douze chiffres (avec le "+33" et sans le premier "0")
	    if(Num_Expediteur[i] == Num_Administrateur[i]);
		else{
			Serial.println(F("Numero pas compatible..."));					// Si un chiffre diffère, on efface le message, empêchant l'instruction d'être effectuée
			Envoi(Theme_Alerte, Text_AlerteIntrusion, Num_Expediteur);	// et on prévient l'utilisateur de l'intrusion
			SuppressionSMS();
			return;															// Une fois le faux numéro détecté, on sort de for() pour éviter d'envoyer inutilement les alertes d'intrusion, et on retourne à la boucle précédente
	    }
	}
}

// void LectureSMS(){
// 	char Texto_LPL;						// Ici, en fait il s'agit du texto "lettre par lettre" qui est affiché
// 	while (Texto_LPL = sms.read()) {	// Lecture des bytes du message et affichage sur le moniteur série
// 		Serial.print(Texto_LPL);
// 	}
// }

void MauvaiseInstruction(){
	if(sms.peek() < 48 || sms.peek() > 51){
		Serial.print(F("Instruction erronee : "));
		goto SortieReception;
	}
}

void InstructionSMS(){
	// ======= Si on a reçu un texto
	// if(sms.available()){									// Avec cette boucle, s'il y a quoi que ce soit de dispo dans le buffer du modem, je le lis
	// ======= Lecture du choix de l'expéditeur
	int Choix_Action = sms.parseInt();					// on cherche un entier ("Int")
	Serial.println(Choix_Action);

	// ======= Instructions
	if(Choix_Action        == 9){						// 0 : instructions utilisateurs
		Envoi(Theme_Instructions,
			  Text_InstructionsUtilisateur,
			  Text_Null);
	} else if(Choix_Action == 1){						// 1 : Allumer / éteindre LED
		LEDallumageSMS();
	} else if(Choix_Action == 2){						// 2 : État de la LED
		ControleEtatLED();
	} else if(Choix_Action == 3){						// 3 : Temps d'utilisation
		TempsUtilisationArduino();
	//else if(Choix_Action == !){} 						!! ATTENTION j'ai bloqué les chiffres au dessus dans InstructionInconnue()
	} else {
		InstructionInconnue();
	}
}

void InstructionInconnue(){
 		Serial.print(F("Instruction inconnue : "));
 		Envoi(Theme_Alerte, Text_AlerteInstruction, Text_AlerteInstructionCorrecte);
}

void SuppressionSMS(){
	int NbMessage = 0;
	Serial.print(F("["));
	do{
		sms.flush();
		NbMessage++;
	} while (sms.available() > 0);
	Serial.print(NbMessage);
	Serial.print(F(" message(s)"));
	Serial.println(F(" efface(s)]\n"));
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
			Envoi(Theme_LED, Text_BoutonLED, Text_LEDOff);
		}
		else {
			LED_Etat = HIGH;					// et vice-versa
			delay(100);
			Envoi(Theme_LED, Text_BoutonLED, Text_LEDOn);
		}
	}
}

void LEDallumageSMS(){
		// SMS_LED = HIGH;										// On passe la booléenne de la LED en high
		if(LED_Etat == HIGH){
			LED_Etat = LOW;
			delay(100);
		    Envoi(Theme_LED, Text_SmsLED, Text_LEDOff);
		}
		else {
			LED_Etat = HIGH;
			delay(100);
			Envoi(Theme_LED, Text_SmsLED, Text_LEDOn);
		}
}

void AllumageExtinctionLED(){
	digitalWrite(LED, LED_Etat);				// On attribue à la LED a valeur de État_led
	Bouton_EtatPrecedent = Bouton_Etat; 		// sans oublier d'enregistrer l'état du bouton
}

void ControleEtatLED(){
	switch (LED_Etat) {									// Cette fonction sert à indiquer à l'utilisateur l'état actuel de la LED
	    case 0:
	      Envoi(Theme_LED, Text_EtatLED, Text_LEDOff);
	      break;
	    case 1:
	      Envoi(Theme_LED, Text_EtatLED, Text_LEDOn);
	      break;
	}
}


// ****************************************************************************
// *                      Fonctionnement et vérification                      *
// ****************************************************************************

void TempsUtilisationArduino(){
	char *Temps_Utilisation;
	Temps_Utilisation = TempsVersString(millis()/1000);							// Je suis obligé de convertir millis en un String, et on me renverra un tableau char d'ailleurs, pour pouvoir l'envoyer par texto ensuite
	Serial.println(Temps_Utilisation);
	Envoi(Theme_TempsUtilisation, Temps_Utilisation, Temps_UtilisationUnite);
}

char * TempsVersString(unsigned long TempsEnSecondes){			// TempsEnSecondes est le temps en secondes issu du calcul "millis()/1000" dans la boucle précédente
	static char Temps_Total[12];
	long Heu 		= TempsEnSecondes / 3600;
	TempsEnSecondes = TempsEnSecondes % 3600;
	int Min 		= TempsEnSecondes / 60;
	int Sec 		= TempsEnSecondes % 60;
	sprintf(Temps_Total, "%02ldh%02dmin%02ds", Heu, Min, Sec);	// c'est en changeant les chiffres après chaque "%0" qu'on définit le nb de zéros
	return Temps_Total;
}

// ****************************************************************************
// *                                   Envoi                                  *
// ****************************************************************************

void Envoi(char Theme[], char Message1[], char Message2[]){
	Serial.print(F("\n[Envoi du message...\n"));
	Serial.print(Message1);
	Serial.println(Message2);

	sms.beginSMS(Num_Administrateur);
	sms.print(SystemeID);
	sms.print(Theme);
	sms.print(Message1);
	sms.print(Message2);

	sms.endSMS();
	Serial.println(F("...fin du message]"));
}
