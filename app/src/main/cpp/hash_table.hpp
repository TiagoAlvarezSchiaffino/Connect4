#ifndef HASH_TABLE_HPP
#define HASH_TABLE_HPP

#include <array>
#include <cstdint>
#include <algorithm>
#include <cassert>

/**
 * @brief A simple transposition table for caching game tree analysis results.
 *
 * The transposition table uses a fixed-size hash map with a struct Entry containing
 * a 56-bit key and an 8-bit upper bound value. The upper bound value indicates whether
 * the cached result is an upper bound or not.
 */
class TranspositionTable {
private:
    /**
     * @brief Represents an entry in the transposition table.
     */
    struct Entry {
        uint64_t key : 56;  // 56-bit key
        uint8_t upperBound; // 8-bit upper bound value
    };

    static constexpr size_t TABLE_SIZE = 128;  // Set your desired table size
    static constexpr uint64_t KEY_MASK = (1ULL << 56) - 1;

    std::array<Entry, TABLE_SIZE> T;  // Array to store entries

    /**
     * @brief Computes the index in the transposition table for a given key.
     * @param key The 56-bit key to compute the index for.
     * @return The computed index.
     */
    size_t index(uint64_t key) const {
        return key % TABLE_SIZE;
    }

public:
    /**
     * @brief Constructs a TranspositionTable with a fixed size.
     */
    TranspositionTable() = default;

    /**
     * @brief Resets the transposition table by filling it with zeroed entries.
     */
    void reset() {
        std::fill(T.begin(), T.end(), Entry{0, 0});
    }

    /**
     * @brief Stores a value in the transposition table for a given key.
     * @param key The 56-bit key.
     * @param val The 8-bit value to store.
     * @param isUpperBound Indicates if the value is an upper bound.
     */
    void put(uint64_t key, uint8_t val, bool isUpperBound) {
        assert(key < KEY_MASK);
        size_t i = index(key);

        // Handle collision (linear probing)
        while (T[i].key != 0 && T[i].key != key) {
            i = (i + 1) % TABLE_SIZE;
        }

        // Update the entry if it already exists
        if (T[i].key == key) {
            T[i].upperBound = val;
        } else {
            // Otherwise, insert a new entry
            T[i] = {key, val};
        }
    }

    /**
     * @brief Retrieves the value for a given key from the transposition table.
     * @param key The 56-bit key.
     * @param isUpperBound Output parameter indicating if the value is an upper bound.
     * @return The 8-bit value associated with the key if present, 0 otherwise.
     */
    uint8_t get(uint64_t key) const {
        assert(key < KEY_MASK);
        size_t i = index(key);

        // Search for the key in the table
        while (T[i].key != 0) {
            if (T[i].key == key) {
                return T[i].upperBound;
            }
            i = (i + 1) % TABLE_SIZE;
        }

        return 0;  // Key not found
    }
};

#endif
