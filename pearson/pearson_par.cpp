/*
Author: David Holmqvist <daae19@student.bth.se>
*/

#include "analysis.hpp"
#include "dataset.hpp"
#include <iostream>
#include <cstdlib>
#include <algorithm>


int main(int argc, char const* argv[])
{
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " [dataset] [outfile] [num_threads]" << std::endl;
        std::exit(1);
    }

    std::string s = argv[3];
    int nThreads = std::stoi(s);
    if (s.empty())
    {
        nThreads = 1;
    }
    else if (!std::all_of(s.begin(), s.end(), ::isdigit))
    {
        std::cerr << "Invalid Argument for [num_threads]" << std::endl;
        std::exit(2);
    }

    numThreads = nThreads;
    dataset data[nThreads];
    pthread_t threads[nThreads];
    pthread_mutex_init(&lock, 0);

    for (size_t i = 0; i < nThreads; i++)
    {
        data[i].data = Dataset::read(argv[1]);
        data[i].threadId = i;
        pthread_create(&(threads[i]), NULL, (void*(*)(void*))Analysis::correlation_coefficients, (void*)&data[i]);
    }

    std::vector<std::vector<double>> reverseResult;

    for (size_t i = 0; i < nThreads; i++)
    {
        pthread_join(threads[i], NULL);
        reverseResult.push_back(data[i].result);
    }

    std::vector<double> corrs;
    for (size_t i = 0; i < reverseResult.size(); i++)
    {
        for (size_t j = 0; j < reverseResult.at(i).size(); j++)
        {
            corrs.push_back(reverseResult.at(i).at(j));
        }
    }
    
    Dataset::write(corrs, argv[2]);

    pthread_mutex_destroy(&lock);
    return 0;
}
