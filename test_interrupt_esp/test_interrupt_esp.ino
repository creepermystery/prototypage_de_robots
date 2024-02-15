// pins moteurs
const int PIN_MOTOR_LEFT = 36;
const int PIN_MOTOR_RIGHT = 39;
const int PIN_DIR_MOTOR_LEFT = 34;
const int PIN_DIR_MOTOR_RIGHT = 35;
const int PIN_INTERRUPT_LEFT = 32;
const int PIN_INTERRUPT_RIGHT = 33;

// pins IR terrestres
const int PIN_FLOOR_IR_LEFT = 23;
const int PIN_FLOOR_IR_RIGHT = 22;

//pins IR frontaux
const int PIN_FRONT_IR_LEFT = 25;
const int PIN_FRONT_IR_RIGHT = 26;

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

// constantes globales
const float rapportReductionMoteur = 52.734;
const int rayonRoue = 66;
const double pi = 3.1415926535897932384626433;

// variables globales
volatile int compteurDroite = 0;
volatile int compteurGauche = 0;
volatile bool etatBoutonOnOff = 0;
volatile int choix = 1;
volatile bool validPressed = 0;

void swapValid ()
{
    validPressed = 1;
}

void incrementerChoix () // Fonction d'incrémentation du bouton de choix "+" (UP)
{
    choix++;
    if (choix >= 7) choix = 1;
}

void decrementerChoix () // Fonction de décrémentation du bouton de choix "-" (DOWN) 
{
    choix--;
    if (choix <= 0) choix = 6;
}

void swapOnOff () // Fonction de passage de ON à OFF
{
    etatBoutonOnOff = !etatBoutonOnOff;
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

    // Pins IR terrestres
    pinMode(PIN_FLOOR_IR_LEFT, INPUT);
    pinMode(PIN_FLOOR_IR_RIGHT, INPUT);

    // Pins IR frontaux
    pinMode(PIN_FRONT_IR_LEFT, INPUT);
    pinMode(PIN_FRONT_IR_RIGHT, INPUT);

    // Pins boutons
    pinMode(PIN_BUTTON_VALID, INPUT);
    pinMode(PIN_BUTTON_UP, INPUT);
    pinMode(PIN_BUTTON_DOWN, INPUT);
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

    // Interruptions boutons choix
    attachInterrupt(PIN_BUTTON_DOWN, decrementerChoix, FALLING);
    attachInterrupt(PIN_BUTTON_UP, incrementerChoix, FALLING);

    // Interruption on/off
    attachInterrupt(PIN_BUTTON_ON_OFF, swapOnOff, FALLING);

    // Interruption valid
    attachInterrupt(PIN_BUTTON_VALID, swapValid, FALLING);
}

void color (int redPWM, int greenPWM, int bluePWM) // Set color
{
    ledcWrite(channelLedRed, redPWM);
    ledcWrite(channelLedGreen, greenPWM);
    ledcWrite(channelLedBlue, bluePWM);
}

void loop ()
{
    Serial.println(choix);
    if (!etatBoutonOnOff) color(0, 0, 0); // Si le robot est éteint, on éteint la DEL RGB
    while (!etatBoutonOnOff); // Si le robot est éteint, on attend

    switch (choix) // On fait le menu de choix du programme
    {
        case 1: // Suivi de ligne, DEL verte
            color(0, 255, 0);
            break;

        case 2: // Evitement d'obstacles, DEL rouge
            color(255, 0, 0);
            break;

        case 3: // Ligne droite, DEL blanche
            color(255, 255, 255);
            break;

        case 4: // Carré, DEL jaune
            color(255, 255, 0);
            break;

        case 5: // Triangle, DEL bleue
            color(0, 0, 255);
            break;

        case 6: // Cercle, DEL violette
            color(238, 130, 238);
            break;

        default: // N'est pas sensé arriver, affiche la couleur d'erreur (marron)
            color(165, 42, 42);
            break;
    }
}
