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
	// initialisation du Port série
	Serial.begin(9600);
	Serial.println("***************");
	Serial.println("** Code Squelette **");
	Serial.println();
	Instructions();
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
	Serial.println("---------------");
	Serial.println("Quelle action ?");
	Serial.println("1 : Envoyer un SMS");
	Serial.println("2 : Verifier les SMS");
	Serial.println("Chiffre + appuyer sur 'Retour'\n");
}

void Envoi(){
	Serial.println("c.a.d la fonction 'Envoi'");
	Serial.println();
	delay(1000);
	Instructions();
}

void Reception(){
	Serial.println("c.a.d la fonction 'Reception'");
	Serial.println();
	delay(1000);
	Instructions();
}

void Lecture_MoniteurSerie(){
	
}