// pins moteurs
const int PIN_MOTOR_LEFT = 16;
const int PIN_MOTOR_RIGHT = 17;
const int PIN_DIR_MOTOR_LEFT = 32;
const int PIN_DIR_MOTOR_RIGHT = 33;
const int PIN_INTERRUPT_LEFT = 34;
const int PIN_INTERRUPT_RIGHT = 35;

// pins IR terrestres
const int PIN_FLOOR_IR_LEFT = 23;
const int PIN_FLOOR_IR_RIGHT = 22;

//pins IR frontaux
const int PIN_BACK_IR = 25;
const int PIN_FRONT_IR = 26;

// pins boutons
const int PIN_BUTTON_VALID = 27;
const int PIN_BUTTON_UP = 14;
const int PIN_BUTTON_DOWN = 12;
const int PIN_BUTTON_ON_OFF = 13;

// pins DELs
const int PIN_LED_RED = 2;
const int PIN_LED_GREEN = 4;
const int PIN_LED_BLUE = 15;

// paramètres PWM
const int freq = 490;
const int resolution = 8; // 8 bits de résolution (de 0 à 255)
const int channelMotorLeft = 0;
const int channelMotorRight = 1;
const int channelLedRed = 2;
const int channelLedGreen = 3;
const int channelLedBlue = 4;
const int channelFrontIr = 5;
const int channelBackIr = 6;

// constantes globales
const float rapportReductionMoteur = 52.734;
const int rayonRoue = 66;
const double pi = 3.1415926535897932384626433;
const int filtre = 1800;

// variables globales
volatile int compteurDroite = 0;
volatile int compteurGauche = 0;
volatile bool etatBoutonOnOff = 0;
volatile int choix = 1;
volatile bool validPressed = 0;

// Anti-Rebonds
const int debounce = 200;
unsigned long debounceValid = millis();
// unsigned long debounceTriggerDroite = millis();
// unsigned long debounceTriggerGauche = millis();
unsigned long debounceIncrementerChoix = millis();
unsigned long debounceDecrementerChoix = millis();
unsigned long debounceOnOff = millis();

void swapValid ()
{
    if (millis() - debounceValid > debounce)
    {
        validPressed = 1;
        debounceValid = millis();
    }
}

void triggerOdometreDroite () // Fonction de comptage de l'odomètre de droite
{
    compteurDroite++;
}

void triggerOdometreGauche () // Fonction de comptage de l'odomètre de gauche
{
    compteurGauche++;
}

void incrementerChoix () // Fonction d'incrémentation du bouton de choix "+" (UP)
{
    if (millis() - debounceIncrementerChoix > debounce)
    {
        choix++;
        if (choix >= 7) choix = 1;
        debounceIncrementerChoix = millis();
    }
}

void decrementerChoix () // Fonction de décrémentation du bouton de choix "-" (DOWN) 
{
    if (millis() - debounceDecrementerChoix > debounce)
    {
        choix--;
        if (choix <= 0) choix = 6;
        debounceDecrementerChoix = millis();
    }
}

void swapOnOff () // Fonction de passage de ON à OFF
{
    if (millis() - debounceOnOff > debounce)
    {
        etatBoutonOnOff = !etatBoutonOnOff;
        debounceOnOff = millis();
    }
}

void setup ()
{
    Serial.begin(9600);
    // Pins moteurs
    pinMode(PIN_MOTOR_LEFT, OUTPUT);
    pinMode(PIN_MOTOR_RIGHT, OUTPUT);
    pinMode(PIN_DIR_MOTOR_LEFT, OUTPUT);
    pinMode(PIN_DIR_MOTOR_RIGHT, OUTPUT);
    pinMode(PIN_INTERRUPT_LEFT, INPUT);
    pinMode(PIN_INTERRUPT_RIGHT, INPUT);

    ledcAttachPin(PIN_MOTOR_LEFT, channelMotorLeft);
    ledcAttachPin(PIN_MOTOR_RIGHT, channelMotorRight);

    ledcSetup(channelMotorLeft, freq, resolution);
    ledcSetup(channelMotorRight, freq, resolution);

    // Pins IR terrestres
    pinMode(PIN_FLOOR_IR_LEFT, INPUT);
    pinMode(PIN_FLOOR_IR_RIGHT, INPUT);

    // Pins IR frontaux
    pinMode(PIN_BACK_IR, INPUT);
    pinMode(PIN_FRONT_IR, INPUT);

    // Pins boutons
    pinMode(PIN_BUTTON_VALID, INPUT_PULLUP);
    pinMode(PIN_BUTTON_UP, INPUT_PULLUP);
    pinMode(PIN_BUTTON_DOWN, INPUT_PULLUP);
    pinMode(PIN_BUTTON_ON_OFF, INPUT_PULLUP);

    // Pins DELs
    pinMode(PIN_LED_RED, OUTPUT);
    pinMode(PIN_LED_GREEN, OUTPUT);
    pinMode(PIN_LED_BLUE, OUTPUT);

    ledcAttachPin(PIN_LED_RED, channelLedRed);
    ledcAttachPin(PIN_LED_GREEN, channelLedGreen);
    ledcAttachPin(PIN_LED_BLUE, channelLedBlue);

    ledcSetup(channelLedRed, freq, resolution);
    ledcSetup(channelLedGreen, freq, resolution);
    ledcSetup(channelLedBlue, freq, resolution);

    // Interruptions odomètre
    attachInterrupt(PIN_INTERRUPT_LEFT, triggerOdometreDroite, FALLING);
    attachInterrupt(PIN_INTERRUPT_RIGHT, triggerOdometreGauche, FALLING);

    // Interruptions boutons choix
    attachInterrupt(PIN_BUTTON_DOWN, decrementerChoix, FALLING);
    attachInterrupt(PIN_BUTTON_UP, incrementerChoix, FALLING);

    // Interruption on/off
    attachInterrupt(PIN_BUTTON_ON_OFF, swapOnOff, FALLING);

    // Interruption valid
    attachInterrupt(PIN_BUTTON_VALID, swapValid, FALLING);
}

void tournerDroite (int angle) 
{
    compteurDroite = 0;
    compteurGauche = 0;
  
    ledcWrite(channelMotorLeft, 70);                // On choisit les bonnes direction de rotation des roues et on démarre le virage
    digitalWrite(PIN_DIR_MOTOR_LEFT, LOW);
    ledcWrite(channelMotorRight, 70);
    digitalWrite(PIN_DIR_MOTOR_RIGHT, LOW);

    while ((compteurDroite + compteurGauche)/2 < 10); // On attend que le virage soit fait

    ledcWrite(channelMotorLeft, 0);                 // On remet toutes les sorties à zéro
    digitalWrite(PIN_DIR_MOTOR_LEFT, HIGH);
    ledcWrite(channelMotorRight, 0);
    digitalWrite(PIN_DIR_MOTOR_RIGHT, LOW);
}

void tournerGauche (int angle)
{
    compteurDroite = 0;
    compteurGauche = 0;

    ledcWrite(channelMotorLeft, 70);            // On choisit les bonnes direction de rotation des roues et on démarre le virage
    digitalWrite(PIN_DIR_MOTOR_LEFT, HIGH);
    ledcWrite(channelMotorRight, 70);
    digitalWrite(PIN_DIR_MOTOR_RIGHT, HIGH);

    while ((compteurDroite + compteurGauche)/2 < 10); // On attend que le virage soit fait

    ledcWrite(channelMotorLeft, 0);             // On remet toutes les sorties à zéro
    digitalWrite(PIN_DIR_MOTOR_LEFT, LOW);
    ledcWrite(channelMotorRight, 0);
    digitalWrite(PIN_DIR_MOTOR_RIGHT, HIGH);
}

void toutDroit (int distanceCommandee)
{
    float distanceEstimee = 0.0;

    ledcWrite(channelMotorLeft, 255);
    ledcWrite(channelMotorRight, 255);

    while (distanceEstimee < distanceCommandee) distanceEstimee = pi*(rayonRoue/rapportReductionMoteur)*(compteurDroite+compteurGauche); // Tant que la distance estimée n'atteint pas la distance voulue, on avance

    ledcWrite(channelMotorLeft, 0);
    ledcWrite(channelMotorRight, 0);
}

void carre (int largeur)
{
    for (int i = 0; i >= 4; i++)
    {
        toutDroit(largeur);
        tournerDroite(90);
    }
}

void triangle (int largeur)
{
    for (int i = 0; i >= 3; i++)
    {
        toutDroit(largeur);
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

    ledcWrite(channelMotorLeft, 255);                   // On fait tourner nos roues proportionellement à la longueur des arcs
    ledcWrite(channelMotorRight, 255*ratioDeuxArcs);


    float longueurParcourueArcExterieur = 0;
    float longueurParcourueArcInterieur = 0;
    
    while (longueurParcourueArcExterieur < longueurArcExterieur && longueurParcourueArcInterieur < longueurArcInterieur) // Tant que les arcs ne sont pas complets on continue d'avancer
    {
        longueurParcourueArcExterieur = pi*(rayonRoue/rapportReductionMoteur)*compteurGauche; // Estimation de la longueur déjà parcourue par la roue extérieure
        longueurParcourueArcInterieur = pi*(rayonRoue/rapportReductionMoteur)*compteurDroite; // Estimation de la longueur déjà parcourue par la roue intérieure
    }

    ledcWrite(channelMotorLeft, 0); // On éteint les moteurs à la fin de la trajectoire
    ledcWrite(channelMotorRight, 0);
}

void cercle (int diametre)
{
    trajectoireCirculaire(diametre/2, 360); // On fait faire au robot une trajectoire circulaire sur 360 degrés
}

void attendre (int temps) // Fonction permettant d'attendre pendant un temps exprimé en millisecondes
{
    unsigned long currentTime = millis();
    while (millis() < currentTime + temps);
}

void suiviLigne () // Actuellement la fonction tourne à l'infini
{
    int valuePWMLeft = 25;
    int valuePWMRight = 25;

    digitalWrite(PIN_DIR_MOTOR_LEFT, HIGH);
    digitalWrite(PIN_DIR_MOTOR_RIGHT, LOW);

    while (choix == 1)
    {
        if (digitalRead(PIN_FLOOR_IR_LEFT)) valuePWMLeft = 0; // Si le robot perçoit la ligne gauche, on fait s'arrêter la roue gauche
        else valuePWMLeft = 255;

        if (digitalRead(PIN_FLOOR_IR_RIGHT)) valuePWMRight = 0; // Si le robot perçoit la ligne droite, on fait s'arrêter la roue droite
        else valuePWMRight = 255;

        ledcWrite(channelMotorLeft, valuePWMLeft);
        ledcWrite(channelMotorRight, valuePWMRight);

        if (!etatBoutonOnOff) break; // On arrête le programme de suivi de ligne si le bouton on/off est pressé
    }
    ledcWrite(channelMotorLeft, 0); // On oublie pas d'arrêter les moteurs
    ledcWrite(channelMotorRight, 0);
}

void evitementObstacles ()
{
    int valuePWMLeft = 255;
    int valuePWMRight = 255;

    while (choix == 2)
    {
        
        if (analogRead(PIN_BACK_IR) > filtre && analogRead(PIN_FRONT_IR)) // Si le capteur détecte un obstacle devant et derrière
        {
            digitalWrite(PIN_DIR_MOTOR_LEFT, LOW);
            digitalWrite(PIN_DIR_MOTOR_RIGHT, LOW);
            ledcWrite(channelMotorLeft, 255);
            ledcWrite(channelMotorRight, 255);
        }
        else if (analogRead(PIN_BACK_IR) > filtre) // Si le capteur détecte un obstacle derrière
        {
            digitalWrite(PIN_DIR_MOTOR_LEFT, HIGH);
            digitalWrite(PIN_DIR_MOTOR_RIGHT, LOW);
            ledcWrite(channelMotorLeft, 255);
            ledcWrite(channelMotorRight, 255);
        }
        else if (analogRead(PIN_FRONT_IR) > filtre) // Si le capteur détecte un obstacle devant
        {
            digitalWrite(PIN_DIR_MOTOR_LEFT, LOW);
            digitalWrite(PIN_DIR_MOTOR_RIGHT, HIGH);
            ledcWrite(channelMotorLeft, 255);
            ledcWrite(channelMotorRight, 255);
        }
        else // Si rien n'est détecté
        {
            ledcWrite(channelMotorLeft, 0);
            ledcWrite(channelMotorRight, 0);
        }

        if (!etatBoutonOnOff) break; // On arrête le programme de d'évitement d'obstacles si le bouton on/off est pressé
    }
    ledcWrite(channelMotorLeft, 0); // On oublie pas d'arrêter les moteurs
    ledcWrite(channelMotorRight, 0);
}

void color (int redPWM, int greenPWM, int bluePWM) // Set color
{
    ledcWrite(channelLedRed, redPWM);
    ledcWrite(channelLedGreen, greenPWM);
    ledcWrite(channelLedBlue, bluePWM);
}

void loop ()
{
    if (!etatBoutonOnOff) color(0, 0, 0); // Si le robot est éteint, on éteint la DEL RGB
    while (!etatBoutonOnOff); // Si le robot est éteint, on attend

    switch (choix) // On fait le menu de choix du programme
    {
        case 1: // Suivi de ligne, DEL verte
            color(0, 255, 0);
            if (validPressed)
            {
                validPressed = 0;
                suiviLigne();
            }
            break;

        case 2: // Evitement d'obstacles, DEL rouge
            color(255, 0, 0);
            if (validPressed)
            {
                validPressed = 0;
                evitementObstacles();
            }
            break;

        case 3: // Ligne droite, DEL blanche
            color(255, 255, 255);
            if (validPressed)
            {
                validPressed = 0;
                toutDroit(10);
            }
            break;

        case 4: // Carré, DEL jaune
            color(255, 255, 0);
            if (validPressed)
            {
                validPressed = 0;
                carre(500);
            }
            break;

        case 5: // Triangle, DEL bleue
            color(0, 0, 255);
            if (validPressed)
            {
                validPressed = 0;
                triangle(500);
            }
            break;

        case 6: // Cercle, DEL violette
            color(238, 130, 238);
            if (validPressed)
            {
                validPressed = 0;
                cercle(500);
            }
            break;

        default: // N'est pas sensé arriver, affiche la couleur d'erreur (marron)
            color(165, 42, 42);
            break;
    }
}
