//error in vsc but IDE should be fine
#include <WiFi.h>
#include <SPI.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

//setup pins and variables
float targetX, targetY = 0;
int rotation, lifting = 0;

struct motorSpeeds {
    int tl;
    int tr;
    int bl;
    int br;
};

float tuneP = 1;
float tuneI = 1;
float tuneD = 1;

const int in = 19;
const int out = 23;
const int tick = 18;
const int pick = 15;

const int motor1 = 13;
const int motor2 = 12;
const int motor3 = 14;
const int motor4 = 27;

const char* ssid = "placeholderSsid";
const char* pass = "placeholderPass";

float readings[20] = { };

const char page[] = R"rawliteral(
<!DOCTYPE html>
<html>
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>ORNflight 0.1</title>
    </head>
<body>
    <div class="joystick" id="leftStick" data-x="0" data-y="0"></div>
    <div class="joystick" id="rightStick" data-val="0">Hello bitches</div>
    <script>
        let joysticks = document.getElementsByClassName("joystick");
        function dragElement(event, element) { //moves the element
            elX = window.getComputedStyle(element).left;
            elY = window.getComputedStyle(element).top;

            element.style.left = `${event.movementX}px`;
            element.style.left = `${event.movementX}px`;
        }
        joysticks.foreach((element) => {
            element.addEventListener("mouseDown", (e) => {
                moving = (event) => dragElement(e, element);
                element.addEventListener("mousemove", moving);
            });
            element.addEventListener("mouseup", () => {   
                element.removeEventListener("mousemove");
            });
        });

    </script>
</body>
</html>
)rawliteral";

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
    WiFi.softAP(ssid, pass, 1, 0, 1);

    Serial.println(ssid);
    Serial.println(pass);
    Serial.println(WiFi.softAPIP());  

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) { //prepares function for requests 
        Serial.print("request");
        request->send(200, "text/html", page);
    });

    server.begin();
    //sensor checks and calibration
}

void loop() {
    
    //webserver connection stuff
    //read sensor data (SPI)
    //something something websocket reading something idk
    //will probably need a storage of x previous readings for stabilization
    //stabilize()
    delay(1000);
    
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

motorSpeeds motorTranslate(float deltaX, float deltaY) { //translates deltas to direct motor adjustment

}

void motorWrite(float motorTL, float motorTR, float motorBL, float motorBR) { 

}



