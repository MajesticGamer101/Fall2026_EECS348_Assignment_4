/*
 * Program: EECS 348 Assignment 4 - Recursive Sudoku Solver
 *
 * Description:
 *   C++ object-oriented program that solves Sudoku puzzles with a
 *   depth-first search with backtracking (brute force). For every puzzle
 *   file it prints the file name, the starting puzzle, and every solution
 *   found, or "No solution found" if the puzzle cannot be solved.
 *
 * Input:
 *   Puzzle file names on the command line. With no arguments the program
 *   reads puzzle1.txt through puzzle5.txt from the current folder.
 *   Each file has 9 rows of 9 entries separated by spaces; a digit 1-9 is a
 *   given value and an underscore (_) is a blank cell.
 *
 * Output:
 *   The file name, the puzzle as read, and each solution (or
 *   "No solution found") printed to the terminal.
 *
 * Collaborators: None
 * Other Sources: Claude (Anthropic)
 * Author: Joseph Akins
 * Creation Date: October 15, 2026
 * Revision Date: October 15, 2026
 *
 * Revisions:
 *   Initial object-oriented implementation.
 *   findEmpty now picks the empty cell with the fewest legal digits.
 */

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

/*
 * Board
 *
 * Holds the 9x9 grid. 0 means an empty cell.
 * Knows how to load itself from a file, print itself,
 * and check whether a digit is allowed in a cell.
 */
class Board {
public:
    static const int SIZE = 9;

    Board() {
        for (int r = 0; r < SIZE; r++)
            for (int c = 0; c < SIZE; c++)
                cells[r][c] = 0;
    }

    /*
     * Reads 81 entries from the file.
     * Input: file name. Output: true if the file was read correctly,
     * false otherwise (error message stored in errorMessage).
     */
    bool load(const string& fileName) {
        ifstream in(fileName);
        if (!in) {
            errorMessage = "Could not open " + fileName;
            return false;
        }
        string token;
        int count = 0;
        while (in >> token && count < SIZE * SIZE) {
            int value;
            if (token == "_") {
                value = 0;
            } else if (token.size() == 1 && token[0] >= '1' && token[0] <= '9') {
                value = token[0] - '0';
            } else {
                errorMessage = "Invalid entry '" + token + "' in " + fileName;
                return false;
            }
            cells[count / SIZE][count % SIZE] = value;
            count++;
        }
        if (count != SIZE * SIZE) {
            errorMessage = fileName + " does not contain 81 entries";
            return false;
        }
        return true;
    }

    /*
     * Returns true if digit can go in (row, col) without repeating
     * in the same row, column, or 3x3 block. The cell itself is skipped.
     */
    bool canPlace(int row, int col, int digit) const {
        for (int i = 0; i < SIZE; i++) {
            if (i != col && cells[row][i] == digit) return false;
            if (i != row && cells[i][col] == digit) return false;
        }
        int blockRow = (row / 3) * 3;
        int blockCol = (col / 3) * 3;
        for (int r = blockRow; r < blockRow + 3; r++)
            for (int c = blockCol; c < blockCol + 3; c++)
                if ((r != row || c != col) && cells[r][c] == digit)
                    return false;
        return true;
    }

    /*
     * Checks that the given (filled) digits do not already break a rule.
     * A puzzle like that has no solution.
     */
    bool givensAreValid() const {
        for (int r = 0; r < SIZE; r++)
            for (int c = 0; c < SIZE; c++)
                if (cells[r][c] != 0 && !canPlace(r, c, cells[r][c]))
                    return false;
        return true;
    }

    /*
     * Finds the empty cell with the fewest legal digits, so the search
     * branches as little as possible (a cell with 0 options ends the path fast).
     * Output: true and the cell's position, or false if the board is full.
     */
    bool findEmpty(int& row, int& col) const {
        int fewest = 10;
        for (int r = 0; r < SIZE; r++)
            for (int c = 0; c < SIZE; c++) {
                if (cells[r][c] != 0) continue;
                int options = 0;
                for (int d = 1; d <= 9; d++)
                    if (canPlace(r, c, d)) options++;
                if (options < fewest) {
                    fewest = options;
                    row = r;
                    col = c;
                    if (options <= 1) return true;
                }
            }
        return fewest < 10;
    }

    int get(int row, int col) const { return cells[row][col]; }
    void set(int row, int col, int value) { cells[row][col] = value; }

    /* Prints the grid with blanks as _ and lines between the 3x3 blocks. */
    void print(ostream& out) const {
        for (int r = 0; r < SIZE; r++) {
            if (r > 0 && r % 3 == 0) out << "------+-------+------\n";
            for (int c = 0; c < SIZE; c++) {
                if (c > 0 && c % 3 == 0) out << "| ";
                if (cells[r][c] == 0) out << '_';
                else out << cells[r][c];
                if (c < SIZE - 1) out << ' ';
            }
            out << '\n';
        }
    }

    string errorMessage;

private:
    int cells[SIZE][SIZE];
};

/*
 * SudokuSolver
 *
 * Runs the recursive depth-first search with backtracking on a Board
 * and keeps every complete solution it finds.
 */
class SudokuSolver {
public:
    explicit SudokuSolver(const Board& start) : board(start) {}

    /* Searches the whole tree. Output: number of solutions found. */
    int solve() {
        solutions.clear();
        if (board.givensAreValid()) search();
        return static_cast<int>(solutions.size());
    }

    const vector<Board>& getSolutions() const { return solutions; }

private:
    Board board;
    vector<Board> solutions;

    /*
     * Recursive step: fill the next empty cell with each candidate digit
     * and recurse. A full board is a solution. If a cell has no candidates,
     * this path fails and the call returns so the caller can try its next digit.
     */
    void search() {
        int row, col;
        if (!board.findEmpty(row, col)) {   // all 81 cells filled
            solutions.push_back(board);
            return;
        }
        for (int digit = 1; digit <= 9; digit++) {
            if (board.canPlace(row, col, digit)) {
                board.set(row, col, digit);
                search();
                board.set(row, col, 0);      // backtrack
            }
        }
    }
};

/*
 * PuzzleRunner
 *
 * Loads one puzzle file, solves it, and prints the required report.
 */
class PuzzleRunner {
public:
    explicit PuzzleRunner(const string& name) : fileName(name) {}

    void run(ostream& out) const {
        out << "==============================\n";
        out << "Puzzle file: " << fileName << "\n";
        out << "==============================\n";

        Board puzzle;
        if (!puzzle.load(fileName)) {
            out << "Error: " << puzzle.errorMessage << "\n\n";
            return;
        }

        out << "Puzzle:\n";
        puzzle.print(out);
        out << '\n';

        SudokuSolver solver(puzzle);
        int count = solver.solve();
        if (count == 0) {
            out << "No solution found\n\n";
            return;
        }

        const vector<Board>& solutions = solver.getSolutions();
        for (int i = 0; i < count; i++) {
            if (count == 1) out << "Solution:\n";
            else out << "Solution " << (i + 1) << " of " << count << ":\n";
            solutions[i].print(out);
            out << '\n';
        }
    }

private:
    string fileName;
};

int main(int argc, char* argv[]) {
    vector<string> files;
    for (int i = 1; i < argc; i++) files.push_back(argv[i]);
    if (files.empty())
        for (int i = 1; i <= 5; i++)
            files.push_back("puzzle" + to_string(i) + ".txt");

    for (const string& f : files) {
        PuzzleRunner runner(f);
        runner.run(cout);
    }
    return 0;
}
