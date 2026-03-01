// #pragma once
#include <list>
#include <mutex>
#include <cstddef>

/**
 * @brief Base class for buffers with templated data type
 * @tparam T The type of data stored in the buffer
 *
 * This template class provides thread-safe buffer operations 
 * (e.g., writes, reads ...)
 * that can be customized by child classes 
 * to work with any data type.
 */
template <typename T>
class BufferBase {
public:
    BufferBase() = default;
    virtual ~BufferBase() = default;

    /**
     * @brief Write data to the buffer
     * @param data The data to append to the buffer
     */
    virtual void write(T data) {
        std::lock_guard<std::mutex> lock(mutex_);
        buffer_.push_back(std::move(data));
        
    }

    /**
     * @brief Replace the entire buffer with new data
     * @param data The data to set as the new buffer content
     */
    virtual void setData(T data) {
        std::lock_guard<std::mutex> lock(mutex_);
        buffer_.clear();
        buffer_.push_back(std::move(data));
    }

    /**
     * @brief Read all data from the buffer without clearing it
     * @return A copy of all data in the buffer
     */
    virtual std::list<T> readAll() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return buffer_;
    }

    /**
     * @brief Read and clear all data from the buffer
     * @return All data that was in the buffer
     */
    virtual std::list<T> consume() {
        std::lock_guard<std::mutex> lock(mutex_);
        std::list<T> result = std::move(buffer_);
        buffer_.clear();
        return result;
    }

    // Returns the first element of the buffer and removes it from the buffer
    virtual T extractNextBuffer() {
        buffer_data_t ret = buffer_.front();
        buffer_.pop_front();
        return ret;
    }    

    virtual void popFront() {
        buffer_.pop_front();
    }

    /**
     * @brief Get the number of elements in the buffer
     * @return The number of elements
     */
    virtual size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return buffer_.size();
    }

    /**
     * @brief Check if the buffer is empty
     * @return true if the buffer is empty, false otherwise
     */
    virtual bool empty() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return buffer_.empty();
    }

    /**
     * @brief Clear all data from the buffer
     */
    virtual void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        buffer_.clear();
    }

protected:
    std::list<T> buffer_;
    mutable std::mutex mutex_;
    int MAX_BUFFER_SIZE = 5;
    int FLUSH_THRESHOLD = 100; //percentage
};
