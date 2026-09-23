//error in vsc but IDE should be fine
#include <WiFi.h>

//port 80 webserver
WiFiServer server(80);

//setup pins and variables
float targetX, targetY = 0;
int rotation, lifting = 0;

float tuneP = 1;
float tuneI = 1;
float tuneD = 1;

//-- pins may be subject to change
const int SDI = 10;
const int SDO = 11;
const int clock = 12;
const int select = 13;

const int motor1 = 6;
const int motor2 = 7;Ö
const int motor3 = 8;
const int motor4 = 9;
//--

const char* ssid = "placeholderSsid";
const char* pass = "placeholderPass";

float readings[20] = { };

void setup() {
    Serial.Begin(115200);

    pinMode(SDI, INPUT);
    pinMode(SDO, OUTPUT);
    pinMode(clock, OUTPUT);
    pinMode(select, OUTPUT);

    pinMode(motor1, OUTPUT);
    pinMode(motor2, OUTPUT);
    pinMode(motor3, OUTPUT);
    pinMode(motor4, OUTPUT);

    //webserver init, connect to device
    WiFi.begin(ssid, pass);
    Serial.WriteLine(ssid);
    Serial.WriteLine(pass);

    while(WiFi.status != WL_CONNECTED) {
        delay(500);
        Serial.printLine()
    }
    //sensor checks and calibration
}

void loop() {
    //webserver connection check
    //look for controller input
    //modify thrust variables per motor 
    //read sensor data (SPI)
    //will probably need a storage of x previous readings for stabilization
    //stabilize()
}

void throttle(int amp) { //might be put inline along with jaw
    lifting = amp;
    //changes thrust of all motors
    //relative to max current
}

void jaw(int amp) {
    //sets relative difference bewteen ccw and cw motor pairs in order to rotate
}

void stabilize(float angleX, float angleY) { //handles stabilizing, roll and pitch
    //calc error from desired v actual angle
    //output calculated with error in integral, derivative* and proportional dimensions'
    //translate output value into motor adjustment
    //add high pass filtering for safety
}



