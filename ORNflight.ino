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
<meta name="viewport" content="width=device-width, initial-scale=1.0 maximum-scale=1.0, user-scalable=no">
<title>ORNflight 0.3</title>
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
if(joystick == joysticks[0]) joystick.style.top = joystick.originY;
else joystick.style.top = `${joystick.boundry.height - joystick.stick.height/2}px`;
joystick.stick = joystick.getBoundingClientRect();
});
}
document.addEventListener("DOMContentLoaded", () => {
setup();
loadWs();
});
function loadWS() { //init and event handling
websocket = new WebSocket(host);
websocket.onopen = wsOpen;
websocket.onmessage = wsMessage;
websocket.onclose = wsClose;
}
function wsOpen() {
console.log("ws is open");
getReadings();
}
function wsClose() {
console.log("ws down");
setTimeout(loadWs, 2000);
}
function report(joy) { //rn unformatted
let stickPlace = joy.getBoundingClientRect();
let joyX = -1*(stickPlace.left + joy.stick.width/2 - (joy.boundry.left + joy.boundry.width/2)) - 1;
let joyY = -1*(stickPlace.top + joy.stick.height/2 - (joy.boundry.top + joy.boundry.height/2)) - 1;
let relX = Math.round(1000* joyX / (joy.boundry.width/2));
let relY;
if(joy == joysticks[1]) relY = Math.round(1000*(joyY + joy.boundry.height/2)/joy.boundry.height);
else relY = Math.round(1000* joyY / (joy.boundry.height/2));
console.log(relX, relY);
if(joy == joysticks[1]) { //1D throttle
joyY += (Math.round(joy.boundry.height*10)) + 2;
}
if(Math.abs(joyX) < 400) joyX = 0; //deadzones
if(Math.abs(joyY) < 400) joyY = 0;
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
websocket.send("ping");
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

void onEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len) {
    if(type == WS_EVT_CONNECT) {
        Serial.println("ws connected");
    }
    else if(type == WS_EVT_DATA) {
        Serial.println("recieved message");
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



