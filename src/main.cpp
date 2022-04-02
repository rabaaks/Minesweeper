#include <iostream>
#include <array>
#include <cassert>
#include <random>
#include <algorithm>

enum class CellState {
    uncovered,
    covered,
    flagged,
};

struct Cell {
    int surroundingMines{};
    bool isMine{};
    // Make sure that the cells stay covered by default
    CellState state{CellState::covered};
};

// Create types and globals that will be used later on
constexpr size_t g_boardSize{ 9 };
constexpr size_t g_numMines{ 9 };

using board_t = std::array<std::array<Cell, g_boardSize>, g_boardSize>;
using boardIndex_t = board_t::size_type;

void printCell(const Cell& cell) {
    // Print based on what the state of the cell is
    switch (cell.state) {
        case CellState::uncovered:
            if (cell.isMine) {
                std::cout << '*';
            } else {
                std::cout << cell.surroundingMines;
            }
            break;
        
        case CellState::covered:
            std::cout << '#';
            break;
        
        case CellState::flagged:
            std::cout << 'X';
            break;
        
        default:
            assert(false && "Bad value passed to printCell");
            break;
    }
}

void printBoard(const board_t& board) {
    // Print the numbers at the top of the board
    for (boardIndex_t i{}; i <= g_boardSize; ++i) {
        std::cout << i << ' ';
    }

    std::cout << '\n';

    for (boardIndex_t rowIndex{}; rowIndex < g_boardSize; ++rowIndex) {
        // Print the row number for each row
        std::cout << rowIndex+1 << ' ';
        for (boardIndex_t colIndex{}; colIndex < g_boardSize; ++colIndex) {
            const Cell& cell{ board[rowIndex][colIndex] };
            printCell(cell);
            // Gap between current and next cell
            std::cout << ' ';
        }
        // Go to next row
        std::cout << '\n';
    }
}

int getNumSurroundingMines(const board_t& board, boardIndex_t row, boardIndex_t col) {
    // If it is a mine then the number is unnecessary
    if (board[row][col].isMine) {
        return 0;
    }

    int numSurroundingMines{};

    // ...It works soooo
    for (boardIndex_t rrow{static_cast<boardIndex_t>(std::max(static_cast<int>(row-1), 0))}; rrow <= std::min(row+1, g_boardSize-1); ++rrow) {
        for (boardIndex_t rcol{static_cast<boardIndex_t>(std::max(static_cast<int>(col-1), 0))}; rcol <= std::min(col+1, g_boardSize-1); ++rcol) {
            if (board[rrow][rcol].isMine) {
                ++numSurroundingMines;
            }
        }
    }

    return numSurroundingMines;
}

board_t createBoard() {
    // Creates the random number generator
    static std::mt19937 mt{ std::random_device{}() };
    static std::uniform_int_distribution uid{ static_cast<boardIndex_t>(0), g_boardSize-1 };

    board_t board{};

    // Add the specified number of mines
    for (int i{}; i < g_numMines; ++i){
        boardIndex_t row{};
        boardIndex_t col{};
        do {
            row = uid(mt);
            col = uid(mt);
        } while (board[row][col].isMine == true);
        board[row][col].isMine = true;
    }

    // Get the number of surrounding mines for each cell
    for (boardIndex_t row{}; row < g_boardSize; ++row) {
        for (boardIndex_t col{}; col < g_boardSize; ++col) {
            Cell& cell{ board[row][col] };
            cell.surroundingMines = getNumSurroundingMines(board, row, col);
        }
    }

    return board;
}

// Returns true for successful mine and false if mine is encountered
int mine(board_t& board, boardIndex_t row, boardIndex_t col) {

    Cell& cell{ board[row][col] };

    if (cell.state == CellState::uncovered) {
        return true;
    }

    if (cell.isMine) {
        return false;
    }

    cell.state = CellState::uncovered;

    // If the cell is next to a mine, don't try to look at neighboring cells
    if (cell.surroundingMines > 0) {
        return true;
    }

    // ???
    for (boardIndex_t rrow{static_cast<boardIndex_t>(std::max(static_cast<int>(row-1), 0))}; rrow <= std::min(row+1, g_boardSize-1); ++rrow) {
        for (boardIndex_t rcol{static_cast<boardIndex_t>(std::max(static_cast<int>(col-1), 0))}; rcol <= std::min(col+1, g_boardSize-1); ++rcol) {
            mine(board, rrow, rcol);
        }
    }

    return true;
}

// Checks if the player has cleared all of the cells besides the ones with mines
bool boardEmpty(const board_t& board) {
    int numFlaggedCells{};
    for (const auto& row : board) {
        for (const Cell& cell : row) {
            if (!(cell.state == CellState::uncovered)) {
                ++numFlaggedCells;
            }
        }
    }

    std::cout << numFlaggedCells << '\n';

    return ((numFlaggedCells == g_numMines) ? true : false);
}

// Returns true if the player won and false if they lost
bool playGame(board_t& board) {
    // Game loop
    while (true) {
        if (boardEmpty(board)) {
            return true;
        }

        // Print the board for the player to make a decision
        printBoard(board);

        std::cout << "Enter the row: ";
        boardIndex_t row{};
        std::cin >> row;
        row -= 1;

        std::cout << "Enter the column: ";
        boardIndex_t col{};
        std::cin >> col;
        col -= 1;

        std::cout << "Do you want to flag or mine? ";
        char fmDecision{};
        std::cin >> fmDecision;

        if (fmDecision == 'm') {
            // If the mine was unsucessful, alert the player and exit the game
            if (!mine(board, row, col)) {
                board[row][col].state = CellState::uncovered;
                std::cout << "There was a mine there!\n";
                return false;
            }
        } else {
                board[row][col].state = CellState::flagged;
        }
    }
}

int main() {
    std::cout << "Creating random board...\n";
    board_t board{ createBoard() };
    std::cout << "Done creating board!\n";

    if (playGame(board)) {
        std::cout << "You won!\n";
    } else {
        std::cout << "You lost!\n";
    }

    return 0;
}
