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
    //loadWS(); //wip
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

    //joyX = Math.round(joyX);
    //joyY = Math.round(joyY);

    //relative to bounds for different screens
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