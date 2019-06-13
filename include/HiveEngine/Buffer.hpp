//
// Created by calper on 5/9/19.
//

#ifndef HIVEENGINE_BUFFER_HPP
#define HIVEENGINE_BUFFER_HPP

#include <vector>
#include <deque>
#include <cstdint>
#include <stdexcept>
#include <HiveEngine/HiveEngine.h>

namespace HiveEngine {
    template<class T>
    class Buffer {
    private:
        bool changed = false;

        std::vector<T> data;
        std::vector<int> state;
        std::deque<size_t> available;

        void expand(size_t size) {
            size_t initial_size = data.size();
            size_t final_size = initial_size + size;
            for (int i = initial_size; i < final_size; ++i) {
                available.push_back(i);
            }
            data.resize(final_size);
            state.resize(final_size, 0);
        }

    public:
        std::pair<T, uint8_t> get(size_t index) {
            if (index >= data.size()) {
                spdlog::error("Buffer was asked to retrieve an index out of range!");
                process_error();
            }
            return std::make_pair(data[index], state[index]);
        }

        void set(size_t index, T item) {
            if (index >= data.size()) {
                spdlog::error("Buffer was asked to set an index out of range!");
                process_error();
            } if (state[index] == 0) {
                spdlog::error("Buffer was asked to set an erased element!");
                process_error();
            }
            data[index] = item;
            mark_changed();
        }

        size_t add(T item) {
            if (available.size() == 0) expand(10);
            size_t index = available.front();
            available.pop_front();
            data[index] = item;
            state[index] = 1;
            mark_changed();
            return index;
        }

        void remove(size_t index) {
            if (index >= data.size()) {
                spdlog::error("Buffer was asked to set an index out of range!");
                process_error();
            }
            if (state[index]) {
                available.push_back(index);
                state[index] = 0;
                mark_changed();
            } else {
                spdlog::error("Buffer was asked to erase an erased element!");
                process_error();
            }
        }

        std::vector<T> get_data() {
            return data;
        }

        std::vector<int> get_state() {
            return state;
        }

        bool is_changed() {
            return changed;
        }

        void mark_changed() {
            changed = true;
        }

        void mark_unchanged() {
            changed = false;
        }

        size_t size() {
            return data.size();
        }
    };

}

#endif //HIVEENGINE_BUFFER_HPP
