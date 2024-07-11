#memory_order
youtube: https://www.youtube.com/watch?v=ZQFzMfHIxng&t=2959s
# memory_order_relaxed
![[Pasted image 20240710004003.png]]
With `memory_order_relaxed`, no memory fences are applied, allowing the compiler and CPU to reorder read/write instructions in any way that they believe will improve performance.
Of course, both CPU and compiler still need to follow the read and write order of one variable. 

`Example`: We have an atomic variable `x` and three non-atomic variables `a b c`.
```cpp
a = 1;
b += 2;
x.fetch_add(1, std::memory_order_relaxed);
c += 2;
```
Due to we are using relaxed memory ordered, so it's possible to CPU and compiler reorder ``all`` instructions to any way they want.
# memory_order_acquire
![[Pasted image 20240711225034.png]]
`Acquire` barrier guarantees that  all memory operations scheduled after the barrier in the program order become visible after the barrier.
- `All operations` is all `reads` and `writes`.
- `All opeartions` is all operations in all variable (not just on the atomic variable).
Reads and writes can't be reordered from after to before the barrier.
- Only for the thread that issued the barrier, which meant other thread still can applied different memory order.
![[Pasted image 20240711225056.png]]
`Example`: All operations after the `x.load(std::memory_order_acquire) which colered by yellow` can't move to before it. But it possible to have some operations before the `x.load` move to after it.
```cpp
std::atomic<int> index{0};
vector<int> a(2, 0);

void producer() {
	index.fetch_add(1, std::memory_order_relaxed);
}

void consumer() {
	index.load(std::memory_order_relaxed);
	a[index] = 3;
}

int main() {
	std::thread t1(producer);
	std::thread t2(consumer);
	t1.join();
	t2.join();
	return 0;
}
```
It's possible to get the `a[0] == 3` and `index == 1`. Because there isn't barrier applied to make sure that the `index.load` need to happen before the `a[index] = 3`.

```cpp
std::atomic<int> index{0};
vector<int> a(2, 0);

void producer() {
	index.fetch_add(1, std::memory_order_relaxed);
}

void consumer() {
	index.load(std::memory_order_acquire);
	a[index] = 3;
}

int main() {
	std::thread t1(producer);
	std::thread t2(consumer);
	t1.join();
	t2.join();
	return 0;
}
```
By applied `std::memory_order_acquire`, we can make sure that if the value of `index == 1` the array `a == {0, 3}`.

# memory_order_release
![[Pasted image 20240711231913.png]]
`Release` barrier guarantees that all memory operations scheduled before the barrier in the program order become visible before the barrier.
Reads and writes can't be reordered from before to after the barrier.
![[Pasted image 20240711231929.png]]
`Example`: All operations after the `x.store(std::memory_order_acquire) which colered by yellow` can't move to after it. But it possible to have some operations after the `x.load` move to before it.
```cpp
std::atomic<int> index{0};
vector<int> a(2, 0);

void producer() {
	a[index] = 3;
	index.fetch_add(1, std::memory_order_relaxed);
}

void consumer() {
	index.load(std::memory_order_acquire);
	std::cout << "a[" << index << "] = " << a[index];
}

int main() {
	std::thread t1(producer);
	std::thread t2(consumer);
	
	t1.join();
	t2.join();
	
	return 0;
}
```
It's possible for consumer to get `a[1] = 3`, due to we don't apply any barrier to prevent the order of operations in producer thread.
```cpp
std::atomic<int> index{0};
vector<int> a(2, 0);

void producer() {
	a[index] = 3;
	index.fetch_add(1, std::memory_order_release);
}

void consumer() {
	index.load(std::memory_order_acquire);
	std::cout << "a[" << index << "] = " << a[index];
}

int main() {
	std::thread t1(producer);
	std::thread t2(consumer);
	
	t1.join();
	t2.join();
	
	return 0;
}
```
By apply the `std::memory_order_release`, we can make sure that the array `a will always = {3, 0}` instead of `{0, 3}`.