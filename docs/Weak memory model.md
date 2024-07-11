#Memory #Cache
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
https://redis.io/glossary/cache-coherence/#:~:text=Cache%20coherence%20refers%20to%20the,cache%20memory%20to%20improve%20performance.

In multiprocessor system where many processes needs a copy of same memory block, the maintenance of consistency among these copies raises a problem referred to as **Cache Coherence Problem.***

This occurs mainly due to these causes:
- Sharing of writable data.
- Process migration.
- Inconsistency due to I/O.

![[Pasted image 20240708003452.png]]

### How main memory loaded to CPU Cache? 
When a CPU fetches data from memory, it typically loads more than just the specific piece of data it needs. This is done to take advantage of spatial locality, a principle that suggests that data near a recently accessed memory location is likely to be accessed soon. Here’s a more detailed explanation of how this process works:
#### Cache Line
- **Cache Line**: The smallest unit of data transfer between the main memory and the cache is called a cache line or cache block. The size of a cache line is typically 32 to 256 bytes, depending on the architecture.
#### Loading Data into Cache
1. **Cache Miss**: When the CPU cannot find the required data in the cache (L1, L2, or L3), it initiates a cache miss.
2. **Fetching Cache Line**: Instead of fetching just the requested data, the CPU fetches an entire cache line that includes the requested data and adjacent data from the main memory.
3. **Updating Cache**: The fetched cache line is loaded into the appropriate cache level (L1, L2, or L3). If necessary, existing data in the cache is evicted based on the cache’s replacement policy (e.g., LRU - Least Recently Used).
4. **Data Propagation**:
    - The cache line is first loaded into the L3 cache.
    - If the data is accessed again soon, it is moved to the L2 cache.
    - If it continues to be accessed frequently, it will eventually be moved to the L1 cache.
#### Example of Cache Line Loading
Let’s say the CPU needs to access data at memory address 0x1000:

1. **Cache Miss**: The CPU checks the L1 cache for data at 0x1000 and does not find it (miss).
2. **Fetch Cache Line**: The CPU fetches a cache line (e.g., 64 bytes) starting from address 0x1000. This means addresses 0x1000 through 0x103F are fetched.
3. **Update Cache**:
    - This cache line is loaded into the L3 cache.
    - If the CPU accesses any data within this range (0x1000 to 0x103F), it will result in a hit in the L3 cache, and the data will be promoted to the L2 cache.
    - If the data is accessed frequently, it will be further promoted to the L1 cache.
#### Benefits of Loading Cache Lines
- **Spatial Locality**: By loading a cache line, the CPU anticipates future accesses to nearby data, reducing the number of cache misses.
- **Efficiency**: Transferring larger blocks of data (cache lines) is more efficient than transferring many small pieces of data due to the way modern memory systems and buses are optimized.
#### Cache Replacement Policies
When a new cache line is loaded into a cache, an existing cache line might need to be evicted. Replacement policies determine which cache line to evict:
- **Least Recently Used (LRU)**: Evicts the least recently accessed cache line.
- **First-In, First-Out (FIFO)**: Evicts the oldest cache line.
- **Random Replacement**: Evicts a randomly selected cache line.
- **Least Frequently Used (LFU)**: Evicts the least frequently accessed cache line.