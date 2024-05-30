#include "batched_spsc_queue.hh"
#include <atomic>
#include <cstdint>

namespace batched_spsc_queue {
Queue::Queue(size_t nb_slots, size_t enqueue_batch_size,
             size_t dequeue_batch_size, size_t element_size, uint8_t *buffer)
    : nb_slots_(nb_slots), enqueue_batch_size_(enqueue_batch_size),
      dequeue_batch_size_(dequeue_batch_size), element_size_(element_size),
      buffer_(buffer) {}

uint8_t *Queue::write_ptr() {
  if (nb_slots_ - writer_size() < enqueue_batch_size_ + 1) {
    return nullptr;
  }

  size_t write_idx = write_idx_.load(std::memory_order_relaxed);
  uint8_t *dst = buffer_ + write_idx * element_size_;
  return dst;
}

void Queue::commit_write() {
  size_t write_idx = write_idx_.load(std::memory_order_relaxed);
  size_t next_write_idx = write_idx + enqueue_batch_size_;
  if (next_write_idx == nb_slots_)
    next_write_idx = 0;

  write_idx_.store(next_write_idx, std::memory_order_release);
}

uint8_t *Queue::read_ptr() {
  if (reader_size() < dequeue_batch_size_)
    return nullptr;

  size_t read_idx = read_idx_.load(std::memory_order_relaxed);
  uint8_t *src = buffer_ + read_idx * element_size_;
  return src;
}

void Queue::commit_read() {
  size_t read_idx = read_idx_.load(std::memory_order_relaxed);
  size_t next_read_idx = read_idx + dequeue_batch_size_;
  if (next_read_idx == nb_slots_)
    next_read_idx = 0;

  read_idx_.store(next_read_idx, std::memory_order_release);
}

[[maybe_unused]] size_t Queue::size() {
  size_t write_idx = write_idx_.load(std::memory_order_acquire);
  size_t read_idx = read_idx_.load(std::memory_order_acquire);

  size_t diff = write_idx - read_idx;

  if (write_idx < read_idx)
    diff += nb_slots_;

  return diff;
}

void Queue::reset() {
  write_idx_.store(0, std::memory_order_release);
  read_idx_.store(0, std::memory_order_release);
}

void Queue::fill() {
  write_idx_.store(nb_slots_, std::memory_order_release);
  read_idx_.store(0, std::memory_order_release);
}

size_t Queue::writer_size() {
  size_t write_idx = write_idx_.load(std::memory_order_relaxed);
  size_t read_idx = read_idx_.load(std::memory_order_acquire);

  size_t diff = write_idx - read_idx;

  if (write_idx < read_idx)
    diff += nb_slots_;

  return diff;
}

size_t Queue::reader_size() {
  size_t write_idx = write_idx_.load(std::memory_order_acquire);
  size_t read_idx = read_idx_.load(std::memory_order_relaxed);

  size_t diff = write_idx - read_idx;

  if (write_idx < read_idx)
    diff += nb_slots_;

  return diff;
}
} // namespace batched_spsc_queue
