
#include "histogram.hpp"
#include <iostream>

// Constructor computes histogram with global sum and tree sum and outputs
// results

HistogramComputation::HistogramComputation(int threadCount, int binCount,
                                           float minMeas, float maxMeas,
                                           std::vector<float> data)
    : threadCount(threadCount), binCount(binCount), minMeas(minMeas),
      maxMeas(maxMeas), data(data), dataCount(data.size()) {

  // "Global" global sum variables
  for (int i = 1; i <= binCount; i++) {
    globalSumBinMaxes.push_back(minMeas + i * (maxMeas - minMeas) / binCount);
    globalSumBinCounts.push_back(0);
  }

  // "Global" tree sum variables
  for (int i = 1; i <= binCount; i++) {
    treeSumBinMaxes.push_back(minMeas + i * (maxMeas - minMeas) / binCount);
    treeSumBinCounts.push_back(0);
  }

  globalOutput = globalSumHistogram();
  treeOutput = treeSumHistogram();

  std::cout << "Global Sum Histogram:" << std::endl << "bin_maxes: ";
  for (float max : std::get<0>(globalOutput)) {
    std::cout << max << ", ";
  }
  std::cout << std::endl << "bin_counts";
  for (int count : std::get<1>(globalOutput)) {
    std::cout << count << ", ";
  }
  std::cout << std::endl;

  std::cout << "Tree Sum Histogram:" << std::endl << "bin_maxes: ";
  for (float max : std::get<0>(treeOutput)) {
    std::cout << max << ", ";
  }
  std::cout << std::endl << "bin_counts";
  for (int count : std::get<1>(treeOutput)) {
    std::cout << count << ", ";
  }
  std::cout << std::endl;
}

// Logic for computing histogram with the global sum
std::tuple<std::vector<float>, std::vector<int>>
HistogramComputation::globalSumHistogram() {
  for (int i = 1; i <= binCount; i++) {
    globalSumBinMaxes.push_back(minMeas + i * (maxMeas - minMeas) / binCount);
  }
  // Define threadCount threads which takes dataCount/threadCount data
  // (startIndex = i * dataCount/threadCount, nextStartIndex = (i+1) *
  // dataCount/threadCount)
  std::vector<std::thread> threadVector;

  for (int i = 0; i < threadCount; i++) {
    std::thread threadi([&] {
      // Thread lambda for the global sum
      int startIndex = i * dataCount / threadCount;
      int nextStartIndex = (i + 1) * dataCount / threadCount;

      std::vector<int> localBinCounts(binCount, 0);

      // Bin data
      for (int j = startIndex; j < nextStartIndex; j++) {
        float dataPoint = data[j];
        for (int k = 0; k < globalSumBinMaxes.size(); k++) {
          if (dataPoint < globalSumBinMaxes[k]) {
            // Update local bin
            localBinCounts[k]++;
            break;
          }
        }
      }

      // Merge with global bins
      std::lock_guard<std::mutex> binCountLock(globalSumBinCountMutex);
      for (int j = 0; j < binCount; j++) {
        globalSumBinCounts[j] += localBinCounts[j];
      }
    });
    // std::thread threadi(globalSumThread, i);
    threadVector.push_back(threadi);
  }

  for (auto &thread : threadVector) {
    thread.join();
  }

  return std::make_tuple(globalSumBinMaxes, globalSumBinCounts);
}

// Logic for computing histogram with the tree sum
std::tuple<std::vector<float>, std::vector<int>>
HistogramComputation::treeSumHistogram() {
  // Define threadCount threads which takes dataCount/threadCount data

  std::vector<std::thread> threadVector;

  for (int i = 0; i < threadCount; i++) {
    std::thread threadi([&] {
      // Thread lambda for the global sum
      int startIndex = i * dataCount / threadCount;
      int nextStartIndex = (i + 1) * dataCount / threadCount;

      std::vector<int> localBinCounts(binCount, 0);

      // Bin data
      for (int j = startIndex; j < nextStartIndex; j++) {
        float dataPoint = data[j];
        for (int k = 0; k < treeSumBinMaxes.size(); k++) {
          if (dataPoint < treeSumBinMaxes[k]) {
            // Update local bin
            localBinCounts[k]++;
            break;
          }
        }
      }

      // Merge with global bins
      std::lock_guard<std::mutex> binCountLock(treeSumBinCountMutex);
      for (int j = 0; j < binCount; j++) {
        treeSumBinCounts[j] += localBinCounts[j];
      }
    });
    threadVector.push_back(threadi);
  }

  for (auto &thread : threadVector) {
    thread.join();
  }

  return std::make_tuple(treeSumBinMaxes, treeSumBinCounts);
}