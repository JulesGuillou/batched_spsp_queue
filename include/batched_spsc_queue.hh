#pragma once

#include <atomic>
#include <cstddef>
#include <cstdint>

#ifndef CACHE_LINE_SIZE
#define CACHE_LINE_SIZE 128
#endif

namespace batched_spsc_queue {
/**
 * @class Queue
 * @brief A batched single-producer single-consumer (SPSC) queue implemented
 * using a circular buffer.
 *
 * This class provides a high-performance queue that allows batching of enqueue
 * and dequeue operations. It is designed for scenarios where a single producer
 * thread enqueues data and a single consumer thread dequeues data. The queue
 * uses a pre-allocated memory buffer to store elements, and it supports batched
 * operations to improve performance. Sequential elements in a batch are
 * guaranteed to be sequential in memory.
 *
 * @note Not meeting the specified conditions results in undefined behavior.
 *       This includes:
 *       - Using the queue with multiple threads for enqueuing.
 *       - Using the queue with multiple threads for dequeuing.
 *       - Using the queue outside of a single-producer single-consumer (SPSC)
 * context.
 *
 * @example examples/minimal_example.cc
 * This is an example of how to use the queue is a very simple context.
 *
 * @example examples/images_manipulation.cc
 * This is an example of how to use the queue as one would for image processing.
 */
class Queue {
public:
  /**
   * @brief Constructs a Queue with the specified parameters.
   *
   * @param nb_slots The number of slots in the circular buffer. Note that not
   * all slots can be used simultaneously. The actual capacity of the queue is
   * nb_slots - enqueue_batch_size to avoid ambiguity between full and empty
   * states. Additionally, nb_slots must be a multiple of both
   * enqueue_batch_size and dequeue_batch_size.
   * @param enqueue_batch_size The number of elements that can be
   * enqueued in a single batch.
   * @param dequeue_batch_size The number of elements that can be
   * dequeued in a single batch.
   * @param element_size The size of each element in bytes.
   * @param buffer A pre-allocated memory block that is large enough to contain
   * nb_slots * element_size bytes.
   *
   * @note Not meeting the specified conditions results in undefined behavior.
   *       This includes:
   *       - nb_slots not being a multiple of enqueue_batch_size or
   * dequeue_batch_size.
   */
  Queue(size_t nb_slots, size_t enqueue_batch_size, size_t dequeue_batch_size,
        size_t element_size, uint8_t *buffer);

  /**
   * @brief Returns a pointer to the next available slot for writing.
   *
   * This method provides a pointer to the memory location where the next
   * element(s) can be written. The caller is responsible for ensuring that the
   * write does not exceed the available space. If the queue is full, this
   * method returns nullptr.
   *
   * @return A pointer to the next available slot for writing, or nullptr if the
   * queue is full.
   *
   * @note The pointer returned by this method is invalidated after calling
   * commit_write().
   */
  uint8_t *write_ptr();

  /**
   * @brief Commits the write operation.
   *
   * This method updates the write index after writing data to the buffer. It
   * should be called after writing data to the location returned by
   * write_ptr().
   *
   * @note The pointer returned by write_ptr() is invalidated after calling this
   * method.
   */
  void commit_write();

  /**
   * @brief Returns a pointer to the next available slot for reading.
   *
   * This method provides a pointer to the memory location where the next
   * element(s) can be read. The caller is responsible for ensuring that the
   * read does not exceed the available data. If the queue is empty, this method
   * returns nullptr.
   *
   * @return A pointer to the next available slot for reading, or nullptr if the
   * queue is empty.
   *
   * @note The pointer returned by this method is invalidated after calling
   * commit_read().
   */
  uint8_t *read_ptr();

  /**
   * @brief Commits the read operation.
   *
   * This method updates the read index after reading data from the buffer. It
   * should be called after reading data from the location returned by
   * read_ptr().
   *
   * @note The pointer returned by read_ptr() is invalidated after calling this
   * method.
   */
  void commit_read();

  /**
   * @brief Returns the number of elements currently in the queue.
   *
   * This method provides the current size of the queue, i.e., the number of
   * elements that have been enqueued but not yet dequeued.
   *
   * @return The number of elements currently in the queue.
   */
  size_t size();

  /**
   * @brief Resets the queue.
   *
   * This method clears the queue and resets the read and write indices. It is
   * not thread-safe and should only be used for testing or benchmarking
   * purposes.
   */
  void reset();

  /**
   * @brief Fills the queue with uninitialized data.
   *
   * This method fills the queue with uninitialized data. It is not thread-safe
   * and should only be used for testing or benchmarking purposes.
   */
  void fill();

private:
  /**
   * @brief Returns the number of elements currently in the queue.
   *
   * This method provides the current size of the queue, i.e., the number of
   * elements that have been enqueued but not yet dequeued.
   *
   * @return The number of elements currently in the queue.
   *
   * @note This method should only be called by the producer thread.
   */
  size_t writer_size();

  /**
   * @brief Returns the number of elements currently in the queue.
   *
   * This method provides the current size of the queue, i.e., the number of
   * elements that have been enqueued but not yet dequeued.
   *
   * @return The number of elements currently in the queue.
   *
   * @note This method should only be called by the consumer thread.
   */
  size_t reader_size();

private:
  /// The number of slots in the circular buffer.
  size_t nb_slots_;

  /// The maximum number of elements that can be enqueued in a single batch.
  size_t enqueue_batch_size_;

  /// The maximum number of elements that can be dequeued in a single batch.
  size_t dequeue_batch_size_;

  /// The size of each element in bytes.
  size_t element_size_;

  /// A pre-allocated memory block for storing elements.
  uint8_t *buffer_;

  /// The current write index.
  alignas(CACHE_LINE_SIZE) std::atomic<size_t> write_idx_;

  /// The current read index.
  alignas(CACHE_LINE_SIZE) std::atomic<size_t> read_idx_;
};
} // namespace batched_spsc_queue
