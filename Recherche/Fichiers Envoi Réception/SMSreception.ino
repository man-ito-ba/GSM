// 160902 Fonctionne !

// Initialisation de la librairie GSM
#include <GSM.h>

// PIN Number for the SIM
#define CodePIN ""

// Création des éléments de la librairie
GSM gsmAccess;
GSM_SMS sms;

// Tableau destiné à conserver le numéro de l'expéditeur d'un SMS reçu par le shield
char Num_ExpediteurSMS[20];

void setup() {
	// Initialisation des communication avec le moniteur serie et attente de l'ouverture du port
	Serial.begin(9600);
	while (!Serial) {
	; // On attend que le port Serial se connecte, ce qui est surtout indispensable seulement pour le port USB natif
	}

	Serial.println("** Reception SMS **");

	// Bool d'état de la connection
	bool notConnected = true;

	// Démarrage du shield GSM
	// Si la SIM a un code PIN, le passer en tant que paramètre de begin(), entre guillemets
	while (notConnected) {
		if (gsmAccess.begin(CodePIN) == GSM_READY) {
			notConnected = false;
		} 
		else {
			Serial.println("--Pas de connection");
			delay(1000);
		}
	}

	Serial.println("--En fonction");
	Serial.println("--En attente de SMS");
}

void loop() {
	char Texto_LPL;		// Ici, en fait il s'agit du texto "lettre par lettre" qui est affiché

	// Si un SMS est reçu()
	if (sms.available()) {
		Serial.println("Expediteur du SMS :");

		// Obtention du numéro de l'expéditeur du SMS
		sms.remoteNumber(Num_ExpediteurSMS, 20);
		Serial.println(Num_ExpediteurSMS);

		// Un exemple de supression de message
		// Ex. : tout message commençant par un "#" devrait être effacé
		if (sms.peek() == '#') {
			Serial.println("Elimination du SMS");
			sms.flush();
		}

		// Lecture des bytes du message et affichage sur le moniteur série
		while (Texto_LPL = sms.read()) {
			Serial.print(Texto_LPL);
		}

		Serial.println("\nFin du message");

		// Suppression des messages de la memoire du modem
		sms.flush();
		Serial.println("Suppression du message OK");
	}

	delay(1000);

}

