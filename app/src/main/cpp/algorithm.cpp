#include "board.hpp"
#include <chrono>
#include <iostream>
#include <limits>

namespace GameSolver {
    namespace Connect4 {

        class Connect4Solver {
        private:
            unsigned long long exploredNodeCount = 0;
            std::chrono::steady_clock::duration timeLimit;
            int depthLimit;

            int negamax(const Position &currentPosition, int depth, int alpha, int beta, std::chrono::steady_clock::time_point startTime);

        public:
            explicit Connect4Solver(std::chrono::steady_clock::duration timeLimit = std::chrono::steady_clock::duration::max(), int depthLimit = std::numeric_limits<int>::max())
                    : timeLimit(timeLimit), depthLimit(depthLimit) {}

            int solve(const Position &initialPosition);

            unsigned long long getExploredNodeCount() const {
                return exploredNodeCount;
            }
        };

        std::chrono::steady_clock::time_point getCurrentTime() {
            return std::chrono::steady_clock::now();
        }

    } // namespace Connect4
} // namespace GameSolver

using namespace GameSolver::Connect4;

int Connect4Solver::negamax(const Position &currentPosition, int depth, int alpha, int beta, std::chrono::steady_clock::time_point startTime) {
    exploredNodeCount++;

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
        if (currentPosition.canPlay(x)) {
            Position nextPosition(currentPosition);
            nextPosition.play(x);  // Corrected method name

            int score = -negamax(nextPosition, depth - 1, -beta, -alpha, startTime);

            if (score >= beta) {
                return score; // Beta cutoff
            }

            if (score > alpha) {
                alpha = score; // Update alpha
            }
        }
    }

    return alpha;
}

int Connect4Solver::solve(const Position &initialPosition) {
    exploredNodeCount = 0;
    auto startTime = std::chrono::steady_clock::now();
    int score = negamax(initialPosition, depthLimit, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), startTime);
    auto endTime = std::chrono::steady_clock::now();
    std::cout << "Nodes explored: " << exploredNodeCount << std::endl;
    return score;
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
