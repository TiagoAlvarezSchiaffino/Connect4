#ifndef HASH_TABLE_HPP
#define HASH_TABLE_HPP

#include <array>
#include <cstdint>
#include <algorithm>
#include <cassert>

/**
 * @brief A simple transposition table for caching game tree analysis results.
 */
class TranspositionTable {
private:
    /**
     * @brief Represents an entry in the transposition table.
     */
    struct Entry {
        uint64_t key : 56;  // 56-bit key to uniquely identify a position
        uint8_t upperBound; // 8-bit upper bound value for the position
    };

    static constexpr size_t TABLE_SIZE = 128;  // Set your desired table size
    static constexpr uint64_t KEY_MASK = (1ULL << 56) - 1;  // Mask to ensure key fits within 56 bits

    std::array<Entry, TABLE_SIZE> T;  // Array to store entries

    size_t collisions;
    size_t totalQueries;

    /**
     * @brief Computes the index in the transposition table for a given key.
     * @param key The 56-bit key to compute the index for.
     * @return The computed index.
     */
    size_t index(uint64_t key) const {
        // Use double hashing for collision resolution
        size_t hash1 = hashFunction1(key);
        size_t hash2 = hashFunction2(key);

        size_t index = hash1;

        // Handle collisions using linear probing
        while (T[index].key != 0 && T[index].key != key) {
            // Linear probing with double hashing
            index = (index + hash2) % TABLE_SIZE;
        }

        return index;
    }

    size_t hashFunction1(uint64_t key) const {
        key ^= (key >> 21);
        key ^= (key << 37);
        key ^= (key >> 4);
        key *= 0x165667919E3779F9ULL;

        return key % TABLE_SIZE;
    }

    size_t hashFunction2(uint64_t key) const {
        key ^= (key >> 37);
        key ^= (key << 21);
        key ^= (key >> 35);
        key *= 0x9E3779B97F4A7C15ULL;

        return 1 + (key % (TABLE_SIZE - 1));
    }

public:
    /**
     * @brief Constructs a TranspositionTable with a fixed size.
     */
    TranspositionTable() : collisions(0), totalQueries(0) {
        reset();
    }

    /**
     * @brief Resets the transposition table by filling it with zeroed entries.
     */
    void reset() {
        std::fill(T.begin(), T.end(), Entry{0, 0});  // Fill the entire table with zeroed entries
    }

    size_t getCollisions() const {
        return collisions;
    }

    size_t getTotalQueries() const {
        return totalQueries;
    }

    double getCollisionRate() const {
        return (totalQueries > 0) ? static_cast<double>(collisions) / totalQueries : 0.0;
    }

    /**
     * @brief Stores a value in the transposition table for a given key.
     * @param key The 56-bit key.
     * @param val The 8-bit value to store.
     * @param isUpperBound Indicates if the value is an upper bound.
     */
    void put(uint64_t key, uint8_t val, bool isUpperBound) {
        assert(key < KEY_MASK);  // Ensure the key fits within the specified 56 bits
        size_t i = index(key);

        // Update the entry if it already exists
        if (T[i].key == key) {
            const_cast<uint8_t&>(T[i].upperBound) = val; // Update the upper bound value
        } else {
            // Otherwise, insert a new entry
            T[i] = {key, val};  // Create a new entry with the provided key and value
        }

    }

    /**
     * @brief Retrieves the value for a given key from the transposition table.
     * @param key The 56-bit key.
     * @param isUpperBound Output parameter indicating if the value is an upper bound.
     * @return The 8-bit value associated with the key if present, 0 otherwise.
     */
    uint8_t get(uint64_t key) const {
        assert(key < KEY_MASK);  // Ensure the key fits within the specified 56 bits
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