/*
Author: David Holmqvist <daae19@student.bth.se>
*/

#include "matrix.hpp"
#include "ppm.hpp"
#include "filters.hpp"
#include <cstdlib>
#include <iostream>

int main(int argc, char const* argv[])
{
    //if (argc != 4) 
    if (argc < 4 || argc > 5) //Only allow arg counts of 4 and 5
    {
        std::cerr << "Usage: " << argv[0] << " [radius] [infile] [outfile] [optional: threads]" << std::endl;
        std::exit(1);
    }

    int threadCount = 1; //Default to 1
    if(argc == 5) //If we've specified another threadcount in the arguments, pick that one
    {
        std::string threadString = argv[4];
        threadCount = std::stoi(threadString);
    }

    PPM::Reader reader {};
    PPM::Writer writer {};

    auto m { reader(argv[2]) };
    auto radius { static_cast<unsigned>(std::stoul(argv[1])) };

    //auto blurred { Filter::blur(m, radius) };
    Matrix blurred = Filter::blurParallel(m, radius, threadCount);
    writer(blurred, argv[3]);

    return 0;
}
