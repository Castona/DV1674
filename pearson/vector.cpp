/*
Author: David Holmqvist <daae19@student.bth.se>
*/

#include "vector.hpp"
#include <iostream>
#include <cmath>
#include <vector>

Vector::Vector()
    : size{0}, data{nullptr}
{
}

Vector::~Vector()
{
    if (data)
    {
        delete[] data;
    }

    size = 0;
}

Vector::Vector(unsigned size)
    : size{size}, data{new double[size]}
{
}

Vector::Vector(unsigned size, double *data)
    : size{size}, data{data}
{
}

Vector::Vector(const Vector &other)
    : Vector{other.size}
{
    for (auto i{0}; i < size; i++)
    {
        data[i] = other.data[i];
    }
}

unsigned Vector::get_size() const
{
    return size;
}

double *Vector::get_data()
{
    return data;
}

double Vector::mean()
{
    double sum{0};

    for (auto i{0}; i < size; i++)
    {
        sum += data[i];
    }

    return sum / static_cast<double>(size);
}

double Vector::magnitude()
{
    auto dot_prod{dot(*this)};
    return std::sqrt(dot_prod);
}

Vector Vector::operator/(double div)
{
    auto result{*this};

    for (auto i{0}; i < size; i++)
    {
        result.data[i] /= div;
    }

    return result;
}

Vector Vector::operator-(double sub)
{
    auto result{*this};

    for (auto i{0}; i < size; i++)
    {
        result.data[i] -= sub;
    }

    return result;
}

double Vector::dot(Vector& rhs)
{
    double result{0};

    for (auto i{0}; i < size; i++)
    {
        result += data[i] * rhs.data[i];
    }

    return result;
}