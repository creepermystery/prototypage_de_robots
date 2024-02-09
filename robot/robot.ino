int PIN_MOTOR_LEFT = 3;
int PIN_MOTOR_RIGHT = 5;
int compteurDroite = 0;
int compteurGauche = 0;
double M_PI = 3.1415926535897932384626433;
bool etatBoutonOnOff = true;
int choix = 0;

void setup ()
{
	attachInterrupt(2, triggerOdometreDroite(), FALLING);
	attachInterrupt(4, triggerOdometreGauche(), FALLING);
}

void triggerOdometreDroite ()
{
	compteurDroite++;
}

void triggerOdometreGauche ()
{
	compteurGauche++;
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

void toutDroit (int distanceCommandee)
{
	int distanceEstimee(0);
	analogWrite(PIN_MOTOR_LEFT, 255);
	analogWrite(PIN_MOTOR_RIGHT, 255);

	while (distanceEstimee < distanceCommandee)
	{
		distanceEstimee = M_PI*rayon*(compteurDroite+compteurGauche);
	}

	analogWrite(PIN_MOTOR_LEFT, 0);
	analogWrite(PIN_MOTOR_RIGHT, 0);
}

void carre ()
{
	for (int i = 0, i = 4, i++)
	{
		toutDroit(10);
		tournerDroite(90);
	}
}

void triangle ()
{
	for (int i = 0, i = 3, i++)
	{
		toutDroit(10);
		tournerDroite(120);
	}
}

void cercle ()
{

}

void loop ()
{
	
}