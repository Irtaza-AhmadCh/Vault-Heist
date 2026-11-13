# Vault Heist — Coding Guardrails

This document defines the coding rules, architectural constraints, and implementation conventions for **Vault Heist: The Living Vault**.

These rules take priority over personal coding preferences that conflict with the project's requirements and agreed architecture.

## 1. General Coding Style

- Keep code simple, straightforward, and beginner-friendly.
- Avoid over-engineering and unnecessary abstractions.
- Prefer clear loops, conditions, and direct logic.
- Use understandable variable and function names.
- Keep implementations easy to explain during the project demo and viva.
- Preserve the existing project architecture, folder structure, naming conventions, and implementation decisions.
- Do not unnecessarily rewrite or replace existing working code.
- Follow the agreed team responsibilities and implementation division.

## 2. Classes and Structs

Both classes and structs are allowed, but they have different responsibilities.

### Structs — Data Models

- Use structs to define the actual models and internal representation of data structures.
- Structs should hold related data fields, pointers, and necessary attributes.
- Use structs for nodes, tree elements, heap elements, and other data structure components.
- Keep structs focused on representing data rather than managing complicated operations.

### Classes — Functionality and Operations

- Use classes to organize related functions and operations.
- Classes should manage the functionality of their respective data structures and other components.
- Keep functions logically grouped within their relevant classes.
- Avoid unnecessary inheritance, complicated OOP patterns, and excessive abstraction.
- Do not create classes simply to wrap a single unnecessary function.

**Core distinction:**

- Structs define how data is represented.
- Classes organize and implement the operations that work on that data.

## 3. Enums and String Literal Conventions

Enums are completely prohibited.

Whenever an enum would normally be used, replace it with standardized string literals.

Follow these conventions:

- Use lowercase `snake_case` for all string-based identifiers.
- Use consistent and predefined string values throughout the project.
- Never create multiple spellings or variations for the same logical value.
- Use exact string comparisons when checking values.
- Do not introduce arbitrary strings throughout the code.
- Define accepted string values centrally within the relevant component.
- Use dedicated conversion functions when converting between external representations and internal string values.
- Invalid or unsupported values must be handled explicitly.
- Avoid repeatedly converting between different representations without a clear requirement.

### Example

Instead of:

```cpp
enum GameState
{
    MENU,
    PLAYING,
    PAUSED,
    GAME_OVER
};
```

Use:

```cpp
const char* state = "menu";
```

Accepted values:

- `"menu"`
- `"playing"`
- `"paused"`
- `"game_over"`

For comparisons:

```cpp
if (strcmp(state, "playing") == 0)
{
    // operation
}
```

For conversions:

```cpp
const char* getStateName(int state)
{
    if (state == 0)
        return "menu";

    if (state == 1)
        return "playing";

    if (state == 2)
        return "paused";

    return "game_over";
}
```

Use conversion functions with meaningful names and consistent mappings. Do not introduce arbitrary numeric or string mappings across different files.

## 4. File Organization — Header and CPP Files

The project will use both `.h` and `.cpp` files.

The choice of file depends on whether a component needs to be accessed by other modules or is only required internally.

### A. Header Files (.h)

Header files are primarily used for reusable components that need to be accessed by other files.

They should contain:

- Struct definitions and data models.
- Class declarations.
- Function prototypes.
- Shared constants and predefined string identifiers.
- Necessary interfaces required by other components.
- Declarations of core data structure operations.

For example, data structure headers should expose the required node structures and class interfaces so game logic and other components can use them.

Header files should not contain unnecessary implementation details.

### B. CPP Files (.cpp)

CPP files contain the actual implementations and functionality.

They should contain:

- Function definitions.
- Data structure algorithms.
- Insertion and deletion implementations.
- Searching and traversal implementations.
- Balancing and heapifying operations.
- Game management and gameplay logic.
- GUI rendering and input handling.
- Internal helper functions that are not required by other files.
- Implementation-specific classes that do not need to be shared.

### C. Header and CPP Separation

Follow this general structure:

| Header file (.h) | Implementation file (.cpp) |
|---|---|
| Struct definitions | Actual algorithms |
| Class declarations | Function definitions |
| Function prototypes | Internal helper functions |
| Shared constants | Implementation details |
| Shared interfaces | Component-specific logic |

**Important rules:**

- If a component is required by multiple files, provide an appropriate header interface.
- If a component is only needed internally, keep it primarily in its CPP file.
- Do not create unnecessary header files for components that do not need to be shared.
- Do not place every implementation in a header file.
- Keep function implementations in CPP files by default.
- Inline implementations in headers are allowed only when there is a genuine technical reason.
- Avoid circular header dependencies.
- Use include guards or `#pragma once` to prevent repeated header inclusion.
- Include only the headers actually required by a file.
- Do not duplicate declarations or implementations unnecessarily.
- Preserve the agreed folder structure.

### D. Example Project Organization

The following is an illustrative structure. Follow the existing agreed architecture if the team has already defined a different organization.

```text
VaultHeist/
|
|-- DataStructures/
|   |-- BinaryTree.h
|   |-- BinaryTree.cpp
|   |-- BST.h
|   |-- BST.cpp
|   |-- AVLTree.h
|   |-- AVLTree.cpp
|   |-- MaxHeap.h
|   |-- MaxHeap.cpp
|
|-- Game/
|   |-- GameManager.cpp
|   |-- GameLogic.cpp
|   |-- EventSystem.cpp
|
|-- GUI/
|   |-- Renderer.cpp
|   |-- InputHandler.cpp
|   |-- AnimationManager.cpp
|
|-- main.cpp
```

## 5. GUI — SFML

- Use SFML (Simple and Fast Multimedia Library) for the GUI.
- Keep GUI code separate from core data structure implementations.
- GUI components should handle rendering, input, and visual interactions.
- The GUI must communicate with game logic through the agreed event system.
- Do not implement data structure operations directly inside GUI code.
- GUI components should display and animate the results of operations without taking responsibility for their implementation.
- Keep SFML-specific dependencies out of core data structure implementations wherever possible.

## 6. STL Restrictions

STL containers are completely prohibited.

- Do not use `std::vector` or any other STL containers.
- Do not include STL container headers.
- Do not use built-in data structures as replacements for the required implementations.
- Implement required structures manually using structs, arrays, pointers, and dynamic memory where appropriate.
- Do not use built-in trees, heaps, priority queues, stacks, or queues as substitutes for required structures.
- Do not assume a standard library feature is permitted simply because it is part of C++.
- If the use of a library or feature is unclear, confirm before using it.

## 7. Comments

- Avoid comments as much as possible.
- Add comments only when genuinely necessary.
- Keep necessary comments short and clear.
- Prefer readable code and meaningful names over explanatory comment blocks.

## 8. C++ Output and Formatting

- Use `endl` instead of `\n` for output line breaks.
- Keep `cout` prompts short, plain, and natural.
- Avoid unnecessarily formal or polished console messages.
- Keep formatting simple and consistent.

## 9. Other Coding Restrictions

Unless explicitly required by the project and agreed upon, do not use:

- Global variables.
- Templates.
- Operator overloading.
- Try-catch blocks.
- STL containers.
- Unnecessary generic helper frameworks.
- Complicated abstractions or design patterns.

Additional rules:

- Prefer direct and understandable functions.
- Use pointers and dynamic arrays where appropriate.
- Avoid unnecessary duplication, but do not introduce complicated generic solutions just to reduce a few lines of code.
- Keep variable scope limited to where it is needed.
- Avoid unnecessary dependencies between unrelated components.

## 10. Data Structure Implementation

All required data structures must be implemented from scratch.

Required structures:

1. Binary Tree
2. Binary Search Tree (BST)
3. AVL Tree
4. Max Heap

### Implementation requirements

- Use structs for nodes and internal data models.
- Use classes to organize operations and functionality.
- Declare reusable data structure interfaces in header files.
- Implement the actual algorithms in their corresponding CPP files.
- Implement all required operations manually.
- Do not substitute built-in implementations.
- Keep insertion, deletion, searching, traversal, balancing, heapifying, and extraction logic understandable.
- Make operations independently testable.
- Use pointers and dynamic memory where appropriate.
- Handle empty structures, invalid operations, and memory management properly.
- Ensure that data structure operations do not depend on SFML or GUI rendering.

## 11. Implementation Separation

Maintain three distinct responsibilities.

### A. Data Structure Layer

- Defines data models using structs.
- Declares reusable class interfaces in header files.
- Implements data structure operations in CPP files.
- Handles insertion, deletion, searching, traversal, balancing, and other required operations.
- Does not contain game management or GUI rendering logic.

### B. Game Logic Layer

- Controls game rules and gameplay.
- Coordinates data structure operations.
- Handles game state and game-related decisions.
- Manages interactions between different game components.
- Uses the required data structure interfaces.
- Does not take responsibility for GUI rendering.

### C. GUI Layer

- Uses SFML for rendering and input handling.
- Displays game states and data structure changes.
- Communicates with game logic through the agreed event system.
- Handles visual animations independently of the underlying data structure implementation.
- Does not directly modify the internal implementation of data structures.

## 12. Event and Communication Conventions

- Use the already agreed event system for communication between game logic and GUI.
- Keep event definitions and naming consistent.
- Use standardized string literals instead of enums for event types and states.
- Define accepted event names centrally within the relevant component.
- Avoid arbitrary event strings in different files.
- Define conversion functions whenever an external event representation needs to be translated into an internal value.
- Keep event interfaces accessible to the components that require them.
- Do not introduce additional communication layers without a clear requirement.

## 13. Implementation and Modification Rules

- Follow the existing architecture and agreed work division.
- Do not change existing naming conventions without a valid reason.
- Do not modify another team member's implementation unnecessarily.
- Do not introduce new dependencies without approval.
- Do not change interfaces or function signatures that other components depend on without coordinating first.
- Ensure newly implemented functionality integrates with the existing codebase.
- Test individual functionality before integrating it with other modules.
- Avoid rewriting existing implementations when a small modification is sufficient.
- Keep shared interfaces stable to minimize integration conflicts.
- Ensure header declarations match their corresponding CPP implementations.

## 14. Rule for Unclear Cases

If an implementation decision:

- Conflicts with these guardrails.
- Introduces an unapproved library or language feature.
- Changes the existing architecture.
- Alters an agreed interface or implementation decision.
- Introduces an alternative to a required manual data structure.
- Requires an unusual header-only implementation.
- Introduces unnecessary dependencies or abstractions.

Do not silently proceed.

Explain the proposed change and obtain confirmation before implementing it.

---

## Core Principle

**Structs define the data. Classes organize the functionality. Header files expose reusable interfaces. CPP files contain implementations. String literals replace enums using predefined conventions. SFML handles the GUI. All required data structures are implemented manually.**

Keep the code simple, maintainable, consistent with the agreed architecture, and understandable by every team member.