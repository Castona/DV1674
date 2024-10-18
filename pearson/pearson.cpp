/*
Author: David Holmqvist <daae19@student.bth.se>
*/

#include "analysis.hpp"
#include "dataset.hpp"
#include <iostream>
#include <cstdlib>

int main(int argc, char const* argv[])
{
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " [dataset] [outfile]" << std::endl;
        std::exit(1);
    }

    dataset data;
    data.data = Dataset::read(argv[1]);
    Analysis::correlation_coefficients((void*)&data);
    Dataset::write(data.result, argv[2]);

    return 0;
}
