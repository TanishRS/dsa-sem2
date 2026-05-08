Toll Plaza Traffic Analyzer

Overview

The Toll Plaza Traffic Analyzer is a C++ application designed as a Data Structures and Algorithms (DSA) mini-project. It simulates a highway toll plaza system, allowing users to record vehicle entries and analyze traffic patterns using multiple algorithmic approaches. The project emphasizes practical implementation of DSA concepts and compares different algorithms based on their time complexity and real-time performance.

⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻

Features

1. Record Vehicle Entry
    Add a vehicle entry with proper validation for ID, lane, timestamp, and vehicle type.
2. Load Demo Dataset
    Populate the system with ~1200 synthetic entries to test performance and visualize algorithm differences.
3. View Entries
    Display all recorded entries in a structured tabular format.
4. Find Busiest Lane (Algorithm Comparison)
    Compares three approaches:
    * Brute Force: $O(n \times k)$ time complexity
    * Sort-Based Approach: $O(n \log n)$ time complexity
    * Count Array (Optimal): $O(n)$ time complexity
5. Find Peak Hour (Algorithm Comparison)
    Identifies the busiest hour using:
    * Linear Scan: $O(n)$ time complexity
    * Nested Quadratic Scan: $O(n^2)$ time complexity
        Includes real-time benchmarking in microseconds.
6. Count Vehicles by Type
    Counts total vehicles for each type using a single traversal: $O(n)$.
7. Sort Entries by Time
    Sorts entries chronologically using std::sort: $O(n \log n)$.
8. Search Vehicle by ID (Multiple Strategies)
    * Linear Search: $O(n)$
    * Binary Search (on sorted copy): $O(\log n)$
    * Hash Map Lookup: $O(1)$ average
9. Search by Time Range (Custom Feature)
    Retrieve all vehicles within a specified time interval: $O(n)$.
10. Export Report to File (Custom Feature)
    Generate a complete .txt report including entries and analysis.
11. Big-O Comparison Table
    Displays time complexity for all implemented algorithms.

⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻

Data Structures Used

* Structs
    VehicleEntry stores:
    * Vehicle ID
    * Lane number
    * Timestamp (string + epoch)
    * Vehicle type
* Vectors (std::vector)
    Dynamic storage for vehicle entries.
* Hash Maps (std::unordered_map)
    Used for:
    * Counting vehicle types
    * Fast ID lookup
* Arrays
    Used for:
    * Lane frequency counting
    * Hour-based traffic analysis

⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻

Algorithms Demonstrated

* Linear Search
* Binary Search
* Hashing (unordered_map)
* Sorting (std::sort – IntroSort)
* Frequency Counting
* Nested Loop (Quadratic Complexity)
* Time-based Analysis using Epoch conversion

⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻

Compilation and Execution

Make sure you have a C++ compiler installed (e.g., g++).

Compile

g++ -std=c++17 toll_plaza.cpp -o toll_plaza

Run

Linux / Mac

./toll_plaza

Windows

toll_plaza.exe

⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻

Usage Instructions

1. Run the program to launch the menu-driven interface.
2. Choose:
    * Option 9 to load demo dataset (recommended for testing)
    * OR Option 1 to manually add entries
3. Use analysis features:
    * Option 3 → Busiest lane comparison
    * Option 4 → Peak hour benchmarking
    * Option 7 → Search comparison
4. Use custom features:
    * Option 10 → Search by time range
    * Option 11 → Export report

⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻

Learning Outcomes

* Understand differences between:
    * $O(n)$, $O(n \log n)$, and $O(n^2)$ algorithms
* Learn trade-offs between:
    * Speed vs memory
    * Simplicity vs efficiency
* Apply DSA concepts in a real-world simulation

⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻

Big-O Summary

Function	Complexity
Record Entry	O(1) (amortized)
Busiest Lane (Brute)	O(n × k)
Busiest Lane (Sort)	O(n log n)
Busiest Lane (Optimal)	O(n)
Peak Hour (Fast)	O(n)
Peak Hour (Slow)	O(n²)
Search (Linear)	O(n)
Search (Binary)	O(log n)
Search (Hash)	O(1)

⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻⸻

Conclusion

This project serves as a practical demonstration of how different algorithms perform on the same dataset. It highlights why choosing the right algorithm is crucial for scalability and efficiency in real-world systems.


* ￼ make this more resume-focused (top 1% style)
* or ￼ convert it into a GitHub README with badges + screenshots 🚀
