// Utilisation du Serial

// Initialisation de la librairie GSM
#include <GSM.h>
// Création des éléments de la librairie
GSM gsmAccess;
GSM_SMS sms;
// Code PIN à indique rentre les guillemets si nécessaire
#define CodePIN ""
// Tableau destiné à conserver le numéro de l'expéditeur d'un SMS reçu par le shield
char Num_ExpediteurSMS[20];

void setup()
{
	// Initialisation des communication avec le moniteur serie et attente de l'ouverture du port
	Serial.begin(9600);
	while (!Serial) {
		; // On attend que le port Serial se connecte, ce qui est surtout indispensable seulement pour le port USB natif
	}
	Serial.println("**************************");
	Serial.println("** Programme SMS in/out **");
	Serial.println("**************************");

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

	Serial.println("-- En fonction");

	Reception();										// Avant de commencer le programme, au depart, on vériie s'il y a déjà des messages en attente
}

void loop()
{
	// Avec cette boucle, s'il y a quoi que ce soit de dispo dans le buffer du Serial, je le lis
	while(Serial.available() > 0){
		int Choix_Action = Serial.parseInt();					// on cherche un entier ("Int")
		if(Serial.read() == '\n'){								// S'il y a une nouvelle ligne, c'est la fin de l'entrée
			// 3 choix possibles dont 1 erroné
			if(Choix_Action == 1){
				Serial.print("Tu as appuye sur ");
				Serial.println(Choix_Action);
				Envoi();
			}
			else if(Choix_Action == 2){
				Serial.print("Tu as appuye sur ");
				Serial.println(Choix_Action);
				Reception();
			}
			else{
				Serial.print("Attention, ");
				Serial.print(Choix_Action);
				Serial.println(" est une entree incorrecte");
				Instructions();
			}
		}
	delay(10);
	}
}

void Instructions(){
	// Ensemble de choix données à l'utilisateur
	Serial.println("---------------");
	Serial.println("Quelle action ?");
	Serial.println("1 : Envoyer un SMS");
	Serial.println("2 : Verifier les SMS");
	Serial.println("Chiffre + appuyer sur 'Retour'\n");
}

void Envoi(){
	// Numéro de téléphone du destinataire de notre message
	Serial.print("Numero du destinataire : ");
	char Num_Destinataire[20];
	// On regarde ce que l'utilisateur tape dans le moniteur série et on l'affiche
	Lecture_MoniteurSerie(Num_Destinataire);
	Serial.println(Num_Destinataire);
	// Text du SMS
	Serial.print("Message : ");
	char TextSMS[200];
	// On regarde ce que l'utilisateur tape dans le moniteur série et on l'affiche
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

	delay(1000);
	Instructions();
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
	}
	// S'il n'y a aucun SMS
	if(!sms.available()){
	    Serial.println("Pas de nouveau message");
	}
	delay(1000);
	Instructions();
}

// Fonction destinée à lire les entrées dans le Serial (temporaire puisqu'à terme l'interaction se fera avec des téléphones mobiles)
int Lecture_MoniteurSerie(char Entree_Utilisateur[]) {
	int Caractere = 0;											// "Caractere" sert à se positionner dans les variables[i] : par exemple, dans Text_SMS[i], on va entrer les lettres à chaque position dans le tableau
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