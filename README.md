# Batched SPSC Queue

## Overview

The Batched SPSC (Single-Producer Single-Consumer) Queue is a high-performance queue implementation designed for scenarios where a single producer thread enqueues data and a single consumer thread dequeues data. This queue supports batched operations to improve performance, making it suitable for high-throughput applications.

## Features

- **Single-Producer Single-Consumer:** Optimized for scenarios with one producer and one consumer.
- **Batched Operations:** Supports batching of enqueue and dequeue operations to reduce synchronization overhead.
- **Circular Buffer:** Utilizes a circular buffer for efficient memory usage.
- **High Performance:** Designed for low-latency and high-throughput applications.
- **Sequential memory:** Sequential elements in a batch are guaranteed to be sequential in memory.

## Performance
```sh
Running ./benchmarks/batched_spsc_queue_benchmarks
Run on (16 X 2900 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x8)
  L1 Instruction 32 KiB (x8)
  L2 Unified 512 KiB (x8)
  L3 Unified 4096 KiB (x2)
Load Average: 0.21, 0.37, 0.36
-------------------------------------------------------------------------------------------------------
Benchmark                                             Time             CPU   Iterations UserCounters...
-------------------------------------------------------------------------------------------------------
BM_Enqueue_NoMemoryTransfer/min_time:5.000         2.96 ns         2.96 ns   2368530833 Bandwidth=644.129Ti/s Enqueues=337.709M/s
BM_Dequeue_NoMemoryTransfer/min_time:5.000         3.11 ns         3.11 ns   2251303793 Bandwidth=613.189Ti/s Dequeues=321.488M/s
BM_Enqueue_WithMemoryTransfer/min_time:5.000     209503 ns       209493 ns        33409 Bandwidth=9.32311Gi/s Enqueues=4.77343k/s
BM_Dequeue_WithMemoryTransfer/min_time:5.000     120045 ns       120039 ns        58865 Bandwidth=16.2708Gi/s Dequeues=8.33064k/s
```

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Performance](#performance)
- [Getting Started](#getting-started)
  - [Prerequisites](#prerequisites)
  - [Building the Project](#building-the-project)
  - [Running Examples](#running-examples)
  - [Running Tests](#running-tests)
  - [Running Benchmarks](#running-benchmarks)
- [Documentation](#documentation)
- [Usage](#usage)
  - [Minimal Example](#minimal-example)
  - [Other Examples](#other-examples)
- [Contributing](#contributing)
- [License](#license)

## Getting Started

### Prerequisites

- **CMake:** Version 3.10 or higher.
- **C++ Compiler:** GCC, Clang, or MSVC (cl).
- **Doxygen:** For generating documentation.

### Building the Project

1. **Clone the Repository:**
   ```sh
   git clone https://github.com/JulesGuillou/batched_spsp_queue.git
   cd batched_spsc_queue
   ```

2. **Configure the Project:**
   ```sh
   cmake -S . -B build
   ```

3. **Build the Project:**
   ```sh
   cmake --build build
   ```

### Running Examples

1. **Build and Run Image Manipulation Example:**
   ```sh
   cmake --build build --target images_manipulation
   ./build/examples/images_manipulation
   ```

2. **Build and Run Minimal Example:**
   ```sh
   cmake --build build --target minimal_example
   ./build/examples/minimal_example
   ```

### Running Tests

1. **Build and Run Tests:**
   ```sh
   cmake --build build --target batched_spsc_queue_tests
   ctest --test-dir build
   ```

### Running Benchmarks

1. **Build and Run Benchmarks:**
   ```sh
   cmake --build build --target batched_spsc_queue_benchmarks
   ./build/benchmarks/batched_spsc_queue_benchmarks
   ```

### Generating Documentation

1. **Generate Documentation:**
   ```sh
   cmake --build build --target doc_doxygen
   ```

2. **View Documentation:**
   Open `build/docs/html/index.html` in your web browser.

## Documentation

The project documentation is generated using Doxygen and provides detailed information about the classes, methods, and usage of the Batched SPSC Queue.

## Usage

### Minimal Example

```cpp
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
```

### Other Examples
Other examples can be found in the `examples` directory.

## Contributing

Contributions are welcome! Please follow these steps to contribute:

1. **Fork the Repository:**
   Click the "Fork" button at the top right of the repository page.

2. **Clone Your Fork:**
   ```sh
   git clone https://github.com/yourusername/batched_spsc_queue.git
   cd batched_spsc_queue
   ```

3. **Create a Branch:**
   ```sh
   git checkout -b feature/your-feature-name
   ```

4. **Make Your Changes:**
   Implement your feature or fix.

5. **Commit Your Changes:**
   ```sh
   git add .
   git commit -m "Add your commit message"
   ```

6. **Push to Your Fork:**
   ```sh
   git push origin feature/your-feature-name
   ```

7. **Create a Pull Request:**
   Go to the original repository and click the "New Pull Request" button.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
