// 160902 Le code fonctionne, même s'il faut relancer plusieurs fois l'arduino (mais ça le fait aussi avec le code maison dans les sktechs Arduino... Donc c'est sans doute une question de réseau pourri chez moi)

// Initialisation de la librairie GSM
#include <GSM.h>

#define CodePIN ""

// Création des éléments de la librairie
GSM gsmAccess;
GSM_SMS sms;
	
void setup() {
	// Initialisation des communication avec le moniteur serie et attente de l'ouverture du port
	Serial.begin(9600);
	while (!Serial) {
		; // On attend que le port Serial se connecte, ce qui est surtout indispensable seulement pour le port USB natif
	}

	Serial.println("** Envoi SMS **");

	// Bool d'état de la connection
	bool notConnected = true;

	// Démarrage du shield GSM
	// Si la SIM a un code PIN, le passer en tant que paramètre de begin(), entre guillemets
	while (notConnected) {
		Serial.println("--Recherche du reseau...");
		if (gsmAccess.begin(CodePIN) == GSM_READY) {
			notConnected = false;
		} 
		else {
			Serial.println("--Pas de connection");
			delay(1000);
		}
	}

	Serial.println("--En fonction");
}

void loop() {
	Serial.print("Numero du destinataire : ");
	char Num_Destinataire[20];	// telephone number to send sms
	Lecture_MoniteurSerie(Num_Destinataire);
	Serial.println(Num_Destinataire);

	// Text du SMS
	Serial.print("Message : ");
	char TextSMS[200];
	Lecture_MoniteurSerie(TextSMS);
	Serial.println("Envoi en cours...");
	Serial.println();
	Serial.println("Contenu du message :");
	Serial.println(TextSMS);

	// Envoi du message
	sms.beginSMS(Num_Destinataire);
	sms.print(TextSMS);
	sms.endSMS();
	Serial.println("\nEnvoi OK !\n");
}

// Fonction destinée à lire les entrées dans le Serial (temporaire puisqu'à terme l'interaction se fera avec des téléphones mobiles)
int Lecture_MoniteurSerie(char Entree_Utilisateur[]) {
	int Caractere = 0;											// "i" sert à se positionner dans les variables[i] : par exemple, dans Text_SMS[i], on va entrer les lettres à chaque position dans le tableau
	while (true) {												// Jusqu'à ce que ("while") ce soit vérifié
		while (Serial.available() > 0) {						// Ici, on cherche à voir s'il y a des infos dans le buffer du Serial. Et la boucle fait que tant qu'il y a des infos, on continue.
			char Entree_Text = Serial.read();					// On met dans "Entree_Text" ce que le moniteur serie va lire "read"
			if (Entree_Text == '\n') {							// Si l'utilisateurs créé une nouvelle ligne, alors on comprend qu'un mot a été entré
				Entree_Utilisateur[Caractere] = '\0';			// Alors on met dans cette position du tableau un caractère NULL
				Serial.flush();									// alors tout est annulé (je crois)
				return 0;										// et on renvoi rien à la loop (je crois)
			}
			if (Entree_Text != '\r') {							// Si le texte entré n'est pas un retour chariot
				Entree_Utilisateur[Caractere] = Entree_Text;	// On utilise le caractère (ex. : la lettre) entrée qui est mis à la position [i] dans le tableau "Entree_Utilisateur"
				Caractere++;									// On avance d'une position dans le tableau
			}
		}
	}
}