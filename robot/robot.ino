// pins moteurs
const int PIN_MOTOR_LEFT = 3;
const int PIN_MOTOR_RIGHT = 5;
const int PIN_DIR_MOTOR_LEFT = 2;
const int PIN_DIR_MOTOR_RIGHT = 4;
const int PIN_INTERRUPT_LEFT = 6;
const int PIN_INTERRUPT_RIGHT = 7;

// pins IR terrestres
const int PIN_FLOOR_IR_LEFT = 18;
const int PIN_FLOOR_IR_RIGHT = 19;

//pins IR frontaux
const int PIN_FRONT_IR_LEFT = 20;
const int PIN_FRONT_IR_RIGHT = 21;

// pins boutons
const int PIN_BUTTON_VALID = 14;
const int PIN_BUTTON_UP = 15;
const int PIN_BUTTON_DOWN = 16;
const int PIN_BUTTON_ON_OFF = 17;

// pins DELs
const int PIN_LED_RED = 11;
const int PIN_LED_GREEN = 13;
const int PIN_LED_BLUE = 12;

// paramètres PWM
const int freq = 490;
const int channel = 0;
const int resolution = 8; // 8 bits de résolution (de 0 à 255)

// constantes globales
const float rapportReductionMoteur = 52.734;
const int rayonRoue = 66;
const double pi = 3.1415926535897932384626433;

// variables globales
int compteurDroite = 0;
int compteurGauche = 0;
bool etatBoutonOnOff = false;
unsigned int choix = 0;

void setup ()
{
	// interruptions odomètre
	attachInterrupt(digitalPinToInterrupt(PIN_INTERRUPT_LEFT), triggerOdometreDroite(), FALLING);
	attachInterrupt(digitalPinToInterrupt(PIN_INTERRUPT_RIGHT), triggerOdometreGauche(), FALLING);

	// interruptions boutons choix
	attachInterrupt(digitalPinToInterrupt(PIN_BUTTON_DOWN), decrementerChoix(), FALLING);
	attachInterrupt(digitalPinToInterrupt(PIN_BUTTON_UP), incrementerChoix(), FALLING);

	// interruption on/off
	attachInterrupt(digitalPinToInterrupt(PIN_BUTTON_ON_OFF), swapOnOff(), FALLING);

}

void triggerOdometreDroite () // Interrution de l'odomètre de droite
{
	compteurDroite++;
}

void triggerOdometreGauche () // Interrution de l'odomètre de gauche
{
	compteurGauche++;
}

void incrementerChoix () // Interrution du bouton de choix "+" (UP)
{
	choix++;
	if (choix == 7) choix = 1;
}

void decrementerChoix () // Interruption du bouton de choix "-" (DOWN) 
{
	choix--;
	if (choix == 0) choix = 6;
}

void swapOnOff () // Interruption du bouton ON/OFF
{
	etatBoutonOnOff = !etatBoutonOnOff;
}

void tournerDroite (int angle) 
{
  compteurDroite = 0;
  compteurGauche = 0;
  
	analogWrite(PIN_MOTOR_LEFT, 70);			// On choisit les bonnes direction de rotation des roues et on démarre le virage
	digitalWrite(PIN_DIR_MOTOR_LEFT, HIGH);
	analogWrite(PIN_MOTOR_RIGHT, 70);
	digitalWrite(PIN_DIR_MOTOR_RIGHT, LOW);

	while ((compteurDroite + compteurGauche)/2 < 10); // On attend que le virage soit fait

	analogWrite(PIN_MOTOR_LEFT, 0);				// On remet toutes les sorties à zéro
	digitalWrite(PIN_DIR_MOTOR_LEFT, LOW);
	analogWrite(PIN_MOTOR_RIGHT, 0);
	digitalWrite(PIN_DIR_MOTOR_RIGHT, LOW);
}

void tournerGauche (int angle)
{
  compteurDroite = 0;
  compteurGauche = 0;
  
	analogWrite(PIN_MOTOR_LEFT, 70);			// On choisit les bonnes direction de rotation des roues et on démarre le virage
	digitalWrite(PIN_DIR_MOTOR_LEFT, LOW);
	analogWrite(PIN_MOTOR_RIGHT, 70);
	digitalWrite(PIN_DIR_MOTOR_RIGHT, HIGH);

	while ((compteurDroite + compteurGauche)/2 < 10); // On attend que le virage soit fait

	analogWrite(PIN_MOTOR_LEFT, 0);				// On remet toutes les sorties à zéro
	digitalWrite(PIN_DIR_MOTOR_LEFT, LOW);
	analogWrite(PIN_MOTOR_RIGHT, 0);
	digitalWrite(PIN_DIR_MOTOR_RIGHT, LOW);
}

void toutDroit (int distanceCommandee)
{
	float distanceEstimee = 0.0;

	analogWrite(PIN_MOTOR_LEFT, 255);
	analogWrite(PIN_MOTOR_RIGHT, 255);

	while (distanceEstimee < distanceCommandee) distanceEstimee = pi*(rayonRoue/rapportReductionMoteur)*(compteurDroite+compteurGauche); // Tant que la distance estimée n'atteint pas la distance voulue, on avance

	analogWrite(PIN_MOTOR_LEFT, 0);
	analogWrite(PIN_MOTOR_RIGHT, 0);
}

void carre ()
{
	for (int i = 0; i >= 4; i++)
	{
		toutDroit(500);
		tournerDroite(90);
	}
}

void triangle ()
{
	for (int i = 0; i >= 3; i++)
	{
		toutDroit(500);
		tournerDroite(120);
	}
}

void trajectoireCirculaire (int rayonTrajectoire, int angle) // Rayon en millimètres et angle en degrès
{
	int rayonCercleExterieur = rayonTrajectoire + 100; // Rayon du cercle parcouru par la roue extérieure
	int rayonCercleInterieur = rayonTrajectoire - 100; // Rayon du cercle parcouru par la roue intérieure

	float longueurArcExterieur = rayonCercleExterieur*(pi/180)*angle; // Longueur de l'arc de la roue extérieure
	float longueurArcInterieur = rayonCercleInterieur*(pi/180)*angle; // Longueur de l'arc de la roue intérieure

	float ratioDeuxArcs = longueurArcInterieur/longueurArcExterieur; // On calcule un ratio de proportionalité dépendant de la longueur des arcs
									// Il faudra peut-être changer cette formule pour l'adapter à la courbe du PWM dépendante de notre driver

	analogWrite(PIN_DIR_MOTOR_LEFT, 255);					// On fait tourner nos roues proportionellement à la longueur des arcs
	analogWrite(PIN_DIR_MOTOR_RIGHT, 255*ratioDeuxArcs);


	float longueurParcourueArcExterieur = 0;
	float longueurParcourueArcInterieur = 0;
	
	while (longueurParcourueArcExterieur < longueurArcExterieur && longueurParcourueArcInterieur < longueurArcInterieur) // Tant que les arcs ne sont pas complets on continue d'avancer
	{
		longueurParcourueArcExterieur = pi*(rayonRoue/rapportReductionMoteur)*compteurGauche; // Estimation de la longueur déjà parcourue par la roue extérieure
		longueurParcourueArcInterieur = pi*(rayonRoue/rapportReductionMoteur)*compteurDroite; // Estimation de la longueur déjà parcourue par la roue intérieure
	}

	analogWrite(PIN_DIR_MOTOR_LEFT, 0); // On éteint les moteurs à la fin de la trajectoire
	analogWrite(PIN_DIR_MOTOR_RIGHT, 0);
}

void cercle ()
{
	trajectoireCirculaire(250, 360); // On fait faire au robot une trajectoire circulaire de 250mm de rayon sur 360 degrés
}

void attendre (int temps) // Fonction permettant d'attendre pendant un temps exprimé en millisecondes
{
	unsigned long currentTime = millis();
	while (millis() < currentTime + temps);
}

void suiviLigne () // Actuellement la fonction tourne à l'infini
{
	int valuePWMLeft = 255;
	int valuePWMRight = 255;

	while (choix == 1)
	{
		if (digitalRead(PIN_FLOOR_IR_LEFT)) valuePWMLeft = 0; // Si le robot perçoit la ligne gauche, on fait s'arrêter la roue gauche
		else valuePWMLeft = 255;

		if (digitalRead(PIN_FLOOR_IR_RIGHT)) valuePWMRight = 0; // Si le robot perçoit la ligne droite, on fait s'arrêter la roue droite
		else valuePWMRight = 255;

		analogWrite(PIN_MOTOR_LEFT, valuePWMLeft);
		analogWrite(PIN_MOTOR_RIGHT, valuePWMRight);

		if (!etatBoutonOnOff) break; // On arrête le programme de suivi de ligne si le bouton on/off est pressé
	}
	analogWrite(PIN_MOTOR_LEFT, 0); // On oublie pas d'arrêter les moteurs
	analogWrite(PIN_MOTOR_RIGHT, 0);
}

void evitementObstacles ()
{
	int valuePWMLeft = 255;
	int valuePWMRight = 255;

	while (choix == 2)
	{
		if (analogRead(PIN_FRONT_IR_LEFT) >700); // Si le capteur détecte un obstacle
		if (analogRead(PIN_FRONT_IR_RIGHT) > 700);

		if (!etatBoutonOnOff) break; // On arrête le programme de d'évitement d'obstacles si le bouton on/off est pressé
	}
	analogWrite(PIN_MOTOR_LEFT, 0); // On oublie pas d'arrêter les moteurs
	analogWrite(PIN_MOTOR_RIGHT, 0);
}

void color (int redPWM, int greenPWM, int bluePWM)
{
	analogWrite(PIN_LED_RED, redPWM);
	analogWrite(PIN_LED_GREEN, greenPWM);
	analogWrite(PIN_LED_BLUE, bluePWM);
}

void loop ()
{
	if (!etatBoutonOnOff) color(0, 0, 0); // Si le robot est éteint, on éteint la DEL RGB
	while (!etatBoutonOnOff); // Si le robot est éteint, on attend
	switch (choix)
	{
		case 1: // Suivi de ligne, DEL verte
			color(0, 255, 0);
			if (digitalRead(PIN_BUTTON_VALID))
			{
				suiviLigne();
				attendre(500);
				break;
			}

		case 2: // Evitement d'obstacles, DEL rouge
			color(255, 0, 0);
			if (digitalRead(PIN_BUTTON_VALID))
			{
				evitementObstacles();
				attendre(500);
				break;
			}

		case 3: // Ligne droite, DEL blanche
			color(255, 255, 255);
			if (digitalRead(PIN_BUTTON_VALID))
			{
				toutDroit(10);
				attendre(500);
				break;
			}

		case 4: // Carré, DEL jaune
			color(255, 255, 0);
			if (digitalRead(PIN_BUTTON_VALID))
			{
				carre();
				attendre(500);
				break;
			}

		case 5: // Triangle, DEL bleue
			color(0, 0, 255);
			if (digitalRead(PIN_BUTTON_VALID))
			{
				triangle();
				attendre(500);
				break;
			}

		case 6: // Cercle, DEL violette
			color(238, 130, 238);
			if (digitalRead(PIN_BUTTON_VALID))
			{
				cercle();
				attendre(500);
				break;
			}

		default: // N'est pas sensé arriver, affiche la couleur d'erreur (marron)
			color(165, 42, 42);
			break;
	}
}
