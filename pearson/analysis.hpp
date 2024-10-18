/*
Author: David Holmqvist <daae19@student.bth.se>
*/
#pragma once
#include "vector.hpp"
#include <vector>
#include <pthread.h>

#if !defined(ANALYSIS_HPP)
#define ANALYSIS_HPP

struct dataset
{
    std::vector<Vector> data;
    int threadId = 0;
    std::vector<double> result;
};

extern pthread_mutex_t lock;
extern int numThreads;

namespace Analysis {
void* correlation_coefficients(void* args);
double pearson(Vector& vec1, Vector& vec2);
};

#endif
