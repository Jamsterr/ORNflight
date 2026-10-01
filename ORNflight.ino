//error in vsc but IDE should be fine
#include <WiFi.h>
#include <SPI.h>
#include <ESPAsyncWebserver>
#include <AsyncTCP>
#include <littleFS>

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

//setup pins and variables
float targetX, targetY = 0;
int rotation, lifting = 0;

float tuneP = 1;
float tuneI = 1;
float tuneD = 1;

const int MISO = 19;
const int MOSI = 23;
const int tick = 18;
const int pick = 5;

const int motor1 = 13;
const int motor2 = 12;
const int motor3 = 14;
const int motor4 = 27;

const char* ssid = "placeholderSsid";
const char* pass = "placeholderPass";

float readings[20] = { };

void setup() {
    Serial.begin(115200);

    pinMode(MISO, INPUT);
    pinMode(MOSI, OUTPUT);
    pinMode(tick, OUTPUT);
    pinMode(pick, OUTPUT);

    pinMode(motor1, OUTPUT);
    pinMode(motor2, OUTPUT);
    pinMode(motor3, OUTPUT);
    pinMode(motor4, OUTPUT);

    //webserver init, might be replaced with async stuff
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, pass, 1, 1);

    Serial.println(ssid);
    Serial.println(pass);
    Serial.println(WiFi.localIP());  

    while(WiFi.status() != WL_CONNECTED) { //doesnt seem to notice connection
        delay(500);
        Serial.print("waiting.");
    }
    Serial.println("connected");
    Serial.println(WiFi.localIP());
    //sensor checks and calibration
}

void loop() {
    //webserver connection stuff
    //read sensor data (SPI)
    //something something websocket reading something idk
    //will probably need a storage of x previous readings for stabilization
    //stabilize()
    delay(500);
}

void throttle(int amp) { //might be put inline along with jaw
    lifting = amp;
    //changes thrust of all motors
    //relative to max current
}

void jaw(int amp) {
    //sets relative difference bewteen ccw and cw motor pairs in order to rotate
}

void stabilize(float target, float reading) { //handles stabilizing, roll and pitch
    //trapozoidal rule seems best for integral
    //calc error from desired v actual angle
    //output calculated with error in integral, derivative* and proportional dimensions'
    //translate output value into motor adjustment
    //add high pass filtering for safety
}

int[] motorTranslate(float deltaX, float deltaY) { //translates deltas to direct motor adjustment

}
void motorWrite(float motorTL, float motorTR, float motorBL, float motorBR) { 

}



