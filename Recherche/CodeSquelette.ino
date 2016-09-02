// Utilisation du Serial

void setup()
{
	// initialisation du Port série
	Serial.begin(9600);
	Serial.println("***************");
	Serial.println("** Code Squelette **");
	Serial.println();
	Instructions();
}

void loop(){
	// Donc en fait, c'est sous la boucle While() suivante que je vais intégrer le code de lecture des textos.
	// À moins que je lui fasse checker les textos genre toutes les 15 min, ce qui économisera sans doute de la durée de vie au matériel plutôt que de le faire rester en éveil tout le temps... Mais peut-être que ça change rien ?
	while(Serial.available() == 0);					// Attente d'une entrée dans le Serial
	int Choix_Action = Serial.parseInt();			// Lecture du choix dans le Serial
	// 3 choix possibles (dont 1 erroné)
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
		Serial.print(Choix_Action);
		Serial.println(" : entree incorrecte");
		Instructions();
	}
	delay(10);
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