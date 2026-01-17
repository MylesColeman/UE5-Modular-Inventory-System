# Unreal Engine 5: Modular Inventory System
**University Year 2 | Programming for Game Engines | WIP**

A robust and efficient C++/Blueprint modular inventory plugin for Unreal Engine 5. Designed with developer accessibility in mind, the system utilises a component-based architecture to provide plug-and-play functionality for any game actor.

> **[🔗 View the full technical breakdown on my Portfolio](https://sites.google.com/view/myles-coleman/projects/inventory-system-plugin)**

## 📺 Video Showcase
*(Video walkthrough coming soon—Project currently in active development)*

## 🕹️ Project Overview
This project serves as a deep dive into Unreal Engine's C++ architecture and the optimisation of large-scale game data. The goal is to move away from performance-heavy "Tick" dependencies toward a clean, event-driven system that handles complex data sorting and physical world interactions seamlessly.

## 🛠️ Key Technical Features

### Architecture & Design Patterns
* **Component-Based Logic:** Designed using `UActorComponents`, allowing the inventory system to be added to players, NPCs, or chests without modifying core class code.
* **Observer Pattern (Event Dispatchers):** Replaced Event Tick UI updates with Event Dispatchers. The UI only refreshes when the backend broadcasts an `OnInventoryUpdated` event, significantly reducing CPU overhead.
* **Data-Driven Pipeline:** Uses Data Tables and custom C++ structs, enabling designers to tweak item attributes (Weight, Mesh, Name) in-editor without recompiling code.

### Advanced Algorithmic Logic
* **Iterative Merge Sort:** Implemented a custom Iterative Merge Sort algorithm adapted for Unreal's `TArray` architecture. This allows for O(n log n) efficiency when sorting inventory items by weight.

### Gameplay Integration
* **Physical Item Interaction:** Bridged the gap between UI and world space using `OnBoxBeginOverlap` logic. Items identify the player's component, verify weight limits, and add themselves to the data structure before destroying the physical actor.
* **Contextual UI:** Features a scrollable menu with context-sensitive inputs for inspecting, dropping, and sorting items.

## 💻 Technical Specs
* **Engine:** Unreal Engine 5
* **Languages:** C++, Blueprint Visual Scripting
* **Core Concepts:** Component-Based Design, Memory Management (Pointers/References), Algorithm Optimisation, UMG Data Binding
* **Tools:** JetBrains Rider, Visual Studio, GitHub
