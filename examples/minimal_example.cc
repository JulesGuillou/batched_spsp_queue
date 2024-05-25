#include "batched_spsc_queue.hh"
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <memory>

using Queue = batched_spsc_queue::Queue;

int main() {
  // Create the queue.
  size_t nb_slots = 128;
  size_t enqueue_batch_size = 1;
  size_t dequeue_batch_size = 2;
  size_t element_size = sizeof(uint8_t);
  auto buffer = std::make_unique<uint8_t[]>(nb_slots * element_size);
  auto queue = Queue(nb_slots, enqueue_batch_size, dequeue_batch_size,
                     element_size, buffer.get());

  uint8_t *write_ptr = nullptr;
  uint8_t *read_ptr = nullptr;

  // Enqueue 0.
  write_ptr = queue.write_ptr();
  *write_ptr = 0;
  queue.commit_write();

  // Enqueue 1.
  write_ptr = queue.write_ptr();
  *write_ptr = 1;
  queue.commit_write();

  // Dequeue and print. Cast to size_t or std::cout would print char not number.
  read_ptr = queue.read_ptr();
  std::cout << (size_t)read_ptr[0] << " " << (size_t)read_ptr[1] << std::endl;
  queue.commit_read();
}
