// pins moteurs
const int PIN_MOTOR_LEFT = 3;
const int PIN_MOTOR_RIGHT = 5;
const int PIN_DIR_MOTOR_LEFT = 2;
const int PIN_DIR_MOTOR_RIGHT = 4;
const int PIN_INTERRUPT_LEFT = 6;
const int PIN_INTERRUPT_RIGHT = 7;

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

// variables globales
const int rapportReductionMoteur = 53;
const int rayonRoue = 66;

int compteurDroite = 0;
int compteurGauche = 0;
double pi = 3.1415926535897932384626433;
bool etatBoutonOnOff = true;
unsigned int choix = 0;

void setup ()
{
	// interruptions odomètre
	attachInterrupt(digitalPinToInterrupt(PIN_INTERRUPT_LEFT), triggerOdometreDroite(), FALLING);
	attachInterrupt(digitalPinToInterrupt(PIN_INTERRUPT_RIGHT), triggerOdometreGauche(), FALLING);

	// interruptions boutons choix
	attachInterrupt(digitalPinToInterrupt(PIN_BUTTON_DOWN), triggerOdometreDroite(), FALLING);
	attachInterrupt(digitalPinToInterrupt(PIN_INTERRUPT_RIGHT), triggerOdometreGauche(), FALLING);
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
	if (choix == 7)
	{
		choix = 1;
	}
}

void decrementerChoix ()
{
	choix--;
	if (choix == 0)
	{
		choix = 6;
	}
}

void tournerDroite (int angle)
{
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
	int distanceEstimee = 0;

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
		toutDroit(100);
		tournerDroite(90);
	}
}

void triangle ()
{
	for (int i = 0; i = 3; i++)
	{
		toutDroit(10);
		tournerDroite(120);
	}
}

void cercle ()
{

}

void errorColor () // affiche la couleur d'erreur (marron)
{
	analogWrite(PIN_LED_RED, 165);
	analogWrite(PIN_LED_GREEN, 42);
	analogWrite(PIN_LED_BLUE, 42);
}

void loop ()
{
	switch (choix)
	{
		case 1: // suivi de ligne, DEL verte
			analogWrite(PIN_LED_RED, 0);
			analogWrite(PIN_LED_GREEN, 255);
			analogWrite(PIN_LED_BLUE, 0);
			if (digitalRead(PIN_BUTTON_VALID))
			{
				// suiviLigne();
			}
			break;

		case 2: // évitement d'obstacles, DEL rouge
			analogWrite(PIN_LED_RED, 255);
			analogWrite(PIN_LED_GREEN, 0);
			analogWrite(PIN_LED_BLUE, 0);
			if (digitalRead(PIN_BUTTON_VALID))
			{
				// evitementObstacles();
			}
			break;

		case 3: // ligne droite, DEL blanche
			analogWrite(PIN_LED_RED, 255);
			analogWrite(PIN_LED_GREEN, 255);
			analogWrite(PIN_LED_BLUE, 255);
			if (digitalRead(PIN_BUTTON_VALID))
			{
				toutDroit(10);
			}
			break;

		case 4: // carré, DEL jaune
			analogWrite(PIN_LED_RED, 255);
			analogWrite(PIN_LED_GREEN, 255);
			analogWrite(PIN_LED_BLUE, 0);
			if (digitalRead(PIN_BUTTON_VALID))
			{
				carre();
			}
			break;

		case 5: // triangle, DEL bleue
			analogWrite(PIN_LED_RED, 0);
			analogWrite(PIN_LED_GREEN, 0);
			analogWrite(PIN_LED_BLUE, 255);
			if (digitalRead(PIN_BUTTON_VALID))
			{
				triangle();
			}
			break;

		case 6: // cercle, DEL violette
			analogWrite(PIN_LED_RED, 238);
			analogWrite(PIN_LED_GREEN, 130);
			analogWrite(PIN_LED_BLUE, 238);
			if (digitalRead(PIN_BUTTON_VALID))
			{
				// cercle();
			}
			break;

		default: // n'est pas sensé arriver, affiche la couleur d'erreur
			errorColor();
			break;
	}
}