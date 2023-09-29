#pragma once
#include <cmath>
#include <condition_variable>
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

// Barrier Class from Stack Overflow
// https://stackoverflow.com/questions/48712881/how-can-i-create-a-barrier-in-c
class Barrier {
public:
  explicit Barrier(int iCount)
      : mThreshold(iCount), mCount(iCount), mGeneration(0) {}

  void block() {
    std::unique_lock<std::mutex> lLock{mMutex};
    auto lGen = mGeneration;
    if (!--mCount) {
      mGeneration++;
      mCount = mThreshold;
      mCond.notify_all();
    } else {
      mCond.wait(lLock, [this, lGen] { return lGen != mGeneration; });
    }
  }

private:
  std::mutex mMutex;
  std::condition_variable mCond;
  std::size_t mThreshold;
  int mCount;
  std::size_t mGeneration;
};
