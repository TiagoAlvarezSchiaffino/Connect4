#include "board.hpp"
#include "hash_table.hpp"
#include <chrono>
#include <iostream>
#include <limits>

namespace GameSolver {
    namespace Connect4 {

        /**
        * Connect4Solver class is responsible for solving the Connect 4 game using the Negamax algorithm.
        */
        class Connect4Solver {
        private:
            unsigned long long exploredNodeCount = 0;  // Counter for the number of nodes explored
            std::chrono::steady_clock::duration timeLimit;  // Time limit for the solver
            int depthLimit;  // Depth limit for the solver

            /**
            * Implementation of the Negamax algorithm with alpha-beta pruning.
            */
            int negamax(const Position &currentPosition, int depth, int alpha, int beta,
                        std::chrono::steady_clock::time_point startTime, TranspositionTable &transTable);

            // Function to avoid losing moves
            bool isLosingMove(const Position &position, int move);

        public:
            /**
            * Constructor for Connect4Solver.
            * @param timeLimit The time limit for the solver.
            * @param depthLimit The depth limit for the solver.
            */
            explicit Connect4Solver(std::chrono::steady_clock::duration timeLimit = std::chrono::steady_clock::duration::max(),
                                    int depthLimit = std::numeric_limits<int>::max())
                    : timeLimit(timeLimit), depthLimit(depthLimit) {}

            /**
             * Solves the Connect 4 game for the given initial position.
             * @param initialPosition The initial position of the game.
             * @return The score of the best move.
             */
            int solve(const Position &initialPosition);

            /**
             * Gets the count of explored nodes during the solving process.
             * @return The count of explored nodes.
             */
            unsigned long long getExploredNodeCount() const {
                return exploredNodeCount;
            }
        };

        bool Connect4Solver::isLosingMove(const Position &position, int move) {
            Position nextPosition(position);
            nextPosition.play(move);

            // Check if the move leads to a losing position
            for (int col = 0; col < Position::WIDTH; ++col) {
                if (nextPosition.canPlay(col)) {
                    Position opponentPosition(nextPosition);
                    opponentPosition.play(col);

                    // If the opponent can win on their next move, this is a losing move
                    if (opponentPosition.isWinningMove(col)) {
                        return true;
                    }
                }
            }

            return false;
        }

    } // namespace Connect4
} // namespace GameSolver

using namespace GameSolver::Connect4;

int Connect4Solver::negamax(const Position &currentPosition, int depth, int alpha, int beta, std::chrono::steady_clock::time_point startTime, TranspositionTable &transTable) {
    exploredNodeCount++;

    // Use the TranspositionTable in the initial position
    uint8_t ttEntry = transTable.get(currentPosition.key());
    if (ttEntry != 0) {
        return (currentPosition.nbMoves() - ttEntry);  // Return the stored score from the transposition table
    }

    if (depth == 0 || currentPosition.nbMoves() >= Position::WIDTH * Position::HEIGHT) {
        return currentPosition.nbMoves(); // Use the number of moves as a score
    }

    auto endTime = getCurrentTime();
    if (endTime - startTime >= timeLimit) {
        return 0; // Time's up, return a neutral score
    }

    int columnOrder[Position::WIDTH] = {3, 2, 4, 1, 5, 0, 6};  // Static column order strategy (center columns first)

    for (int i = 0; i < Position::WIDTH; i++) {
        int x = columnOrder[i];
        if (currentPosition.canPlay(x) && !isLosingMove(currentPosition, x)) {
            Position nextPosition(currentPosition);
            nextPosition.play(x);

            // Use the TranspositionTable in the recursive calls
            int score = -negamax(nextPosition, depth - 1, -beta, -alpha, startTime, transTable);

            if (score >= beta) {
                return score; // Beta cutoff
            }

            if (score > alpha) {
                alpha = score; // Update alpha
            }
        }

        // Store the score in the TranspositionTable inside the loop
        transTable.put(currentPosition.key(), static_cast<uint8_t>(alpha), false);
    }

    return alpha;
}

int Connect4Solver::solve(const Position &initialPosition) {
    exploredNodeCount = 0;
    auto startTime = std::chrono::steady_clock::now();
    TranspositionTable transTable;  // Create a TranspositionTable instance
    transTable.reset();

    int bestScore = 0;  // Initialize with a neutral value
    int depth;

    for (depth = 1; depth <= depthLimit; ++depth) {
        int score = negamax(initialPosition, depth, std::numeric_limits<int>::min() + 1, std::numeric_limits<int>::max(), startTime, transTable);
        auto endTime = std::chrono::steady_clock::now();

        // Check if the time limit has been exceeded
        if (endTime - startTime >= timeLimit) {
            std::cout << "Time's up! ";
            break;
        }

        // Update the best score
        bestScore = score;
        std::cout << "Depth " << depth << " completed. Nodes explored: " << exploredNodeCount << std::endl;
    }

    std::cout << "Nodes explored: " << exploredNodeCount << std::endl;
    std::cout << "Completed search up to depth " << (depth - 1) << ". ";
    return bestScore;
}

int main() {
    Connect4Solver solver(std::chrono::seconds(5), 10);
    std::string line;

    for (int lineNumber = 1; std::getline(std::cin, line); lineNumber++) {
        Position currentPosition;
        if (currentPosition.play(line) != line.size()) {  // Corrected method name
            std::cerr << "Line " << lineNumber << ": Invalid move " << (currentPosition.nbMoves() + 1) << " \"" << line << "\"" << "\n";
            continue;
        }
        int score = solver.solve(currentPosition);
        std::cout << line << " " << score << " " << solver.getExploredNodeCount() << "\n";
    }

    return 0;
}
