/*
Author: David Holmqvist <daae19@student.bth.se>
*/

#include "analysis.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <list>
#include <vector>
#include <sys/types.h>
#include <unistd.h>

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
extern int numThreads = 1;

namespace Analysis {

void* correlation_coefficients(void* args)
{
    dataset* data = (dataset*)args; 

    std::vector<double> result {};
    size_t dataSize = data->data.size();
    size_t dividedWork = dataSize / numThreads;
    size_t tid = data->threadId;
    size_t size = dividedWork * (tid + 1);

    for (size_t sample1 = dividedWork * tid; sample1 < size; sample1++)
    {
        for (size_t sample2 = sample1 + 1; sample2 < dataSize; sample2++)
        {
            result.push_back(pearson(*&data->data.at(sample1), *&data->data.at(sample2)));
        }

        if (size == dataSize - 1)
            break;
    }

    data->result = result;
    
    return NULL;
}

double pearson(Vector& vec1, Vector& vec2)
{
    
    auto x_mean { vec1.mean() };
    auto y_mean { vec2.mean() };

    auto x_mm { vec1 - x_mean };
    auto y_mm { vec2 - y_mean };

    auto x_mag { x_mm.magnitude() };
    auto y_mag { y_mm.magnitude() };

    auto x_mm_over_x_mag { x_mm / x_mag };
    auto y_mm_over_y_mag { y_mm / y_mag };

    auto r { x_mm_over_x_mag.dot(y_mm_over_y_mag) };

    return std::max(std::min(r, 1.0), -1.0);
}
};
