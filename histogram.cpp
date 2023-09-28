
#include <iostream>
#include "histogram.hpp"

// Constructor computes histogram with global sum and tree sum and outputs results
HistogramComputation::HistogramComputation(int threadCount, int binCount, float minMeas, float maxMeas, std::vector<float> data) :
    threadCount(threadCount),
    binCount(binCount),
    minMeas(minMeas),
    maxMeas(maxMeas),
    data(data),
    dataCount(data.size())
{
    globalOutput = globalSumHistogram();
    treeOutput = treeSumHistogram();

    std::vector<float> globalSumBinMaxes;
    std::mutex globalSumBinCountMutex;

    std::vector<float> treeSumBinMaxes;
    std::mutex treeSumBinCountMutex;


    std::cout << "Global Sum Histogram:" << std::endl << "bin_maxes: ";
    for (float max : std::get<0>(globalOutput))
    {
        std::cout << max << ", ";
    }
    std::cout << std::endl << "bin_counts";
    for (int count : std::get<1>(globalOutput))
    {
        std::cout << count << ", ";
    }
    std::cout << std::endl;

    std::cout << "Tree Sum Histogram:" << std::endl << "bin_maxes: ";
    for (float max : std::get<0>(treeOutput))
    {
        std::cout << max << ", ";
    }
    std::cout << std::endl << "bin_counts";
    for (int count : std::get<1>(treeOutput))
    {
        std::cout << count << ", ";
    }
    std::cout << std::endl;

}

// Logic for computing histogram with the global sum
std::tuple<std::vector<float>, std::vector<int>> HistogramComputation::globalSumHistogram()
{

    // Create bins

    std::vector<int> globalSumBinCounts(globalSumBinMaxes.size(), 0);

    for (int i = 1; i <= binCount; i++) {
        globalSumBinMaxes.push_back(minMeas + i * (maxMeas - minMeas) / binCount);
    }
    // Define threadCount threads which takes dataCount/threadCount data (startIndex = i * dataCount/threadCount, nextStartIndex = (i+1) * dataCount/threadCount)
    std::vector<std::thread> threadVector;

    for (int i = 0; i < threadCount; i++) {
        int startIndex = i * dataCount / threadCount;
        int nextStartIndex = (i + 1) * dataCount / threadCount;
        //std::thread threadi(globalSumThread,i);
        std::thread threadi([=] 
            {
                // Thread lambda for the global sum
                int startIndex = i * dataCount / threadCount;
                int nextStartIndex = (i + 1) * dataCount / threadCount;

                std::vector<int> localBinCounts(binCount, 0);

                // Bin data
                for (int j = startIndex; j < nextStartIndex; j++)
                {
                    float dataPoint = data[j];
                    for (int k = 0; k < globalSumBinMaxes.size(); k++)
                    {
                        if (dataPoint < globalSumBinMaxes[k])
                        {
                            // Update local bin
                            localBinCounts[k]++;
                            break;
                        }
                    }
                }


                // Merge with global bins
                globalSumBinCountMutex.lock();
                for (int j = 0; j < binCount; j++)
                {
                    globalSumBinCounts[j] += localBinCounts[j];
                }
                globalSumBinCountMutex.unlock();
            }
        );
        threadVector.push_back(threadi);
    }

    for (auto& thread : threadVector) {
        thread.join();
    }

    return std::make_tuple(globalSumBinMaxes, globalSumBinCounts);
}

// Logic for computing histogram with the tree sum
std::tuple<std::vector<float>, std::vector<int>> HistogramComputation::treeSumHistogram()
{
    // Create bins

    std::vector<int> treeSumBinCounts(treeSumBinMaxes.size(), 0);

    for (int i = 1; i <= binCount; i++) {
        globalSumBinMaxes.push_back(minMeas + i * (maxMeas - minMeas) / binCount);
    }
    // Define threadCount threads which takes dataCount/threadCount data (startIndex = i * dataCount/threadCount, nextStartIndex = (i+1) * dataCount/threadCount)
    std::vector<std::thread> threadVector;

    for (int i = 0; i < threadCount; i++) {
        int startIndex = i * dataCount / threadCount;
        int nextStartIndex = (i + 1) * dataCount / threadCount;
        //std::thread threadi(globalSumThread,i);
        std::thread threadi([=]
            {
                // Thread lambda for the global sum
                int startIndex = i * dataCount / threadCount;
                int nextStartIndex = (i + 1) * dataCount / threadCount;

                std::vector<int> localBinCounts(binCount, 0);

                // Bin data
                for (int j = startIndex; j < nextStartIndex; j++)
                {
                    float dataPoint = data[j];
                    for (int k = 0; k < treeSumBinMaxes.size(); k++)
                    {
                        if (dataPoint < treeSumBinMaxes[k])
                        {
                            // Update local bin
                            localBinCounts[k]++;
                            break;
                        }
                    }
                }


                // Merge with global bins
                treeSumBinCountMutex.lock();
                for (int j = 0; j < binCount; j++)
                {
                    treeSumBinCounts[j] += localBinCounts[j];
                }
                treeSumBinCountMutex.unlock();
            }
        );
        threadVector.push_back(threadi);
    }

    for (auto& thread : threadVector) {
        thread.join();
    }

    return std::make_tuple(treeSumBinMaxes, treeSumBinCounts);
    
}

