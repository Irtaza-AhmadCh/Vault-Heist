#include "benchmark.h"

// ============================================================================
// Sorting Helper Functions for Experiment 7
// ============================================================================

namespace SortingAlgos {
    // Instrumented QuickSort
    static void quickSortHelper(std::vector<int>& arr, int low, int high, long long& comps, long long& swaps) {
        if (low >= high) return;

        // Median-of-three pivot selection
        int mid = low + (high - low) / 2;
        if (arr[mid] < arr[low]) { std::swap(arr[low], arr[mid]); swaps++; }
        comps++;
        if (arr[high] < arr[low]) { std::swap(arr[low], arr[high]); swaps++; }
        comps++;
        if (arr[high] < arr[mid]) { std::swap(arr[mid], arr[high]); swaps++; }
        comps++;
        std::swap(arr[mid], arr[high - 1]);
        swaps++;
        int pivot = arr[high - 1];

        int i = low;
        int j = high - 1;
        while (true) {
            while (arr[++i] < pivot) comps++;
            comps++; // loop exit comparison
            while (pivot < arr[--j]) comps++;
            comps++; // loop exit comparison
            if (i >= j) break;
            std::swap(arr[i], arr[j]);
            swaps++;
        }
        std::swap(arr[i], arr[high - 1]);
        swaps++;

        quickSortHelper(arr, low, i - 1, comps, swaps);
        quickSortHelper(arr, i + 1, high, comps, swaps);
    }

    void quickSort(std::vector<int>& arr, long long& comps, long long& swaps) {
        comps = 0;
        swaps = 0;
        if (arr.size() <= 1) return;
        quickSortHelper(arr, 0, (int)arr.size() - 1, comps, swaps);
    }

    // Instrumented MergeSort
    static void merge(std::vector<int>& arr, int left, int mid, int right, std::vector<int>& temp, long long& comps, long long& moves) {
        int i = left;
        int j = mid + 1;
        int k = left;

        while (i <= mid && j <= right) {
            comps++;
            if (arr[i] <= arr[j]) {
                temp[k++] = arr[i++];
            } else {
                temp[k++] = arr[j++];
            }
            moves++;
        }
        while (i <= mid) {
            temp[k++] = arr[i++];
            moves++;
        }
        while (j <= right) {
            temp[k++] = arr[j++];
            moves++;
        }
        for (int p = left; p <= right; p++) {
            arr[p] = temp[p];
            moves++;
        }
    }

    static void mergeSortHelper(std::vector<int>& arr, int left, int right, std::vector<int>& temp, long long& comps, long long& moves) {
        if (left >= right) return;
        int mid = left + (right - left) / 2;
        mergeSortHelper(arr, left, mid, temp, comps, moves);
        mergeSortHelper(arr, mid + 1, right, temp, comps, moves);
        merge(arr, left, mid, right, temp, comps, moves);
    }

    void mergeSort(std::vector<int>& arr, long long& comps, long long& moves) {
        comps = 0;
        moves = 0;
        if (arr.size() <= 1) return;
        std::vector<int> temp(arr.size());
        mergeSortHelper(arr, 0, (int)arr.size() - 1, temp, comps, moves);
    }

    // Instrumented InsertionSort
    void insertionSort(std::vector<int>& arr, long long& comps, long long& moves) {
        comps = 0;
        moves = 0;
        int n = (int)arr.size();
        for (int i = 1; i < n; i++) {
            int key = arr[i];
            int j = i - 1;
            while (j >= 0) {
                comps++;
                if (arr[j] > key) {
                    arr[j + 1] = arr[j];
                    moves++;
                    j--;
                } else {
                    break;
                }
            }
            arr[j + 1] = key;
            moves++;
        }
    }

    // Instrumented HeapSort implementation for direct metrics
    static void heapifySiftDown(std::vector<int>& arr, int n, int i, long long& comps, long long& swaps) {
        while (true) {
            int largest = i;
            int left = 2 * i + 1;
            int right = 2 * i + 2;

            if (left < n) {
                comps++;
                if (arr[left] > arr[largest]) {
                    largest = left;
                }
            }
            if (right < n) {
                comps++;
                if (arr[right] > arr[largest]) {
                    largest = right;
                }
            }

            if (largest != i) {
                std::swap(arr[i], arr[largest]);
                swaps++;
                i = largest;
            } else {
                break;
            }
        }
    }

    void instrumentedHeapSort(std::vector<int>& arr, long long& comps, long long& swaps) {
        comps = 0;
        swaps = 0;
        int n = (int)arr.size();
        if (n <= 1) return;

        // Bottom-up construction O(n)
        for (int i = (n / 2) - 1; i >= 0; i--) {
            heapifySiftDown(arr, n, i, comps, swaps);
        }

        // Repeated extraction O(n log n)
        for (int i = n - 1; i > 0; i--) {
            std::swap(arr[0], arr[i]);
            swaps++;
            heapifySiftDown(arr, i, 0, comps, swaps);
        }
    }
}

// ============================================================================
// Utility Methods
// ============================================================================

std::vector<int> BenchmarkSuite::generateRandomArray(int n, int minVal, int maxVal) {
    std::mt19937 rng(42); // fixed seed for strict scientific reproducibility
    std::uniform_int_distribution<int> dist(minVal, maxVal);
    std::vector<int> arr(n);
    for (int i = 0; i < n; i++) {
        arr[i] = dist(rng);
    }
    return arr;
}

std::vector<int> BenchmarkSuite::generateSortedArray(int n) {
    std::vector<int> arr(n);
    for (int i = 0; i < n; i++) {
        arr[i] = i + 1;
    }
    return arr;
}

std::vector<int> BenchmarkSuite::generateReverseSortedArray(int n) {
    std::vector<int> arr(n);
    for (int i = 0; i < n; i++) {
        arr[i] = n - i;
    }
    return arr;
}

std::vector<int> BenchmarkSuite::generateDuplicateHeavyArray(int n, int distinctCount) {
    std::mt19937 rng(1337);
    std::uniform_int_distribution<int> dist(1, distinctCount);
    std::vector<int> arr(n);
    for (int i = 0; i < n; i++) {
        arr[i] = dist(rng);
    }
    return arr;
}

void BenchmarkSuite::printHeader(const std::string& title) {
    std::cout << "\n" << std::string(80, '=') << "\n";
    std::cout << "  " << title << "\n";
    std::cout << std::string(80, '=') << "\n";
}

void BenchmarkSuite::printDivider() {
    std::cout << std::string(80, '-') << "\n";
}

// ============================================================================
// Experiment 1: Height Comparison
// ============================================================================

void BenchmarkSuite::runExperiment1_HeightComparison() {
    printHeader("EXPERIMENT 1: TREE HEIGHT COMPARISON (BST vs. AVL)");
    std::cout << "Testing n = [100, 1,000, 10,000] for both Sorted and Random inputs.\n";
    std::cout << "Demonstrates BST degeneration O(n) vs AVL balance bound O(log n).\n\n";

    std::cout << "|   n   | Input Type | BST Height | AVL Height | AVL Theoretical Max (1.44*log2(n)) |\n";
    std::cout << "|-------|------------|------------|------------|------------------------------------|\n";

    int testSizes[] = {100, 1000, 10000};
    for (int n : testSizes) {
        // 1. Sorted Input
        {
            BST bst;
            AVLTree avl(nullptr);
            for (int i = 1; i <= n; i++) {
                bst.insert(i);
                avl.insert(i);
            }
            int bstH = bst.getHeight();
            int avlH = avl.getRootHeight();
            int maxTheo = (int)std::ceil(1.4404 * (std::log2(n + 2) - 0.3277));
            std::cout << "| " << std::setw(5) << n
                      << " | Sorted     | " << std::setw(10) << bstH
                      << " | " << std::setw(10) << avlH
                      << " | " << std::setw(34) << maxTheo << " |\n";
        }

        // 2. Random Input
        {
            BST bst;
            AVLTree avl(nullptr);
            std::vector<int> randArr = generateRandomArray(n, 1, n * 10);
            for (int val : randArr) {
                bst.insert(val);
                avl.insert(val);
            }
            int bstH = bst.getHeight();
            int avlH = avl.getRootHeight();
            int maxTheo = (int)std::ceil(1.4404 * (std::log2(n + 2) - 0.3277));
            std::cout << "| " << std::setw(5) << n
                      << " | Random     | " << std::setw(10) << bstH
                      << " | " << std::setw(10) << avlH
                      << " | " << std::setw(34) << maxTheo << " |\n";
        }
    }
}

// ============================================================================
// Experiment 2: Search Cost
// ============================================================================

void BenchmarkSuite::runExperiment2_SearchCost() {
    printHeader("EXPERIMENT 2: SEARCH COST (10,000 LOOKUPS)");
    std::cout << "Measuring average comparisons and execution time across 10,000 random searches.\n";
    std::cout << "Demonstrates O(log n) vs O(n) lookup efficiency.\n\n";

    std::cout << "|   n   | Tree Type | Input Order | Avg Comparisons | Total Time (us) | Avg Time (ns) |\n";
    std::cout << "|-------|-----------|-------------|-----------------|-----------------|---------------|\n";

    int testSizes[] = {100, 1000, 10000};
    const int NUM_SEARCHES = 10000;

    for (int n : testSizes) {
        // Generate queries (mix of present and absent keys)
        std::vector<int> searchKeys = generateRandomArray(NUM_SEARCHES, 1, n * 2);

        // 1. Random Tree Construction
        std::vector<int> randData = generateRandomArray(n, 1, n * 2);
        BST bstRand;
        AVLTree avlRand(nullptr);
        for (int v : randData) {
            bstRand.insert(v);
            avlRand.insert(v);
        }

        // Measure BST on Random Data
        {
            long long totalComps = 0;
            auto start = std::chrono::high_resolution_clock::now();
            for (int key : searchKeys) {
                BSTNode* cur = bstRand.getRoot();
                while (cur != nullptr) {
                    totalComps++;
                    if (key == cur->key) break;
                    totalComps++;
                    if (key < cur->key) cur = cur->left;
                    else cur = cur->right;
                }
            }
            auto end = std::chrono::high_resolution_clock::now();
            double durationUs = std::chrono::duration<double, std::micro>(end - start).count();
            double avgComps = (double)totalComps / NUM_SEARCHES;
            double avgNs = (durationUs * 1000.0) / NUM_SEARCHES;

            std::cout << "| " << std::setw(5) << n
                      << " | BST       | Random      | " << std::setw(15) << std::fixed << std::setprecision(2) << avgComps
                      << " | " << std::setw(15) << std::fixed << std::setprecision(1) << durationUs
                      << " | " << std::setw(13) << std::fixed << std::setprecision(1) << avgNs << " |\n";
        }

        // Measure AVL on Random Data
        {
            long long totalComps = 0;
            auto start = std::chrono::high_resolution_clock::now();
            for (int key : searchKeys) {
                AVLNode* cur = avlRand.getRoot();
                while (cur != nullptr) {
                    totalComps++;
                    if (key == cur->key) break;
                    totalComps++;
                    if (key < cur->key) cur = cur->left;
                    else cur = cur->right;
                }
            }
            auto end = std::chrono::high_resolution_clock::now();
            double durationUs = std::chrono::duration<double, std::micro>(end - start).count();
            double avgComps = (double)totalComps / NUM_SEARCHES;
            double avgNs = (durationUs * 1000.0) / NUM_SEARCHES;

            std::cout << "| " << std::setw(5) << n
                      << " | AVL       | Random      | " << std::setw(15) << std::fixed << std::setprecision(2) << avgComps
                      << " | " << std::setw(15) << std::fixed << std::setprecision(1) << durationUs
                      << " | " << std::setw(13) << std::fixed << std::setprecision(1) << avgNs << " |\n";
        }

        // 2. Sorted Tree Construction
        BST bstSorted;
        AVLTree avlSorted(nullptr);
        for (int i = 1; i <= n; i++) {
            bstSorted.insert(i);
            avlSorted.insert(i);
        }

        // Measure BST on Sorted Data (Degenerate Line)
        {
            long long totalComps = 0;
            auto start = std::chrono::high_resolution_clock::now();
            for (int key : searchKeys) {
                BSTNode* cur = bstSorted.getRoot();
                while (cur != nullptr) {
                    totalComps++;
                    if (key == cur->key) break;
                    totalComps++;
                    if (key < cur->key) cur = cur->left;
                    else cur = cur->right;
                }
            }
            auto end = std::chrono::high_resolution_clock::now();
            double durationUs = std::chrono::duration<double, std::micro>(end - start).count();
            double avgComps = (double)totalComps / NUM_SEARCHES;
            double avgNs = (durationUs * 1000.0) / NUM_SEARCHES;

            std::cout << "| " << std::setw(5) << n
                      << " | BST       | Sorted      | " << std::setw(15) << std::fixed << std::setprecision(2) << avgComps
                      << " | " << std::setw(15) << std::fixed << std::setprecision(1) << durationUs
                      << " | " << std::setw(13) << std::fixed << std::setprecision(1) << avgNs << " |\n";
        }

        // Measure AVL on Sorted Data
        {
            long long totalComps = 0;
            auto start = std::chrono::high_resolution_clock::now();
            for (int key : searchKeys) {
                AVLNode* cur = avlSorted.getRoot();
                while (cur != nullptr) {
                    totalComps++;
                    if (key == cur->key) break;
                    totalComps++;
                    if (key < cur->key) cur = cur->left;
                    else cur = cur->right;
                }
            }
            auto end = std::chrono::high_resolution_clock::now();
            double durationUs = std::chrono::duration<double, std::micro>(end - start).count();
            double avgComps = (double)totalComps / NUM_SEARCHES;
            double avgNs = (durationUs * 1000.0) / NUM_SEARCHES;

            std::cout << "| " << std::setw(5) << n
                      << " | AVL       | Sorted      | " << std::setw(15) << std::fixed << std::setprecision(2) << avgComps
                      << " | " << std::setw(15) << std::fixed << std::setprecision(1) << durationUs
                      << " | " << std::setw(13) << std::fixed << std::setprecision(1) << avgNs << " |\n";
        }
    }
}

// ============================================================================
// Experiment 3: Rotation Count
// ============================================================================

void BenchmarkSuite::runExperiment3_RotationCount() {
    printHeader("EXPERIMENT 3: AVL ROTATION FREQUENCY ANALYSIS");
    std::cout << "Measuring average rotations per insertion and deletion, and max rotations per delete.\n";
    std::cout << "Confirms theoretical bound: <= 2 rotations per insert, O(log n) cascading delete.\n\n";

    std::cout << "|   n   | Input Pattern | Avg Rotations/Insert | Avg Rotations/Delete | Max Rotations Single Delete |\n";
    std::cout << "|-------|---------------|----------------------|----------------------|-----------------------------|\n";

    int testSizes[] = {100, 1000, 10000};
    EventLog eventLog(32);

    for (int n : testSizes) {
        // 1. Sorted Input
        {
            AVLTree avl(&eventLog);
            long long totalInsertRotations = 0;
            for (int i = 1; i <= n; i++) {
                avl.insert(i);
                int rCount = 0;
                for (int e = 0; e < eventLog.count; e++) {
                    if (eventLog.items[e].type == EVENT_ROTATE) rCount++;
                }
                totalInsertRotations += rCount;
                eventLog.clear();
            }

            // Deletions: Delete n/2 elements
            long long totalDeleteRotations = 0;
            int maxDeleteRot = 0;
            int deleteCount = n / 2;
            for (int i = 1; i <= deleteCount; i++) {
                avl.deleteNode(i * 2);
                int rCount = 0;
                for (int e = 0; e < eventLog.count; e++) {
                    if (eventLog.items[e].type == EVENT_ROTATE) rCount++;
                }
                totalDeleteRotations += rCount;
                if (rCount > maxDeleteRot) maxDeleteRot = rCount;
                eventLog.clear();
            }

            double avgIns = (double)totalInsertRotations / n;
            double avgDel = (double)totalDeleteRotations / deleteCount;

            std::cout << "| " << std::setw(5) << n
                      << " | Sorted        | " << std::setw(20) << std::fixed << std::setprecision(4) << avgIns
                      << " | " << std::setw(20) << std::fixed << std::setprecision(4) << avgDel
                      << " | " << std::setw(27) << maxDeleteRot << " |\n";
        }

        // 2. Random Input
        {
            AVLTree avl(&eventLog);
            std::vector<int> randData = generateRandomArray(n, 1, n * 10);
            long long totalInsertRotations = 0;
            for (int val : randData) {
                avl.insert(val);
                int rCount = 0;
                for (int e = 0; e < eventLog.count; e++) {
                    if (eventLog.items[e].type == EVENT_ROTATE) rCount++;
                }
                totalInsertRotations += rCount;
                eventLog.clear();
            }

            long long totalDeleteRotations = 0;
            int maxDeleteRot = 0;
            int deleteCount = n / 2;
            for (int i = 0; i < deleteCount; i++) {
                avl.deleteNode(randData[i]);
                int rCount = 0;
                for (int e = 0; e < eventLog.count; e++) {
                    if (eventLog.items[e].type == EVENT_ROTATE) rCount++;
                }
                totalDeleteRotations += rCount;
                if (rCount > maxDeleteRot) maxDeleteRot = rCount;
                eventLog.clear();
            }

            double avgIns = (double)totalInsertRotations / n;
            double avgDel = (double)totalDeleteRotations / deleteCount;

            std::cout << "| " << std::setw(5) << n
                      << " | Random        | " << std::setw(20) << std::fixed << std::setprecision(4) << avgIns
                      << " | " << std::setw(20) << std::fixed << std::setprecision(4) << avgDel
                      << " | " << std::setw(27) << maxDeleteRot << " |\n";
        }
    }
}

// ============================================================================
// Experiment 4: Order Statistics (k-th Smallest Query)
// ============================================================================

void BenchmarkSuite::runExperiment4_OrderStatistics() {
    printHeader("EXPERIMENT 4: ORDER STATISTICS (1,000 K-TH QUERIES)");
    std::cout << "Comparing AVL subtreeSize augmentation O(log n) vs plain BST inorder scan O(n).\n";
    std::cout << "Demonstrates order-statistic optimization over standard traversal.\n\n";

    std::cout << "|   n   | BST Inorder Scan (ms) | AVL Augmented kth (ms) | Speedup Factor |\n";
    std::cout << "|-------|-----------------------|------------------------|----------------|\n";

    int testSizes[] = {100, 1000, 10000};
    const int NUM_QUERIES = 1000;

    for (int n : testSizes) {
        std::vector<int> data = generateRandomArray(n, 1, n * 5);
        BST bst;
        AVLTree avl(nullptr);
        for (int v : data) {
            bst.insert(v);
            avl.insert(v);
        }

        int actualSize = avl.getRootSize();
        std::mt19937 rng(999);
        std::uniform_int_distribution<int> dist(1, actualSize);
        std::vector<int> kQueries(NUM_QUERIES);
        for (int i = 0; i < NUM_QUERIES; i++) {
            kQueries[i] = dist(rng);
        }

        // 1. Measure BST Inorder Scan O(n)
        auto bstStart = std::chrono::high_resolution_clock::now();
        volatile int bstDummy = 0;
        for (int k : kQueries) {
            bstDummy += bst.kthSmallest(k);
        }
        auto bstEnd = std::chrono::high_resolution_clock::now();
        double bstMs = std::chrono::duration<double, std::milli>(bstEnd - bstStart).count();

        // 2. Measure AVL Augmented kth O(log n)
        auto avlStart = std::chrono::high_resolution_clock::now();
        volatile int avlDummy = 0;
        for (int k : kQueries) {
            avlDummy += avl.kth(k);
        }
        auto avlEnd = std::chrono::high_resolution_clock::now();
        double avlMs = std::chrono::duration<double, std::milli>(avlEnd - avlStart).count();

        double speedup = bstMs / (avlMs > 0.0001 ? avlMs : 0.0001);

        std::cout << "| " << std::setw(5) << n
                  << " | " << std::setw(21) << std::fixed << std::setprecision(3) << bstMs
                  << " | " << std::setw(22) << std::fixed << std::setprecision(3) << avlMs
                  << " | " << std::setw(12) << std::fixed << std::setprecision(1) << speedup << "x |\n";
    }
}

// ============================================================================
// Experiment 5: Heap Construction (Bottom-Up vs Repeated Insertion)
// ============================================================================

void BenchmarkSuite::runExperiment5_HeapConstruction() {
    printHeader("EXPERIMENT 5: HEAP CONSTRUCTION COMPLEXITY ANALYSIS");
    std::cout << "Empirically confirming Floyd's bottom-up buildMaxHeap O(n) vs Repeated Insertion O(n log n).\n\n";

    std::cout << "|   n   | Array Pattern | Bottom-Up Time (us) | Repeated Insert Time (us) | Bottom-Up Comps | Insert Comps |\n";
    std::cout << "|-------|---------------|---------------------|---------------------------|-----------------|--------------|\n";

    int testSizes[] = {100, 1000, 10000};

    for (int n : testSizes) {
        std::string patterns[] = {"Random", "Sorted", "Reverse"};
        for (const std::string& pat : patterns) {
            std::vector<int> arr;
            if (pat == "Random") arr = generateRandomArray(n, 1, n * 5);
            else if (pat == "Sorted") arr = generateSortedArray(n);
            else arr = generateReverseSortedArray(n);

            // 1. Bottom-up Construction
            std::vector<int> bottomUpCopy = arr;
            long long buComps = 0;
            long long buSwaps = 0;
            auto buStart = std::chrono::high_resolution_clock::now();
            for (int i = (n / 2) - 1; i >= 0; i--) {
                SortingAlgos::heapifySiftDown(bottomUpCopy, n, i, buComps, buSwaps);
            }
            auto buEnd = std::chrono::high_resolution_clock::now();
            double buTimeUs = std::chrono::duration<double, std::micro>(buEnd - buStart).count();

            // 2. Repeated Insertion
            std::vector<int> insertHeap;
            long long insComps = 0;
            long long insSwaps = 0;
            auto insStart = std::chrono::high_resolution_clock::now();
            for (int val : arr) {
                insertHeap.push_back(val);
                int idx = (int)insertHeap.size() - 1;
                while (idx > 0) {
                    int p = (idx - 1) / 2;
                    insComps++;
                    if (insertHeap[idx] > insertHeap[p]) {
                        std::swap(insertHeap[idx], insertHeap[p]);
                        insSwaps++;
                        idx = p;
                    } else {
                        break;
                    }
                }
            }
            auto insEnd = std::chrono::high_resolution_clock::now();
            double insTimeUs = std::chrono::duration<double, std::micro>(insEnd - insStart).count();

            std::cout << "| " << std::setw(5) << n
                      << " | " << std::setw(13) << pat
                      << " | " << std::setw(19) << std::fixed << std::setprecision(1) << buTimeUs
                      << " | " << std::setw(25) << std::fixed << std::setprecision(1) << insTimeUs
                      << " | " << std::setw(15) << buComps
                      << " | " << std::setw(12) << insComps << " |\n";
        }
    }
}

// ============================================================================
// Experiment 6: Heap Sort Analysis
// ============================================================================

void BenchmarkSuite::runExperiment6_HeapSort() {
    printHeader("EXPERIMENT 6: HEAP SORT PERFORMANCE PROFILE");
    std::cout << "Testing Heap Sort across Random, Sorted, Reverse-sorted, and Duplicate-heavy arrays.\n";
    std::cout << "Demonstrates consistent guaranteed O(n log n) runtime without worst-case degradation.\n\n";

    std::cout << "|   n   | Distribution | Runtime (us) | Comparisons | Swaps | Comps / (n*log2(n)) |\n";
    std::cout << "|-------|--------------|--------------|-------------|-------|---------------------|\n";

    int testSizes[] = {100, 1000, 10000};
    std::string dists[] = {"Random", "Sorted", "Reverse", "Duplicates"};

    for (int n : testSizes) {
        double nLogn = n * std::log2(n);
        for (const std::string& dist : dists) {
            std::vector<int> arr;
            if (dist == "Random") arr = generateRandomArray(n, 1, n * 5);
            else if (dist == "Sorted") arr = generateSortedArray(n);
            else if (dist == "Reverse") arr = generateReverseSortedArray(n);
            else arr = generateDuplicateHeavyArray(n, 5);

            long long comps = 0;
            long long swaps = 0;
            auto start = std::chrono::high_resolution_clock::now();
            SortingAlgos::instrumentedHeapSort(arr, comps, swaps);
            auto end = std::chrono::high_resolution_clock::now();
            double timeUs = std::chrono::duration<double, std::micro>(end - start).count();

            double ratio = (double)comps / nLogn;

            std::cout << "| " << std::setw(5) << n
                      << " | " << std::setw(12) << dist
                      << " | " << std::setw(12) << std::fixed << std::setprecision(1) << timeUs
                      << " | " << std::setw(11) << comps
                      << " | " << std::setw(5) << swaps
                      << " | " << std::setw(19) << std::fixed << std::setprecision(2) << ratio << " |\n";
        }
    }
}

// ============================================================================
// Experiment 7: Sorting Comparison (Course Sorting Algorithms)
// ============================================================================

void BenchmarkSuite::runExperiment7_SortingComparison() {
    printHeader("EXPERIMENT 7: SORTING ALGORITHM BENCHMARK COMPARISON");
    std::cout << "Benchmarking Heap Sort against QuickSort, MergeSort, and InsertionSort.\n";
    std::cout << "Evaluates Runtime, Comparisons, Swaps/Moves, and Auxiliary Space Complexity.\n\n";

    std::cout << "|   n   | Algorithm     | Runtime (us) | Comparisons | Swaps/Moves | Aux Space | Time Complexity |\n";
    std::cout << "|-------|---------------|--------------|-------------|-------------|-----------|-----------------|\n";

    int testSizes[] = {100, 1000, 10000};

    for (int n : testSizes) {
        std::vector<int> baseArr = generateRandomArray(n, 1, n * 10);

        // 1. Heap Sort
        {
            std::vector<int> arr = baseArr;
            long long comps = 0, swaps = 0;
            auto start = std::chrono::high_resolution_clock::now();
            SortingAlgos::instrumentedHeapSort(arr, comps, swaps);
            auto end = std::chrono::high_resolution_clock::now();
            double timeUs = std::chrono::duration<double, std::micro>(end - start).count();

            std::cout << "| " << std::setw(5) << n
                      << " | Heap Sort     | " << std::setw(12) << std::fixed << std::setprecision(1) << timeUs
                      << " | " << std::setw(11) << comps
                      << " | " << std::setw(11) << swaps
                      << " | O(1)      | O(n log n)      |\n";
        }

        // 2. QuickSort
        {
            std::vector<int> arr = baseArr;
            long long comps = 0, swaps = 0;
            auto start = std::chrono::high_resolution_clock::now();
            SortingAlgos::quickSort(arr, comps, swaps);
            auto end = std::chrono::high_resolution_clock::now();
            double timeUs = std::chrono::duration<double, std::micro>(end - start).count();

            std::cout << "| " << std::setw(5) << n
                      << " | QuickSort     | " << std::setw(12) << std::fixed << std::setprecision(1) << timeUs
                      << " | " << std::setw(11) << comps
                      << " | " << std::setw(11) << swaps
                      << " | O(log n)  | O(n log n)      |\n";
        }

        // 3. MergeSort
        {
            std::vector<int> arr = baseArr;
            long long comps = 0, moves = 0;
            auto start = std::chrono::high_resolution_clock::now();
            SortingAlgos::mergeSort(arr, comps, moves);
            auto end = std::chrono::high_resolution_clock::now();
            double timeUs = std::chrono::duration<double, std::micro>(end - start).count();

            std::cout << "| " << std::setw(5) << n
                      << " | MergeSort     | " << std::setw(12) << std::fixed << std::setprecision(1) << timeUs
                      << " | " << std::setw(11) << comps
                      << " | " << std::setw(11) << moves
                      << " | O(n)      | O(n log n)      |\n";
        }

        // 4. InsertionSort
        {
            std::vector<int> arr = baseArr;
            long long comps = 0, moves = 0;
            auto start = std::chrono::high_resolution_clock::now();
            SortingAlgos::insertionSort(arr, comps, moves);
            auto end = std::chrono::high_resolution_clock::now();
            double timeUs = std::chrono::duration<double, std::micro>(end - start).count();

            std::cout << "| " << std::setw(5) << n
                      << " | InsertionSort | " << std::setw(12) << std::fixed << std::setprecision(1) << timeUs
                      << " | " << std::setw(11) << comps
                      << " | " << std::setw(11) << moves
                      << " | O(1)      | O(n^2)          |\n";
        }

        std::cout << "|-------|---------------|--------------|-------------|-------------|-----------|-----------------|\n";
    }
}

// ============================================================================
// Runner Entry Point
// ============================================================================

void BenchmarkSuite::runAll() {
    std::cout << "\n";
    std::cout << "################################################################################\n";
    std::cout << "#       VAULT HEIST: THE LIVING VAULT - BENCHMARK & ANALYSIS SUITE             #\n";
    std::cout << "#       Comprehensive Empirical Evaluation of Custom Data Structures           #\n";
    std::cout << "################################################################################\n";

    runExperiment1_HeightComparison();
    runExperiment2_SearchCost();
    runExperiment3_RotationCount();
    runExperiment4_OrderStatistics();
    runExperiment5_HeapConstruction();
    runExperiment6_HeapSort();
    runExperiment7_SortingComparison();

    std::cout << "\n";
    std::cout << "################################################################################\n";
    std::cout << "#                ALL 7 BENCHMARK EXPERIMENTS COMPLETED                         #\n";
    std::cout << "################################################################################\n\n";
}

int main() {
    BenchmarkSuite::runAll();
    return 0;
}
