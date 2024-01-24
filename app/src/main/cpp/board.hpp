#ifndef BOARD_HPP
#define BOARD_HPP

#include <iostream>
#include <string>

namespace GameSolver { namespace Connect4 {

        /**
         * A class representing a Connect 4 game position.
         * All functions are relative to the current player to play.
         *
         * This class does not support positions with alignments, as there is no need to solve an already won position.
         */
        class Position {
        public:
            static const int WIDTH = 7;  // Width of the board
            static const int HEIGHT = 6; // Height of the board
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
                    if (col < 0 || col >= Position::WIDTH || !canPlay(col) || winningMove(col))
                        return seq.size(); // invalid move
                    playColumn(col);
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

                // Check for vertical alignments
                if (h >= 3 && board[col][h - 1] == currentPlayer && board[col][h - 2] == currentPlayer && board[col][h - 3] == currentPlayer)
                    return true;

                for (int dy = -1; dy <= 1; dy++) {    // Iterate on horizontal (dy = 0) or two diagonal directions (dy = -1 or dy = 1).
                    int continuousStones = 0;         // Counter of the number of stones of the current player surrounding the played stone in the tested direction.
                    for (int dx = -1; dx <= 1; dx += 2) // Count continuous stones of the current player on the left, then right of the played column.
                        for (int x = col + dx, y = h + dx * dy; x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT && board[x][y] == currentPlayer; continuousStones++) {
                            x += dx;
                            y += dx * dy;
                        }
                    if (continuousStones >= 3) return true; // There is an alignment if at least 3 other stones of the current user
                    // are surrounding the played stone in the tested direction.
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

            /*
             * Default constructor, builds an empty position.
             */

        private:
            int board[WIDTH][HEIGHT]; // 0 if the cell is empty, 1 for the first player, and 2 for the second player.
            int columnHeight[WIDTH];  // Number of stones per column
            unsigned int moves;       // Number of moves played since the beginning of the game.
        };

    }} // end namespaces

#endif
