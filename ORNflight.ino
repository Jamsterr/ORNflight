//error in vsc but IDE should be fine
#include <WiFi.h>
#include <SPI.h>
#include <PID_v1.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>

const char page[] = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0 maximum-scale=1.0, user-scalable=no">
<title>ORNflight 0.4</title>
<style>
body {
border: none;
background-color: black;
color: white;
overflow: hidden;
}
.joystick {
width: 8vw;
height: 8vw;
z-index: 5;
background-color: #333;
border: 1vw solid #222;
border-radius: 50%;
position: absolute;
}
.boundBox {
position: absolute;
display: flex;
justify-content: center;
align-items: center;
bottom: 0;
margin: 1em;
width: 25vw;
height: 25vw;
background-color: #fff;
}
</style>
</head> 
<body>
<div class="boundBox">
<div class="joystick" id="leftStick" data-roll="0" data-pitch="0"></div>
</div>
<button style="margin-left: 5em;" onclick=screenHandler() id="fullscreen" data-isfullscreen="false">Fullscreen</button>
<div class="boundBox" style="right: 0; height: 100vh;">
<div class="joystick" id="rightStick" data-throttle="0" data-rotate="0"></div>
</div>
<script>
let joysticks = document.querySelectorAll(".joystick");
const host = `ws://${window.location.hostname}/ws`;
let websocket;
async function screenHandler() {
let button = document.getElementById("fullscreen");
if(button.dataset.isfullscreen === "false") { 
await document.body.requestFullscreen();
button.innerHTML = "leave";
button.dataset.isfullscreen = "true";
setup();
}
else {
await document.exitFullscreen();
button.innerHTML = "Fullscreen";
button.dataset.isfullscreen = "false";
setup();
}              
}
window.addEventListener("resize", () =>  {
reset(); //doesnt work
});
function setup() {
joysticks.forEach(joy => {
joy.stick = joy.getBoundingClientRect();
joy.boundry = joy.parentElement.getBoundingClientRect();
joy.dragging = null;
joy.originX = getComputedStyle(joy).left;
joy.originY = getComputedStyle(joy).top;
if(joy == joysticks[1]) joy.style.top = `${joy.boundry.height - joy.stick.height/2}px`;
});
}
function reset() {
joysticks.forEach(joystick => {
joystick.boundry = joystick.parentElement.getBoundingClientRect();
joystick.originX = `${joystick.boundry.width/2 - joystick.stick.width/2}px`;
joystick.originY = `${joystick.boundry.height/2 - joystick.stick.height/2}px`;
joystick.style.left = joystick.originX;
joystick.relY = 0;
joystick.relX = 0;
if(joystick == joysticks[0]) joystick.style.top = joystick.originY;
else joystick.style.top = `${joystick.boundry.height - joystick.stick.height/2}px`;
joystick.stick = joystick.getBoundingClientRect();
});
}
document.addEventListener("DOMContentLoaded", () => {
setup();
loadWS();
});
function loadWS() { //init and event handling
websocket = new WebSocket(host);
websocket.onopen = wsOpen;
//websocket.onmessage = wsMessage;
websocket.onclose = wsClose;
}
function wsOpen() {
console.log("ws is open");
}
function wsClose() {
console.log("ws down");
setTimeout(loadWS, 2000);
}
function report(joy) { //rn unformatted
let stickPlace = joy.getBoundingClientRect();
let id;
let joyX = -1*(stickPlace.left + joy.stick.width/2 - (joy.boundry.left + joy.boundry.width/2));
let joyY = -1*(stickPlace.top + joy.stick.height/2 - (joy.boundry.top + joy.boundry.height/2));
let relX = Math.round(1000* joyX / (joy.boundry.width/2));
let relY;
if(joy == joysticks[1]) {
    relY = Math.round(1000*(joyY + joy.boundry.height/2)/joy.boundry.height);
    id = "2";
}
else {
    relX = Math.round(1000* joyY / (joy.boundry.height/2));
    id = "1";
}
if(Math.abs(relX) < 200) relX = 0; //deadzones
if(joy == joysticks[0] && Math.abs(relY) < 200 || joy == joysticks[1] && Math.abs(relY) < 50 ) relY = 0;
if(joy.relY != relY) websocket.send(`y${id}::${relY}`);
if(joy.relX != relX) websocket.send(`x${id}::${relX}`);
joy.relX = relX;
joy.relY = relY;
console.log(`y${id}::${relY}`);
console.log(`x${id}::${relX}`);
}
function dragElement(touch, element) {     
let leftVal = touch.clientX;
let topVal = touch.clientY;
leftVal -= parseInt(element.boundry.left); //relative offsets
topVal -= parseInt(element.boundry.top);
leftVal = Math.min(leftVal, parseInt(element.boundry.width));
topVal = Math.min(topVal, parseInt(element.boundry.height));
leftVal = Math.max(leftVal, 0);
topVal = Math.max(topVal, 0);
element.style.top = `${topVal - parseInt(element.stick.height)/2}px`;
element.style.left = `${leftVal - parseInt(element.stick.width)/2}px`;
report(element);
}
document.addEventListener("touchstart", e => {
let touch = e.changedTouches[0];
let pokeX = touch.clientX;
let pokeY = touch.clientY;
joysticks.forEach(joystick => {
let boundry = joystick.boundry;
if(pokeX >= boundry.left && pokeY >= boundry.top) {
if(pokeX <= boundry.left + boundry.width && pokeY <= boundry.top + boundry.height){
joystick.dragging = touch.identifier;
}
}
});
});
document.addEventListener("touchmove", e => {
joysticks.forEach(joy => {
for(let i = 0; i < e.touches.length; i++) {
if(joy.dragging === e.touches[i].identifier) dragElement(e.touches[i], joy);
}
});
});
document.addEventListener("touchend", e => {
joysticks.forEach(joy => {
if(joy.dragging === e.changedTouches[0].identifier) {
joy.dragging = null;
joy.style.left = joy.originX;
if(joy === joysticks[0]) joy.style.top = joy.originY; //works for now :P
report(joy);
}
})
});
</script>
</body>
</html>
)rawliteral";

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

void onEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len) {
    if(type == WS_EVT_CONNECT) {
        Serial.println("ws connected");
    }
    else if(type == WS_EVT_DATA) {
        String message = "";
            Serial.print("\n");
        for(size_t i = 0; i < len; i++) {
            message += (char)data[i];
        }
        Serial.print(message);
    }
}
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

    ws.onEvent(onEvent);
    server.addHandler(&ws);
    server.begin();
    ledcAttach(motor1, 50, 16);
    ledcAttach(motor2, 50, 16);
    ledcAttach(motor3, 50, 16);
    ledcAttach(motor4, 50, 16);
    delay(2000); //get away
    for(int pulse = 1000; pulse < 2000; pulse += 50) {
        ledcWrite(motor1, map(pulse, 0, 20000, 0, 65535));
        ledcWrite(motor2, map(pulse, 0, 20000, 0, 65535));
        ledcWrite(motor3, map(pulse, 0, 20000, 0, 65535));
        ledcWrite(motor4, map(pulse, 0, 20000, 0, 65535));
        delay(100);
    }
    delay(3000);
    ledcWrite(motor1, map(1000, 0, 20000, 0, 65535));
    ledcWrite(motor2, map(1000, 0, 20000, 0, 65535));
    ledcWrite(motor3, map(1000, 0, 20000, 0, 65535));
    ledcWrite(motor4, map(1000, 0, 20000, 0, 65535));
    delay(200);
    ledcWrite(motor1, map(0, 0, 20000, 0, 65535));
    ledcWrite(motor2, map(0, 0, 20000, 0, 65535));
    ledcWrite(motor3, map(0, 0, 20000, 0, 65535));
    ledcWrite(motor4, map(0, 0, 20000, 0, 65535));

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



