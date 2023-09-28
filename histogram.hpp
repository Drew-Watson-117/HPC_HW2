#pragma once
#include <vector>
#include <tuple>
#include <mutex>

class HistogramComputation
{
    public:
        std::tuple<std::vector<float>,std::vector<int>> globalOutput;
        std::tuple<std::vector<float>,std::vector<int>> treeOutput;
        int binCount;
        int threadCount; 
        const int binCount; 
        float minMeas; 
        float maxMeas;
        std::vector<float> data;
        int dataCount;
        std::vector<float> globalSumBinMaxes;
        std::vector<int> globalSumBinCounts;
        std::mutex globalSumBinCountMutex;
        std::vector<float> treeSumBinMaxes;
        std::vector<int> treeSumBinCounts;
        std::mutex treeSumBinCountMutex;
        HistogramComputation(int threadCount, int binCount, float minMeas, float maxMeas, std::vector<float> data);
    
    private:
        std::tuple<std::vector<float>, std::vector<int>> treeSumHistogram();
        std::tuple<std::vector<float>, std::vector<int>> globalSumHistogram();
        void treeSumThread(int threadIndex, int startIndex, int nextStartIndex, std::vector<float> binMaxes);
        void globalSumThread(int threadIndex);

};