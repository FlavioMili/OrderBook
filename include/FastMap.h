// ----------------------------------------------------------------------------- //
//                                                                               //
//  Order Book Simulator                                                         //
//  Copyright (c) 2025 Flavio Milinanni. All Rights Reserved.                    //
//                                                                               //
// ----------------------------------------------------------------------------- //

#ifndef FAST_MAP_INCLUDED
#define FAST_MAP_INCLUDED

#include <vector>
#include <cstdint>
#include "Order.h"

// A simple, open-addressing hash map inspired by the 1BRC solutions.
// This is not a general-purpose hash map; it's tailored for this specific use case.
class FastMap {
private:
  struct Entry {
    uint32_t key;
    Order* value;
    enum class State { EMPTY, OCCUPIED, DELETED };
    State state = State::EMPTY;
  };

  std::vector<Entry> table;
  size_t table_size;
  size_t element_count = 0;

  size_t hash(uint32_t key) const {
    key = ((key >> 16) ^ key) * 0x45d9f3b;
    key = ((key >> 16) ^ key) * 0x45d9f3b;
    key = (key >> 16) ^ key;
    return key & (table_size - 1);
  }

  void resize() {
    size_t new_size = table_size * 2;
    std::vector<Entry> new_table(new_size);
    for (const auto& entry : table) {
      if (entry.state == Entry::State::OCCUPIED) {
        size_t index = hash(entry.key);
        while (new_table[index].state != Entry::State::EMPTY) {
          index = (index + 1) & (new_size - 1);
        }
        new_table[index] = entry;
      }
    }
    table = std::move(new_table);
    table_size = new_size;
  }

public:
  FastMap() : table_size(1024) { // Initial size
    table.resize(table_size);
  }

  Order*& operator[](uint32_t key) {
    if (element_count * 2 > table_size) {
      resize();
    }

    size_t index = hash(key);
    size_t tombstone_index = -1;

    while (table[index].state != Entry::State::EMPTY) {
      if (table[index].state == Entry::State::OCCUPIED && table[index].key == key) {
        return table[index].value;
      }
      if (table[index].state == Entry::State::DELETED && tombstone_index == (size_t)-1) {
        tombstone_index = index;
      }
      index = (index + 1) & (table_size - 1);
    }

    if (tombstone_index != (size_t)-1) {
      index = tombstone_index;
    }

    table[index].state = Entry::State::OCCUPIED;
    table[index].key = key;
    element_count++;
    return table[index].value;
  }

  Order** find(uint32_t key) {
    size_t index = hash(key);
    while (table[index].state != Entry::State::EMPTY) {
      if (table[index].state == Entry::State::OCCUPIED && table[index].key == key) {
        return &table[index].value;
      }
      index = (index + 1) & (table_size - 1);
    }
    return nullptr;
  }

  void erase(uint32_t key) {
    size_t index = hash(key);
    while (table[index].state != Entry::State::EMPTY) {
      if (table[index].state == Entry::State::OCCUPIED && table[index].key == key) {
        table[index].state = Entry::State::DELETED;
        element_count--;
        return;
      }
      index = (index + 1) & (table_size - 1);
    }
  }
};

#endif // !FAST_MAP_INCLUDED
