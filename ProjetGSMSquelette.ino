/* GSM */

/* Objectif du programme
	
	Communiquer avec l'utilisateur à partir du GSM
	Un capteur de luminosité (simulée par un potentiomètre) vérifie la luminosité
	Une LED est activée lorsque la luminositée est trop faible
		- la LED peut également être allumée par un bouton
		- ainsi que par texto
	Dans les trois cas – luminositée trop faible ou LED forcée par bouton ou texto
	un SMS est envoyé à l'utilisateur pour informer de l'activation de la LED

	Le programme vérifie également l'état du bouton (?) ou de la LED (?) pour envoyer des alertes.
	Le programme envoie régulièrement le nombre d'heures d'utilisation restantes d'une LED pour préparer à son changement prochain (mais je vais rencontrer un problème : la fonction millis() ne va pas dépasser les 49 jours... donc comment compter ? Je fais un reset tous les 30 jours plutôt ?)

	*/

// Librairies
#include <GSM.h>
// Création des éléments de la librairie
GSM gsmAccess;
GSM_SMS sms;
// Code PIN à indique rentre les guillemets si nécessaire
#define CodePIN ""
// Tableau destiné à conserver le numéro de l'expéditeur d'un SMS reçu par le shield
char Num_ExpediteurSMS[20];

/* Matériel */
const int LED = 13;
unsigned long LED_UtilisationRestante;		// Cet variable doit être très longue...
byte LED_Etat;

const int Potentio = A0;

const byte Bouton = 3;
byte Bouton_Etat;

// Diverses string
int LED_SwitchSMS;

void setup(){
	// initialisation du Port série
		Serial.begin(9600);
		Serial.println("***************");
		Serial.println("***  Start  ***");
		Serial.println("***************");
	pinMode(LED, OUTPUT);
	digitalWrite(LED, HIGH);
	pinMode(Bouton, INPUT_PULLUP);			// Monté avec la résistance interne, le bouton renverra donc un LOW quand il sera pressé !

	digitalWrite(LED, LOW);					// La LED s'éteint quand tout dans la fonction SETUP a été effectué ; c'est pratique car 1. déjà on peut voir que la LED a bien fonctionné, et 2. on sait quand le SETUP a terminé.
}


void loop(){
	Reception();

	Verification_Periodique();
	
	Etat_Boutons();
	
	delay(10);
}

void Envoi(){
	/* Envoi :
		1. D'une alerte d'état
		2. D'une information de l'état général
		3. Du bon allumage d'un LED
	*/

}


void Reception(){
	/* La reception peut ordonner : 
		1. une vérification de l'état général
		2. l'allumage des LED

		Donc d'un côté j'ai le processus de vérification des messages, et ensuite son traitement, et l'envoi des données
	*/
	if(){
		// Si je reçois une demande de récapitulatif d'instructions
		Recapitulatif_Instructions();
	}
}

void Recapitulatif_Instructions(){
	// Une fonction appelée par la réception de la suite "1234567890" ou "Instructions" 
	char InstructionsUtilisateur[] = 
		"Instructions :\n
		1 : Allumer/Eteindre LED\n
		2 : Verifier etat Capteur\n
		3 : Verifier etat LED"
	// Envoi effectif des instructions
	sms.beginSMS(Num_ExpediteurSMS);
	sms.print(InstructionsUtilisateur);
	sms.endSMS();
}

void Verification_Periodique(){
	/* 3 conditions
		1. Est-ce que le niveau du potentiomètre est en dessous des 25% ?
		2. Est-ce que j'ai passé disons 5min sans informer de l'état du système ?

		Si ces conditions sont vérifiées : 
		- j'ai d'un côté une alerte pour le pot
		- et de l'autre une simple information
	*/

	/* Est-ce que je devrais avoir des conditions différentes ?
		3. Depuis combien de temps la LED travaille-t-elle ?
		- Une information, mais envoyée seulement toutes les 1000h.
	*/
}


void Etat_Boutons(){
	// Le bouton allume des LED
	Bouton_Etat = digitalRead(Bouton);
}


void Allumage_LED(){
	// Leds allumées par bouton ou SMS
	// Par le bouton
	if(Bouton_Etat == LOW && LED_Etat == LOW){
		digitalWrite(LED, HIGH);
		LED_Etat = HIGH;
	}
	if(Bouton_Etat == LOW && LED_Etat == HIGH){
		digitalWrite(LED, LOW);
		LED_Etat = LOW;
	}
	// Par SMS
	if(LED_SwitchSMS == HIGH && LED_Etat == LOW){
		digitalWrite(LED, HIGH);
		LED_Etat = HIGH;
		LED_SwitchSMS = LOW;
	}
	if(LED_SwitchSMS == HIGH && LED_Etat == HIGH){
		digitalWrite(LED, LOW);
		LED_Etat = LOW;
		LED_SwitchSMS = LOW;
	}
}


int Lecture_Moniteur(char Entree_Utilisateur){

}


void Affichage_Moniteur(){

}