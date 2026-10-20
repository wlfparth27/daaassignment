# 0/1 Knapsack Dynamic Programming with Live ASCII Visualization (C)

This project implements the **0/1 Knapsack Problem** using Dynamic Programming in C. It reads test cases from an input file, displays a step-by-step live ASCII animation of the DP table building in the terminal, writes the results to an output file (`result.txt`), and prepares everything for GitHub submission.

---

## Features
- **File Parsing**: Automatically reads test cases separated by `#Test` headers, handling item counts ($n$) and capacities ($W$).
- **Dynamic Programming Solver**: Computes the optimal maximum total value using $O(n \times W)$ time and space complexity.
- **Live ASCII Visualization**: Animates cell-by-cell table population in real time with active brackets `[ ]` and status messages.
- **Result Export**: Automatically saves all test case outputs to `result.txt` formatted as `#Test case input (n w) result`.

---

## Project Structure

```text
01-knapsack-ascii-c/
├── input.txt       # Test cases separated by #Test
├── knapsack.c      # C source code with DP solver and ASCII visualizer
├── result.txt      # Auto-generated output file (created upon execution)
└── README.md       # Project documentation