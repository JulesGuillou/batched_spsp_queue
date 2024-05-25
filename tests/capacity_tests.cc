#include "batched_spsc_queue.hh"
#include <cstdint>
#include <cstdlib>
#include <gtest/gtest.h>
#include <memory>

namespace batched_spsc_queue {
TEST(Capacity_Is_Respected_100_1_1_1, BATCHED_SPSC_QUEUE) {
  size_t nb_slots = 100;
  size_t enqueue_batch_size = 1;
  size_t dequeue_batch_size = 1;
  size_t element_size = 1;
  auto buffer = std::make_unique<uint8_t>(nb_slots * element_size);

  for (size_t i = 0; i < 100 * 10; i++) {
    auto queue = Queue(nb_slots, enqueue_batch_size, dequeue_batch_size,
                       element_size, buffer.get());

    // Enqueue-Dequeue i elements to shift internal read/write indexes by i.
    for (size_t j = 0; j < i; j++) {
      ASSERT_NE(queue.write_ptr(), nullptr);
      queue.commit_write();
      ASSERT_NE(queue.read_ptr(), nullptr);
      queue.commit_read();
    }

    // Should be able to enqueue 99 elements.
    for (size_t j = 0; j < 99; j++) {
      ASSERT_NE(queue.write_ptr(), nullptr);
      queue.commit_write();
    }

    // Should be full now.
    ASSERT_EQ(queue.write_ptr(), nullptr);

    // Should be ablse to dequeue 99 elements.
    for (size_t j = 0; j < 99; j++) {
      ASSERT_NE(queue.read_ptr(), nullptr);
      queue.commit_read();
    }

    // Should be empty now.
    ASSERT_EQ(queue.read_ptr(), nullptr);
  }
}

TEST(Capacity_Is_Respected_300_3_2_1, BATCHED_SPSC_QUEUE) {
  size_t nb_slots = 300;
  size_t enqueue_batch_size = 3;
  size_t dequeue_batch_size = 2;
  size_t element_size = 1;
  auto buffer = std::make_unique<uint8_t>(nb_slots * element_size);

  for (size_t i = 0; i < 300 * 10; i++) {
    auto queue = Queue(nb_slots, enqueue_batch_size, dequeue_batch_size,
                       element_size, buffer.get());

    // Enqueue-Dequeue i elements to shift internal read/write indexes by i.
    // In this case, one has to enqueue twice before being able to dequeue
    // everything (three times).
    for (size_t j = 0; j < i; j++) {
      ASSERT_NE(queue.write_ptr(), nullptr);
      queue.commit_write();
      ASSERT_NE(queue.write_ptr(), nullptr);
      queue.commit_write();
      ASSERT_NE(queue.read_ptr(), nullptr);
      queue.commit_read();
      ASSERT_NE(queue.read_ptr(), nullptr);
      queue.commit_read();
      ASSERT_NE(queue.read_ptr(), nullptr);
      queue.commit_read();
    }

    // Should be able to enqueue 297 elements (99 3-elements enqueue).
    for (size_t j = 0; j < 99; j++) {
      ASSERT_NE(queue.write_ptr(), nullptr);
      queue.commit_write();
    }

    // Should be full now.
    ASSERT_EQ(queue.write_ptr(), nullptr);

    // Should be ablse to dequeue 298 elements (148 2-elements dequeue).
    for (size_t j = 0; j < 148; j++) {
      ASSERT_NE(queue.read_ptr(), nullptr);
      queue.commit_read();
    }

    // Should be empty now.
    ASSERT_EQ(queue.read_ptr(), nullptr);
  }
}

TEST(Capacity_Is_Respected_300_2_3_1, BATCHED_SPSC_QUEUE) {
  size_t nb_slots = 300;
  size_t enqueue_batch_size = 2;
  size_t dequeue_batch_size = 3;
  size_t element_size = 1;
  auto buffer = std::make_unique<uint8_t>(nb_slots * element_size);

  for (size_t i = 0; i < 300 * 10; i++) {
    auto queue = Queue(nb_slots, enqueue_batch_size, dequeue_batch_size,
                       element_size, buffer.get());

    // Enqueue-Dequeue i elements to shift internal read/write indexes by i.
    // In this case, one has to enqueue three times before being able to dequeue
    // everything (twice).
    for (size_t j = 0; j < i; j++) {
      ASSERT_NE(queue.write_ptr(), nullptr);
      queue.commit_write();
      ASSERT_NE(queue.write_ptr(), nullptr);
      queue.commit_write();
      ASSERT_NE(queue.write_ptr(), nullptr);
      queue.commit_write();
      ASSERT_NE(queue.read_ptr(), nullptr);
      queue.commit_read();
      ASSERT_NE(queue.read_ptr(), nullptr);
      queue.commit_read();
    }

    // Should be able to enqueue 298 elements (149 2-elements enqueue).
    for (size_t j = 0; j < 149; j++) {
      ASSERT_NE(queue.write_ptr(), nullptr);
      queue.commit_write();
    }

    // Should be full now.
    ASSERT_EQ(queue.write_ptr(), nullptr);

    // Should be ablse to dequeue 297 elements (99 3-elements dequeue).
    for (size_t j = 0; j < 99; j++) {
      ASSERT_NE(queue.read_ptr(), nullptr);
      queue.commit_read();
    }

    // Should be empty now.
    ASSERT_EQ(queue.read_ptr(), nullptr);
  }
}
} // namespace batched_spsc_queue
