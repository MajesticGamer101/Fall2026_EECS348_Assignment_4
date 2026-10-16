#include <iostream>
#include <fstream>
#include <string>
#include <array>
#include <vector>
#include <cctype>

class Sudoku {
private:
    std::array<std::array<int, 9>, 9> grid{};
    std::string filename;
    unsigned long long solutionCount = 0;

    bool canPlace(int row, int col, int value) const {
        for (int i = 0; i < 9; ++i) {
            if (grid[row][i] == value || grid[i][col] == value)
                return false;
        }
        int blockRow = (row / 3) * 3;
        int blockCol = (col / 3) * 3;
        for (int r = blockRow; r < blockRow + 3; ++r)
            for (int c = blockCol; c < blockCol + 3; ++c)
                if (grid[r][c] == value)
                    return false;
        return true;
    }

    bool validStartingGrid() {
        for (int r = 0; r < 9; ++r) {
            for (int c = 0; c < 9; ++c) {
                if (grid[r][c] == 0) continue;
                int value = grid[r][c];
                grid[r][c] = 0;
                bool valid = canPlace(r, c, value);
                grid[r][c] = value;
                if (!valid) return false;
            }
        }
        return true;
    }

    void printGrid() const {
        for (int r = 0; r < 9; ++r) {
            for (int c = 0; c < 9; ++c) {
                if (grid[r][c] == 0) std::cout << '_';
                else std::cout << grid[r][c];
                if (c < 8) std::cout << ' ';
            }
            std::cout << '\n';
        }
    }

    void solveRecursive() {
        int bestRow = -1, bestCol = -1;
        std::vector<int> bestCandidates;

        for (int r = 0; r < 9; ++r) {
            for (int c = 0; c < 9; ++c) {
                if (grid[r][c] != 0) continue;
                std::vector<int> candidates;
                for (int value = 1; value <= 9; ++value)
                    if (canPlace(r, c, value)) candidates.push_back(value);
                if (candidates.empty()) return;
                if (bestRow == -1 || candidates.size() < bestCandidates.size()) {
                    bestRow = r;
                    bestCol = c;
                    bestCandidates = candidates;
                }
            }
        }

        if (bestRow == -1) {
            ++solutionCount;
            std::cout << "Solution " << solutionCount << ":\n";
            printGrid();
            std::cout << '\n';
            return;
        }

        for (int value : bestCandidates) {
            grid[bestRow][bestCol] = value;
            solveRecursive();
            grid[bestRow][bestCol] = 0;
        }
    }

public:
    explicit Sudoku(const std::string& file) : filename(file) {}

    bool load() {
        std::ifstream input(filename);
        if (!input) return false;
        char ch;
        for (int r = 0; r < 9; ++r) {
            for (int c = 0; c < 9; ++c) {
                if (!(input >> ch)) return false;
                if (ch == '_') grid[r][c] = 0;
                else if (ch >= '1' && ch <= '9') grid[r][c] = ch - '0';
                else return false;
            }
        }
        if (input >> ch) return false;
        return true;
    }

    void run() {
        std::cout << "File: " << filename << "\n";
        if (!load()) {
            std::cout << "Could not read a valid 9x9 puzzle file.\n\n";
            return;
        }
        std::cout << "Original puzzle:\n";
        printGrid();
        std::cout << "\n";
        if (validStartingGrid()) solveRecursive();
        if (solutionCount == 0) std::cout << "No solution found\n";
        std::cout << "Total solutions: " << solutionCount << "\n\n";
    }
};

int main(int argc, char* argv[]) {
    if (argc > 1) {
        for (int i = 1; i < argc; ++i) {
            Sudoku puzzle(argv[i]);
            puzzle.run();
        }
    } else {
        for (int i = 1; i <= 5; ++i) {
            Sudoku puzzle("puzzle" + std::to_string(i) + ".txt");
            puzzle.run();
        }
    }
    return 0;
}
