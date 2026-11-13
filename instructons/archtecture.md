# Vault Heist – System Architecture

## 1. Architecture Overview

Vault Heist uses a **Layered Architecture with Event-Driven Communication**.

The project is divided into separate modules, each responsible for a specific part of the game. Data structures remain independent of the GUI, making them easier to test and maintain.

## 2. Project Structure

```text
VaultHeist/
│
├── src/
│   ├── main
│   ├── structures/
│   ├── game/
│   ├── events/
│   ├── gui/
│   └── utils/
│
├── tests/
├── levels/
├── README.md
└── architecture.md
```

## 3. Module Responsibilities

| Module | Responsibility |
|---|---|
| structures | Implements Binary Tree, BST, AVL Tree and Max Heap. |
| game | Handles game logic, levels, objectives and scoring. |
| events | Records data structure operations for animation, undo and replay. |
| gui | Handles game screens and visual representation. |
| utils | Provides saving, loading and data structure validation. |
| tests | Contains unit tests for data structures and algorithms. |
| levels | Stores level configurations and game data. |

## 4. System Workflow

The application follows this workflow:

```text
User Input
    |
    v
    GUI
    |
    v
 GameEngine
    |
    v
Data Structures
    |
    v
 EventManager
    |
    v
  Renderer
    |
    v
Visual Animation
```

1. The user performs an operation through the GUI.
2. GameEngine processes the request.
3. The relevant data structure performs the operation.
4. EventManager records the generated events.
5. Renderer uses these events to update the visualization.
6. The game checks the level objectives and updates the score.

## 5. Event-Driven Communication

Every major data structure operation generates events describing what happened.

Examples:
- `INSERT`
- `DELETE`
- `COMPARE`
- `ROTATE`
- `SWAP`
- `HEAPIFY`

These events are used by the GUI to animate operations and support undo and replay.

The data structures do not directly interact with the GUI.

## 6. Design Principles

- **Separation of Concerns:** Each module has a specific responsibility.
- **Independence:** Data structures work independently of the GUI.
- **Reusability:** The same visualization components can be reused across different game acts.
- **Testability:** Data structures and algorithms can be tested without running the GUI.
- **Maintainability:** Changes in one module should have minimal impact on other modules.

## 7. Architecture Summary

Vault Heist follows a simple layered architecture where game logic, data structures, events and GUI are separated into independent modules.

Event-driven communication connects data structure operations with their visual representation, allowing the game to support animations, undo and replay.