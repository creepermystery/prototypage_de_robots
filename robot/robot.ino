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

void triggerOdometreDroite ()
{
	compteurDroite++;
}

void triggerOdometreGauche ()
{
	compteurGauche++;
}

void incrementerChoix ()
{
	choix++;
	if (choix == 7) choix = 1;
}

void decrementerChoix ()
{
	choix--;
	if (choix == 0) choix = 6;
}

void swapOnOff ()
{
	etatBoutonOnOff = !etatBoutonOnOff;
}

void tournerDroite (int angle)
{
  compteurDroite = 0;
  compteurGauche = 0;
  
	analogWrite(PIN_MOTOR_LEFT, 70);
	digitalWrite(PIN_DIR_MOTOR_LEFT, HIGH);
	analogWrite(PIN_MOTOR_RIGHT, 70);
	digitalWrite(PIN_DIR_MOTOR_RIGHT, LOW);

	while ((compteurDroite + compteurGauche)/2 < 10); // On attend que le virage soit fait

	analogWrite(PIN_MOTOR_LEFT, 0);
	digitalWrite(PIN_DIR_MOTOR_LEFT, LOW);
	analogWrite(PIN_MOTOR_RIGHT, 0);
	digitalWrite(PIN_DIR_MOTOR_RIGHT, LOW);
}

void tournerGauche (int angle)
{
  compteurDroite = 0;
  compteurGauche = 0;
  
	analogWrite(PIN_MOTOR_LEFT, 70);
	digitalWrite(PIN_DIR_MOTOR_LEFT, LOW);
	analogWrite(PIN_MOTOR_RIGHT, 70);
	digitalWrite(PIN_DIR_MOTOR_RIGHT, HIGH);

	while ((compteurDroite + compteurGauche)/2 < 10); // On attend que le virage soit fait

	analogWrite(PIN_MOTOR_LEFT, 0);
	digitalWrite(PIN_DIR_MOTOR_LEFT, LOW);
	analogWrite(PIN_MOTOR_RIGHT, 0);
	digitalWrite(PIN_DIR_MOTOR_RIGHT, LOW);
}

void toutDroit (int distanceCommandee)
{
	float distanceEstimee = 0.0;

	analogWrite(PIN_MOTOR_LEFT, 255);
	analogWrite(PIN_MOTOR_RIGHT, 255);

	while (distanceEstimee < distanceCommandee)
	{
		distanceEstimee = pi*(rayonRoue/rapportReductionMoteur)*(compteurDroite+compteurGauche);
	}

	analogWrite(PIN_MOTOR_LEFT, 0);
	analogWrite(PIN_MOTOR_RIGHT, 0);
}

void carre ()
{
	for (int i = 0; i = 4; i++)
	{
		toutDroit(500);
		tournerDroite(90);
	}
}

void triangle ()
{
	for (int i = 0; i = 3; i++)
	{
		toutDroit(500);
		tournerDroite(120);
	}
}

void trajectoireCirculaire (int rayonTrajectoire, int angle) // Rayon en millimètres et angle en degrès
{
	int rayonCercleExterieur = rayonTrajectoire + 100; // Rayon du cercle parcouru par la roue extérieure
	int rayonCercleInterieur = rayonTrajectoire - 100; // Rayon du cercle parcouru par la roue intérieure

	float longueurArcExterieur = rayonCercleExterieur*(pi/180)*angle; // Longueur de l'arc parcouru par la roue extérieure
	float longueurArcInterieur = rayonCercleInterieur*(pi/180)*angle; // Longueur de l'arc parcouru par la roue intérieure

	float ratioDeuxArcs = longueurArcInterieur/longueurArcExterieur;

	analogWrite(PIN_DIR_MOTOR_LEFT, 255);
	analogWrite(PIN_DIR_MOTOR_RIGHT, 255*ratioDeuxArcs);

	while (longueurParcourueArcExterieur < longueurArcExterieur && longueurParcourueArcInterieur < longueurArcInterieur)
	{
		longueurParcourueArcExterieur = pi*(rayonRoue/rapportReductionMoteur)*compteurGauche;
		longueurParcourueArcInterieur = pi*(rayonRoue/rapportReductionMoteur)*compteurDroite;
	}

	analogWrite(PIN_DIR_MOTOR_LEFT, 0);
	analogWrite(PIN_DIR_MOTOR_RIGHT, 0);
}

void cercle ()
{
	trajectoireCirculaire(250, 360); // On fait faire au robot une trajectoire circulaire de 250mm de rayon sur 360 degrés
}

void errorColor () // affiche la couleur d'erreur (marron)
{
	analogWrite(PIN_LED_RED, 165);
	analogWrite(PIN_LED_GREEN, 42);
	analogWrite(PIN_LED_BLUE, 42);
}

void attendre (int temps)
{
	unsigned long currentTime = millis();
	while (millis() < currentTime + temps);
}

void suiviLigne ()
{
	int valuePWMLeft = 255;
	int valuePWMRight = 255;

	while (choix == 1)
	{
		if (digitalRead(PIN_FLOOR_IR_LEFT)) valuePWMLeft = 0;
		else valuePWMLeft = 255;

		if (digitalRead(PIN_FLOOR_IR_RIGHT)) valuePWMRight = 0;
		else valuePWMRight = 255;

		analogWrite(PIN_MOTOR_LEFT, valuePWMLeft);
		analogWrite(PIN_MOTOR_RIGHT, valuePWMRight);
	}
}

void evitementObstacles ()
{
	int valuePWMLeft = 255;
	int valuePWMRight = 255;

	while (choix == 2)
	{
		if (analogRead(PIN_FRONT_IR_LEFT) >700);
		if (analogRead(PIN_FRONT_IR_RIGHT) > 700);
	}
}

void loop ()
{
	while (!etatBoutonOnOff);
	switch (choix)
	{
		case 1: // suivi de ligne, DEL verte
			analogWrite(PIN_LED_RED, 0);
			analogWrite(PIN_LED_GREEN, 255);
			analogWrite(PIN_LED_BLUE, 0);
			if (digitalRead(PIN_BUTTON_VALID))
			{
				suiviLigne();
				attendre(500);
			}
			break;

		case 2: // évitement d'obstacles, DEL rouge
			analogWrite(PIN_LED_RED, 255);
			analogWrite(PIN_LED_GREEN, 0);
			analogWrite(PIN_LED_BLUE, 0);
			if (digitalRead(PIN_BUTTON_VALID))
			{
				evitementObstacles();
				attendre(500);
			}
			break;

		case 3: // ligne droite, DEL blanche
			analogWrite(PIN_LED_RED, 255);
			analogWrite(PIN_LED_GREEN, 255);
			analogWrite(PIN_LED_BLUE, 255);
			if (digitalRead(PIN_BUTTON_VALID))
			{
				toutDroit(10);
				attendre(500);
			}
			break;

		case 4: // carré, DEL jaune
			analogWrite(PIN_LED_RED, 255);
			analogWrite(PIN_LED_GREEN, 255);
			analogWrite(PIN_LED_BLUE, 0);
			if (digitalRead(PIN_BUTTON_VALID))
			{
				carre();
				attendre(500);
			}
			break;

		case 5: // triangle, DEL bleue
			analogWrite(PIN_LED_RED, 0);
			analogWrite(PIN_LED_GREEN, 0);
			analogWrite(PIN_LED_BLUE, 255);
			if (digitalRead(PIN_BUTTON_VALID))
			{
				triangle();
				attendre(500);
			}
			break;

		case 6: // cercle, DEL violette
			analogWrite(PIN_LED_RED, 238);
			analogWrite(PIN_LED_GREEN, 130);
			analogWrite(PIN_LED_BLUE, 238);
			if (digitalRead(PIN_BUTTON_VALID))
			{
				cercle();
				attendre(500);
			}
			break;

		default: // n'est pas sensé arriver, affiche la couleur d'erreur
			errorColor();
			break;
	}
}
