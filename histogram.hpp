#pragma once
#include <mutex>
#include <tuple>
#include <vector>

class HistogramComputation {
public:
  std::tuple<std::vector<float>, std::vector<int>> globalOutput;
  std::tuple<std::vector<float>, std::vector<int>> treeOutput;
  int binCount;
  int threadCount;
  float minMeas;
  float maxMeas;
  int dataCount;
  std::vector<float> data;
  // Global global sum variables
  std::vector<float> globalSumBinMaxes;
  std::vector<int> globalSumBinCounts;
  std::mutex globalSumBinCountMutex;
  // Global tree sum variables
  std::vector<float> treeSumBinMaxes;
  std::vector<int> treeSumBinCounts;
  std::mutex treeSumBinCountMutex;

  HistogramComputation(int threadCount, const int binCount, float minMeas,
                       float maxMeas, std::vector<float> data);

private:
  std::tuple<std::vector<float>, std::vector<int>> treeSumHistogram();
  std::tuple<std::vector<float>, std::vector<int>> globalSumHistogram();
};