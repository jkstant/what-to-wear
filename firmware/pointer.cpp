// Device driver for an angular pointer mounted on a servo
#include "pointer.h"

Pointer::Pointer(int pin)
  : pin(pin), timer(*this) {

}

void Pointer::moveToAngle(int angle) {
    CriticalSection cs;
    if(positionDesired == angle) {
        return;
    }
    
    positionDesired = angle;
    
    if(!isMoving()) {
        startServo();
        timer.start();
    }
}

bool Pointer::isMoving() {
    return moving;
}

void Pointer::startServo() {
    servo.attach(pin);
    moving = true;
}

void Pointer::updateServo() {
    if(positionDesired == positionActual) {
        stopServo();
        timer.stop();
        return;
    }
    
    updatePositionActual();
    servo.write(positionActual);
}

void Pointer::updatePositionActual() {
    int deltaPosition = max(min(positionDesired - positionActual, RATE), -RATE);
    positionActual += deltaPosition;
}

void Pointer::stopServo() {
    servo.detach();
    moving = false;
}

Pointer::ServoTimer::ServoTimer(Pointer &pointer)
  : Timer(ServoTimer::PERIOD, Timer::timer_callback_fn(nullptr)),
  pointer(pointer) {
}

void Pointer::ServoTimer::timeout() {
    pointer.updateServo();
}