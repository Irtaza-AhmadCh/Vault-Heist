Vault Heist -- Coding Guardrails
This document defines the coding rules to follow when implementing
Vault Heist: The Living Vault.

These rules take priority over personal coding preferences that conflict
with the project's requirements.
1. General Coding Style
- Keep code simple, straightforward, and beginner-friendly.
- Avoid over-engineering and unnecessary abstractions.
- Prefer clear loops, conditions, and direct logic.
- Use understandable variable and function names.
- Keep implementations easy to explain during the project demo and
  viva.
- Preserve the existing project structure and implementation decisions
  when modifying code.
2. Classes and Structs
- Classes are allowed and will be used.
- Use simple classes mainly to organize related functions and data.
- Implement the actual data structures using struct.
- Do not introduce complicated object-oriented patterns or unnecessary
  inheritance.
3. File Organization
- Do not create separate header files (.h or .hpp).
- Put implementations in .cpp files.
- Keep code organized according to the agreed project folder
  structure.
4. GUI
- Use SFML (Simple and Fast Multimedia Library) for the GUI.
- Keep GUI code separate from the core data structure implementations.
- The GUI should interact with the game logic without taking over the
  data structure implementations.
5. STL Restrictions
STL containers are completely prohibited.
- Do not use std::vector or other STL containers.
- Do not include STL container headers.
- Do not use built-in data structures as replacements for the
  structures required by the project.
- Implement the required structures manually using structs, arrays,
  pointers, and dynamic memory where appropriate.
Do not assume that a library or feature is permitted just because it is
part of the C++ standard library. If its use is unclear, ask before
using it.
6. Comments
- Avoid comments as much as possible.
- Add a comment only when it is genuinely necessary.
- If a comment is needed, keep it short and clear.
- Prefer readable code over explanatory comment blocks.
7. C++ Output and Formatting
- Use endl instead of \n for output line breaks.
- Keep cout prompts short, plain, and natural.
- Avoid unnecessarily formal or polished console messages.
- Keep formatting simple and consistent.
8. Other Coding Preferences
- Avoid global variables.
- Avoid unnecessary generic logic and helper frameworks.
- Prefer direct, understandable functions.
- Use pointers and dynamic arrays where appropriate for manual data
  structure implementation.
- Do not use try-catch, templates, operator overloading, or other
  restricted features from earlier coursework unless the project
  requirements explicitly allow them and their use is agreed upon.
9. Data Structure Implementation
Implement the project's required data structures from scratch:
- Binary Tree
- Binary Search Tree (BST)
- AVL Tree
- Max Heap
Use structs for their nodes and internal structure. Do not substitute
built-in tree, heap, priority queue, or container implementations.

Keep operations such as insertion, deletion, searching, traversal,
balancing, heapifying, and extraction understandable and independently
testable.
10. Implementation Principles
- Keep data structure logic separate from game logic.
- Keep game logic separate from SFML rendering and input handling.
- Use the agreed event system to communicate operations that the GUI
  needs to display or animate.
- Do not add extra layers, frameworks, or dependencies without a clear
  need.
- Make implementation choices that the team can understand, maintain,
  test, and explain.
11. Rule for Unclear Cases
If a proposed implementation conflicts with these guardrails, or if a
library, language feature, or shortcut may violate them, do not silently
use it. Explain the choice and confirm before proceeding.
Core principle: Implement the required functionality manually, keep
the code simple, and make every part understandable to the team.