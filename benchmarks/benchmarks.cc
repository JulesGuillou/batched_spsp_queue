#include "batched_spsc_queue.hh"
#include <benchmark/benchmark.h>
#include <cstdint>
#include <cstring>
#include <memory>

using Queue = batched_spsc_queue::Queue;

static void BM_Enqueue_NoMemoryTransfer(benchmark::State &state) {
  size_t nb_slots = 1000;
  size_t enqueue_batch_size = 8;
  size_t dequeue_batch_size = 8;
  size_t element_size = sizeof(uint8_t);
  auto buffer = std::make_unique<uint8_t[]>(nb_slots * element_size);
  auto queue = Queue(nb_slots, enqueue_batch_size, dequeue_batch_size,
                     element_size, buffer.get());

  for (auto _ : state) {
    uint8_t *batch_begin = queue.write_ptr();
    if (batch_begin == nullptr) {
      queue.reset();
      batch_begin = queue.write_ptr();
    }

    queue.commit_write();
  }

  state.counters["Enqueues"] =
      benchmark::Counter(static_cast<double>(state.iterations()), benchmark::Counter::kIsRate);

  state.counters["Bandwidth"] = benchmark::Counter(
      static_cast<double>(state.iterations()) * 8 * 512 * 512, benchmark::Counter::kIsRate,
      benchmark::Counter::kIs1024);
}

static void BM_Dequeue_NoMemoryTransfer(benchmark::State &state) {
  size_t nb_slots = 1000;
  size_t enqueue_batch_size = 8;
  size_t dequeue_batch_size = 8;
  size_t element_size = sizeof(uint8_t);
  auto buffer = std::make_unique<uint8_t[]>(nb_slots * element_size);
  auto queue = Queue(nb_slots, enqueue_batch_size, dequeue_batch_size,
                     element_size, buffer.get());

  queue.fill();

  for (auto _ : state) {
    uint8_t *batch_begin = queue.read_ptr();
    if (batch_begin == nullptr) {
      queue.fill();
      batch_begin = queue.read_ptr();
    }

    queue.commit_read();
  }

  state.counters["Dequeues"] =
      benchmark::Counter(static_cast<double>(state.iterations()), benchmark::Counter::kIsRate);

  state.counters["Bandwidth"] = benchmark::Counter(
      static_cast<double>(state.iterations()) * 8 * 512 * 512, benchmark::Counter::kIsRate,
      benchmark::Counter::kIs1024);
}

static void BM_Enqueue_WithMemoryTransfer(benchmark::State &state) {
  size_t nb_slots = 1000;
  size_t enqueue_batch_size = 8;
  size_t dequeue_batch_size = 8;
  size_t element_size = 512 * 512 * sizeof(uint8_t);
  auto buffer = std::make_unique<uint8_t[]>(nb_slots * element_size);
  auto queue = Queue(nb_slots, enqueue_batch_size, dequeue_batch_size,
                     element_size, buffer.get());

  uint8_t source[8 * 512 * 512];
  memset(source, 0, 8 * 512 * 512);

  benchmark::DoNotOptimize(source);
  benchmark::DoNotOptimize(buffer.get());

  for (auto _ : state) {
    uint8_t *batch_begin = queue.write_ptr();
    if (batch_begin == nullptr) {
      queue.reset();
      batch_begin = queue.write_ptr();
    }

    memcpy(batch_begin, source, 8 * 512 * 512);
    queue.commit_write();
  }

  state.counters["Enqueues"] =
      benchmark::Counter(static_cast<double>(state.iterations()), benchmark::Counter::kIsRate);

  state.counters["Bandwidth"] = benchmark::Counter(
      static_cast<double>(state.iterations()) * 8 * 512 * 512, benchmark::Counter::kIsRate,
      benchmark::Counter::kIs1024);
}

static void BM_Dequeue_WithMemoryTransfer(benchmark::State &state) {
  size_t nb_slots = 1000;
  size_t enqueue_batch_size = 8;
  size_t dequeue_batch_size = 8;
  size_t element_size = 512 * 512 * sizeof(uint8_t);
  auto buffer = std::make_unique<uint8_t[]>(nb_slots * element_size);
  auto queue = Queue(nb_slots, enqueue_batch_size, dequeue_batch_size,
                     element_size, buffer.get());

  uint8_t dest[8 * 512 * 512];
  queue.fill();

  benchmark::DoNotOptimize(dest);
  benchmark::DoNotOptimize(buffer.get());

  for (auto _ : state) {
    uint8_t *batch_begin = queue.read_ptr();
    if (batch_begin == nullptr) {
      queue.fill();
      batch_begin = queue.read_ptr();
    }

    memcpy(dest, batch_begin, 8 * 512 * 512);
    queue.commit_read();
  }

  state.counters["Dequeues"] =
      benchmark::Counter(static_cast<double>(state.iterations()), benchmark::Counter::kIsRate);

  state.counters["Bandwidth"] = benchmark::Counter(
      static_cast<double>(state.iterations()) * 8 * 512 * 512, benchmark::Counter::kIsRate,
      benchmark::Counter::kIs1024);
}

BENCHMARK(BM_Enqueue_NoMemoryTransfer)->MinTime(5.0);
BENCHMARK(BM_Dequeue_NoMemoryTransfer)->MinTime(5.0);
BENCHMARK(BM_Enqueue_WithMemoryTransfer)->MinTime(5.0);
BENCHMARK(BM_Dequeue_WithMemoryTransfer)->MinTime(5.0);

BENCHMARK_MAIN();
