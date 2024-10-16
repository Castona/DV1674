/*
Author: David Holmqvist <daae19@student.bth.se>
*/

#if !defined(VECTOR_HPP)
#define VECTOR_HPP

class Vector {
private:
    unsigned size;
    double* data;

public:
    Vector();
    Vector(unsigned size);
    Vector(unsigned size, double* data);
    Vector(const Vector& other);
    ~Vector();

    double magnitude();
    double mean();
    double dot(Vector& rhs);

    unsigned get_size() const;
    double* get_data();

    Vector operator/(double div);
    Vector operator-(double sub);
};

#endif