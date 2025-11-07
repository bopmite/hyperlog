#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>
#include <memory>

namespace hyperlog::mempool {

template<typename T, size_t BlockSize = 4096>
class Pool {
private:
    union Node {
        T data;
        Node* next;
    };

    struct Block {
        std::unique_ptr<uint8_t[]> memory;
        Block(size_t size) : memory(new uint8_t[size]) {}
    };

    std::vector<Block> blocks_;
    Node* free_list_;
    size_t nodes_per_block_;

    void allocate_block() {
        size_t block_size = sizeof(Node) * nodes_per_block_;
        blocks_.emplace_back(block_size);
        
        auto* block_start = reinterpret_cast<Node*>(blocks_.back().memory.get());
        for (size_t i = 0; i < nodes_per_block_ - 1; ++i) {
            block_start[i].next = &block_start[i + 1];
        }
        block_start[nodes_per_block_ - 1].next = free_list_;
        free_list_ = block_start;
    }

public:
    Pool() : free_list_(nullptr), nodes_per_block_(BlockSize / sizeof(Node)) {
        if (nodes_per_block_ == 0) nodes_per_block_ = 1;
        allocate_block();
    }

    ~Pool() = default;

    Pool(const Pool&) = delete;
    Pool& operator=(const Pool&) = delete;
    Pool(Pool&&) = default;
    Pool& operator=(Pool&&) = default;

    template<typename... Args>
    T* allocate(Args&&... args) {
        if (!free_list_) {
            allocate_block();
        }
        
        Node* node = free_list_;
        free_list_ = node->next;
        
        return new (&node->data) T(std::forward<Args>(args)...);
    }

    void deallocate(T* ptr) {
        if (!ptr) return;
        
        ptr->~T();
        
        Node* node = reinterpret_cast<Node*>(ptr);
        node->next = free_list_;
        free_list_ = node;
    }
};

}