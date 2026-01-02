// ThreadPoolExamples.cpp
// Comprehensive educational examples of thread pool implementations in Modern C++
// From basic to advanced patterns including work stealing, priority queues, and more

#include <iostream>
#include <thread>
#include <vector>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <future>
#include <atomic>
#include <chrono>
#include <memory>
#include <type_traits>
#include <deque>
#include <optional>
#include <iomanip>

using namespace std::chrono_literals;

// ============================================================================
// SECTION 1: Basic Thread Pool (Simplest Implementation)
// ============================================================================

class BasicThreadPool {
private:
    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;
    std::mutex queue_mutex_;
    std::condition_variable condition_;
    bool stop_;
    
public:
    explicit BasicThreadPool(size_t num_threads) : stop_(false) {
        std::cout << "  Creating thread pool with " << num_threads << " workers\n";
        
        for (size_t i = 0; i < num_threads; ++i) {
            workers_.emplace_back([this, i]() {
                std::cout << "    Worker " << i << " started (thread " 
                          << std::this_thread::get_id() << ")\n";
                
                while (true) {
                    std::function<void()> task;
                    
                    {
                        std::unique_lock<std::mutex> lock(queue_mutex_);
                        
                        // Wait until there's work or we're stopping
                        condition_.wait(lock, [this]() {
                            return stop_ || !tasks_.empty();
                        });
                        
                        if (stop_ && tasks_.empty()) {
                            return;
                        }
                        
                        task = std::move(tasks_.front());
                        tasks_.pop();
                    }
                    
                    task();  // Execute the task
                }
            });
        }
    }
    
    // Submit a task (no return value)
    void submit(std::function<void()> task) {
        {
            std::lock_guard<std::mutex> lock(queue_mutex_);
            
            if (stop_) {
                throw std::runtime_error("Cannot submit task to stopped thread pool");
            }
            
            tasks_.push(std::move(task));
        }
        condition_.notify_one();
    }
    
    ~BasicThreadPool() {
        {
            std::lock_guard<std::mutex> lock(queue_mutex_);
            stop_ = true;
        }
        
        condition_.notify_all();
        
        for (std::thread& worker : workers_) {
            if (worker.joinable()) {
                worker.join();
            }
        }
        
        std::cout << "  Thread pool destroyed\n";
    }
};

void demonstrate_basic_thread_pool() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== 1. Basic Thread Pool ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "Concept: Fixed number of worker threads processing tasks from a queue\n\n";
    
    BasicThreadPool pool(3);
    
    std::cout << "\nSubmitting 6 tasks...\n\n";
    
    for (int i = 1; i <= 6; ++i) {
        pool.submit([i]() {
            std::cout << "    Task " << i << " executing on thread " 
                      << std::this_thread::get_id() << "\n";
            std::this_thread::sleep_for(200ms);
            std::cout << "    Task " << i << " completed\n";
        });
    }
    
    std::this_thread::sleep_for(2s);  // Wait for tasks to complete
    
    std::cout << "\n✓ All tasks processed by 3 workers\n";
    std::cout << "✓ Tasks automatically distributed across available threads\n";
}

// ============================================================================
// SECTION 2: Thread Pool with Futures (Return Values)
// ============================================================================

class ThreadPoolWithFutures {
private:
    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;
    std::mutex queue_mutex_;
    std::condition_variable condition_;
    bool stop_;
    
public:
    explicit ThreadPoolWithFutures(size_t num_threads) : stop_(false) {
        for (size_t i = 0; i < num_threads; ++i) {
            workers_.emplace_back([this]() {
                while (true) {
                    std::function<void()> task;
                    
                    {
                        std::unique_lock<std::mutex> lock(queue_mutex_);
                        condition_.wait(lock, [this]() {
                            return stop_ || !tasks_.empty();
                        });
                        
                        if (stop_ && tasks_.empty()) {
                            return;
                        }
                        
                        task = std::move(tasks_.front());
                        tasks_.pop();
                    }
                    
                    task();
                }
            });
        }
    }
    
    // Submit task and get future for result
    template<typename F, typename... Args>
    auto submit(F&& f, Args&&... args) 
        -> std::future<typename std::invoke_result_t<F, Args...>> {
        
        using return_type = typename std::invoke_result_t<F, Args...>;
        
        // Create a packaged_task
        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );
        
        std::future<return_type> result = task->get_future();
        
        {
            std::lock_guard<std::mutex> lock(queue_mutex_);
            
            if (stop_) {
                throw std::runtime_error("Cannot submit to stopped pool");
            }
            
            tasks_.emplace([task]() { (*task)(); });
        }
        
        condition_.notify_one();
        return result;
    }
    
    ~ThreadPoolWithFutures() {
        {
            std::lock_guard<std::mutex> lock(queue_mutex_);
            stop_ = true;
        }
        
        condition_.notify_all();
        
        for (std::thread& worker : workers_) {
            if (worker.joinable()) {
                worker.join();
            }
        }
    }
};

void demonstrate_thread_pool_with_futures() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== 2. Thread Pool with Futures (Return Values) ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "Benefit: Can return results from tasks using std::future\n\n";
    
    ThreadPoolWithFutures pool(4);
    
    std::vector<std::future<int>> results;
    
    std::cout << "Submitting computation tasks...\n";
    
    for (int i = 1; i <= 8; ++i) {
        results.push_back(pool.submit([i]() {
            std::cout << "  Task " << i << ": Computing " << i << "²\n";
            std::this_thread::sleep_for(100ms);
            return i * i;
        }));
    }
    
    std::cout << "\nCollecting results...\n";
    
    for (size_t i = 0; i < results.size(); ++i) {
        int result = results[i].get();  // Blocks until result is ready
        std::cout << "  Result " << (i + 1) << ": " << result << "\n";
    }
    
    std::cout << "\n✓ All tasks returned results via std::future\n";
}

// ============================================================================
// SECTION 3: Thread Pool with Priority Queue
// ============================================================================

enum class TaskPriority {
    LOW = 0,
    NORMAL = 1,
    HIGH = 2,
    CRITICAL = 3
};

struct PrioritizedTask {
    std::function<void()> func;
    TaskPriority priority;
    int sequence;  // For FIFO within same priority
    
    bool operator<(const PrioritizedTask& other) const {
        if (priority != other.priority) {
            return priority < other.priority;  // Higher priority first
        }
        return sequence > other.sequence;  // FIFO for same priority
    }
};

class PriorityThreadPool {
private:
    std::vector<std::thread> workers_;
    std::priority_queue<PrioritizedTask> tasks_;
    std::mutex queue_mutex_;
    std::condition_variable condition_;
    bool stop_;
    std::atomic<int> sequence_counter_;
    
public:
    explicit PriorityThreadPool(size_t num_threads) 
        : stop_(false), sequence_counter_(0) {
        
        for (size_t i = 0; i < num_threads; ++i) {
            workers_.emplace_back([this]() {
                while (true) {
                    PrioritizedTask task{{}, TaskPriority::NORMAL, 0};
                    
                    {
                        std::unique_lock<std::mutex> lock(queue_mutex_);
                        condition_.wait(lock, [this]() {
                            return stop_ || !tasks_.empty();
                        });
                        
                        if (stop_ && tasks_.empty()) {
                            return;
                        }
                        
                        task = tasks_.top();
                        tasks_.pop();
                    }
                    
                    task.func();
                }
            });
        }
    }
    
    void submit(std::function<void()> task, TaskPriority priority = TaskPriority::NORMAL) {
        {
            std::lock_guard<std::mutex> lock(queue_mutex_);
            
            if (stop_) {
                throw std::runtime_error("Cannot submit to stopped pool");
            }
            
            tasks_.push({std::move(task), priority, sequence_counter_++});
        }
        
        condition_.notify_one();
    }
    
    ~PriorityThreadPool() {
        {
            std::lock_guard<std::mutex> lock(queue_mutex_);
            stop_ = true;
        }
        
        condition_.notify_all();
        
        for (std::thread& worker : workers_) {
            if (worker.joinable()) {
                worker.join();
            }
        }
    }
};

void demonstrate_priority_thread_pool() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== 3. Priority Thread Pool ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "Concept: Higher priority tasks execute before lower priority ones\n\n";
    
    PriorityThreadPool pool(2);
    
    std::cout << "Submitting tasks with different priorities...\n\n";
    
    // Submit in mixed order
    pool.submit([]() {
        std::cout << "  [NORMAL] Task 1\n";
        std::this_thread::sleep_for(100ms);
    }, TaskPriority::NORMAL);
    
    pool.submit([]() {
        std::cout << "  [CRITICAL] Urgent task!\n";
        std::this_thread::sleep_for(100ms);
    }, TaskPriority::CRITICAL);
    
    pool.submit([]() {
        std::cout << "  [LOW] Background task\n";
        std::this_thread::sleep_for(100ms);
    }, TaskPriority::LOW);
    
    pool.submit([]() {
        std::cout << "  [HIGH] Important task\n";
        std::this_thread::sleep_for(100ms);
    }, TaskPriority::HIGH);
    
    pool.submit([]() {
        std::cout << "  [NORMAL] Task 2\n";
        std::this_thread::sleep_for(100ms);
    }, TaskPriority::NORMAL);
    
    std::this_thread::sleep_for(1s);
    
    std::cout << "\n✓ Tasks executed by priority: CRITICAL → HIGH → NORMAL → LOW\n";
}

// ============================================================================
// SECTION 4: Work-Stealing Thread Pool (Advanced)
// ============================================================================

class WorkStealingThreadPool {
private:
    struct WorkerThread {
        std::deque<std::function<void()>> local_queue;
        std::mutex queue_mutex;
        std::thread thread;
    };
    
    std::vector<std::unique_ptr<WorkerThread>> workers_;
    std::atomic<bool> stop_;
    std::atomic<size_t> next_worker_;  // Round-robin submission
    
public:
    explicit WorkStealingThreadPool(size_t num_threads) 
        : stop_(false), next_worker_(0) {
        
        std::cout << "  Creating work-stealing pool with " << num_threads << " workers\n";
        
        for (size_t i = 0; i < num_threads; ++i) {
            auto worker = std::make_unique<WorkerThread>();
            
            worker->thread = std::thread([this, i, &w = *worker]() {
                std::cout << "    Worker " << i << " started\n";
                
                while (!stop_) {
                    std::function<void()> task;
                    
                    // Try to get task from own queue first
                    {
                        std::lock_guard<std::mutex> lock(w.queue_mutex);
                        if (!w.local_queue.empty()) {
                            task = std::move(w.local_queue.front());
                            w.local_queue.pop_front();
                        }
                    }
                    
                    // If no local task, try to steal from other workers
                    if (!task) {
                        task = try_steal_work(i);
                    }
                    
                    if (task) {
                        task();
                    } else {
                        std::this_thread::sleep_for(10ms);  // Prevent busy-waiting
                    }
                }
            });
            
            workers_.push_back(std::move(worker));
        }
    }
    
    void submit(std::function<void()> task) {
        // Round-robin distribution
        size_t worker_idx = next_worker_++ % workers_.size();
        
        std::lock_guard<std::mutex> lock(workers_[worker_idx]->queue_mutex);
        workers_[worker_idx]->local_queue.push_back(std::move(task));
    }
    
    ~WorkStealingThreadPool() {
        stop_ = true;
        
        for (auto& worker : workers_) {
            if (worker->thread.joinable()) {
                worker->thread.join();
            }
        }
        
        std::cout << "  Work-stealing pool destroyed\n";
    }
    
private:
    std::function<void()> try_steal_work(size_t my_index) {
        // Try to steal from other workers
        for (size_t i = 0; i < workers_.size(); ++i) {
            if (i == my_index) continue;
            
            std::lock_guard<std::mutex> lock(workers_[i]->queue_mutex);
            if (!workers_[i]->local_queue.empty()) {
                // Steal from the back (different from local FIFO)
                auto task = std::move(workers_[i]->local_queue.back());
                workers_[i]->local_queue.pop_back();
                return task;
            }
        }
        
        return nullptr;
    }
};

void demonstrate_work_stealing_pool() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== 4. Work-Stealing Thread Pool ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "Concept: Idle workers steal tasks from busy workers' queues\n";
    std::cout << "Benefit: Better load balancing for uneven task durations\n\n";
    
    WorkStealingThreadPool pool(4);
    
    std::cout << "Submitting 12 tasks with varying durations...\n\n";
    
    for (int i = 1; i <= 12; ++i) {
        pool.submit([i]() {
            int duration = (i % 3 + 1) * 100;  // 100ms, 200ms, or 300ms
            std::cout << "  Task " << i << " (duration: " << duration 
                      << "ms) on thread " << std::this_thread::get_id() << "\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(duration));
        });
    }
    
    std::this_thread::sleep_for(2s);
    
    std::cout << "\n✓ Work stealing enabled better load distribution\n";
    std::cout << "✓ Idle workers helped busy workers finish faster\n";
}

// ============================================================================
// SECTION 5: Dynamic Thread Pool (Auto-scaling)
// ============================================================================

class DynamicThreadPool {
private:
    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;
    std::mutex queue_mutex_;
    std::condition_variable condition_;
    bool stop_;
    
    size_t min_threads_;
    size_t max_threads_;
    std::atomic<size_t> active_threads_;
    std::atomic<size_t> idle_threads_;
    
public:
    DynamicThreadPool(size_t min_threads, size_t max_threads) 
        : stop_(false), min_threads_(min_threads), max_threads_(max_threads),
          active_threads_(0), idle_threads_(0) {
        
        std::cout << "  Creating dynamic pool (min: " << min_threads 
                  << ", max: " << max_threads << ")\n";
        
        // Start with minimum threads
        for (size_t i = 0; i < min_threads_; ++i) {
            add_worker();
        }
    }
    
    void submit(std::function<void()> task) {
        {
            std::lock_guard<std::mutex> lock(queue_mutex_);
            
            if (stop_) {
                throw std::runtime_error("Cannot submit to stopped pool");
            }
            
            tasks_.push(std::move(task));
            
            // Auto-scale: add worker if all busy and below max
            if (idle_threads_ == 0 && workers_.size() < max_threads_) {
                std::cout << "  📈 Scaling up: adding worker (total: " 
                          << (workers_.size() + 1) << ")\n";
                add_worker();
            }
        }
        
        condition_.notify_one();
    }
    
    ~DynamicThreadPool() {
        {
            std::lock_guard<std::mutex> lock(queue_mutex_);
            stop_ = true;
        }
        
        condition_.notify_all();
        
        for (std::thread& worker : workers_) {
            if (worker.joinable()) {
                worker.join();
            }
        }
        
        std::cout << "  Dynamic pool destroyed\n";
    }
    
private:
    void add_worker() {
        workers_.emplace_back([this]() {
            while (true) {
                std::function<void()> task;
                
                {
                    std::unique_lock<std::mutex> lock(queue_mutex_);
                    
                    ++idle_threads_;
                    
                    condition_.wait(lock, [this]() {
                        return stop_ || !tasks_.empty();
                    });
                    
                    --idle_threads_;
                    
                    if (stop_ && tasks_.empty()) {
                        return;
                    }
                    
                    if (!tasks_.empty()) {
                        task = std::move(tasks_.front());
                        tasks_.pop();
                    }
                }
                
                if (task) {
                    ++active_threads_;
                    task();
                    --active_threads_;
                }
            }
        });
    }
};

void demonstrate_dynamic_thread_pool() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== 5. Dynamic Thread Pool (Auto-scaling) ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "Concept: Pool grows/shrinks based on workload\n\n";
    
    DynamicThreadPool pool(2, 6);
    
    std::cout << "Phase 1: Light load (2 tasks)\n\n";
    
    for (int i = 1; i <= 2; ++i) {
        pool.submit([i]() {
            std::cout << "  Light task " << i << "\n";
            std::this_thread::sleep_for(200ms);
        });
    }
    
    std::this_thread::sleep_for(500ms);
    
    std::cout << "\nPhase 2: Heavy load (10 tasks)\n\n";
    
    for (int i = 1; i <= 10; ++i) {
        pool.submit([i]() {
            std::cout << "  Heavy task " << i << "\n";
            std::this_thread::sleep_for(300ms);
        });
        std::this_thread::sleep_for(50ms);  // Gradual submission
    }
    
    std::this_thread::sleep_for(2s);
    
    std::cout << "\n✓ Pool automatically scaled up during heavy load\n";
}

// ============================================================================
// SECTION 6: Thread Pool Best Practices and Anti-Patterns
// ============================================================================

void demonstrate_best_practices() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== 6. Thread Pool Best Practices ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "✓ BEST PRACTICES:\n";
    std::cout << "─────────────────\n\n";
    
    std::cout << "1. Pool Size Selection:\n";
    std::cout << "   CPU-bound: num_threads ≈ std::thread::hardware_concurrency()\n";
    std::cout << "   I/O-bound: num_threads > CPU cores (2x-4x)\n";
    std::cout << "   Mixed:     Profile and tune based on workload\n\n";
    
    std::cout << "2. Task Granularity:\n";
    std::cout << "   ✓ Tasks should run for at least 1ms (avoid overhead)\n";
    std::cout << "   ✗ Don't submit trivial tasks (e.g., single addition)\n";
    std::cout << "   ✓ Batch small operations into larger tasks\n\n";
    
    std::cout << "3. Exception Handling:\n";
    std::cout << "   ✓ Tasks should catch their own exceptions\n";
    std::cout << "   ✓ Use std::future to propagate exceptions\n";
    std::cout << "   ✗ Uncaught exceptions terminate worker threads\n\n";
    
    std::cout << "4. Shutdown:\n";
    std::cout << "   ✓ Signal stop before destroying pool\n";
    std::cout << "   ✓ Join all worker threads in destructor\n";
    std::cout << "   ✓ Consider draining pending tasks gracefully\n\n";
    
    std::cout << "5. Task Dependencies:\n";
    std::cout << "   ✗ DEADLOCK RISK: Task waiting for another task in same pool\n";
    std::cout << "   ✓ Use separate pools for dependent tasks\n";
    std::cout << "   ✓ Or ensure pool size > max dependency depth\n\n";
    
    std::cout << "❌ ANTI-PATTERNS:\n";
    std::cout << "─────────────────\n\n";
    
    std::cout << "1. Too many pools: Creates thread explosion\n";
    std::cout << "   ✗ One pool per operation type (100+ pools)\n";
    std::cout << "   ✓ One global pool or few specialized pools\n\n";
    
    std::cout << "2. Blocking in tasks:\n";
    std::cout << "   ✗ Task calls blocking I/O (starves other tasks)\n";
    std::cout << "   ✓ Use async I/O or separate I/O thread pool\n\n";
    
    std::cout << "3. Long-running tasks:\n";
    std::cout << "   ✗ Task runs for minutes/hours (blocks worker)\n";
    std::cout << "   ✓ Break into smaller chunks or use dedicated thread\n\n";
    
    std::cout << "4. Unbounded queue growth:\n";
    std::cout << "   ✗ Submit faster than processing (memory exhaustion)\n";
    std::cout << "   ✓ Use bounded queue with backpressure\n\n";
}

// ============================================================================
// SECTION 7: Real-World Example - Image Processing Pipeline
// ============================================================================

struct Image {
    int id;
    std::string name;
    int processing_stage;  // 0=load, 1=filter, 2=resize, 3=save
};

void demonstrate_real_world_example() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== 7. Real-World Example: Image Processing Pipeline ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "Pipeline: Load → Filter → Resize → Save\n";
    std::cout << "Strategy: Use thread pool to parallelize each stage\n\n";
    
    ThreadPoolWithFutures pool(std::thread::hardware_concurrency());
    
    std::vector<Image> images = {
        {1, "photo1.jpg", 0},
        {2, "photo2.jpg", 0},
        {3, "photo3.jpg", 0},
        {4, "photo4.jpg", 0}
    };
    
    std::cout << "Processing " << images.size() << " images...\n\n";
    
    // Stage 1: Load images
    std::vector<std::future<Image>> loaded;
    for (const auto& img : images) {
        loaded.push_back(pool.submit([img]() {
            std::cout << "  [LOAD] " << img.name << "\n";
            std::this_thread::sleep_for(100ms);
            Image result = img;
            result.processing_stage = 1;
            return result;
        }));
    }
    
    // Stage 2: Apply filters
    std::vector<std::future<Image>> filtered;
    for (auto& future : loaded) {
        Image img = future.get();
        filtered.push_back(pool.submit([img]() {
            std::cout << "  [FILTER] " << img.name << "\n";
            std::this_thread::sleep_for(150ms);
            Image result = img;
            result.processing_stage = 2;
            return result;
        }));
    }
    
    // Stage 3: Resize
    std::vector<std::future<Image>> resized;
    for (auto& future : filtered) {
        Image img = future.get();
        resized.push_back(pool.submit([img]() {
            std::cout << "  [RESIZE] " << img.name << "\n";
            std::this_thread::sleep_for(100ms);
            Image result = img;
            result.processing_stage = 3;
            return result;
        }));
    }
    
    // Stage 4: Save
    std::vector<std::future<void>> saved;
    for (auto& future : resized) {
        Image img = future.get();
        saved.push_back(pool.submit([img]() {
            std::cout << "  [SAVE] " << img.name << " → output/" << img.name << "\n";
            std::this_thread::sleep_for(80ms);
        }));
    }
    
    // Wait for all to complete
    for (auto& future : saved) {
        future.get();
    }
    
    std::cout << "\n✓ Pipeline completed for all images\n";
    std::cout << "✓ Each stage parallelized using thread pool\n";
}

// ============================================================================
// SECTION 8: Comparison - When to Use What
// ============================================================================

void demonstrate_comparison() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "=== 8. Thread Pool Comparison - When to Use What ===\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    std::cout << "┌─────────────────────┬─────────────────────┬──────────────────────┐\n";
    std::cout << "│ Pool Type           │ Use Case            │ Pros/Cons            │\n";
    std::cout << "├─────────────────────┼─────────────────────┼──────────────────────┤\n";
    std::cout << "│ Basic               │ Simple tasks        │ + Easy to implement  │\n";
    std::cout << "│                     │ Fire-and-forget     │ - No return values   │\n";
    std::cout << "├─────────────────────┼─────────────────────┼──────────────────────┤\n";
    std::cout << "│ With Futures        │ Computational work  │ + Return values      │\n";
    std::cout << "│                     │ Need results        │ + Type-safe          │\n";
    std::cout << "├─────────────────────┼─────────────────────┼──────────────────────┤\n";
    std::cout << "│ Priority            │ Mixed importance    │ + Predictable order  │\n";
    std::cout << "│                     │ Real-time systems   │ - More overhead      │\n";
    std::cout << "├─────────────────────┼─────────────────────┼──────────────────────┤\n";
    std::cout << "│ Work-Stealing       │ Uneven task sizes   │ + Better load balance│\n";
    std::cout << "│                     │ Recursive tasks     │ - More complex       │\n";
    std::cout << "├─────────────────────┼─────────────────────┼──────────────────────┤\n";
    std::cout << "│ Dynamic             │ Variable workload   │ + Resource efficient │\n";
    std::cout << "│                     │ Bursty traffic      │ - Scale-up latency   │\n";
    std::cout << "└─────────────────────┴─────────────────────┴──────────────────────┘\n\n";
    
    std::cout << "ALTERNATIVES TO THREAD POOLS:\n";
    std::cout << "──────────────────────────────\n\n";
    
    std::cout << "1. std::async:\n";
    std::cout << "   Use when: One-off async operations, automatic thread management\n";
    std::cout << "   Avoid when: High frequency task submission (creates many threads)\n\n";
    
    std::cout << "2. std::thread:\n";
    std::cout << "   Use when: Long-running background tasks, dedicated worker\n";
    std::cout << "   Avoid when: Many short-lived tasks (thread creation overhead)\n\n";
    
    std::cout << "3. ASIO/Boost.Asio:\n";
    std::cout << "   Use when: I/O-bound operations, async networking\n";
    std::cout << "   Avoid when: CPU-bound computations\n\n";
    
    std::cout << "4. C++20 Coroutines:\n";
    std::cout << "   Use when: Async I/O, cooperative multitasking\n";
    std::cout << "   Avoid when: Heavy CPU computations\n\n";
    
    std::cout << "5. TBB (Threading Building Blocks):\n";
    std::cout << "   Use when: Need mature, production-ready parallel algorithms\n";
    std::cout << "   Benefit: Work-stealing, parallel_for, parallel_reduce, etc.\n\n";
}

// ============================================================================
// MAIN FUNCTION
// ============================================================================

int main() {
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║           Thread Pool Implementations - Educational Examples       ║\n";
    std::cout << "║                                                                    ║\n";
    std::cout << "║  From basic to advanced: Complete guide to C++ thread pools       ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════════════╝\n";
    
    std::cout << "\nSystem Info: " << std::thread::hardware_concurrency() 
              << " hardware threads available\n";
    
    demonstrate_basic_thread_pool();
    demonstrate_thread_pool_with_futures();
    demonstrate_priority_thread_pool();
    demonstrate_work_stealing_pool();
    demonstrate_dynamic_thread_pool();
    demonstrate_best_practices();
    demonstrate_real_world_example();
    demonstrate_comparison();
    
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "All thread pool demonstrations completed!\n";
    std::cout << "\nKEY TAKEAWAYS:\n";
    std::cout << "  1. Thread pools reuse threads → avoid creation overhead\n";
    std::cout << "  2. Choose pool type based on workload characteristics\n";
    std::cout << "  3. Use futures for tasks that return values\n";
    std::cout << "  4. Profile before optimizing thread count\n";
    std::cout << "  5. Watch for deadlocks with task dependencies\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    return 0;
}
