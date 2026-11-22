# Vault Heist --- GUI-Independent Testing and Benchmarking Plan

## 1. Purpose and scope

This document explains the work that can be completed and verified
**without opening or running the SFML GUI**, and the benchmarking work
required for the project.

The main principle is that the data structures and algorithms must work
independently of the GUI. Tests in this document should exercise the
model, validators, game logic where practical, event generation, and
serialization through ordinary C++ test programs.

This plan is based on:

-   **Project Deliverable 1**, especially Sections 4, 5, 8, 9 and 10
    (architecture, data structures, save/load, testing and
    benchmarking).
-   **System Architecture**, which separates structures, game logic,
    events and GUI.
-   **Coding Guardrails**, which require manually implemented
    structures, no STL containers, no SFML dependency in core data
    structures, and independently testable operations.
-   **Two-Person Development Roadmap**, Phase 5 and the testing
    responsibilities.

It does not replace the required manual GUI testing. GUI checks are
listed separately at the end so they are not confused with headless
testing.

------------------------------------------------------------------------

## 2. What "testing without GUI" means

A test is GUI-independent when it can run without:

-   Opening an SFML window.
-   Drawing or rendering nodes, edges, badges or array cells.
-   Mouse clicks, dragging, keyboard input or animation playback.
-   Running the animation controller or renderer.

Instead, the test directly creates a data structure or game/model
object, calls its functions, and checks the returned result and internal
invariants.

Examples:

-   Insert keys into a BST and verify the ordering.
-   Delete a node and verify the resulting tree.
-   Insert values into an AVL tree and verify its balance factors,
    heights and subtree sizes.
-   Build a Max Heap and verify every parent is at least as large as its
    existing children.
-   Run Heap Sort and compare its output with a trusted sorted
    reference.
-   Serialize a tree, load it again, and compare the reconstructed
    structure.

**Important architecture rule:** SFML-specific code must not be required
by the data-structure tests. The model should produce its results and
operation events independently; the GUI is a consumer of those
results/events.

------------------------------------------------------------------------

## 3. Suggested test organization

Follow the repository structure already agreed by the team. The roadmap
suggests the following test grouping:

``` text
tests/
    binary_tree/
    bst/
    avl/
    heap/
```

If the project already has a different test-folder arrangement, preserve
it rather than restructuring the project just for this document.

Keep tests grouped by the module being tested:

  -----------------------------------------------------------------------
  Test group                          Main responsibility
  ----------------------------------- -----------------------------------
  `binary_tree`                       General binary-tree operations and
                                      reconstruction

  `bst`                               BST operations, ordering and
                                      deletion

  `avl`                               AVL rotations, balance, metadata
                                      and order statistics

  `heap`                              Max Heap operations, extraction and
                                      Heap Sort

  Shared/model tests                  Validators, serialization, event
                                      output and relevant game logic
  -----------------------------------------------------------------------

Use the team's existing build setup and test framework. The project
documents do not prescribe a particular testing framework, so do not
introduce a new dependency without team approval.

------------------------------------------------------------------------

## 4. Binary Tree tests

These tests do not need the GUI. Construct trees directly through the
model API.

### 4.1 Traversals

Test all four required traversals:

-   Preorder
-   Inorder
-   Postorder
-   Level order

For each traversal, include:

1.  Empty tree.
2.  Single-node tree.
3.  Skewed tree.
4.  Full or reasonably complete tree.

Also verify that the recursive and iterative versions of each traversal
produce the same sequence, where both versions are implemented.

**Expected result:** Each traversal returns the correct node-ID sequence
for the constructed tree. Empty and single-node cases must be handled
correctly.

### 4.2 Reconstruction

Test reconstruction from:

-   Preorder + inorder.
-   Postorder + inorder.

Recommended test procedure:

1.  Construct a known tree.
2.  Obtain its traversal sequences.
3.  Rebuild a tree from the relevant pair of traversals.
4.  Compare the rebuilt tree with the original using structural
    comparison (`isSameTree`, or the project's equivalent).
5.  Include a single-node tree and a multi-level tree.

**Expected result:** The rebuilt tree has the same structure and values
as the original for valid traversal pairs.

The deliverable explicitly requires round-trip reconstruction testing.
It does not specify a separate policy for malformed or inconsistent
traversal input; if the implementation accepts such input, agree on the
expected error behavior and test it consistently.

### 4.3 Other general-tree operations

The required BinaryTree API also includes height, diameter, LCA,
path-to-node, mirror, and structural comparison. Add model-level tests
for the operations implemented by the team:

-   Height for empty, single-node and skewed trees.
-   Diameter for a single node, a chain and a tree whose longest path
    passes through the root.
-   LCA for nodes in the same subtree and in different subtrees.
-   `pathTo` for the root and a deeper node.
-   Mirror twice should restore the original structure.
-   `isSameTree` should distinguish different values and different
    shapes.
-   `isSymmetric` should correctly identify symmetric and non-symmetric
    trees.

Document whether height is measured in **edges or nodes**. Deliverable 1
explicitly says to choose one convention and document it.

------------------------------------------------------------------------

## 5. Binary Search Tree (BST) tests

### 5.1 Insertion and search

Test:

-   Inserting into an empty BST.
-   Inserting several values in mixed order.
-   Searching for the root.
-   Searching for an existing leaf.
-   Searching for a missing key.
-   Inserting a duplicate.

**Expected result:** The BST ordering rule remains true: keys in the
left subtree are smaller, and keys in the right subtree are larger.
Duplicates are rejected consistently.

### 5.2 Deletion --- all cases

The test suite must cover all three deletion cases:

  -----------------------------------------------------------------------
  Case                    Test setup              What to verify
  ----------------------- ----------------------- -----------------------
  Leaf                    Delete a leaf node      The leaf is removed and
                                                  its parent link is
                                                  correct

  One child               Delete a node with only The parent is linked to
                          a left child            the surviving child

  One child               Delete a node with only The parent is linked to
                          a right child           the surviving child

  Two children            Delete a node with both The inorder-successor
                          children                procedure produces a
                                                  valid BST

  Root                    Delete the root         The new root and all
                                                  links are correct

  Missing key             Delete a key that is    The structure remains
                          not present             unchanged and the
                                                  operation is handled
                                                  safely
  -----------------------------------------------------------------------

For the two-child case, verify that the inorder successor is selected
according to the implementation and that the final BST ordering is
valid.

After every deletion, run the BST validator or independently check the
ordering.

### 5.3 Other BST operations

Test the implemented functions:

-   `findMin` and `findMax`.
-   Successor and predecessor.
-   Range query `[lo, hi]`.
-   `kthSmallest` for the plain BST, if included.
-   BST LCA.

For range queries, include ranges that:

-   Include every key.
-   Include only one key.
-   Include no keys.
-   Exclude the left or right side of the tree.
-   Have lower and upper bounds equal.

For `kthSmallest`, include valid boundary values of `k` and invalid
values if the API defines their handling.

### 5.4 Degeneration case

Deliverable 1 describes a boss-level generator that inserts
`1, 2, 3, …, n`, producing a right-leaning chain.

Test this input pattern at the model level:

1.  Insert ascending keys.
2.  Verify the resulting ordering.
3.  Measure or inspect the resulting height using the documented height
    convention.
4.  Search for a key near the end of the chain.

This test is useful both for correctness and for the BST-versus-AVL
benchmark.

------------------------------------------------------------------------

## 6. AVL Tree tests

AVL tests must verify not only the returned search-tree contents, but
also the AVL metadata after operations.

### 6.1 Four rotation cases

Create one focused test for each insertion rotation case:

  Rotation case   Example insertion order
  --------------- -------------------------
  LL              30, 20, 10
  RR              10, 20, 30
  LR              30, 10, 20
  RL              10, 30, 20

For each case, verify:

-   The final inorder traversal is sorted.
-   The expected root/subtree arrangement is produced.
-   Every node has the correct stored height.
-   Every node has the correct `subtreeSize`.
-   Every node satisfies the AVL balance condition.
-   The validator reports a valid AVL tree.

The examples above are test inputs, not a requirement to expose manual
rotation controls through the GUI.

### 6.2 AVL deletion

Test deletion of:

-   A leaf.
-   A node with one child.
-   A node with two children.
-   The root.
-   A missing key, if supported.

Include deletion cases that require rebalancing. In particular,
Deliverable 1 calls out the case where the heavy child has balance
factor zero.

After each deletion, verify:

-   BST ordering.
-   Stored height equals recomputed height.
-   Stored subtree size equals recomputed subtree size.
-   The absolute balance factor is at most one at every node.
-   The tree remains searchable and its inorder traversal is correct.

### 6.3 Order statistics: `kth` and `rank`

The deliverable requires checking that `kth` and `rank` agree with a
sorted reference after every operation.

Recommended procedure:

1.  Maintain a known set of inserted keys in the test.
2.  Insert or delete one key in the AVL tree.
3.  Build the expected sorted reference sequence in the test using a
    simple test-side method that does not replace the project's data
    structure implementation.
4.  Check every valid `kth(k)` result against the reference sequence.
5.  Check `rank(key)` against the documented definition.
6.  Repeat after every insertion and deletion.

The deliverable defines `rank(key)` as the number of keys smaller than
or equal to the key. Make the test use that definition.

Also test `countInRange(lo, hi)` if it is implemented, using the
documented formula/behavior.

### 6.4 Rotation and metadata validation

The validator should be exercised after every AVL operation in unit
tests. It should check:

-   BST ordering.
-   Correct stored heights.
-   Correct stored subtree sizes.
-   AVL balance at every node.
-   Parent-pointer consistency, if parent pointers are used.
-   No cycles or shared nodes.

------------------------------------------------------------------------

## 7. Max Heap and Heap Sort tests

The heap is array-based. These tests should inspect the underlying array
and active heap size; no tree drawing is needed.

### 7.1 Max Heap property

After each relevant operation, verify that every parent is greater than
or equal to each of its existing children.

For a zero-based array, the project document gives:

-   Parent: `(i - 1) / 2`
-   Left child: `2i + 1`
-   Right child: `2i + 2`

Check only children that are inside the active heap region.

Run this check after:

-   `buildMaxHeap`.
-   Insertion / `siftUp`.
-   `siftDown` or heapify.
-   `extractMax`.

### 7.2 Build Max Heap

The required input cases are:

-   Empty array.
-   One element.
-   Duplicate values.
-   Already-valid heap.
-   Reverse-sorted input.
-   Random input.

Deliverable 1's benchmark plan additionally calls for ordered inputs.
Use controlled test data and verify the heap property after
construction.

### 7.3 Extract maximum

Test:

-   Extracting from a heap with multiple elements.
-   Extracting until one element remains.
-   Extracting the final element.
-   Handling extraction from an empty heap, according to the API's
    defined behavior.

For every successful extraction, verify:

-   The returned value is the maximum of the previous active heap.
-   The active heap size decreases correctly.
-   The remaining active region is still a valid Max Heap.
-   The sorted/extracted region is maintained consistently if the
    implementation tracks it.

### 7.4 Heap Sort

Compare the implementation's output with a trusted sorted reference in
the test.

Required input categories:

-   Empty.
-   Singleton.
-   Duplicate values.
-   Negative values.
-   Already sorted.
-   Reverse sorted.
-   Random values.

Also check that the final result is in ascending order, as required by
the Act 4 final objective.

### 7.5 Active heap and sorted region

Where the implementation tracks both regions during Heap Sort, verify:

-   The active heap size decreases after each extraction.
-   The sorted region grows consistently.
-   The active region satisfies the Max Heap property.
-   The final array is fully sorted.
-   No element is lost or duplicated by swaps/extractions.

------------------------------------------------------------------------

## 8. Validator tests

Deliverable 1 says the validator should run after every operation in
debug mode and in all unit tests.

Test the validator itself, not just the data structures that call it.

It must verify the applicable invariants:

  -----------------------------------------------------------------------
  Structure                           Validator checks
  ----------------------------------- -----------------------------------
  BST                                 All left-subtree keys are smaller;
                                      all right-subtree keys are larger

  AVL                                 BST ordering, stored height, stored
                                      size, balance factor

  Trees with parent pointers          Parent links are consistent

  Tree structures                     No cycles and no shared nodes;
                                      traversal count matches node count

  Max Heap                            Parent-child heap property; active
                                      heap size and sorted region are
                                      consistent
  -----------------------------------------------------------------------

Include at least:

-   A valid structure that should pass.
-   A deliberately invalid structure or controlled test fixture that
    should fail each applicable check.

Do not corrupt production objects in a way that makes the test itself
unsafe. Use a controlled test setup and restore or discard it after the
check.

------------------------------------------------------------------------

## 9. Event generation tests --- without animation

The architecture requires operations to produce events that the GUI can
later animate. The model/event boundary can be tested without rendering
anything.

Test that representative operations produce the correct event types and
payloads, according to the agreed event contract.

Examples from Deliverable 1 include:

  -----------------------------------------------------------------------
  Operation                           Events worth checking
  ----------------------------------- -----------------------------------
  Traversal/search                    `VISIT`, `COMPARE`

  BST insertion                       `COMPARE`, `INSERT`

  BST deletion                        `UNLINK`, and `SWAP_VALUE` for the
                                      two-child case

  AVL balancing                       `ROTATE`, `UPDATE_HEIGHT`,
                                      `UPDATE_SIZE`, `BALANCE_ALERT`
                                      where applicable

  Heap construction                   `HEAP_BUILD`

  Heap swap                           `HEAP_SWAP` with the correct
                                      indices and reason

  Heapify                             `HEAPIFY_START`, `HEAPIFY_DONE`

  Extract maximum                     `EXTRACT_MAX`, `HEAP_SIZE_CHANGE`

  Heap Sort completion                `MARK_SORTED`,
                                      `HEAP_SORT_COMPLETE`, `DONE` as
                                      applicable
  -----------------------------------------------------------------------

Check:

-   The event type uses the exact agreed string.
-   The affected node IDs or array indices are correct.
-   Required old/new values are present where the event contract
    requires them.
-   Event order matches the logical operation.
-   The final model state is correct independently of the event list.

Do **not** test animation timing, easing, drawing, node positions or
visual effects in these tests. Those belong to GUI testing.

The architecture and deliverable describe event-driven communication,
but they do not prescribe a separate test framework or exact event
assertion API. Use the team's finalized event definitions.

------------------------------------------------------------------------

## 10. Save/load and serialization tests --- without GUI

Deliverable 1 specifies:

-   Trees are serialized using preorder with null markers.
-   Heaps are serialized as the underlying array plus current active
    heap size.
-   Campaign progress and stars are saved.
-   The current endless-mode tree/heap and random seed are saved.
-   AVL height and size are recomputed on load rather than stored.
-   The validator runs before the loaded game starts.

Test these behaviors without opening the GUI.

### 10.1 Tree serialization

1.  Create a known tree.
2.  Serialize it using preorder with null markers.
3.  Load/reconstruct it.
4.  Compare the original and reconstructed structures.
5.  Include empty, single-node, skewed and multi-level trees.

### 10.2 Heap serialization

1.  Create a heap with a known array and active heap size.
2.  Serialize the array and active size.
3.  Load it again.
4.  Verify the array values and active size.
5.  Verify the Max Heap property for the active region.

### 10.3 AVL loading

After loading an AVL tree:

-   Recompute heights.
-   Recompute subtree sizes.
-   Run the validator before treating the loaded tree as valid.
-   Verify inorder contents and order-statistic results.

### 10.4 Game save data

Where the save system is implemented, verify that campaign progress,
stars, endless-mode state and random seed are preserved as specified.

The deliverable does not define a detailed file-corruption recovery
policy. Test malformed or incomplete save files according to the
behavior the team agrees to implement; do not assume a particular
recovery mechanism is required by the brief.

------------------------------------------------------------------------

## 11. Game-logic tests that do not need GUI

The game logic layer is separate from rendering. Its rules can therefore
be tested with direct function calls or model-level test fixtures.

Where these systems have been implemented, test:

-   Level success and failure conditions.
-   Objective validation against the underlying data structure.
-   Alarm-step and penalty updates.
-   Score and star calculations.
-   Hint-related star cap behavior.
-   Level reset/retry behavior.
-   Endless-mode score/state updates.
-   Undo/replay state restoration, if the implementation supports model
    snapshots or inverse events.
-   Save/load state restoration.

Use the exact scoring and alarm rules in Deliverable 1 and the team's
finalized gameplay decisions. Do not invent a second scoring formula for
the tests.

For example, the brief states that entering a room or comparing against
a room's key costs one alarm step, and that the alarm capacity is based
on `ceil(1.5 × optimalSteps) + 2`. Tests should use the model's computed
optimal steps, not hand-entered expected values that bypass the model.

------------------------------------------------------------------------

## 12. Benchmarking --- required work

Benchmarking is different from unit testing.

-   **Unit tests** answer: "Is the result correct?"
-   **Benchmarks** answer: "How does execution cost change as input size
    and input pattern change?"

Deliverable 1, Section 10, explicitly requires benchmarks to run:

-   Without the GUI.
-   Without event recording.
-   With a flag that disables events.

The benchmark should time the data-structure algorithms themselves, not
SFML rendering, animation playback or event construction.

### 12.1 Benchmark harness requirements

Create a separate console benchmark executable or benchmark mode that:

1.  Creates the input data.
2.  Constructs or resets the relevant structure.
3.  Enables the no-event-recording mode.
4.  Starts timing immediately before the operation being measured.
5.  Executes the operation.
6.  Stops timing immediately after the operation.
7.  Verifies the result outside the timed section where possible.
8.  Records the input size, input pattern, measured time and relevant
    operation counts.

Use the project's existing language/library setup. The project documents
do not prescribe a specific timing library or benchmark framework.

**Important:** Do not include GUI startup, window creation, drawing,
animation delays, file I/O or event recording in the measured algorithm
time.

### 12.2 Event recording must be disabled

Provide a flag or equivalent agreed configuration that prevents event
creation/recording during benchmarks.

Verify that:

-   The benchmark actually runs with event recording disabled.
-   The normal application can still run with its usual event behavior.
-   Disabling events does not change the algorithm's final result.
-   The benchmark does not accidentally measure event allocation or
    event-log growth.

Do not remove event support from the normal application just to make
benchmarking easier.

------------------------------------------------------------------------

## 13. Exact benchmark experiments from Deliverable 1, Section 10

### Experiment A --- BST versus AVL height

  Variable         Required values
  ---------------- --------------------
  Input size `n`   100, 1,000, 10,000
  Input pattern    Sorted and random
  Structures       BST and AVL
  Metric           Final height

Procedure:

1.  Generate a sorted sequence of `n` unique keys.
2.  Insert it into a fresh BST and record its final height.
3.  Insert the same sequence into a fresh AVL tree and record its final
    height.
4.  Repeat with a random sequence of `n` unique keys.
5.  Record the results for every size and pattern.

Use the same keys and insertion order for the BST and AVL comparison.
Clearly state whether height is measured in edges or nodes.

**What to explain in the report:** The effect of sorted insertion on an
ordinary BST compared with AVL balancing, and how the measured heights
relate to the expected complexity.

### Experiment B --- Search cost

  Variable          Required values
  ----------------- -------------------------------------------------------
  Trees             The same BST and AVL trees from the height comparison
  Search workload   10,000 random searches
  Metrics           Average comparisons and elapsed time

Procedure:

1.  Use the same constructed BST and AVL datasets.
2.  Generate one search workload of 10,000 keys.
3.  Run that same workload against both trees.
4.  Count key comparisons.
5.  Measure total search time.
6.  Calculate average comparisons per search.

State how the search keys were generated and whether they include
successful searches, unsuccessful searches, or both. Keep the workload
identical for the two structures.

### Experiment C --- AVL rotation count

  -----------------------------------------------------------------------
  Variable                            Required values
  ----------------------------------- -----------------------------------
  Operations                          Insert and delete

  Input pattern                       Random and sorted

  Metrics                             Average rotations per operation;
                                      maximum rotations per delete
  -----------------------------------------------------------------------

Procedure:

1.  Run AVL insertion workloads using random and sorted key orders.
2.  Count rotations performed during insertion.
3.  Run deletion workloads on populated AVL trees.
4.  Count rotations performed during deletion.
5.  Report average rotations per operation.
6.  Report the maximum rotations observed for a single deletion.

Define clearly whether a double rotation is counted as one rotation case
or as two primitive rotations. Use the same counting convention
throughout the report.

### Experiment D --- Order statistics

  Comparison               Metric
  ------------------------ ------------------------
  AVL `kth`                Time for 1,000 queries
  Plain BST inorder scan   Time for 1,000 queries

Procedure:

1.  Prepare the same key set in an AVL tree and a plain BST.
2.  Prepare the same 1,000 `k` queries for both approaches.
3.  For AVL, use the subtree-size-based `kth` operation.
4.  For the plain BST, use the inorder-counter scan described in the
    project.
5.  Verify that both approaches return the same answer for each query.
6.  Record total time for the 1,000 queries.

Do not compare different query sets. The reference BST method is the
plain inorder scan, not an AVL implementation.

### Experiment E --- Heap construction

  Variable         Required values
  ---------------- ---------------------------------------------------
  Input size `n`   100, 1,000, 10,000
  Input pattern    Random and ordered
  Operation        Bottom-up `buildMaxHeap`
  Metrics          Build time; confirm `O(n)` bottom-up construction

Procedure:

1.  Generate arrays of each required size.
2.  Prepare random and ordered input patterns.
3.  Run bottom-up `buildMaxHeap` on each fresh array.
4.  Record build time.
5.  Verify the Max Heap property after construction.

Explain why bottom-up heap construction is `O(n)`. Do not confuse it
with repeatedly inserting every item into a heap.

### Experiment F --- Heap Sort

  Variable         Required values
  ---------------- ---------------------------------------------------
  Input size       Use the same controlled sizes: 100, 1,000, 10,000
  Input patterns   Random, sorted, reverse-sorted, duplicate-heavy
  Metrics          Execution time, comparisons and swaps

Procedure:

1.  Generate each input pattern at each selected size.
2.  Run the project's Heap Sort implementation.
3.  Record elapsed time, comparisons and swaps.
4.  Verify the output is in ascending order and contains the same
    elements as the input.
5.  Explain the expected `O(n log n)` time complexity.

The project brief specifies the input patterns and metrics; using 100,
1,000 and 10,000 here keeps the Heap Sort experiment aligned with the
other stated size series. If the team chooses additional sizes, label
them as additional experiments rather than replacing the required
patterns.

### Experiment G --- Sorting comparison

Deliverable 1 asks for Heap Sort to be compared with other sorting
algorithms covered in the course.

For each course-covered sorting algorithm that the team actually
implements or is permitted to use for the comparison, report:

-   Runtime.
-   Number of swaps.
-   Number of comparisons.
-   Auxiliary space.

Use identical input arrays and input patterns for all algorithms. Do not
silently compare different data or different input sizes.

The brief does not name the course sorting algorithms. The team must
select the algorithms that were actually covered in its course; do not
assume a particular list.

------------------------------------------------------------------------

## 14. Benchmark methodology and result quality

The following are recommended controls to make the measurements
credible. They are practical methodology recommendations, not extra
numeric requirements stated in Deliverable 1.

-   Run each experiment more than once and report the number of runs.
-   Use fresh structures/arrays for independent runs.
-   Use the same generated input for algorithms being compared.
-   Keep data generation outside the timed section unless data
    generation itself is the subject of the benchmark.
-   Keep event recording disabled for every benchmark run.
-   Do not include console printing inside the timed section.
-   Avoid running unrelated heavy applications during measurement.
-   Report the machine/compiler/build configuration used.
-   Use the same compiler settings for algorithms being compared.
-   Report the timing unit, such as microseconds or milliseconds.
-   If repeated runs vary, report the aggregation method (for example,
    average or median) and use it consistently.
-   Keep correctness checks outside the timed section where possible,
    but always perform them.

Do not present a single timing measurement as definitive evidence.
Include the measurement method and enough context for another person to
understand or repeat the experiment.

------------------------------------------------------------------------

## 15. Suggested benchmark result tables

Use tables like these in the benchmark output and final report.

### 15.1 BST versus AVL height

         n Input pattern     BST height   AVL height
  -------- --------------- ------------ ------------
       100 Sorted                       
       100 Random                       
     1,000 Sorted                       
     1,000 Random                       
    10,000 Sorted                       
    10,000 Random                       

### 15.2 Search cost

  --------------------------------------------------------------------------
             n Input     Structure      Number of       Average   Total time
               pattern                   searches   comparisons 
  ------------ --------- ----------- ------------ ------------- ------------
                         BST               10,000               

                         AVL               10,000               
  --------------------------------------------------------------------------

### 15.3 AVL rotations

  -------------------------------------------------------------------------------------------
  Operation   Input pattern             n       Total   Operations        Average     Maximum
                                            rotations    performed   rotations/op   rotations
                                                                                       in one
                                                                                       delete
  ----------- ---------------- ---------- ----------- ------------ -------------- -----------
  Insert      Sorted                                                                      N/A

  Insert      Random                                                                      N/A

  Delete      Sorted-derived                                                      
              tree                                                                

  Delete      Random-derived                                                      
              tree                                                                
  -------------------------------------------------------------------------------------------

### 15.4 Order statistics

  ----------------------------------------------------------------------------
  Structure/method      Number of queries           Total time Results match
                                                               reference?
  ------------------ -------------------- -------------------- ---------------
  AVL `kth`                         1,000                      

  BST inorder scan                  1,000                      
  ----------------------------------------------------------------------------

### 15.5 Heap construction

         n Input pattern     Build time Max Heap validator passes?
  -------- --------------- ------------ ----------------------------
       100 Random                       
       100 Ordered                      
     1,000 Random                       
     1,000 Ordered                      
    10,000 Random                       
    10,000 Ordered                      

### 15.6 Heap Sort

  --------------------------------------------------------------------------------
             n Input pattern        Execution   Comparisons        Swaps Sorted
                                         time                            output
                                                                         valid?
  ------------ ----------------- ------------ ------------- ------------ ---------
           100 Random                                                    

           100 Sorted                                                    

           100 Reverse-sorted                                            

           100 Duplicate-heavy                                           

         1,000 Random                                                    

         1,000 Sorted                                                    

         1,000 Reverse-sorted                                            

         1,000 Duplicate-heavy                                           

        10,000 Random                                                    

        10,000 Sorted                                                    

        10,000 Reverse-sorted                                            

        10,000 Duplicate-heavy                                           
  --------------------------------------------------------------------------------

### 15.7 Sorting comparison

  -----------------------------------------------------------------------------------
  Algorithm             n Input         Runtime   Comparisons       Swaps Auxiliary
                          pattern                                         space
  ----------- ----------- --------- ----------- ------------- ----------- -----------
  Heap Sort                                                               

  Course                                                                  
  algorithm 1                                                             

  Course                                                                  
  algorithm 2                                                             
  -----------------------------------------------------------------------------------

Fill tables with measured results only. Never invent or estimate results
and present them as actual measurements.

------------------------------------------------------------------------

## 16. What to include in the test report

Deliverable 1 requires a unit test suite and a test report. The report
should make it possible to see what was tested and whether it passed.

Recommended structure:

1.  **Test environment**
    -   Compiler and version.
    -   Build configuration.
    -   Operating system.
    -   Test command or run instructions.
2.  **Test scope**
    -   Binary Tree.
    -   BST.
    -   AVL.
    -   Order statistics.
    -   Max Heap.
    -   Heap Sort.
    -   Validators.
    -   Serialization and relevant model/game logic.
3.  **Test cases and results**
    -   Test case name.
    -   Input/setup.
    -   Expected behavior.
    -   Actual result.
    -   Pass/fail status.
4.  **Coverage summary**
    -   Which required operations were tested.
    -   Which edge cases were tested.
    -   Any known untested behavior.
5.  **Defects found and fixes**
    -   Brief description.
    -   Correction made.
    -   Regression test added.
6.  **Benchmark results**
    -   Experimental setup.
    -   Result tables.
    -   Complexity explanation.
    -   Interpretation of BST versus AVL and Heap Sort behavior.

Do not claim that every case passed unless the test output confirms it.
Keep failed tests and unresolved issues visible until they are fixed.

------------------------------------------------------------------------

## 17. Work division for the two members

The roadmap assigns the testing work as follows.

  -----------------------------------------------------------------------
  Member A --- Tree Systems           Member B --- AVL, Heap and Game
                                      Interface
  ----------------------------------- -----------------------------------
  Binary Tree unit tests              AVL unit tests

  BST unit tests                      Max Heap unit tests

  Act 1--2 GUI testing (separate GUI  Act 3--4 GUI testing (separate GUI
  task)                               task)

  Tree performance tests              Heap performance tests

  Tree documentation                  AVL and Heap documentation
  -----------------------------------------------------------------------

Shared responsibilities:

-   Test the complete campaign.
-   Fix integration bugs.
-   Verify undo/replay and save/load.
-   Prepare the report.
-   Capture AVL rotation screenshots and heap demonstrations.
-   Prepare the demonstration and viva.

The roadmap also says to assign one owner to each shared module to avoid
two people editing the same files at once. Keep the existing module
ownership and interfaces stable.

------------------------------------------------------------------------

## 18. GUI testing is separate

The following checks are required by Deliverable 1, but they are **not**
GUI-independent tests:

-   Every level, including Act 4 levels 4.1--4.5, can be completed,
    failed and retried.
-   Zoom, pan and fit work on a 200-node tree / large heap without frame
    drops (target 60 FPS).
-   Animation does not leave nodes or array cells at stale positions
    after undo.
-   Heap tree view and array view remain synchronized throughout Heap
    Sort.

The roadmap additionally assigns manual GUI testing for Acts 1--2 and
Acts 3--4 to the respective members.

The written report also needs GUI screenshots, all four AVL rotation
screenshots, and demonstrations of heap construction, extraction,
heapify and the final sorted result. These require the running
application and should not be counted as headless test evidence.

------------------------------------------------------------------------

## 19. Final checklist

### GUI-independent tests

-   [ ] Binary Tree traversal tests cover empty, single-node, skewed and
    full trees.
-   [ ] Recursive and iterative traversals agree where both exist.
-   [ ] Reconstruction round-trip tests cover preorder/inorder and
    postorder/inorder.
-   [ ] Binary Tree height, diameter, LCA, path, mirror and comparison
    operations are tested where implemented.
-   [ ] BST insertion, search, duplicate rejection and all deletion
    cases are tested.
-   [ ] BST min/max, successor/predecessor and range query are tested.
-   [ ] AVL LL, RR, LR and RL cases are tested.
-   [ ] AVL deletion and the heavy-child-balance-zero case are tested.
-   [ ] AVL heights, balance factors and subtree sizes are validated.
-   [ ] `kth` and `rank` agree with a sorted reference after operations.
-   [ ] Max Heap build, insertion, sift operations and extraction are
    tested.
-   [ ] Heap tests include empty, singleton, duplicate, already-heap,
    reverse-sorted and random inputs.
-   [ ] Heap Sort tests include empty, singleton, duplicates, negative,
    sorted, reverse-sorted and random inputs.
-   [ ] Validators are tested on valid and invalid structures.
-   [ ] Representative operation events and payloads are checked without
    running animations.
-   [ ] Tree and heap serialization round trips are tested.
-   [ ] AVL metadata is recomputed and validated after loading.
-   [ ] Relevant game logic can be tested without rendering.

### Benchmarking

-   [ ] Benchmark mode runs without the GUI.
-   [ ] Event recording is disabled through a flag or equivalent
    configuration.
-   [ ] BST/AVL height is measured for n = 100, 1,000, 10,000 with
    sorted and random inputs.
-   [ ] Search uses the same trees and 10,000 random searches; average
    comparisons and time are recorded.
-   [ ] AVL insert/delete rotation counts are recorded for random and
    sorted inputs.
-   [ ] AVL `kth` is compared with a BST inorder scan for 1,000 queries.
-   [ ] Bottom-up heap construction is measured for n = 100, 1,000,
    10,000 with random and ordered inputs.
-   [ ] Heap Sort is measured for random, sorted, reverse-sorted and
    duplicate-heavy inputs.
-   [ ] Heap Sort time, comparisons and swaps are recorded.
-   [ ] Heap Sort is compared with course-covered sorting algorithms.
-   [ ] Sorting comparison includes runtime, swaps/comparisons and
    auxiliary space.
-   [ ] Every benchmark result is correctness-checked.
-   [ ] The report explains the observed results and the relevant
    complexities.

### Separate GUI and submission evidence

-   [ ] Manual GUI checklist is completed.
-   [ ] AVL rotation screenshots are captured.
-   [ ] Heap construction, extraction, heapify and final sorted-result
    demonstrations are captured.
-   [ ] Benchmark tables contain actual measured results.
-   [ ] Unit test report is prepared.
-   [ ] Both members can explain the implementation and results for the
    viva.

------------------------------------------------------------------------

## 20. Important implementation constraints

While implementing the tests and benchmark harness:

-   Keep data structures independent of SFML and GUI rendering.
-   Implement required structures and algorithms manually.
-   Do not use STL containers or built-in tree/heap implementations.
-   Follow the project's structs-for-data and classes-for-functionality
    convention.
-   Keep declarations in headers and implementations in CPP files
    according to the existing architecture.
-   Do not introduce enums; use the project's standardized
    string-literal conventions.
-   Do not add a new library or change shared interfaces without team
    agreement.
-   Keep tests simple and understandable so both members can explain
    them during the viva.

**Bottom line:** Complete model correctness testing first without the
GUI. Then run the benchmark experiments with event recording disabled.
Finally, perform the separate manual GUI checks and capture the visual
evidence required by the deliverable.

------------------------------------------------------------------------

## 21. Implemented headless runners

The repository provides two independent C++ entry points. Compile and run
them from the repository root in PowerShell; no batch file or SFML
libraries are involved:

```powershell
g++ -std=c++17 testmain.cpp game/logic/bst/bst.cpp game/logic/avl/avl_tree.cpp game/logic/heap/max_heap.cpp game/logic/event.cpp -o test_runner.exe
.\test_runner.exe

g++ -std=c++17 -O2 benchmarkmain.cpp benchmark.cpp game/logic/bst/bst.cpp game/logic/avl/avl_tree.cpp game/logic/heap/max_heap.cpp game/logic/event.cpp '-Wl,--stack,16777216' -o benchmark_runner.exe
.\benchmark_runner.exe
```

`testmain.cpp` runs GUI-independent checks against BinaryTree, BST, AVL
and MaxHeap. It writes the same pass/fail summary shown in the console to
`test_results.txt`. The runner exits with status 1 when any check fails.

`benchmarkmain.cpp` runs the suite implemented in `benchmark.cpp`. Event
recording is disabled by passing null event-log pointers. The report
contains all seven Section 10 experiments: validated BST/AVL heights and
searches, AVL rotation counts, order statistics, bottom-up heap
construction, project Heap Sort, and the sorting comparison already
included in the benchmark source. Height is independently recomputed,
node counts are checked, and AVL validation runs after each insertion.

Timing uses the median of three runs. Inputs are generated and correctness
checks are performed outside timed regions. Search comparison counts are
collected in a separate pass, with one key-to-node comparison counted per
visited node. Heap Sort comparison and swap counters are collected in a
separate pass while event recording remains disabled. The benchmark exits
nonzero if any validation fails and writes `benchmark_results.txt` with
the measured results.
