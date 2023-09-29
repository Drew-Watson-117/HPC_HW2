
#include "histogram.hpp"
#include <cmath>
#include <iostream>

// Constructor computes histogram with global sum, tree sum, and serial and
// outputs results

HistogramComputation::HistogramComputation(int threadCount, int binCount,
                                           float minMeas, float maxMeas,
                                           std::vector<float> data)
    : threadCount(threadCount), binCount(binCount), minMeas(minMeas),
      maxMeas(maxMeas), data(data), dataCount(data.size()) {

  // Instantiate global sum variables
  for (int i = 1; i <= binCount; i++) {
    globalSumBinMaxes.push_back(minMeas + i * (maxMeas - minMeas) / binCount);
    globalSumBinCounts.push_back(0);
  }

  // Instantiate tree sum variables
  for (int i = 1; i <= binCount; i++) {
    treeSumBinMaxes.push_back(minMeas + i * (maxMeas - minMeas) / binCount);
  }
  for (int i = 0; i < threadCount; ++i) {
    treeSumLocalBins.push_back({});
    for (int j = 0; j < binCount; ++j) {
      treeSumLocalBins[i].push_back(0);
    }
  }

  // Run histogram calculations
  globalOutput = globalSumHistogram();
  treeOutput = treeSumHistogram();
  serialOutput = serialHistogram();

  // PRINT OUTPUT
  std::cout << "Global Sum Histogram:" << std::endl << "bin_maxes: ";
  for (float max : std::get<0>(globalOutput)) {
    std::cout << max << ", ";
  }
  std::cout << std::endl << "bin_counts: ";
  for (int count : std::get<1>(globalOutput)) {
    std::cout << count << ", ";
  }
  std::cout << std::endl << std::endl;

  std::cout << "Tree Sum Histogram:" << std::endl << "bin_maxes: ";
  for (float max : std::get<0>(treeOutput)) {
    std::cout << max << ", ";
  }
  std::cout << std::endl << "bin_counts: ";
  for (int count : std::get<1>(treeOutput)) {
    std::cout << count << ", ";
  }
  std::cout << std::endl << std::endl;

  std::cout << "Serial Histogram:" << std::endl << "bin_maxes: ";
  for (float max : std::get<0>(serialOutput)) {
    std::cout << max << ", ";
  }
  std::cout << std::endl << "bin_counts: ";
  for (int count : std::get<1>(serialOutput)) {
    std::cout << count << ", ";
  }
  std::cout << std::endl << std::endl;
}

// Logic for computing histogram with the global sum
std::tuple<std::vector<float>, std::vector<int>>
HistogramComputation::globalSumHistogram() {

  // Define threadCount threads which takes dataCount/threadCount data
  // (startIndex = i * dataCount/threadCount, nextStartIndex = (i+1) *
  // dataCount/threadCount)
  std::vector<std::thread> threadVector;

  for (int i = 0; i < threadCount; i++) {
    std::thread threadi([&, i] {
      // Thread lambda for the global sum
      int startIndex = i * dataCount / threadCount;
      int nextStartIndex = (i + 1) * dataCount / threadCount;

      std::vector<int> localBinCounts(binCount, 0);

      // Bin data
      for (int j = startIndex; j < nextStartIndex; j++) {
        float dataPoint = data[j];
        for (int k = 0; k < globalSumBinMaxes.size(); k++) {
          if (dataPoint <= globalSumBinMaxes[k]) {
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
    threadVector.push_back(std::move(threadi));
  }

  for (auto &thread : threadVector) {
    thread.join();
  }
  std::tuple<std::vector<float>, std::vector<int>> returnVal =
      std::make_tuple(globalSumBinMaxes, globalSumBinCounts);
  return std::make_tuple(globalSumBinMaxes, globalSumBinCounts);
}

// Logic for computing histogram with the tree sum
std::tuple<std::vector<float>, std::vector<int>>
HistogramComputation::treeSumHistogram() {

  std::vector<std::thread> threadVector;
  Barrier barrier(threadCount);

  // Define threadCount threads which takes dataCount/threadCount data
  for (int i = 0; i < threadCount; i++) {
    std::thread threadi([&, i] {
      // Thread lambda for the global sum
      int threadIndex = i;
      int startIndex = i * dataCount / threadCount;
      int nextStartIndex = (i + 1) * dataCount / threadCount;

      // Bin data
      for (int j = startIndex; j < nextStartIndex; j++) {
        float dataPoint = data[j];
        for (int k = 0; k < treeSumBinMaxes.size(); k++) {
          if (dataPoint <= treeSumBinMaxes[k]) {
            // Update local bin
            treeSumLocalBins[threadIndex][k]++;
            break;
          }
        }
      }
      // Barrier until all threads are done with local sum
      barrier.block();

      // Merge local bins with another thread
      int combineNeighbor = 2;
      bool combined = false;
      for (int layer = 0; layer < std::log2(threadCount);
           ++layer) { // n threads => log2(n) combination iterations
        if (threadIndex % combineNeighbor != 0 && combined == false) {
          // Combine it with the neighbor
          for (size_t l = 0; l < binCount; l++) {
            size_t threadToCombineTo = threadIndex - (combineNeighbor / 2);
            treeSumLocalBins[threadToCombineTo][l] +=
                treeSumLocalBins[threadIndex][l];
          }
          combined = true;
        }
        // Barrier blocks until all threads are done combining on a given layer
        barrier.block();
        combineNeighbor *= 2;
      }
    });
    threadVector.push_back(std::move(threadi));
  }

  for (auto &thread : threadVector) {
    thread.join();
  }

  return std::make_tuple(treeSumBinMaxes, treeSumLocalBins[0]);
}

// Serial histogram calculation to test the other two against
std::tuple<std::vector<float>, std::vector<int>>
HistogramComputation::serialHistogram() {
  std::vector<float> binMaxes;
  std::vector<int> binCounts;
  for (int i = 1; i <= binCount; i++) {
    binMaxes.push_back(minMeas + i * (maxMeas - minMeas) / binCount);
    binCounts.push_back(0);
  }
  for (float dataPoint : data) {
    // bin
    for (int i = 0; i < binCount; i++) {
      if (dataPoint <= binMaxes[i]) {
        binCounts[i]++;
        break;
      }
    }
  }
  return std::make_tuple(binMaxes, binCounts);
}
