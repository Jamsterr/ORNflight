//setup pins and variables
void setup() {
    //webserver init, connect to device

    //sensor checks and calibration
}

void loop() {
    //webserver connection check
    //look for controller input
    //modify thrust variables per motor 
    //read sensor data (SPI)
    //PID calc on sensor data with stabilize()
}

void throttle(int amp) {
    //changes thrust of all motors
    //relative to max current
}

void jaw(int amp) {
    //sets relative difference bewteen ccw and cw motor pairs to rotate
}

void stabilize(float angleX, float angleY) {
    //pid formula for angling x and y or roll and pitch
}



