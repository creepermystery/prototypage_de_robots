// Load Wi-Fi library
#include <WiFi.h>

// Replace with your network credentials

const char* ssid = "JUNIA_LAB";
const char* password = "813nV3nue@";
const char* ssid = "Freebox-4A8444";
const char* password = "conpernem-alarius*-exaruerat-oblito";

// pins moteurs gauche
const int PIN_DIR_MOTOR_LEFT = 27;
const int PIN_MOTOR_LEFT = 14;
const int PIN_ODOMETER_A_LEFT = 12;
const int PIN_ODOMETER_B_LEFT = 13;

// pins moteur droite
const int PIN_DIR_MOTOR_RIGHT = 19;
const int PIN_MOTOR_RIGHT = 21;
const int PIN_ODOMETER_A_RIGHT = 22;
const int PIN_ODOMETER_B_RIGHT = 23;

// pins IR terrestres
const int PIN_FLOOR_IR_LEFT = 25;
const int PIN_FLOOR_IR_RIGHT = 26;

// pins IR frontaux
const int PIN_BACK_IR = 33;
const int PIN_FRONT_IR = 32;

// pins boutons
const int PIN_BUTTON_VALID = 16;
const int PIN_BUTTON_UP = 5;
const int PIN_BUTTON_DOWN = 17;
const int PIN_BUTTON_ON_OFF = 18;

// pins DELs
const int PIN_LED_RED = 2;
const int PIN_LED_GREEN = 4;
const int PIN_LED_BLUE = 15;

// paramètres PWM
const int FREQ = 490;
const int RESOLUTION = 8; // 8 bits de résolution (de 0 à 255)
const int CHANNEL_MOTOR_LEFT = 0;
const int CHANNEL_MOTOR_RIGHT = 1;
const int CHANNEL_LED_RED = 2;
const int CHANNEL_LED_GREEN = 3;
const int CHANNEL_LED_BLUE = 4;

// constantes globales
const float RAPPORT_REDUCTION_MOTEUR = 52.734;
const int CPR_ODOMETRE = 6;
const int RAYON_ROUE = 33;
const int FILTER = 1800;
const float ECART_ROUES = 21.5;
const int LIMITE_DECALAGE = 100;
const long timeoutTime = 2000;

// variables globales
volatile long compteurDroite = 0;
volatile long compteurGauche = 0;
volatile bool etatBoutonOnOff = 0;
volatile int choix = 1;
volatile bool validPressed = 0;
String header;
String state = "off";
unsigned long currentTime = millis();
unsigned long previousTime = 0;
WiFiServer server(80);

// Anti-Rebonds
const int debounce = 200;
unsigned long debounceValid = millis();
unsigned long debounceIncrementerChoix = millis();
unsigned long debounceDecrementerChoix = millis();
unsigned long debounceOnOff = millis();

void swapValid () // Passe valid à 1
{
    if (millis() - debounceValid > debounce)
    {
        validPressed = 1;
        debounceValid = millis();
    }
}

void triggerOdometreDroite () // Fonction de comptage de l'odomètre de droite
{
    if (digitalRead(PIN_ODOMETER_A_RIGHT) == digitalRead(PIN_ODOMETER_B_RIGHT)) compteurDroite++;
    else compteurDroite--;
}

void triggerOdometreGauche () // Fonction de comptage de l'odomètre de gauche
{
    if (digitalRead(PIN_ODOMETER_A_LEFT) == digitalRead(PIN_ODOMETER_B_LEFT)) compteurGauche--;
    else compteurGauche++;
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
    // Pins moteurs gauche
    pinMode(PIN_DIR_MOTOR_LEFT, OUTPUT);
    pinMode(PIN_MOTOR_LEFT, OUTPUT);
    pinMode(PIN_ODOMETER_A_LEFT, INPUT_PULLUP);
    pinMode(PIN_ODOMETER_B_LEFT, INPUT_PULLUP);

    ledcAttachPin(PIN_MOTOR_LEFT, CHANNEL_MOTOR_LEFT);
    ledcSetup(CHANNEL_MOTOR_LEFT, FREQ, RESOLUTION);

    // Pins moteurs droite
    pinMode(PIN_DIR_MOTOR_RIGHT, OUTPUT);
    pinMode(PIN_MOTOR_RIGHT, OUTPUT);
    pinMode(PIN_ODOMETER_A_RIGHT, INPUT_PULLUP);
    pinMode(PIN_ODOMETER_B_RIGHT, INPUT_PULLUP);

    ledcAttachPin(PIN_MOTOR_RIGHT, CHANNEL_MOTOR_RIGHT);
    ledcSetup(CHANNEL_MOTOR_RIGHT, FREQ, RESOLUTION);

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

    ledcAttachPin(PIN_LED_RED, CHANNEL_LED_RED);
    ledcAttachPin(PIN_LED_GREEN, CHANNEL_LED_GREEN);
    ledcAttachPin(PIN_LED_BLUE, CHANNEL_LED_BLUE);

    ledcSetup(CHANNEL_LED_RED, FREQ, RESOLUTION);
    ledcSetup(CHANNEL_LED_GREEN, FREQ, RESOLUTION);
    ledcSetup(CHANNEL_LED_BLUE, FREQ, RESOLUTION);

    // Interruptions odomètre
    attachInterrupt(PIN_ODOMETER_A_LEFT, triggerOdometreDroite, FALLING);
    attachInterrupt(PIN_ODOMETER_A_RIGHT, triggerOdometreGauche, FALLING);

    // Interruptions boutons
    attachInterrupt(PIN_BUTTON_DOWN, decrementerChoix, FALLING);
    attachInterrupt(PIN_BUTTON_UP, incrementerChoix, FALLING);
    attachInterrupt(PIN_BUTTON_ON_OFF, swapOnOff, FALLING);
    attachInterrupt(PIN_BUTTON_VALID, swapValid, FALLING);

      Serial.begin(115200);

    // Connect to Wi-Fi network with SSID and password
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    // Print local IP address and start web server
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    server.begin();
}

void attendre (int temps) // Fonction permettant d'attendre pendant un temps exprimé en millisecondes
{
    unsigned long currentTime = millis();
    while (millis() < currentTime + temps) if (!etatBoutonOnOff) break;
}

void tournerDroite (int angle) 
{
    compteurDroite = 0;
    compteurGauche = 0;

    float angleEstime = 0;

    int distanceRoueDroite = 0;
    int distanceRoueGauche = 0;

    int err = 0;

    ledcWrite(CHANNEL_MOTOR_LEFT, 255); // On choisit les bonnes direction de rotation des roues et on démarre le virage
    digitalWrite(PIN_DIR_MOTOR_LEFT, LOW);
    ledcWrite(CHANNEL_MOTOR_RIGHT, 255);
    digitalWrite(PIN_DIR_MOTOR_RIGHT, LOW);

    while (angleEstime < angle) // On attend que le virage soit fait
    {
        distanceRoueDroite = 2*PI*RAYON_ROUE * abs(compteurDroite)/(CPR_ODOMETRE*RAPPORT_REDUCTION_MOTEUR);
        distanceRoueGauche = 2*PI*RAYON_ROUE * abs(compteurGauche)/(CPR_ODOMETRE*RAPPORT_REDUCTION_MOTEUR);
        angleEstime = abs(distanceRoueDroite/(ECART_ROUES/2.0) + distanceRoueGauche/(ECART_ROUES/2.0))/2.0;

        err = abs(compteurDroite) - abs(compteurGauche);  // On ajuste la rotation selon le décalage des odomètres

        if (err > LIMITE_DECALAGE)
        {
            ledcWrite(CHANNEL_MOTOR_LEFT, 255);  // ...Si le moteur droit va plus vite
            ledcWrite(CHANNEL_MOTOR_RIGHT, 255-20);
        }
        else if (err < -1*LIMITE_DECALAGE)
        {
            ledcWrite(CHANNEL_MOTOR_LEFT, 255-20); // ...Si le moteur gauche va plus vite
            ledcWrite(CHANNEL_MOTOR_RIGHT, 255);
        }
        else
        {
            ledcWrite(CHANNEL_MOTOR_LEFT, 255); // ...Si les deux sont synchrones
            ledcWrite(CHANNEL_MOTOR_RIGHT, 255);
        }

        if (!etatBoutonOnOff) break;
    }

    ledcWrite(CHANNEL_MOTOR_LEFT, 0); // On remet les sorties à zéro
    ledcWrite(CHANNEL_MOTOR_RIGHT, 0);
}

void tournerGauche (int angle)
{
    compteurDroite = 0;
    compteurGauche = 0;
    float angleEstime = 0;
    int distanceRoueDroite = 0;
    int distanceRoueGauche = 0;
    int err = 0;

    ledcWrite(CHANNEL_MOTOR_LEFT, 70);                // On choisit les bonnes direction de rotation des roues et on démarre le virage
    digitalWrite(PIN_DIR_MOTOR_LEFT, LOW);
    ledcWrite(CHANNEL_MOTOR_RIGHT, 70);
    digitalWrite(PIN_DIR_MOTOR_RIGHT, LOW);

    while (angleEstime < angle) // On attend que le virage soit fait
    {
        distanceRoueDroite = 2*PI*RAYON_ROUE * abs(compteurDroite)/(CPR_ODOMETRE*RAPPORT_REDUCTION_MOTEUR);
        distanceRoueGauche = 2*PI*RAYON_ROUE * abs(compteurGauche)/(CPR_ODOMETRE*RAPPORT_REDUCTION_MOTEUR);
        angleEstime = (abs(distanceRoueDroite/(ECART_ROUES/2.0) + distanceRoueGauche/(ECART_ROUES/2.0))/2.0)*180/PI;

        err = abs(compteurDroite) - abs(compteurGauche);  // On ajuste la rotation selon le décalage des odomètres

        if (err > LIMITE_DECALAGE)
        {
            ledcWrite(CHANNEL_MOTOR_LEFT, 70);  // ...Si le moteur droit va plus vite
            ledcWrite(CHANNEL_MOTOR_RIGHT, 50);
        }
        else if (err < -1*LIMITE_DECALAGE)
        {
            ledcWrite(CHANNEL_MOTOR_LEFT, 50); // ...Si le moteur gauche va plus vite
            ledcWrite(CHANNEL_MOTOR_RIGHT, 70);
        }
        else
        {
            ledcWrite(CHANNEL_MOTOR_LEFT, 70); // ...Si les deux sont synchrones
            ledcWrite(CHANNEL_MOTOR_RIGHT, 70);
        }

        if (!etatBoutonOnOff) break;
    }

    ledcWrite(CHANNEL_MOTOR_LEFT, 0); // On remet les sorties à zéro
    ledcWrite(CHANNEL_MOTOR_RIGHT, 0);
}

void toutDroit (int distanceCommandee)
{
    compteurDroite = 0;
    compteurGauche = 0;

    float distanceR = 0.0;
    float distanceL = 0.0;

    digitalWrite(PIN_DIR_MOTOR_LEFT, HIGH);
    digitalWrite(PIN_DIR_MOTOR_RIGHT, LOW);

    ledcWrite(CHANNEL_MOTOR_LEFT, 255);
    ledcWrite(CHANNEL_MOTOR_RIGHT, 255);

    while (distanceR < distanceCommandee && distanceL < distanceCommandee)
    {
        distanceR = (abs(compteurDroite)/(CPR_ODOMETRE*RAPPORT_REDUCTION_MOTEUR))*2*PI*RAYON_ROUE;
        distanceL = (abs(compteurGauche)/(CPR_ODOMETRE*RAPPORT_REDUCTION_MOTEUR))*2*PI*RAYON_ROUE;

        if (distanceR > distanceL*1.05)
        {
            ledcWrite(CHANNEL_MOTOR_LEFT, 255);
            ledcWrite(CHANNEL_MOTOR_RIGHT, 255-20);
        }
        else if (distanceR*1.05 < distanceL)
        {
            ledcWrite(CHANNEL_MOTOR_LEFT, 255);
            ledcWrite(CHANNEL_MOTOR_RIGHT, 255-20);
        }
        else
        {
            ledcWrite(CHANNEL_MOTOR_LEFT, 255);
            ledcWrite(CHANNEL_MOTOR_RIGHT, 255);
        }

        if (!etatBoutonOnOff) break;
    }

    ledcWrite(CHANNEL_MOTOR_LEFT, 0);
    ledcWrite(CHANNEL_MOTOR_RIGHT, 0);
}

void trajectoireCirculaire (int rayonTrajectoire, int angle) // Rayon en millimètres et angle en degrès
{
    compteurDroite = 0;
    compteurGauche = 0;

    int channelRoueExterieure;
    int channelRoueInterieure;

    volatile long *compteurExterieur;
    volatile long *compteurInterieur;

    if (angle > 0) // Virage à gauche
    {
        channelRoueExterieure = CHANNEL_MOTOR_RIGHT;
        channelRoueInterieure = CHANNEL_MOTOR_LEFT;
        compteurExterieur = &compteurDroite;
        compteurInterieur = &compteurGauche;
    }
    else if (angle < 0) // Virage à droite
    {
        channelRoueExterieure = CHANNEL_MOTOR_LEFT;
        channelRoueInterieure = CHANNEL_MOTOR_RIGHT;
        compteurExterieur = &compteurGauche;
        compteurInterieur = &compteurDroite;
    }
    else return; // Si l'angle est égal à 0 on stoppe la fonction

    float rayonCercleExterieur = rayonTrajectoire + ECART_ROUES/2.0; // Rayon du cercle parcouru par chaque roue
    float rayonCercleInterieur = rayonTrajectoire - ECART_ROUES/2.0;

    float longueurArcExterieur = rayonCercleExterieur * (PI/180.0) * angle; // Longueur de l'arc parcouru par chaque roue
    float longueurArcInterieur = rayonCercleInterieur * (PI/180.0) * angle;

    float rapportDeuxArcs = longueurArcInterieur/longueurArcExterieur;

    digitalWrite(PIN_DIR_MOTOR_LEFT, HIGH); // Les deux moteurs tournent vers l'avant
    digitalWrite(PIN_DIR_MOTOR_RIGHT, LOW);

    ledcWrite(channelRoueExterieure, 255); // On lance les moteurs à la vitesse nécessaire
    ledcWrite(channelRoueInterieure, 255*rapportDeuxArcs);

    float longueurParcourueArcExterieur = 0;
    float longueurParcourueArcInterieur = 0;
    
    while (longueurParcourueArcExterieur < longueurArcExterieur && longueurParcourueArcInterieur < longueurArcInterieur) // Tant que les arcs ne sont pas complets on continue d'avancer
    {
        longueurParcourueArcExterieur = 2*PI*RAYON_ROUE*(*compteurExterieur/(RAPPORT_REDUCTION_MOTEUR*CPR_ODOMETRE)); // Estimation de la longueur déjà parcourue par chaque roue
        longueurParcourueArcInterieur = 2*PI*RAYON_ROUE*(*compteurInterieur/(RAPPORT_REDUCTION_MOTEUR*CPR_ODOMETRE));

        if (longueurParcourueArcExterieur*rapportDeuxArcs > longueurParcourueArcInterieur * 1.05) // Si le moteur extérieur se déplace trop vite
        {
            ledcWrite(channelRoueExterieure, 255 - 20);
            ledcWrite(channelRoueInterieure, 255*rapportDeuxArcs);
        }
        else if (longueurParcourueArcExterieur*rapportDeuxArcs < longueurParcourueArcInterieur * 0.95) // Si le moteur intérieur se déplace trop vite
        {
            ledcWrite(channelRoueExterieure, 255);
            ledcWrite(channelRoueInterieure, 255*rapportDeuxArcs - 20);
        }
        else // Si les deux moteurs se déplacent à vitesse synchrone
        {
            ledcWrite(channelRoueExterieure, 255);
            ledcWrite(channelRoueInterieure, 255*rapportDeuxArcs);
        }
        if (!etatBoutonOnOff || choix != 6) break;
    }

    ledcWrite(CHANNEL_MOTOR_LEFT, 0); // On éteint les moteurs à la fin de la trajectoire
    ledcWrite(CHANNEL_MOTOR_RIGHT, 0);
}

void carre (int largeur)
{
    for (int i = 0; i < 4; i++)
    {
        toutDroit(largeur);
        tournerDroite(90);
    }
}

void triangle (int largeur)
{
    for (int i = 0; i < 3; i++)
    {
        toutDroit(largeur);
        tournerDroite(120);
    }
}

void cercle (int diametre)
{
    trajectoireCirculaire(diametre/2, 360); // On fait faire au robot une trajectoire circulaire sur 360 degrés
}

void suiviLigne () // La fonction tourne à l'infini
{
    int valuePWMLeft = 255;
    int valuePWMRight = 255;

    digitalWrite(PIN_DIR_MOTOR_LEFT, HIGH);
    digitalWrite(PIN_DIR_MOTOR_RIGHT, LOW);

    while (true)
    {
        if (digitalRead(PIN_FLOOR_IR_LEFT)) valuePWMLeft = 0; // Si le robot perçoit la ligne gauche, on fait s'arrêter la roue gauche
        else valuePWMLeft = 255;

        if (digitalRead(PIN_FLOOR_IR_RIGHT)) valuePWMRight = 0; // Si le robot perçoit la ligne droite, on fait s'arrêter la roue droite
        else valuePWMRight = 255;

        ledcWrite(CHANNEL_MOTOR_LEFT, valuePWMLeft);
        ledcWrite(CHANNEL_MOTOR_RIGHT, valuePWMRight);

        if (!etatBoutonOnOff || choix != 1) break; // On arrête le programme de suivi de ligne si le bouton on/off est pressé ou si le choix change
    }

    ledcWrite(CHANNEL_MOTOR_LEFT, 0); // On oublie pas d'arrêter les moteurs
    ledcWrite(CHANNEL_MOTOR_RIGHT, 0);
}

void evitementObstacles () // La fonction tourne à l'infini
{

    digitalWrite(PIN_DIR_MOTOR_LEFT, HIGH);
    digitalWrite(PIN_DIR_MOTOR_RIGHT, LOW);

    ledcWrite(CHANNEL_MOTOR_LEFT, 255);
    ledcWrite(CHANNEL_MOTOR_RIGHT, 255);

    while (true)
    {
        
        if (analogRead(PIN_BACK_IR) > FILTER && analogRead(PIN_FRONT_IR)) // Si le capteur détecte un obstacle devant et derrière
        {
            digitalWrite(PIN_DIR_MOTOR_LEFT, LOW);
            digitalWrite(PIN_DIR_MOTOR_RIGHT, LOW);
            ledcWrite(CHANNEL_MOTOR_LEFT, 255);
            ledcWrite(CHANNEL_MOTOR_RIGHT, 255);
        }
        else if (analogRead(PIN_BACK_IR) > FILTER) // Si le capteur détecte un obstacle derrière
        {
            digitalWrite(PIN_DIR_MOTOR_LEFT, HIGH);
            digitalWrite(PIN_DIR_MOTOR_RIGHT, LOW);
            ledcWrite(CHANNEL_MOTOR_LEFT, 255);
            ledcWrite(CHANNEL_MOTOR_RIGHT, 255);
        }
        else if (analogRead(PIN_FRONT_IR) > FILTER) // Si le capteur détecte un obstacle devant
        {
            digitalWrite(PIN_DIR_MOTOR_LEFT, LOW);
            digitalWrite(PIN_DIR_MOTOR_RIGHT, HIGH);
            ledcWrite(CHANNEL_MOTOR_LEFT, 255);
            ledcWrite(CHANNEL_MOTOR_RIGHT, 255);
        }
        else // Si rien n'est détecté
        {
            ledcWrite(CHANNEL_MOTOR_LEFT, 0);
            ledcWrite(CHANNEL_MOTOR_RIGHT, 0);
        }

        if (!etatBoutonOnOff || choix != 2) break; // On arrête le programme de d'évitement d'obstacles si le bouton on/off est pressé ou si le choix change
    }
    ledcWrite(CHANNEL_MOTOR_LEFT, 0); // On oublie pas d'arrêter les moteurs
    ledcWrite(CHANNEL_MOTOR_RIGHT, 0);
}

void color (int redPWM, int greenPWM, int bluePWM) // Set color
{
    ledcWrite(CHANNEL_LED_RED, redPWM);
    ledcWrite(CHANNEL_LED_GREEN, greenPWM);
    ledcWrite(CHANNEL_LED_BLUE, bluePWM);
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
                validPressed = 0;
            }
            break;

        case 2: // Evitement d'obstacles, DEL rouge
            color(255, 0, 0);
            if (validPressed)
            {
                validPressed = 0;
                evitementObstacles();
                validPressed = 0;
            }
            break;

        case 3: // Ligne droite, DEL blanche
            color(255, 255, 255);
            if (validPressed)
            {
                validPressed = 0;
                toutDroit(1000);
                validPressed = 0;
            }
            break;

        case 4: // Carré, DEL jaune
            color(255, 255, 0);
            if (validPressed)
            {
                validPressed = 0;
                carre(500);
                validPressed = 0;
            }
            break;

        case 5: // Triangle, DEL bleue
            color(0, 0, 255);
            if (validPressed)
            {
                validPressed = 0;
                triangle(500);
                validPressed = 0;
            }
            break;

        case 6: // Cercle, DEL violette
            color(238, 130, 238);
            if (validPressed)
            {
                validPressed = 0;
                cercle(500);
                validPressed = 0;
            }
            break;

        default: // N'est pas sensé arriver, affiche la couleur d'erreur (marron)
            color(165, 42, 42);
            break;
    }
    WiFiClient client = server.available(); // Listen for incoming clients

    if (client) { // If a new client connects,
        currentTime = millis();
        previousTime = currentTime;
        Serial.println("New Client."); // print a message out in the serial port
        String currentLine = ""; // make a String to hold incoming data from the client
        while (client.connected() && currentTime - previousTime <= timeoutTime) { // loop while the client's connected
            currentTime = millis();
            if (client.available()) { // if there's bytes to read from the client,
                char c = client.read(); // read a byte, then
                Serial.write(c); // print it out the serial monitor
                header += c;
                if (c == '\n') { // if the byte is a newline character
                    // if the current line is blank, you got two newline characters in a row.
                    // that's the end of the client HTTP request, so send a response:
                    if (currentLine.length() == 0) {
                        // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
                        // and a content-type so the client knows what's coming, then a blank line:
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-type:text/html");
                        client.println("Connection: close");
                        client.println();

                        // turns the GPIOs on and off
                            if (header.indexOf("GET /off") >= 0) swapOnOff();
                            else if(header.indexOf("GET /Avant") >= 0) toutDroit(100);
                            else if(header.indexOf("GET /Gauche") >= 0) tournerGauche(90);
                            else if(header.indexOf("GET /Droite") >= 0) tournerDroite(90);
                            else if(header.indexOf("GET /Arriere") >= 0);
                            else if(header.indexOf("GET /Triangle") >= 0) triangle(500);
                            else if(header.indexOf("GET /Suivideligne") >= 0) suiviLigne();
                            else if(header.indexOf("GET /Lignedroite") >= 0) toutDroit(1000);
                            else if(header.indexOf("GET /Carre") >= 0) carre(500);
                            else if(header.indexOf("GET /Evitementdobstacle") >= 0) evitementObstacles();
                            else if(header.indexOf("GET /Cercle") >= 0) cercle(500);

                        // Display the HTML web page
                        client.println("<!DOCTYPE html><html>");
                        client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
                        client.println("<link rel=\"icon\" href=\"data:,\">");
                        // CSS to style the on/off buttons 
                        // Feel free to change the background-color and font-size attributes to fit your preferences
                        client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
                        client.println(".styled {border: 0;line-height: 2.5;padding: 0 20px;font-size: 1rem; text-align: center;");
                        client.println("color: #fff; border-radius: 30px;background-color: #edbef8; }");
                        client.println(".styled:hover {background-color:#e791fa ;}");
                        client.println(".styled:active {box-shadow:inset 2px 2px 3px rgb(0 0 0 / 60%);background-color:#d000ff ;}");
                        client.println("body{background-color: #f6e1f5;margin: 0px;}");
                        client.println(".container{display:flex;align-content: center;}");
                        client.println(".fonction1{width: 800px;align-content: center;}");
                        client.println(".fonction2{width: 500px;align-content: center;}");
                        client.println(".entete{background-color: #e791fa ;color:#fff;} </style></head>");

                        // Web Page Heading
                        client.println("<body><div class=\"entete\"><h1>Commande du robot mobile</h1></div>");

                        // Boutton ON/OFF 
                        if (state=="off") {
                            client.println("<div><p>Bouton ON/OFF </p><p><a href=\"/on\"><button class=\"styled\">ON</button></a></p><br><br> </div>");
                            state="on";
                        } else {
                            client.println("<div><p>Bouton ON/OFF </p><p><a href=\"/off\"><button class=\"styled\">OFF</button></a></p><br><br> </div>");
                            state="off";
                        } 

                     // fleche haut
                        client.println("<div><p>Fleches pour deplacer le robot </p><p><a href=\"/Avant\"><button class=\"styled\">Avant</button></a></p> </div>");
                        // fleche gauche et droite
                        client.println("<div class = \"container\"><div class=\"fonction1\"><br>");
                        client.println("<p><a href=\"/Gauche\"><button class=\"styled\">Gauche</button></a></p> </div>");
                        client.println("<div class=\"fonction1\"><br><p><a href=\"/Droite\"><button class=\"styled\">Droite</button></a></p></div></div>");
                        //fleche bas
                        client.println("<div><br><p><a href=\"/Arriere\"><button class=\"styled\">Arriere</button></a></p></div>");
                        //Mode de fonctionnements 
                        client.println("<div><br><p>Modes de fonctionnement</p><br></div>");
                        client.println("<div class = \"container\">");
                        client.println("<div class=\"fonction2\"><p><a href=\"/Triangle\"><button class=\"styled\">Triangle</button></a></p></div> ");
                        client.println("<div class=\"fonction2\"><p><a href=\"/Suivideligne\"><button class=\"styled\">Suivi de ligne</button></a></p></div> ");
                        client.println("<div class=\"fonction2\"><p><a href=\"/Lignedroite\"><button class=\"styled\">Ligne droite</button></a></p></div> <br> </div>");
                        client.println("<div><br></div>");
                        client.println("<div class = \"container\">");
                        client.println("<div class=\"fonction2\"><p><a href=\"/Carre\"><button class=\"styled\">Carre</button></a></p></div> ");
                        client.println("<div class=\"fonction2\"><p><a href=\"/Evitementdobstacle\"><button class=\"styled\">Evitement d'obstacle</button></a></p></div> ");
                        client.println("<div class=\"fonction2\"><p><a href=\"/Cercle\"><button class=\"styled\">Cercle</button></a></p></div> <br> </div>");
                        client.println("</body></html>");
                        
                        // The HTTP response ends with another blank line
                        client.println();
                        // Break out of the while loop
                        break;
                    } else { // if you got a newline, then clear currentLine
                        currentLine = "";
                    }
                } else if (c != '\r') { // if you got anything else but a carriage return character,
                    currentLine += c; // add it to the end of the currentLine
                }
            }
        }
        // Clear the header variable
        header = "";
        // Close the connection
        client.stop();
        Serial.println("Client disconnected.");
        Serial.println("");
    }
}
