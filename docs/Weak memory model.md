
## What is weak-memory model ?
The weak memory model allows CPU make a lot of optimization by reordering the order of instructions.
If there are no fence apply (memory_order_relaxed), the execution order isn't prevent.
Ex.
```// Thread 1:
r1 = y.load(std::memory_order_relaxed); // A
x.store(r1, std::memory_order_relaxed); // B
// Thread 2:
r2 = x.load(std::memory_order_relaxed); // C 
y.store(42, std::memory_order_relaxed); // D

https://en.cppreference.com/w/cpp/atomic/memory_order
```

In this example, the result is able to r1 = r2 = 42, due to there are no fence which prevent this scenario not happen
``` 
D -> A -> B -> C
```

Both CPU and compiler optimization may effect the order of instructions.
### Compiler Optimization
The compiler may reorder instructions as long as it preserves the program's semantics. Since `std::memory_order_relaxed` does not enforce any ordering constraints, the compiler might reorder the loads and stores within each thread independently.

- **Thread 1**: The compiler might reorder `y.load()` (A) and `x.store()` (B) if it determines that doing so could improve performance.
- **Thread 2**: Similarly, `x.load()` (C) and `y.store()` (D) could be reordered.

### Store Buffers (Cache) in CPUs
Store buffers are a feature of modern CPUs designed to improve performance by allowing a CPU to continue executing instructions without waiting for a store operation to complete. When a store operation is executed, the data is first placed in the store buffer. From there, it is eventually written to the main memory or the cache hierarchy. This allows the CPU to proceed with subsequent instructions while the store operation is still in progress.
#### Impact on Memory Visibility
Because store buffers are local to each CPU core, the data in the store buffer is not immediately visible to other CPU cores. This can lead to scenarios where one thread writes a value, but another thread does not immediately see that updated value because the store has not yet been committed to the shared memory.

Ex: 
1. **Load `y` in Thread 1 (A)**:
    - Thread 1 reads the value of `y` from memory. Assume `y` is initially `0`, so `r1 = 0`.
2. **Store `x` in Thread 1 (B)**:
    - Thread 1 stores the value of `r1` into `x`. This value (`0`) is placed into Thread 1's store buffer.
3. **Load `x` in Thread 2 (C)**:
    - Thread 2 reads the value of `x`. At this point, if the value in Thread 1's store buffer has not been flushed to memory, Thread 2 might see the initial value of `x`, which is `0`. So, `r2 = 0`.
4. **Store `y` in Thread 2 (D)**:
    - Thread 2 stores the value `42` into `y`. This value is placed into Thread 2's store buffer.

#### Potential Visibility Issues

Due to the store buffers, the following situations can occur:

- **Delayed Visibility**: The value `0` stored by Thread 1 into `x` may not be immediately visible to Thread 2 because it resides in Thread 1's store buffer.
- **Reordering by the CPU**: Even though the stores are placed in the store buffers in program order, they may be committed to memory in a different order. This is particularly true under relaxed memory ordering.

#### Memory Model and Consistency
Without any memory barriers or stronger memory orderings, there are no guarantees about the visibility order of these operations:

- **Thread 1** might store `0` to `x` (B) and proceed without waiting for this store to be visible to other threads.
- **Thread 2** might load `x` (C) before the store from Thread 1 (B) is visible, resulting in `r2 = 0`.
- **Thread 2** might store `42` to `y` (D) and similarly proceed without waiting for this store to be visible to other threads.
- **Thread 1** might load `y` (A) after the store from Thread 2 (D) is visible, resulting in `r1 = 42`.
## CPU Cache Line
Data in primary memory can be accessed faster than secondary memory but still, access times of primary memory are generally in a few microseconds, whereas the CPU is capable of performing operations in nanoseconds
### Working of Cache Memory
In order to understand the working of cache we must understand few points:
- Cache memory is faster, they can be accessed very fast
- Cache memory is smaller, a large amount of data cannot be stored

Whenever CPU needs any data it searches for corresponding data in the cache (fast process) if data is found, it processes the data according to instructions, however, if data is not found in the cache CPU search for that data in primary memory(slower process) and loads it into the cache. This ensures frequently accessed data are always found in the cache and hence minimizes the time required to access the data.
### How does Cache Memory Improve CPU Performance?
Cache memory improves CPU performance by reducing the time it takes for the CPU to access data. By storing frequently accessed data closer to the CPU, it minimizes the need for the CPU to fetch data from the slower main memory.
### What is a Cache Hit and a Cache Miss?
- **Cache Hit:**** When the CPU finds the required data in the cache memory, allowing for quick access. On searching in the cache if data is found, a cache hit has occurred.
- **Cache Miss:**** When the required data is not found in the cache, forcing the CPU to retrieve it from the slower main memory. On searching in the cache if data is not found, a cache miss has occurred.
### Types of Cache Memory
Multi-level caches are used to balance speed and cost. L1 cache is the fastest and most expensive per byte, so it’s small. L2 and L3 caches are progressively larger and slower, providing a larger total cache size while managing costs and maintaining reasonable speed.

1. ***L1 or Level 1 Cache:**** It is the first level of cache memory that is present inside the processor. It is present in a small amount inside every core of the processor separately. The size of this memory ranges from 2KB to 64 KB.
2. ***L2 or Level 2 Cache:**** It is the second level of cache memory that may present inside or outside the CPU. If not present inside the core, It can be shared between two cores depending upon the architecture and is connected to a processor with the high-speed bus. The size of memory ranges from 256 KB to 512 KB.
3. ***L3 or Level 3 Cache:**** It is the third level of cache memory that is present outside the CPU and is shared by all the cores of the CPU. Some high processors may have this cache. This cache is used to increase the performance of the L2 and L1 cache. The size of this memory ranges from 1 MB to 8MB.

### Cache Coherence
In multiprocessor system where many processes needs a copy of same memory block, the maintenance of consistency among these copies raises a problem referred to as **Cache Coherence Problem.***

This occurs mainly due to these causes:
- Sharing of writable data.
- Process migration.
- Inconsistency due to I/O.

![[Pasted image 20240708003452.png]]