#include "benchmark.h"
#include "game/logic/bst/bst.h"
#include "game/logic/avl/avl_tree.h"
#include "game/logic/heap/max_heap.h"
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>

static const int runCount = 3;
static const int inputSizes[3] = {100, 1000, 10000};

struct AvlAudit {
    int nodes;
    int height;
    int subtreeSize;
    bool valid;
};

struct SortMetrics {
    long long comparisons;
    long long moves;
};

static unsigned int nextRandom(unsigned int& state) {
    state = state * 1664525u + 1013904223u;
    return state;
}

static void fillInput(int* values, int n, char pattern, unsigned int seed) {
    unsigned int state = seed;
    if (pattern == 'S' || pattern == 'V' || pattern == 'R') {
        for (int i = 0; i < n; ++i) values[i] = pattern == 'V' ? n - i : i + 1;
        if (pattern == 'R') {
            for (int i = n - 1; i > 0; --i) {
                int j = static_cast<int>(nextRandom(state) % static_cast<unsigned int>(i + 1));
                int temp = values[i]; values[i] = values[j]; values[j] = temp;
            }
        }
    } else {
        for (int i = 0; i < n; ++i) {
            values[i] = pattern == 'D'
                ? static_cast<int>(nextRandom(state) % 11u) - 5
                : i + 1;
        }
        if (pattern == 'P') {
            for (int i = n - 1; i > 0; --i) {
                int j = static_cast<int>(nextRandom(state) % static_cast<unsigned int>(i + 1));
                int temp = values[i]; values[i] = values[j]; values[j] = temp;
            }
        }
    }
}

static const char* inputName(char pattern) {
    if (pattern == 'S') return "sorted";
    if (pattern == 'V') return "reverse_sorted";
    if (pattern == 'D') return "duplicate_heavy";
    return "random_unique";
}

static AvlAudit auditAvlNode(AVLNode* node, AVLNode* expectedParent,
                             long long minKey, long long maxKey) {
    AvlAudit result = {0, 0, 0, true};
    if (node == nullptr) return result;
    if (node->key <= minKey || node->key >= maxKey || node->parent != expectedParent) {
        result.valid = false;
    }
    AvlAudit left = auditAvlNode(node->left, node, minKey, node->key);
    AvlAudit right = auditAvlNode(node->right, node, node->key, maxKey);
    result.nodes = left.nodes + right.nodes + 1;
    result.height = (left.height > right.height ? left.height : right.height) + 1;
    result.subtreeSize = left.subtreeSize + right.subtreeSize + 1;
    int balance = left.height - right.height;
    result.valid = result.valid && left.valid && right.valid && balance >= -1 && balance <= 1;
    result.valid = result.valid && node->height == result.height && node->size == result.subtreeSize;
    return result;
}

static bool buildAndValidate(BST& bst, AVLTree& avl, const int* keys, int n) {
    bool valid = true;
    for (int i = 0; i < n; ++i) {
        bst.insert(keys[i]);
        avl.insert(keys[i]);
        if (avl.getRootSize() != i + 1 || !avl.isBalanced()) valid = false;
    }
    AvlAudit audit = auditAvlNode(avl.getRoot(), nullptr, -2147483649LL, 2147483648LL);
    return valid && bst.isValidBST() && avl.isBalanced() && audit.valid &&
           audit.nodes == n && audit.subtreeSize == n && avl.getRootSize() == n;
}

static double medianTime(double times[runCount]) {
    if (times[0] > times[1]) { double t = times[0]; times[0] = times[1]; times[1] = t; }
    if (times[1] > times[2]) { double t = times[1]; times[1] = times[2]; times[2] = t; }
    if (times[0] > times[1]) { double t = times[0]; times[0] = times[1]; times[1] = t; }
    return times[1];
}

static long long countBstSearchComparisons(BSTNode* root, const int* queries, int queryCount,
                                           int& successful) {
    long long comparisons = 0;
    successful = 0;
    for (int q = 0; q < queryCount; ++q) {
        BSTNode* node = root;
        while (node != nullptr) {
            ++comparisons;
            if (queries[q] == node->key) { ++successful; break; }
            node = queries[q] < node->key ? node->left : node->right;
        }
    }
    return comparisons;
}

static long long countAvlSearchComparisons(AVLNode* root, const int* queries, int queryCount,
                                           int& successful) {
    long long comparisons = 0;
    successful = 0;
    for (int q = 0; q < queryCount; ++q) {
        AVLNode* node = root;
        while (node != nullptr) {
            ++comparisons;
            if (queries[q] == node->key) { ++successful; break; }
            node = queries[q] < node->key ? node->left : node->right;
        }
    }
    return comparisons;
}

static double timeBstSearch(BST& bst, const int* queries, int queryCount, int expectedHits,
                            bool& valid) {
    double times[runCount];
    volatile int resultSink = 0;
    for (int run = 0; run < runCount; ++run) {
        int hits = 0;
        auto start = std::chrono::steady_clock::now();
        for (int q = 0; q < queryCount; ++q) if (bst.search(queries[q])) ++hits;
        auto stop = std::chrono::steady_clock::now();
        times[run] = std::chrono::duration<double, std::micro>(stop - start).count();
        if (hits != expectedHits) valid = false;
        resultSink = resultSink + hits;
    }
    return medianTime(times);
}

static double timeAvlSearch(AVLTree& avl, const int* queries, int queryCount, int expectedHits,
                            bool& valid) {
    double times[runCount];
    volatile int resultSink = 0;
    for (int run = 0; run < runCount; ++run) {
        int hits = 0;
        auto start = std::chrono::steady_clock::now();
        for (int q = 0; q < queryCount; ++q) if (avl.search(queries[q])) ++hits;
        auto stop = std::chrono::steady_clock::now();
        times[run] = std::chrono::duration<double, std::micro>(stop - start).count();
        if (hits != expectedHits) valid = false;
        resultSink = resultSink + hits;
    }
    return medianTime(times);
}

static void benchmarkHeights(std::ofstream& report, bool& allValid) {
    report << "EXPERIMENT A: BST and AVL height (height is measured in nodes; empty tree = 0)" << std::endl;
    report << "n,pattern,inserted_keys,bst_height_nodes,avl_actual_nodes,avl_stored_height_nodes,avl_recomputed_height_nodes,avl_validator" << std::endl;
    for (int s = 0; s < 3; ++s) {
        int n = inputSizes[s];
        int* keys = new int[n];
        for (int patternIndex = 0; patternIndex < 2; ++patternIndex) {
            char pattern = patternIndex == 0 ? 'S' : 'R';
            fillInput(keys, n, pattern, 31091u + static_cast<unsigned int>(n));
            BST bst(nullptr);
            AVLTree avl(nullptr);
            bool valid = buildAndValidate(bst, avl, keys, n);
            AvlAudit audit = auditAvlNode(avl.getRoot(), nullptr, -2147483649LL, 2147483648LL);
            bool caseValid = valid && audit.nodes == n && avl.getRootHeight() == audit.height;
            if (!caseValid) allValid = false;
            report << n << ',' << (pattern == 'S' ? "sorted" : "random") << ',' << n << ','
                   << bst.getHeight() << ',' << audit.nodes << ',' << avl.getRootHeight() << ','
                   << audit.height << ',' << (caseValid ? "PASS" : "FAIL") << std::endl;
        }
        delete[] keys;
    }
    report << "AVL checks: validator after each insertion; actual node count, BST order, parent links, balance, stored height and subtree size verified." << std::endl << std::endl;
}

static void benchmarkSearch(std::ofstream& report, bool& allValid) {
    report << "EXPERIMENT B: 10000 searches per case; query domain [1,2n], so successful and unsuccessful searches are mixed." << std::endl;
    report << "Comparison convention: one logical key-to-node comparison per visited node; failed searches count each visited node. Count pass is separate from timing." << std::endl;
    report << "n,tree_input,structure,queries,successful_searches,average_key_comparisons,total_time_us_median_of_3,validation" << std::endl;
    const int queryCount = 10000;
    for (int s = 0; s < 3; ++s) {
        int n = inputSizes[s];
        int* keys = new int[n];
        int* queries = new int[queryCount];
        for (int patternIndex = 0; patternIndex < 2; ++patternIndex) {
            char pattern = patternIndex == 0 ? 'S' : 'R';
            fillInput(keys, n, pattern, 31091u + static_cast<unsigned int>(n));
            unsigned int state = 8227u + static_cast<unsigned int>(n);
            for (int q = 0; q < queryCount; ++q) {
                queries[q] = 1 + static_cast<int>(nextRandom(state) % static_cast<unsigned int>(2 * n));
            }
            BST bst(nullptr);
            AVLTree avl(nullptr);
            bool valid = buildAndValidate(bst, avl, keys, n);
            int bstHits = 0, avlHits = 0;
            long long bstComparisons = countBstSearchComparisons(bst.getRoot(), queries, queryCount, bstHits);
            long long avlComparisons = countAvlSearchComparisons(avl.getRoot(), queries, queryCount, avlHits);
            if (bstHits != avlHits || bstHits <= 0 || bstHits >= queryCount) valid = false;
            double bstUs = timeBstSearch(bst, queries, queryCount, bstHits, valid);
            double avlUs = timeAvlSearch(avl, queries, queryCount, avlHits, valid);
            if (!valid) allValid = false;
            const char* input = pattern == 'S' ? "sorted" : "random";
            report << n << ',' << input << ",BST," << queryCount << ',' << bstHits << ','
                   << std::fixed << std::setprecision(3) << static_cast<double>(bstComparisons) / queryCount << ','
                   << std::setprecision(2) << bstUs << ',' << (valid ? "PASS" : "FAIL") << std::endl;
            report << n << ',' << input << ",AVL," << queryCount << ',' << avlHits << ','
                   << std::fixed << std::setprecision(3) << static_cast<double>(avlComparisons) / queryCount << ','
                   << std::setprecision(2) << avlUs << ',' << (valid ? "PASS" : "FAIL") << std::endl;
        }
        delete[] keys;
        delete[] queries;
    }
    report << std::endl;
}

static void benchmarkRotations(std::ofstream& report, bool& allValid) {
    report << "EXPERIMENT C: AVL rotation counts; one rotation means one primitive left/right rotation; a double case counts as two." << std::endl;
    report << "Each input contains n unique keys. Delete workload removes n/2 existing keys from a fresh populated tree." << std::endl;
    report << "n,pattern,insert_operations,insert_primitive_rotations,average_rotations_per_insert,delete_operations,delete_primitive_rotations,average_rotations_per_delete,max_primitive_rotations_per_delete,validation" << std::endl;
    for (int s = 0; s < 3; ++s) {
        int n = inputSizes[s];
        int deleteCount = n / 2;
        int* keys = new int[n];
        for (int patternIndex = 0; patternIndex < 2; ++patternIndex) {
            char pattern = patternIndex == 0 ? 'S' : 'R';
            fillInput(keys, n, pattern, 42209u + static_cast<unsigned int>(n));
            int rotations = 0;
            AVLTree avl(nullptr, &rotations);
            long long insertRotations = 0;
            bool valid = true;
            for (int i = 0; i < n; ++i) {
                rotations = 0;
                avl.insert(keys[i]);
                insertRotations += rotations;
                if (avl.getRootSize() != i + 1 || !avl.isBalanced()) valid = false;
            }
            long long deleteRotations = 0;
            int maxDeleteRotations = 0;
            for (int i = 0; i < deleteCount; ++i) {
                int key = pattern == 'S' ? keys[i * 2 + 1] : keys[i];
                rotations = 0;
                avl.deleteNode(key);
                deleteRotations += rotations;
                if (rotations > maxDeleteRotations) maxDeleteRotations = rotations;
                if (!avl.isBalanced() || avl.getRootSize() != n - i - 1) valid = false;
            }
            AvlAudit audit = auditAvlNode(avl.getRoot(), nullptr, -2147483649LL, 2147483648LL);
            valid = valid && audit.valid && audit.nodes == n - deleteCount;
            if (!valid) allValid = false;
            report << n << ',' << (pattern == 'S' ? "sorted" : "random") << ',' << n << ','
                   << insertRotations << ',' << std::fixed << std::setprecision(4)
                   << static_cast<double>(insertRotations) / n << ',' << deleteCount << ','
                   << deleteRotations << ',' << static_cast<double>(deleteRotations) / deleteCount << ','
                   << maxDeleteRotations << ',' << (valid ? "PASS" : "FAIL") << std::endl;
        }
        delete[] keys;
    }
    report << std::endl;
}

static void benchmarkOrderStatistics(std::ofstream& report, bool& allValid) {
    report << "EXPERIMENT D: same random unique key set and 1000 valid k queries; median of 3 runs, result equality checked." << std::endl;
    report << "n,bst_inorder_scan_total_us,avl_kth_total_us,queries,results_match,validation" << std::endl;
    for (int s = 0; s < 3; ++s) {
        int n = inputSizes[s];
        int* keys = new int[n];
        int queries[1000];
        int bstResults[1000];
        int avlResults[1000];
        fillInput(keys, n, 'R', 9349u + static_cast<unsigned int>(n));
        unsigned int state = 77731u + static_cast<unsigned int>(n);
        for (int q = 0; q < 1000; ++q) queries[q] = 1 + static_cast<int>(nextRandom(state) % static_cast<unsigned int>(n));
        BST bst(nullptr);
        AVLTree avl(nullptr);
        bool valid = buildAndValidate(bst, avl, keys, n);
        double bstTimes[runCount], avlTimes[runCount];
        volatile long long resultSink = 0;
        for (int run = 0; run < runCount; ++run) {
            auto start = std::chrono::steady_clock::now();
            for (int q = 0; q < 1000; ++q) resultSink = resultSink + bst.kthSmallest(queries[q]);
            auto stop = std::chrono::steady_clock::now();
            bstTimes[run] = std::chrono::duration<double, std::micro>(stop - start).count();
        }
        for (int run = 0; run < runCount; ++run) {
            auto start = std::chrono::steady_clock::now();
            for (int q = 0; q < 1000; ++q) resultSink = resultSink + avl.kth(queries[q]);
            auto stop = std::chrono::steady_clock::now();
            avlTimes[run] = std::chrono::duration<double, std::micro>(stop - start).count();
        }
        bool matches = true;
        for (int q = 0; q < 1000; ++q) {
            bstResults[q] = bst.kthSmallest(queries[q]);
            avlResults[q] = avl.kth(queries[q]);
            if (bstResults[q] != avlResults[q] || bstResults[q] == -1) matches = false;
        }
        valid = valid && matches;
        if (!valid) allValid = false;
        report << n << ',' << std::fixed << std::setprecision(2) << medianTime(bstTimes) << ','
               << medianTime(avlTimes) << ",1000," << (matches ? "yes" : "no") << ','
               << (valid ? "PASS" : "FAIL") << std::endl;
        delete[] keys;
    }
    report << std::endl;
}

static void benchmarkHeapBuild(std::ofstream& report, bool& allValid) {
    report << "EXPERIMENT E: project bottom-up MaxHeap::buildMaxHeap; fresh input and heap per run; median of 3." << std::endl;
    report << "n,pattern,build_time_us_median_of_3,active_size,max_heap_validator" << std::endl;
    for (int s = 0; s < 3; ++s) {
        int n = inputSizes[s];
        int* input = new int[n];
        for (int patternIndex = 0; patternIndex < 2; ++patternIndex) {
            char pattern = patternIndex == 0 ? 'R' : 'S';
            fillInput(input, n, pattern, 7823u + static_cast<unsigned int>(n));
            double times[runCount];
            bool valid = true;
            int finalSize = 0;
            for (int run = 0; run < runCount; ++run) {
                MaxHeap heap(n, nullptr);
                heap.loadRawArray(input, n);
                auto start = std::chrono::steady_clock::now();
                heap.buildMaxHeap();
                auto stop = std::chrono::steady_clock::now();
                times[run] = std::chrono::duration<double, std::micro>(stop - start).count();
                finalSize = heap.getSize();
                if (!heap.isMaxHeap() || finalSize != n) valid = false;
            }
            if (!valid) allValid = false;
            report << n << ',' << (pattern == 'R' ? "random" : "ordered_ascending") << ','
                   << std::fixed << std::setprecision(2) << medianTime(times) << ',' << finalSize << ','
                   << (valid ? "PASS" : "FAIL") << std::endl;
        }
        delete[] input;
    }
    report << std::endl;
}

static bool sortedAndPreserved(const int* output, const int* input, int n, char pattern) {
    for (int i = 1; i < n; ++i) if (output[i - 1] > output[i]) return false;
    if (pattern != 'D') {
        for (int i = 0; i < n; ++i) if (output[i] != i + 1) return false;
        return true;
    }
    int inputCounts[11] = {0};
    int outputCounts[11] = {0};
    for (int i = 0; i < n; ++i) {
        if (input[i] < -5 || input[i] > 5 || output[i] < -5 || output[i] > 5) return false;
        ++inputCounts[input[i] + 5];
        ++outputCounts[output[i] + 5];
    }
    for (int i = 0; i < 11; ++i) if (inputCounts[i] != outputCounts[i]) return false;
    return true;
}

static void benchmarkHeapSort(std::ofstream& report, bool& allValid) {
    report << "EXPERIMENT F: project's MaxHeap::heapSort; value comparisons and swaps counted by the implementation; median of 3 timing runs." << std::endl;
    report << "A comparison is one integer value comparison. A swap is one exchange of distinct array indexes. Count pass is separate from timing." << std::endl;
    report << "n,pattern,execution_time_us_median_of_3,comparisons,swaps,sorted_and_preserved" << std::endl;
    const char patterns[4] = {'R', 'S', 'V', 'D'};
    for (int s = 0; s < 3; ++s) {
        int n = inputSizes[s];
        int* input = new int[n];
        int* work = new int[n];
        for (int p = 0; p < 4; ++p) {
            char pattern = patterns[p];
            fillInput(input, n, pattern, 32917u + static_cast<unsigned int>(n));
            double times[runCount];
            bool valid = true;
            for (int run = 0; run < runCount; ++run) {
                for (int i = 0; i < n; ++i) work[i] = input[i];
                auto start = std::chrono::steady_clock::now();
                MaxHeap::heapSort(work, n, nullptr);
                auto stop = std::chrono::steady_clock::now();
                times[run] = std::chrono::duration<double, std::micro>(stop - start).count();
                if (!sortedAndPreserved(work, input, n, pattern)) valid = false;
            }
            for (int i = 0; i < n; ++i) work[i] = input[i];
            long long comparisons = 0, swaps = 0;
            MaxHeap::heapSort(work, n, nullptr, &comparisons, &swaps);
            if (!sortedAndPreserved(work, input, n, pattern)) valid = false;
            if (!valid) allValid = false;
            report << n << ',' << inputName(pattern) << ',' << std::fixed << std::setprecision(2)
                   << medianTime(times) << ',' << comparisons << ',' << swaps << ','
                   << (valid ? "PASS" : "FAIL") << std::endl;
        }
        delete[] input;
        delete[] work;
    }
    report << std::endl;
}

static void swapValues(int* values, int a, int b, SortMetrics* metrics) {
    if (a == b) return;
    int temp = values[a]; values[a] = values[b]; values[b] = temp;
    if (metrics != nullptr) { ++metrics->moves; }
}

static int quickPartition(int* values, int low, int high, SortMetrics* metrics) {
    int pivot = values[high];
    int boundary = low - 1;
    for (int i = low; i < high; ++i) {
        if (metrics != nullptr) ++metrics->comparisons;
        if (values[i] <= pivot) {
            ++boundary;
            swapValues(values, boundary, i, metrics);
        }
    }
    swapValues(values, boundary + 1, high, metrics);
    return boundary + 1;
}

static void quickSortRange(int* values, int low, int high, SortMetrics* metrics) {
    if (low >= high) return;
    int pivot = quickPartition(values, low, high, metrics);
    quickSortRange(values, low, pivot - 1, metrics);
    quickSortRange(values, pivot + 1, high, metrics);
}

static void quickSort(int* values, int n, SortMetrics* metrics) {
    if (metrics != nullptr) { metrics->comparisons = 0; metrics->moves = 0; }
    quickSortRange(values, 0, n - 1, metrics);
}

static void mergeSortRange(int* values, int* temp, int left, int right, SortMetrics* metrics) {
    if (left >= right) return;
    int middle = left + (right - left) / 2;
    mergeSortRange(values, temp, left, middle, metrics);
    mergeSortRange(values, temp, middle + 1, right, metrics);
    int a = left, b = middle + 1, write = left;
    while (a <= middle && b <= right) {
        if (metrics != nullptr) ++metrics->comparisons;
        if (values[a] <= values[b]) temp[write++] = values[a++];
        else temp[write++] = values[b++];
        if (metrics != nullptr) ++metrics->moves;
    }
    while (a <= middle) { temp[write++] = values[a++]; if (metrics != nullptr) ++metrics->moves; }
    while (b <= right) { temp[write++] = values[b++]; if (metrics != nullptr) ++metrics->moves; }
    for (int i = left; i <= right; ++i) { values[i] = temp[i]; if (metrics != nullptr) ++metrics->moves; }
}

static void mergeSort(int* values, int n, SortMetrics* metrics) {
    if (metrics != nullptr) { metrics->comparisons = 0; metrics->moves = 0; }
    int* temp = new int[n];
    mergeSortRange(values, temp, 0, n - 1, metrics);
    delete[] temp;
}

static void insertionSort(int* values, int n, SortMetrics* metrics) {
    if (metrics != nullptr) { metrics->comparisons = 0; metrics->moves = 0; }
    for (int i = 1; i < n; ++i) {
        int key = values[i];
        int j = i - 1;
        while (j >= 0) {
            if (metrics != nullptr) ++metrics->comparisons;
            if (values[j] > key) {
                values[j + 1] = values[j];
                if (metrics != nullptr) ++metrics->moves;
                --j;
            } else break;
        }
        values[j + 1] = key;
        if (metrics != nullptr) ++metrics->moves;
    }
}

typedef void (*SortFunction)(int*, int, SortMetrics*);

static bool sortedPermutationOneToN(const int* values, int n) {
    for (int i = 0; i < n; ++i) if (values[i] != i + 1) return false;
    return true;
}

static void benchmarkSortingComparison(std::ofstream& report, bool& allValid) {
    report << "EXPERIMENT G: Heap Sort compared with the sorting implementations already present in the benchmark (QuickSort, MergeSort, InsertionSort)." << std::endl;
    report << "All algorithms receive the same random permutation at each n. Timings are median of 3; counters are a separate pass." << std::endl;
    report << "n,algorithm,time_us_median_of_3,comparisons,swaps_or_moves,auxiliary_space,time_complexity,correct" << std::endl;
    const char* names[4] = {"HeapSort", "QuickSort", "MergeSort", "InsertionSort"};
    const char* spaces[4] = {"O(1)", "O(log n) average; O(n) worst", "O(n)", "O(1)"};
    const char* complexities[4] = {"O(n log n)", "O(n log n) average; O(n^2) worst", "O(n log n)", "O(n^2)"};
    SortFunction functions[3] = {quickSort, mergeSort, insertionSort};
    for (int s = 0; s < 3; ++s) {
        int n = inputSizes[s];
        int* input = new int[n];
        int* work = new int[n];
        fillInput(input, n, 'R', 66047u + static_cast<unsigned int>(n));
        for (int algorithm = 0; algorithm < 4; ++algorithm) {
            double times[runCount];
            bool valid = true;
            for (int run = 0; run < runCount; ++run) {
                for (int i = 0; i < n; ++i) work[i] = input[i];
                auto start = std::chrono::steady_clock::now();
                if (algorithm == 0) MaxHeap::heapSort(work, n, nullptr);
                else functions[algorithm - 1](work, n, nullptr);
                auto stop = std::chrono::steady_clock::now();
                times[run] = std::chrono::duration<double, std::micro>(stop - start).count();
                if (!sortedPermutationOneToN(work, n)) valid = false;
            }
            for (int i = 0; i < n; ++i) work[i] = input[i];
            long long comparisons = 0, moves = 0;
            if (algorithm == 0) MaxHeap::heapSort(work, n, nullptr, &comparisons, &moves);
            else {
                SortMetrics metrics = {0, 0};
                functions[algorithm - 1](work, n, &metrics);
                comparisons = metrics.comparisons;
                moves = metrics.moves;
            }
            if (!sortedPermutationOneToN(work, n)) valid = false;
            if (!valid) allValid = false;
            report << n << ',' << names[algorithm] << ',' << std::fixed << std::setprecision(2)
                   << medianTime(times) << ',' << comparisons << ',' << moves << ',' << spaces[algorithm]
                   << ',' << complexities[algorithm] << ',' << (valid ? "PASS" : "FAIL") << std::endl;
        }
        delete[] input;
        delete[] work;
    }
    report << std::endl;
}

int runBenchmarkSuite() {
    std::ofstream report("benchmark_results.txt");
    if (!report) {
        std::cerr << "Cannot write benchmark_results.txt" << std::endl;
        return 2;
    }
    bool allValid = true;
    report << "Vault Heist: The Living Vault - Headless Benchmark Report" << std::endl;
#if defined(__VERSION__)
    report << "Compiler: GCC " << __VERSION__ << std::endl;
#else
    report << "Compiler: GCC-compatible C++17 compiler" << std::endl;
#endif
    report << "Build mode: optimized benchmark build (-O2); timing unit: microseconds; timing aggregation: median of 3." << std::endl;
    report << "SFML/GUI: not linked. Event recording: disabled by passing nullptr to all benchmark structures." << std::endl;
    report << "Random workloads use a fixed integer generator seed for repeatability. Data generation and correctness checks are outside timed regions." << std::endl << std::endl;
    benchmarkHeights(report, allValid);
    benchmarkSearch(report, allValid);
    benchmarkRotations(report, allValid);
    benchmarkOrderStatistics(report, allValid);
    benchmarkHeapBuild(report, allValid);
    benchmarkHeapSort(report, allValid);
    benchmarkSortingComparison(report, allValid);
    report << "OVERALL_VALIDATION," << (allValid ? "PASS" : "FAIL") << std::endl;
    report.close();
    std::cout << "Benchmark report written to benchmark_results.txt" << std::endl;
    return allValid ? 0 : 1;
}
