#ifndef BOARD_HPP
#define BOARD_HPP

#include <iostream>
#include <string>
#include <algorithm>

namespace GameSolver {
    namespace Connect4 {

/**
 * A class representing a Connect 4 game position.
 * All functions are relative to the current player to play.
 *
 * This class does not support positions with alignments, as there is no need to solve an already won position.
 */
        class Position {
        public:
            static constexpr int WIDTH = 7;   // Width of the board
            static constexpr int HEIGHT = 6;  // Height of the board
            static_assert(WIDTH < 10, "Board's width must be less than 10");

            Position() : board{}, columnHeight{}, moves{0} {}

            /**
             * Checks if a column is playable.
             * @param col: 0-based index of the column to play
             * @return true if the column is playable, false if the column is already full.
             */
            bool canPlay(int col) const
            {
                return columnHeight[col] < HEIGHT;
            }

            /**
             * Plays a playable column.
             * This function should not be called on a non-playable column or a column that makes an alignment.
             *
             * @param col: 0-based index of a playable column.
             */
            void playColumn(int col)
            {
                int currentPlayer = 1 + moves % 2;
                board[col][columnHeight[col]++] = currentPlayer;
                moves++;
            }

            /*
             * Plays a sequence of successive played columns, mainly used to initialize a board.
             * @param seq: a sequence of digits corresponding to the 1-based index of the column played.
             *
             * @return number of played moves. Processing will stop at the first invalid move, which can be:
             *           - an invalid character (non-digit or digit >= WIDTH)
             *           - playing a column that is already full
             *           - playing a column that makes an alignment (we only solve non-alignments).
             *         The caller can check if the move sequence was valid by comparing the number of
             *         processed moves to the length of the sequence.
             */
            unsigned int playSequence(const std::string& seq)
            {
                for (char ch : seq) {
                    int col = ch - '1';
                    if (isValidMove(col))
                        playColumn(col);
                    else
                        return seq.size();  // invalid move
                }
                return seq.size();
            }

            /**
             * Checks if the current player wins by playing a given column.
             * This function should never be called on a non-playable column.
             * @param col: 0-based index of a playable column.
             * @return true if the current player makes an alignment by playing the corresponding column col.
             */
            bool winningMove(int col) const
            {
                int currentPlayer = 1 + moves % 2;
                int h = columnHeight[col];

                // Check for vertical, horizontal, and diagonal alignments
                for (int dx = -1; dx <= 1; ++dx) {
                    for (int dy = -1; dy <= 1; ++dy) {
                        if (dx == 0 && dy == 0) continue;  // Skip the case where both dx and dy are zero

                        int count = countStonesInDirection(col, h, dx, dy);
                        if (count >= 3) return true;
                    }
                }

                return false;
            }

            /**
             * Returns the number of moves played from the beginning of the game.
             */
            unsigned int nbMoves() const
            {
                return moves;
            }

        private:
            int board[WIDTH][HEIGHT]{};  // 0 if the cell is empty, 1 for the first player, and 2 for the second player.
            int columnHeight[WIDTH]{};   // Number of stones per column
            unsigned int moves{0};      // Number of moves played since the beginning of the game.

            /**
             * Checks if a move to the given column is valid.
             * @param col: 0-based index of the column to check
             * @return true if the move is valid, false otherwise.
             */
            bool isValidMove(int col) const
            {
                return col >= 0 && col < WIDTH && canPlay(col) && !winningMove(col);
            }

            /**
             * Counts the number of stones of the current player in a given direction from a specific position.
             * @param startCol: The starting column
             * @param startRow: The starting row
             * @param dx: The change in column direction (-1, 0, or 1)
             * @param dy: The change in row direction (-1, 0, or 1)
             * @return The count of stones in the specified direction.
             */
            int countStonesInDirection(int startCol, int startRow, int dx, int dy) const
            {
                int currentPlayer = board[startCol][startRow];

                for (int i = 1; i <= 3; ++i) {
                    int col = startCol + i * dx;
                    int row = startRow + i * dy;

                    if (col < 0 || col >= WIDTH || row < 0 || row >= HEIGHT || board[col][row] != currentPlayer)
                        return i - 1;  // Return the count directly without using a variable.
                }

                return 3;
            }
        };

    }  // namespace Connect4
}  // namespace GameSolver

#endif
