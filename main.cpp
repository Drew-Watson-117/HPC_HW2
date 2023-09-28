#include "histogram.hpp"

int main(int argc, char const* argv[])
{
    int threadCount = (int)(argv[0]);
    int binCount = (int)(argv[1]);
    float minMeas = atof(argv[2]);
    float maxMeas = atof(argv[3]);
    long dataCount = (long)(argv[4]);

    srand(100);
    std::vector<float> data;

    // Load vector with random float values
    for (int i = 0; i < dataCount; i++)
    {
        float randomFloat = (maxMeas-minMeas) * (float)(rand()) / (float)(RAND_MAX) + minMeas; //Guarantees randomFloat to be in [minMeas,maxMeas]
        data.push_back(randomFloat);
    }

    HistogramComputation computeHistograms = HistogramComputation(threadCount, binCount, minMeas, maxMeas, data);

    return 0;
}