#include "histogram.hpp"
#include <iostream>

int main(int argc, char const* argv[])
{
    if (argc == 6)
    {
        int threadCount = atoi(argv[1]);
        int binCount = atoi(argv[2]);
        float minMeas = atof(argv[3]);
        float maxMeas = atof(argv[4]);
        int dataCount = atoi(argv[5]);

        srand(100);
        std::vector<float> data;

        // Load vector with random float values
        for (int i = 0; i < dataCount; i++)
        {
            float randomFloat = (maxMeas - minMeas) * (float)(rand()) / (float)(RAND_MAX)+minMeas; //Guarantees randomFloat to be in [minMeas,maxMeas]
            data.push_back(randomFloat);
        }

        HistogramComputation computeHistograms(threadCount, binCount, minMeas, maxMeas, data);
    }

    return 0;
}
