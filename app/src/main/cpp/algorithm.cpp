#include "board.hpp"
#include <chrono> // Use <chrono> for time-related functions
#include <iostream>

namespace GameSolver { namespace Connect4 {

        /**
         * A class to solve Connect 4 positions using the Negamax variant of the min-max algorithm.
         */
        class Connect4Solver {
        private:
            unsigned long long exploredNodeCount; // Counter for explored nodes.

            /**
             * Recursively solves a Connect 4 position using the Negamax variant of the min-max algorithm.
             * @return The score of a position:
             *  - 0 for a draw game
             *  - Positive score if you can win whatever your opponent is playing. Your score is
             *    the number of moves before the end you can win (the faster you win, the higher your score)
             *  - Negative score if your opponent can force you to lose. Your score is the opposite of
             *    the number of moves before the end you will lose (the faster you lose, the lower your score).
             */
            int negamax(const Position &currentPosition) {
                exploredNodeCount++; // Increment the counter of explored nodes.

                if (currentPosition.nbMoves() == Position::WIDTH * Position::HEIGHT) // Check for a draw game
                    return 0;

                for (int column = 0; column < Position::WIDTH; column++) // Check if the current player can win the next move
                    if (currentPosition.canPlay(column) && currentPosition.winningMove(column))
                        return (Position::WIDTH * Position::HEIGHT - currentPosition.nbMoves()) / 2;

                int bestScore = static_cast<int>(-Position::WIDTH * Position::HEIGHT); // Initialize the best possible score with a lower bound of the score.

                for (int column = 0; column < Position::WIDTH; column++) // Compute the score of all possible next moves and keep the best one
                    if (currentPosition.canPlay(column)) {
                        Position nextPosition(currentPosition);
                        nextPosition.playColumn(column); // It's the opponent's turn in the next position after the current player plays the column.
                        int score = -negamax(nextPosition); // If the current player plays column x, their score will be the opposite of the opponent's score after playing column x
                        bestScore = std::max(bestScore, score); // Use std::max for clarity.
                    }

                return bestScore;
            }

        public:

            int solve(const Position &initialPosition)
            {
                exploredNodeCount = 0;
                return negamax(initialPosition);
            }

            unsigned long long getExploredNodeCount() const
            {
                return exploredNodeCount;
            }

        };

    }} // namespace GameSolver::Connect4

unsigned long long getCurrentTimeMicroseconds() {
    using namespace std::chrono;
    return duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();
}

/**
 * Main function.
 * Reads Connect 4 positions, line by line, from standard input
 * and writes one line per position to standard output containing:
 *  - Score of the position
 *  - Number of nodes explored
 *  - Time spent in microseconds to solve the position.
 *
 * Any invalid position (invalid sequence of move or already won game)
 * will generate an error message to standard error and an empty line to standard output.
 */
int main() {
    GameSolver::Connect4::Connect4Solver solver;
    std::string line;

    for (int lineNumber = 1; std::getline(std::cin, line); lineNumber++) {
        GameSolver::Connect4::Position currentPosition;
        if (currentPosition.playSequence(line) != line.size()) {
            std::cerr << "Line " << lineNumber << ": Invalid move " << (currentPosition.nbMoves() + 1) << " \"" << line << "\"" << std::endl;
        } else {
            auto startTime = getCurrentTimeMicroseconds();
            int score = solver.solve(currentPosition);
            auto endTime = getCurrentTimeMicroseconds();
            std::cout << line << " " << score << " " << solver.getExploredNodeCount() << " " << (endTime - startTime);
        }
        std::cout << std::endl;
    }
}
