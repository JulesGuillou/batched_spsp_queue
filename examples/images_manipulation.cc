#include "batched_spsc_queue.hh"
#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <future>
#include <iostream>
#include <memory>
#include <ostream>

constexpr size_t IMG_WIDTH = 1024;
constexpr size_t IMG_HEIGHT = 1024;
constexpr size_t ENQUEUE_BATCH_SIZE = 8;
constexpr size_t DEQUEUE_BATCH_SIZE = 64;

using Queue = batched_spsc_queue::Queue;

void producer(Queue &queue) {
  // Enqueue 16 batch for a total of 128 images.
  for (size_t i = 0; i < 16; i++) {
    // Wait until there is enough space to perform an enqueue.
    uint8_t *batch_begin = nullptr;
    while (batch_begin == nullptr) {
      // write_ptr() returns a pointer to the next write location. In this case
      // this is the write location for the next batch to enqueue. One should
      // check for nullity as this happens when the queue is full.
      batch_begin = queue.write_ptr();
    }

    // Fill the batch of image with:
    // - 0s for the first image
    // - 1s for the second image
    // - ...
    // - (n-1) for the nth image
    for (size_t j = 0; j < ENQUEUE_BATCH_SIZE; j++) {
      size_t offset = IMG_WIDTH * IMG_HEIGHT * j;
      auto image_begin = batch_begin + offset;
      auto image_end = image_begin + IMG_WIDTH * IMG_HEIGHT;
      auto pixel_value = i * ENQUEUE_BATCH_SIZE + j;
      std::fill(image_begin, image_end, pixel_value);
    }

    // commit_write() performs the enqueue. The pointer provided by write_ptr()
    // is invalidated when commit_write() is called.
    queue.commit_write();
  }
}

void consumer(Queue &queue) {
  // Dequeue 2 batch for a total of 128 images.
  for (size_t i = 0; i < 2; i++) {
    // Wait until there is enough images to perform a dequeue.
    uint8_t *batch_begin = nullptr;
    while (batch_begin == nullptr) {
      // read_ptr() returns a pointer to the next read location. In this case
      // this is the read location for the next batch to dequeue. One should
      // check for nullity as this happens when the queue is empty.
      batch_begin = queue.read_ptr();
    }

    // Check the content of the batch is correct.
    for (size_t j = 0; j < DEQUEUE_BATCH_SIZE; j++) {
      size_t offset = IMG_WIDTH * IMG_HEIGHT * j;
      auto image_begin = batch_begin + offset;
      auto image_end = image_begin + IMG_WIDTH * IMG_HEIGHT;
      auto target_value = i * DEQUEUE_BATCH_SIZE + j;
      auto good = std::all_of(image_begin, image_end,
                              [target_value](uint8_t real_value) {
                                return real_value == target_value;
                              });
      if (!good)
        std::cout << "ERROR" << std::endl;
    }

    // commit_read() performs the enqueue. The pointer provided by read_ptr()
    // is invalidated when commit_read() is called.
    queue.commit_read();
  }
}

int main() {
  // Create the queue.
  size_t nb_slots = 128;
  size_t enqueue_batch_size = ENQUEUE_BATCH_SIZE;
  size_t dequeue_batch_size = DEQUEUE_BATCH_SIZE;
  size_t element_size = IMG_WIDTH * IMG_HEIGHT * sizeof(uint8_t);
  auto buffer = std::make_unique<uint8_t[]>(nb_slots * element_size);
  auto queue = Queue(nb_slots, enqueue_batch_size, dequeue_batch_size,
                     element_size, buffer.get());

  // Launch producer and consumer on separate threads.
  std::future<void> producer_thread = std::async(producer, std::ref(queue));
  std::future<void> consumer_thread = std::async(consumer, std::ref(queue));

  producer_thread.get();
  consumer_thread.get();
}
