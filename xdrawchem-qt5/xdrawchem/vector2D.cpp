#include "vector2D.h"
#include <math.h>

Vector2D::Vector2D() : x(0), y(0)
{

}

Vector2D::Vector2D(double x, double y) {
    this->x = x;
    this->y = y;
}

double Vector2D::magnitude() {
    return sqrt(magnitudeSquared());
}

double Vector2D::magnitudeSquared() {
    return x*x + y*y;
}

void Vector2D::normalize() {
    double mag = magnitude();

    if(magnitude() == 0) {
        return;
    }

    x = x / mag;
    y = y / mag;
}

void Vector2D::rotate(double angle) {
    this->rotate(angle, false);
}

void Vector2D::rotate(double angle, bool useTranspose) {
    double cosAngle = floor(cos(angle) * 10000000 + 0.5) / 10000000;
    double sinAngle = floor(sin(angle) * 10000000 + 0.5) / 10000000;

    double xCopy = x;

    if(useTranspose) {
        x = x * cosAngle + y * sinAngle;
        y = xCopy * -sinAngle + y * cosAngle;
    }else {
        x = x * cosAngle + y * -sinAngle;
        y = xCopy * sinAngle + y * cosAngle;
    }
}

bool Vector2D::equals(Vector2D* v) {
    return this->x == v->x && this->y == v->y;
}

double Vector2D::dotProduct(Vector2D* v) {
    return x * v->x + y * v->y;
}

double Vector2D::crossProduct(Vector2D *v) {
    return x * v->y - y * v->x;
}

void Vector2D::round() {
    round(10);
}

void Vector2D::round(int decimals) {
    int factor = pow(10, decimals);

    x = floor(x * factor + 0.5) / factor;
    y = floor(y * factor + 0.5) / factor;
}

void Vector2D::reverse() {
    x = -x;
    y = -y;
}
