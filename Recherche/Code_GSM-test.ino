// Librairie GSM
#include <GSM.h>

#define CodePIN ""

// Création des éléments de la librairie
GSM gsmAccess;
GSM_SMS sms;

bool Nouvelles_Donnees = false;

const byte NombreDeLettres = 200;
char Entree_Lettres_MoniteurSerie[NombreDeLettres]; // an array to store the received data

void setup() {
	// Initialisation des communication avec le moniteur serie et attente de l'ouverture du port
	Serial.begin(9600);
	while (!Serial) {
		; // On attend que le port Serial se connecte, ce qui est surtout indispensable seulement pour le port USB natif
	}

	Serial.println("\nProgramme \"Envoi SMS\"\n");
	
	// Bool d'état de la connection
	bool PasDeConnection = true;
	
	// Démarrage du shield GSM
	// Si la SIM a un code PIN, le passer en tant que paramètre de begin(), entre guillemets
	while (PasDeConnection = true) {							// Tant qu'on n'a pas de connection, on boucle
		Serial.println("En attente de la connection reseau...");
		if (gsmAccess.begin(CodePIN) == GSM_READY) {
			PasDeConnection = false;					// Connection établie, on sort de la boucle en transformant la booléenne PasdeConnection en "false"
		} 
		else {
			Serial.println("Pas de Connection");
			delay(1000);
		}
	}

	Serial.println("GSM en fonction !");
}

void loop() {
	// Numéro de l'expéditeur
	Serial.print("Numero de l'expediteur : ");		// Affichage de l'instruction dans le moniteur série
	char Numero_Destinataire[20];					// numéro à qui envoyer le SMS
	Lecture_du_MoniteurSerie(Numero_Destinataire);		// La lecture du numéro se fait par entrée dans le moniteur Série
	Serial.println(Numero_Destinataire);			// Affichage du numéro dans le moniteur série

	// Text du SMS
	Serial.print("Texte a envoyer : ");				// Affichage de l'instruction dans le moniteur série
	char Texte_SMS[200];
	Lecture_du_MoniteurSerie(Texte_SMS);				// La lecture du texte se fait par entrée dans le moniteur Série
	Serial.println("Envoi en cours...");			// Affichage du processus
	Serial.println();
	Serial.println("Message :");					// Affichage du résultat
	Serial.println(Texte_SMS);

	// Envoi du message
	sms.beginSMS(Numero_Destinataire);
	sms.print(Texte_SMS);
	sms.endSMS();
	Serial.println("\nMessage Envoye !\n");			// \n = retour à la ligne

	delay(100);
}

int Lecture_du_MoniteurSerie(char Entree_Lettres_MoniteurSerie[]) {
	static byte NombreDeLettres = 0;
	char Entree_Texte;

	while (Serial.available() > 0 && Nouvelles_Donnees == false) {
		Entree_Texte = Serial.read();

		if (Entree_Texte != '\n') {
			Entree_Lettres_MoniteurSerie[NombreDeLettres] = Entree_Texte;
			NombreDeLettres++;
			if (NombreDeLettres >= NombreDeLettres) {
				NombreDeLettres = NombreDeLettres - 1;
			}
		}
		else {
			Entree_Lettres_MoniteurSerie[NombreDeLettres] = '\0'; // terminate the string
			NombreDeLettres = 0;
			Nouvelles_Donnees = true;
		}
	}
}