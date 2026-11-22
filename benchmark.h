#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <iomanip>
#include <random>
#include <algorithm>
#include <cmath>

#include "game/logic/bst/bst.h"
#include "game/logic/avl/avl_tree.h"
#include "game/logic/heap/max_heap.h"
#include "game/logic/event.h"

// ============================================================================
// Data Transfer Objects for Benchmark Results
// ============================================================================

struct HeightResult {
    int n;
    int bstHeightSorted;
    int avlHeightSorted;
    int bstHeightRandom;
    int avlHeightRandom;
    int theoreticalLog2;
};

struct SearchCostResult {
    int n;
    const char* distribution;
    double bstAvgComps;
    double avlAvgComps;
    double bstTotalTimeUs;
    double avlTotalTimeUs;
};

struct RotationResult {
    int n;
    const char* distribution;
    double avgRotationsPerInsert;
    double avgRotationsPerDelete;
    int maxRotationsSingleDelete;
};

struct OrderStatResult {
    int n;
    double bstInorderTimeMs;
    double avlAugmentedTimeMs;
    double speedup;
};

struct HeapBuildResult {
    int n;
    const char* distribution;
    double bottomUpTimeUs;
    double repeatedInsertTimeUs;
    long long bottomUpComps;
    long long bottomUpSwaps;
    long long insertComps;
    long long insertSwaps;
};

struct HeapSortResult {
    int n;
    const char* distribution;
    double timeUs;
    long long comparisons;
    long long swaps;
};

struct SortCompResult {
    int n;
    const char* algorithm;
    double timeUs;
    long long comparisons;
    long long swapsOrMoves;
    const char* auxiliarySpace;
    const char* timeComplexity;
};

// ============================================================================
// Benchmark Suite Runner
// ============================================================================

class BenchmarkSuite {
public:
    static void runAll();

    // Experiment 1: Height comparison (n = 100, 1000, 10000; sorted vs random; BST vs AVL)
    static void runExperiment1_HeightComparison();

    // Experiment 2: Search cost (10,000 random searches; average comparisons and time)
    static void runExperiment2_SearchCost();

    // Experiment 3: Rotation count (AVL insert & delete; random vs sorted; avg & max per delete)
    static void runExperiment3_RotationCount();

    // Experiment 4: Order statistics (k-th on AVL vs inorder scan on BST for 1,000 queries)
    static void runExperiment4_OrderStatistics();

    // Experiment 5: Heap construction (bottom-up O(n) vs repeated insertion O(n log n))
    static void runExperiment5_HeapConstruction();

    // Experiment 6: Heap Sort analysis (random, sorted, reverse, duplicates; runtime, comps, swaps)
    static void runExperiment6_HeapSort();

    // Experiment 7: Sorting comparison (Heap Sort vs QuickSort, MergeSort, InsertionSort)
    static void runExperiment7_SortingComparison();

    // Utility helpers
    static std::vector<int> generateRandomArray(int n, int minVal = 1, int maxVal = 1000000);
    static std::vector<int> generateSortedArray(int n);
    static std::vector<int> generateReverseSortedArray(int n);
    static std::vector<int> generateDuplicateHeavyArray(int n, int distinctCount = 5);

    // Formatted Markdown table printer
    static void printHeader(const std::string& title);
    static void printDivider();
};

#endif // BENCHMARK_H
