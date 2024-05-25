#include "batched_spsc_queue.hh"
#include <chrono>
#include <cstdint>
#include <future>
#include <gtest/gtest.h>
#include <numeric>
#include <stdexcept>

namespace batched_spsc_queue {
std::atomic<bool> stop_flag{false};

void sleep_us(size_t us) {
  auto start = std::chrono::high_resolution_clock::now();
  auto end = start + std::chrono::microseconds(us);

  while (std::chrono::high_resolution_clock::now() < end) {
  }
}

size_t *write_ptr_blocking(Queue &queue) {
  uint8_t *write_ptr = nullptr;
  while (write_ptr == nullptr && !stop_flag.load())
    write_ptr = queue.write_ptr();

  return reinterpret_cast<size_t *>(write_ptr);
}

size_t *read_ptr_blocking(Queue &queue) {
  uint8_t *read_ptr = nullptr;
  while (read_ptr == nullptr && !stop_flag.load())
    read_ptr = queue.read_ptr();

  return reinterpret_cast<size_t *>(read_ptr);
}

bool enqueue_task(Queue &queue, size_t nb_to_enqueue, size_t enqueue_batch_size,
                  size_t wait_us) {
  if (nb_to_enqueue % enqueue_batch_size != 0)
    throw std::runtime_error("enqueue_task invalid parameters.");

  size_t nb_iter = nb_to_enqueue / enqueue_batch_size;

  for (size_t i = 0; i < nb_iter; i++) {
    size_t *write_ptr = write_ptr_blocking(queue);
    if (write_ptr == nullptr)
      return false;
    std::iota(write_ptr, write_ptr + enqueue_batch_size,
              i * enqueue_batch_size);
    queue.commit_write();
    sleep_us(wait_us);
  }

  return true;
}

bool dequeue_task(Queue &queue, size_t nb_to_dequeue, size_t dequeue_batch_size,
                  size_t wait_us) {
  if (nb_to_dequeue % dequeue_batch_size != 0)
    throw std::runtime_error("dequeue_task invalid parameters");

  size_t nb_iter = nb_to_dequeue / dequeue_batch_size;

  for (size_t i = 0; i < nb_iter; i++) {
    size_t *read_ptr = read_ptr_blocking(queue);
    if (read_ptr == nullptr)
      return false;
    for (size_t j = 0; j < dequeue_batch_size; j++) {
      if (read_ptr[j] != i * dequeue_batch_size + j)
        return false;
    }
    queue.commit_read();
    sleep_us(wait_us);
  }

  return true;
}

TEST(MT_00, SPSCQueue) {
  stop_flag.store(false);
  size_t nb_slots = 300;
  size_t enqueue_batch_size = 2;
  size_t dequeue_batch_size = 3;
  size_t element_size = sizeof(size_t);
  auto buffer = std::make_unique<uint8_t[]>(nb_slots * element_size);
  auto queue = Queue(nb_slots, enqueue_batch_size, dequeue_batch_size,
                     element_size, buffer.get());

  std::future<bool> enqueue_thread =
      std::async(enqueue_task, std::ref(queue), 3000000, 2, 0);
  std::future<bool> dequeue_thread =
      std::async(dequeue_task, std::ref(queue), 3000000, 3, 0);

  EXPECT_TRUE(dequeue_thread.get());
  stop_flag.store(true);
  EXPECT_TRUE(enqueue_thread.get());
}

TEST(MT_01, SPSCQueue) {
  stop_flag.store(false);
  size_t nb_slots = 300;
  size_t enqueue_batch_size = 2;
  size_t dequeue_batch_size = 3;
  size_t element_size = sizeof(size_t);
  auto buffer = std::make_unique<uint8_t[]>(nb_slots * element_size);
  auto queue = Queue(nb_slots, enqueue_batch_size, dequeue_batch_size,
                     element_size, buffer.get());

  std::future<bool> enqueue_thread =
      std::async(enqueue_task, std::ref(queue), 3000000, 2, 2);
  std::future<bool> dequeue_thread =
      std::async(dequeue_task, std::ref(queue), 3000000, 3, 0);

  EXPECT_TRUE(dequeue_thread.get());
  stop_flag.store(true);
  EXPECT_TRUE(enqueue_thread.get());
}

TEST(MT_02, SPSCQueue) {
  stop_flag.store(false);
  size_t nb_slots = 300;
  size_t enqueue_batch_size = 2;
  size_t dequeue_batch_size = 3;
  size_t element_size = sizeof(size_t);
  auto buffer = std::make_unique<uint8_t[]>(nb_slots * element_size);
  auto queue = Queue(nb_slots, enqueue_batch_size, dequeue_batch_size,
                     element_size, buffer.get());

  std::future<bool> enqueue_thread =
      std::async(enqueue_task, std::ref(queue), 3000000, 2, 0);
  std::future<bool> dequeue_thread =
      std::async(dequeue_task, std::ref(queue), 3000000, 3, 1);

  EXPECT_TRUE(dequeue_thread.get());
  stop_flag.store(true);
  EXPECT_TRUE(enqueue_thread.get());
}

TEST(MT_03, SPSCQueue) {
  stop_flag.store(false);
  size_t nb_slots = 300;
  size_t enqueue_batch_size = 3;
  size_t dequeue_batch_size = 2;
  size_t element_size = sizeof(size_t);
  auto buffer = std::make_unique<uint8_t[]>(nb_slots * element_size);
  auto queue = Queue(nb_slots, enqueue_batch_size, dequeue_batch_size,
                     element_size, buffer.get());

  std::future<bool> enqueue_thread =
      std::async(enqueue_task, std::ref(queue), 3000000, 3, 0);
  std::future<bool> dequeue_thread =
      std::async(dequeue_task, std::ref(queue), 3000000, 2, 0);

  EXPECT_TRUE(dequeue_thread.get());
  stop_flag.store(true);
  EXPECT_TRUE(enqueue_thread.get());
}

TEST(MT_04, SPSCQueue) {
  stop_flag.store(false);
  size_t nb_slots = 300;
  size_t enqueue_batch_size = 3;
  size_t dequeue_batch_size = 2;
  size_t element_size = sizeof(size_t);
  auto buffer = std::make_unique<uint8_t[]>(nb_slots * element_size);
  auto queue = Queue(nb_slots, enqueue_batch_size, dequeue_batch_size,
                     element_size, buffer.get());

  std::future<bool> enqueue_thread =
      std::async(enqueue_task, std::ref(queue), 3000000, 3, 2);
  std::future<bool> dequeue_thread =
      std::async(dequeue_task, std::ref(queue), 3000000, 2, 0);

  EXPECT_TRUE(dequeue_thread.get());
  stop_flag.store(true);
  EXPECT_TRUE(enqueue_thread.get());
}

TEST(MT_05, SPSCQueue) {
  stop_flag.store(false);
  size_t nb_slots = 300;
  size_t enqueue_batch_size = 3;
  size_t dequeue_batch_size = 2;
  size_t element_size = sizeof(size_t);
  auto buffer = std::make_unique<uint8_t[]>(nb_slots * element_size);
  auto queue = Queue(nb_slots, enqueue_batch_size, dequeue_batch_size,
                     element_size, buffer.get());

  std::future<bool> enqueue_thread =
      std::async(enqueue_task, std::ref(queue), 3000000, 3, 0);
  std::future<bool> dequeue_thread =
      std::async(dequeue_task, std::ref(queue), 3000000, 2, 1);

  EXPECT_TRUE(dequeue_thread.get());
  stop_flag.store(true);
  EXPECT_TRUE(enqueue_thread.get());
}
} // namespace batched_spsc_queue
