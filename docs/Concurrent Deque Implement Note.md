paper name: Correct and Efficient Work-Stealing for Weak Memory Models

## Paper Note  
###
- For any given deque, push and pop operations execute on a single thread. Concurrency can only occur between one execution of push or take in the owner thread, and one or more executions of steal in different threads.
- 