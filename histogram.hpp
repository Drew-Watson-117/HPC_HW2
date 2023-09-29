#pragma once
#include <cmath>
#include <mutex>
#include <thread>
#include <tuple>
#include <vector>

class HistogramComputation {
public:
  std::tuple<std::vector<float>, std::vector<int>> globalOutput;
  std::tuple<std::vector<float>, std::vector<int>> treeOutput;
  std::tuple<std::vector<float>, std::vector<int>> serialOutput;

  int threadCount;
  int binCount;
  float minMeas;
  float maxMeas;
  std::vector<float> data;
  int dataCount;
  // Global global sum variables
  std::vector<float> globalSumBinMaxes;
  std::vector<int> globalSumBinCounts;
  std::mutex globalSumBinCountMutex;
  // Global tree sum variables
  std::vector<float> treeSumBinMaxes;
  std::vector<std::vector<int>> treeSumLocalBins;

  HistogramComputation(int threadCount, const int binCount, float minMeas,
                       float maxMeas, std::vector<float> data);

private:
  std::tuple<std::vector<float>, std::vector<int>> treeSumHistogram();
  std::tuple<std::vector<float>, std::vector<int>> globalSumHistogram();
  std::tuple<std::vector<float>, std::vector<int>> serialHistogram();
};

class Barrier {
public:
  Barrier();
  void block(int threadCount);

private:
  int threadsAtBarrier;
  std::mutex mutex;
};