const int ONOFF = 2;
const int RED = 5;
const int GREEN = 3;
const int BLUE = 6;

volatile bool swap = 0;

void setup ()
{
  Serial.begin(9600);
  pinMode(ONOFF, INPUT_PULLUP);
  pinMode(RED, OUTPUT);
  pinMode(BLUE, OUTPUT);
  pinMode(GREEN, OUTPUT);
  attachInterrupt(0, allezla, FALLING);
}

void color (int red, int green, int blue)
{
  analogWrite(RED, red);
  analogWrite(GREEN, green);
  analogWrite(BLUE, blue);
}

void allezla ()
{
  swap = !swap;
}

void loop ()
{
  Serial.println(swap);
  if (!swap) color(0, 0, 0);
  while (!swap);
  color(255, 0, 0);
}