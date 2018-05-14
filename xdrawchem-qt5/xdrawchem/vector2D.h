#ifndef VECTOR2D_H
#define VECTOR2D_H


class Vector2D
{
public:
    double x,y;

    Vector2D();
    Vector2D(double x, double y);

    /**
     * @brief magnitude Calculates the magnitude of this vector
     * @return The respective magnitude
     */
    double magnitude();

    /**
     * @brief magnitudeSquared Calculates the squared magnitude of this vector
     * @return The respective squared magnitude
     */
    double magnitudeSquared();

    /**
     * @brief normalize Normalizes this vector to a unit length of 1
     */
    void normalize();

    /**
     * @brief rotate Rotates this vector by the given angle
     * @param angle The rotation-angle in radians
     * @param useTranspose Whether the transposed rotation matrix should be used
     */
    void rotate(double angle, bool useTranspose);

    /**
     * @brief rotate Rotates this vector by the given angle using a standard rotation matrix
     * @param angle The rotation-angle in radians
     */
    void rotate(double angle);

    /**
     * @brief reverse Reverses this vector. That is multiplying it by -1.
     */
    void reverse();

    /**
     * @brief equals Checks whether this vector is equal to the given one
     * @param v The vector to compare with
     * @return Whether the vectors are equals
     */
    bool equals(Vector2D* v);

    /**
     * @brief dotProduct Calculates the dot product between this vector and the given one
     * @param v The vector to calculate the dot product with
     * @return The respective dot product
     */
    double dotProduct(Vector2D* v);

    /**
     * @brief crossProduct Calculates the cross product between this vector and the given one
     * @param v The vector to calculate the cross product with
     * @return The respective cross product
     */
    double crossProduct(Vector2D* v);

    /**
     * @brief round Rounds the coordinates of this vector to a fixed number of decimals
     */
    void round();

    /**
     * @brief roud Rounds the coordinates of this vector to the given number of decimals
     * @param decimals The number of decimals to round to
     */
    void round(int decimals);

    Vector2D* operator+(Vector2D* v2) {
        return new Vector2D(this->x + v2->x, this->y + v2->y);
    }

    Vector2D* operator-(Vector2D* v2) {
        return new Vector2D(this->x - v2->x, this->y - v2->y);
    }

    Vector2D* operator*(double factor) {
        return new Vector2D(this->x * factor, this->y * factor);
    }

    Vector2D* operator/(double divisor) {
        return new Vector2D(this->x / divisor, this->y / divisor);
    }

    Vector2D* operator*(int factor) {
        return new Vector2D(this->x * factor, this->y * factor);
    }
};

#endif // VECTOR2D_H
