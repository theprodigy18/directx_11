# directx_11

# DropOut Coding Style Guide

This document defines the official code style for the **Drop Engine** project. The goal is to maintain clarity, consistency, and structure across the entire codebase — whether you are working solo or collaborating with others in the future.

---

## 📁 File & Folder Naming

- Use `snake_case` for all file and folder names.
- Avoid spaces, capital letters, or special characters.
- **Examples:**
  - Folders: `core_system/`, `asset_loader/`
  - Files: `input_manager.cpp`, `texture_utils.hpp`

---

## 🧱 Namespaces

- Use **all lowercase** for namespace names.
- Each namespace should be a single word, even when nested.
- **Example:**

```cpp
namespace core::input 
{
    void HandleEvent();
}
```

---

## 🔷 Class, Struct, and Enum Naming

- Use `PascalCase` for class, struct, and enum type names.
- **Examples:**

```cpp
class GameManager;
struct PlayerStats;
enum LogLevel;
```

---

## 🟩 Enum Values & Constants

- Use **all uppercase with underscores** (`UPPER_CASE_WITH_UNDERSCORES`).
- **Examples:**

```cpp
enum LogLevel 
{
  LOG_INFO,
  LOG_WARNING,
  LOG_ERROR
};

const int MAX_PLAYER_COUNT {4};
constexpr int MAX_TILE {1000};
```

---

## 🟡 Variable Naming

- Use `camelCase` for local variables and function parameters.
- **Examples:**

```cpp
int playerScore {0};
float deltaTime {0.f};
bool isVisible {false};
```

---

## 🟠 Global Variables

- Prefix global variables with `g_` and use `camelCase`.
- **Examples:**

```cpp
bool g_gameRunning {false};
InputManager* g_inputHandler {nullptr};
```

---

## 🔴 Private Members

- Prefix private class/struct members with `_` and use `camelCase`.
- **Example:**

```cpp
class Renderer 
{
private:
  int _frameCount {0};
  bool _vsyncEnabled {false};
};
```

---

## 🔵 Function Naming

- Use `PascalCase` for all function names, including global and member functions.
- **Examples:**

```cpp
void Initialize();
int CalculateScore(int base, int multiplier);
```

---

## 🧱 Initialization Style

- Use **brace initialization (`{}`)** instead of `=` for initializing variables and objects.
- This helps avoid narrowing conversions and ensures uniform syntax.
- **Examples:**

```cpp
int count {5};
const char* name {"Player"};
Vec2 position {0.f, 0.f};
```

---

## 💡 Additional Notes

- Avoid excessive abbreviations unless they are widely recognized (e.g., `pos`, `dir`, `vec`).
- Prefer meaningful and descriptive names over vague ones (avoid `temp`, `foo`, `data`).
- Be consistent with pointer/reference formatting (e.g., prefer either `Type* ptr` or `Type *ptr` — pick one and stick to it).
- Ideally, one header file defines one main concept (class/struct/etc.) unless they are tightly related.

---

> Following this style guide helps ensure that the Drop Engine codebase remains clean, readable, and maintainable — for both yourself and any future contributors.
