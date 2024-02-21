/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/

// Load Wi-Fi library
#include <WiFi.h>

// Replace with your network credentials
//const char* ssid = "Freebox-4A8444";
//const char* password = "conpernem-alarius*-exaruerat-oblito";
const char* ssid = "JUNIA_STUDENTS";
const char* password = "813nV3nue@Jun1a";

//const char* ssid = "Freebox-4A8444";
//const char* password = "conpernem-alarius*-exaruerat-oblito";
// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String output18State = "off";
String output27State = "off";

// Assign output variables to GPIO pins
const int output18 = 18;
const int output27 = 27;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 100000;
bool state = false;
void ON_OFF(){
  if(state){
     Serial.println("off");
     state=!state;
  }else{
     Serial.println("on");
     state=!state;
  }

}

void setup() {
  Serial.begin(115200);
  // Initialize the output variables as outputs
  pinMode(output18, OUTPUT);
  pinMode(output27, OUTPUT);
  // Set outputs to LOW
  digitalWrite(output18, LOW);
  digitalWrite(output27, LOW);

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

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
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
            if ("ON_OFF()") {
              Serial.println("COUBEH");
              output18State = "on";
              digitalWrite(output18, HIGH);
            } 
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
            client.println("<div><p>Bouton ON/OFF </p><input class=\"styled\" type=\"button\" value=\"ON/OFF\" onClick = \"ON_OFF()\" /><br><br> </div>");
           // fleche haut
            client.println("<div><p>Fleches pour deplacer le robot </p><input class=\"styled\" type=\"button\" value=\"Avant\" /> </div>");
            // fleche gauche et droite
            client.println("<div class = \"container\"><div class=\"fonction1\"><br>");
            client.println("<input class=\"styled\" type=\"button\" value=\"Gauche\" /></div>");
            client.println("<div class=\"fonction1\"><br><input class=\"styled\" type=\"button\" value=\"Droite\" /></div></div>");
            //fleche bas
            client.println("<div><br><input class=\"styled\" type=\"button\" value=\"Arriere\" /></div>");
            //Mode de fonctionnements 
            client.println("<div><br><p>Modes de fonctionnement</p><br></div>");
            client.println("<div class = \"container\">");
            client.println("<div class=\"fonction2\"><input class=\"styled\" type=\"button\" value=\"\Trinagle \" /></div> ");
            client.println("<div class=\"fonction2\"><input class=\"styled\" type=\"button\" value=\"Suivi de ligne\" /></div> ");
            client.println("<div class=\"fonction2\"><input class=\"styled\" type=\"button\" value=\"Ligne droite\" /></div> <br> </div>");
            client.println("<div><br></div>");
            client.println("<div class = \"container\">");
            client.println("<div class=\"fonction2\"><input class=\"styled\" type=\"button\" value=\"Carre\" /></div> ");
            client.println("<div class=\"fonction2\"><input class=\"styled\" type=\"button\" value=\"Evitement d'obstacle\" /></div> ");
            client.println("<div class=\"fonction2\"><input class=\"styled\" type=\"button\" value=\"Cercle\" /></div> <br> </div>");
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
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
