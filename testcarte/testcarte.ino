const int PIN_LED_RED = 2;
const int PIN_LED_BLUE = 15;
const int PIN_LED_GREEN = 4;
const int channelLedRed = 2;
const int channelLedGreen = 3;
const int channelLedBlue = 4;
const int freq = 490;
const int resolution = 8;

void setup()
{
    pinMode(PIN_LED_RED, OUTPUT);
    pinMode(PIN_LED_GREEN, OUTPUT);
    pinMode(PIN_LED_BLUE, OUTPUT);

    ledcSetup(channelLedRed, freq, resolution);
    ledcSetup(channelLedGreen, freq, resolution);
    ledcSetup(channelLedBlue, freq, resolution);
    ledcAttachPin(PIN_LED_RED, channelLedRed);

    ledcAttachPin(PIN_LED_GREEN, channelLedGreen);
    ledcAttachPin(PIN_LED_BLUE, channelLedBlue);

}
void color (int redPWM, int greenPWM, int bluePWM) // Set color
{
    analogWrite(PIN_LED_RED, redPWM);
    analogWrite(PIN_LED_GREEN, greenPWM);
    analogWrite(PIN_LED_BLUE, bluePWM);
}

void loop()
{
  color(0,255,0);
}
