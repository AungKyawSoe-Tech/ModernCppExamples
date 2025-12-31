#include <iostream>
#include <future>
#include <thread>
#include <chrono>
#include <vector>
#include <numeric>
#include <algorithm>
#include <string>
#include <random>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <atomic>
#include <type_traits>
#include <variant>
#include <optional>
#include <any>
#include <tuple>
#include <filesystem>
#include <sstream>
#include <fstream>
#include <map>
#include <array>
#include <functional>

using namespace std;
using namespace std::chrono;
using namespace std::chrono_literals;

// ===================================================================
// 1. BASIC FUTURE/PROMISE
// ===================================================================
void example_basic_future_promise() {
    cout << "\n=== 1. BASIC FUTURE/PROMISE ===" << endl;
    
    promise<int> promise_obj;
    future<int> future_obj = promise_obj.get_future();
    
    thread t([&promise_obj]() {
        cout << "  Thread: Doing some computation..." << endl;
        this_thread::sleep_for(1s);
        
        int result = 42;
        cout << "  Thread: Setting promise value: " << result << endl;
        promise_obj.set_value(result);
    });
    
    cout << "Main: Waiting for result..." << endl;
    int result = future_obj.get();
    cout << "Main: Got result: " << result << endl;
    
    t.join();
}

// ===================================================================
// 2. FUTURE WITH EXCEPTION
// ===================================================================
void example_future_exception() {
    cout << "\n=== 2. FUTURE WITH EXCEPTION ===" << endl;
    
    promise<int> promise_obj;
    future<int> future_obj = promise_obj.get_future();
    
    thread t([&promise_obj]() {
        try {
            cout << "  Thread: Simulating error..." << endl;
            throw runtime_error("Something went wrong!");
        }
        catch (...) {
            promise_obj.set_exception(current_exception());
        }
    });
    
    try {
        cout << "Main: Waiting for result..." << endl;
        int result = future_obj.get();
        cout << "Main: Got result: " << result << endl;
    }
    catch (const exception& e) {
        cout << "Main: Caught exception: " << e.what() << endl;
    }
    
    t.join();
}

// ===================================================================
// 3. ASYNC (HIGH-LEVEL FUTURE)
// ===================================================================
void example_async() {
    cout << "\n=== 3. ASYNC (HIGH-LEVEL FUTURE) ===" << endl;
    
    auto future1 = async(launch::async, []() {
        cout << "  Async task 1 running..." << endl;
        this_thread::sleep_for(500ms);
        return 42;
    });
    
    auto future2 = async(launch::async, []() {
        cout << "  Async task 2 running..." << endl;
        this_thread::sleep_for(300ms);
        return 100;
    });
    
    auto future3 = async(launch::deferred, []() {
        cout << "  Deferred task running..." << endl;
        return 200;
    });
    
    cout << "Main: Doing other work..." << endl;
    this_thread::sleep_for(100ms);
    
    cout << "Result 1: " << future1.get() << endl;
    cout << "Result 2: " << future2.get() << endl;
    cout << "Result 3 (deferred): " << future3.get() << endl;
}

// ===================================================================
// 4. SHARED FUTURE (MULTIPLE WAITERS)
// ===================================================================
void example_shared_future() {
    cout << "\n=== 4. SHARED FUTURE (MULTIPLE WAITERS) ===" << endl;
    
    promise<int> promise_obj;
    future<int> future_obj = promise_obj.get_future();
    shared_future<int> shared_future_obj = future_obj.share();
    
    vector<thread> threads;
    for (int i = 0; i < 3; i++) {
        threads.emplace_back([i, shared_future_obj]() {
            cout << "  Thread " << i << ": Waiting for result..." << endl;
            int result = shared_future_obj.get();
            cout << "  Thread " << i << ": Got result: " << result << endl;
        });
    }
    
    thread producer([&promise_obj]() {
        this_thread::sleep_for(1s);
        cout << "  Producer: Setting value to 999" << endl;
        promise_obj.set_value(999);
    });
    
    for (auto& t : threads) t.join();
    producer.join();
}

// ===================================================================
// 5. FUTURE WITH TIMEOUT
// ===================================================================
void example_future_timeout() {
    cout << "\n=== 5. FUTURE WITH TIMEOUT ===" << endl;
    
    promise<int> promise_obj;
    future<int> future_obj = promise_obj.get_future();
    
    thread slow_worker([&promise_obj]() {
        cout << "  Slow worker: Starting (3 seconds)" << endl;
        this_thread::sleep_for(3s);
        promise_obj.set_value(42);
        cout << "  Slow worker: Finished" << endl;
    });
    
    cout << "Main: Waiting with timeout (2 seconds)..." << endl;
    
    future_status status = future_obj.wait_for(2s);
    
    if (status == future_status::ready) {
        cout << "Main: Got result: " << future_obj.get() << endl;
    }
    else if (status == future_status::timeout) {
        cout << "Main: Timeout! Task not ready yet" << endl;
    }
    
    this_thread::sleep_for(2s);
    if (slow_worker.joinable()) {
        if (future_obj.wait_for(0s) == future_status::ready) {
            cout << "Main: Finally got result: " << future_obj.get() << endl;
        }
        slow_worker.join();
    }
}

// ===================================================================
// 6. PACKAGED TASK
// ===================================================================
void example_packaged_task() {
    cout << "\n=== 6. PACKAGED TASK ===" << endl;
    
    packaged_task<int(int, int)> task([](int a, int b) {
        cout << "  Task executing with " << a << " and " << b << endl;
        this_thread::sleep_for(500ms);
        return a + b;
    });
    
    future<int> result = task.get_future();
    
    thread t(move(task), 10, 20);
    
    cout << "Main: Waiting for packaged task result..." << endl;
    cout << "Result: " << result.get() << endl;
    
    t.join();
}

// ===================================================================
// 7. ASYNC PARALLEL COMPUTATION
// ===================================================================
void example_parallel_computation() {
    cout << "\n=== 7. ASYNC PARALLEL COMPUTATION ===" << endl;
    
    auto compute_chunk = [](int start, int end) {
        long long sum = 0;
        for (int i = start; i <= end; i++) {
            sum += i;
        }
        cout << "  Chunk " << start << "-" << end << ": " << sum << endl;
        return sum;
    };
    
    const int N = 1000000;
    const int num_threads = 4;
    const int chunk_size = N / num_threads;
    
    vector<future<long long>> futures;
    
    auto start_time = high_resolution_clock::now();
    
    for (int i = 0; i < num_threads; i++) {
        int chunk_start = i * chunk_size + 1;
        int chunk_end = (i == num_threads - 1) ? N : (i + 1) * chunk_size;
        
        futures.push_back(async(launch::async, compute_chunk, 
                               chunk_start, chunk_end));
    }
    
    long long total = 0;
    for (auto& f : futures) {
        total += f.get();
    }
    
    auto end_time = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end_time - start_time);
    
    cout << "Total sum 1.." << N << ": " << total << endl;
    cout << "Parallel time: " << duration.count() << "ms" << endl;
    
    // Sequential comparison
    start_time = high_resolution_clock::now();
    long long seq_total = 0;
    for (int i = 1; i <= N; i++) {
        seq_total += i;
    }
    end_time = high_resolution_clock::now();
    duration = duration_cast<milliseconds>(end_time - start_time);
    
    cout << "Sequential sum: " << seq_total << endl;
    cout << "Sequential time: " << duration.count() << "ms" << endl;
}

// ===================================================================
// 8. FUTURE CONTINUATIONS (MANUAL IMPLEMENTATION)
// ===================================================================
void example_future_continuations() {
    cout << "\n=== 8. FUTURE CONTINUATIONS (MANUAL) ===" << endl;
    
    // Using nested async for continuations
    auto future1 = async(launch::async, []() {
        cout << "  Step 1: Computing 42..." << endl;
        this_thread::sleep_for(300ms);
        return 42;
    });
    
    // Continuation 1
    auto future2 = async(launch::async, [future1 = move(future1)]() mutable {
        int x = future1.get();  // Wait for first result
        cout << "  Step 2: Doubling " << x << "..." << endl;
        this_thread::sleep_for(300ms);
        return x * 2;
    });
    
    // Continuation 2
    auto future3 = async(launch::async, [future2 = move(future2)]() mutable {
        int x = future2.get();  // Wait for second result
        cout << "  Step 3: Adding 100 to " << x << "..." << endl;
        this_thread::sleep_for(300ms);
        return x + 100;
    });
    
    cout << "Main: Waiting for pipeline..." << endl;
    int result = future3.get();
    cout << "Final result: " << result << endl;
}

// ===================================================================
// 9. WHEN_ALL IMPLEMENTATION (SIMPLIFIED)
// ===================================================================
void example_when_all() {
    cout << "\n=== 9. WHEN_ALL (WAIT FOR ALL FUTURES) ===" << endl;
    
    auto task1 = async(launch::async, []() {
        this_thread::sleep_for(800ms);
        cout << "  Task 1 complete" << endl;
        return string("Result 1");
    });
    
    auto task2 = async(launch::async, []() {
        this_thread::sleep_for(500ms);
        cout << "  Task 2 complete" << endl;
        return 42;
    });
    
    auto task3 = async(launch::async, []() {
        this_thread::sleep_for(300ms);
        cout << "  Task 3 complete" << endl;
        return 3.14;
    });
    
    cout << "Main: Waiting for all tasks..." << endl;
    
    // Manual when_all - just get all results
    string result1 = task1.get();
    int result2 = task2.get();
    double result3 = task3.get();
    
    cout << "All tasks complete!" << endl;
    cout << "Results: " << result1 << ", " << result2 << ", " << result3 << endl;
}

// ===================================================================
// 10. WHEN_ANY IMPLEMENTATION (SIMPLIFIED)
// ===================================================================
void example_when_any() {
    cout << "\n=== 10. WHEN_ANY (FIRST COMPLETED) ===" << endl;
    
    vector<future<string>> tasks;
    
    tasks.push_back(async(launch::async, []() {
        this_thread::sleep_for(1200ms);
        return string("Task 1 (slow)");
    }));
    
    tasks.push_back(async(launch::async, []() {
        this_thread::sleep_for(700ms);
        return string("Task 2 (medium)");
    }));
    
    tasks.push_back(async(launch::async, []() {
        this_thread::sleep_for(300ms);
        return string("Task 3 (fast)");
    }));
    
    cout << "Main: Waiting for first task to complete..." << endl;
    
    // Simple when_any using wait_for
    while (true) {
        for (size_t i = 0; i < tasks.size(); i++) {
            if (tasks[i].wait_for(chrono::milliseconds(10)) == future_status::ready) {
                cout << "Task " << i << " completed first!" << endl;
                cout << "Result: " << tasks[i].get() << endl;
                
                // Get remaining results
                for (size_t j = 0; j < tasks.size(); j++) {
                    if (j != i) {
                        cout << "Task " << j << ": " << tasks[j].get() << endl;
                    }
                }
                return;
            }
        }
        this_thread::sleep_for(100ms);
    }
}

// ===================================================================
// 11. THREAD POOL WITH FUTURES
// ===================================================================
class ThreadPool {
private:
    vector<thread> workers;
    queue<function<void()>> tasks;
    mutex queue_mutex;
    condition_variable condition;
    atomic<bool> stop;
    
public:
    ThreadPool(size_t num_threads) : stop(false) {
        for (size_t i = 0; i < num_threads; ++i) {
            workers.emplace_back([this] {
                while (true) {
                    function<void()> task;
                    {
                        unique_lock<mutex> lock(queue_mutex);
                        condition.wait(lock, [this] {
                            return stop || !tasks.empty();
                        });
                        
                        if (stop && tasks.empty()) return;
                        
                        task = move(tasks.front());
                        tasks.pop();
                    }
                    task();
                }
            });
        }
    }
    
    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args) -> future<invoke_result_t<F, Args...>> {
        using return_type = invoke_result_t<F, Args...>;
        
        auto task = make_shared<packaged_task<return_type()>>(
            bind(forward<F>(f), forward<Args>(args)...)
        );
        
        future<return_type> result = task->get_future();
        {
            lock_guard<mutex> lock(queue_mutex);
            if (stop) {
                throw runtime_error("enqueue on stopped ThreadPool");
            }
            tasks.emplace([task]() { (*task)(); });
        }
        condition.notify_one();
        return result;
    }
    
    ~ThreadPool() {
        {
            lock_guard<mutex> lock(queue_mutex);
            stop = true;
        }
        condition.notify_all();
        for (thread& worker : workers) {
            if (worker.joinable()) worker.join();
        }
    }
};

void example_thread_pool() {
    cout << "\n=== 11. THREAD POOL WITH FUTURES ===" << endl;
    
    ThreadPool pool(4);
    vector<future<int>> results;
    
    cout << "Submitting 8 tasks to thread pool..." << endl;
    
    for (int i = 0; i < 8; i++) {
        results.emplace_back(pool.enqueue([i]() -> int {
            cout << "  Task " << i << " executing" << endl;
            this_thread::sleep_for(chrono::milliseconds(100 * (i % 3 + 1)));
            return i * i;
        }));
    }
    
    cout << "Collecting results..." << endl;
    for (size_t i = 0; i < results.size(); i++) {
        cout << "Result " << i << ": " << results[i].get() << endl;
    }
    
    cout << "All tasks completed!" << endl;
}

// ===================================================================
// 12. FUTURE WITH CANCELLATION
// ===================================================================
class CancellableFuture {
private:
    promise<int> promise_obj;
    future<int> future_obj;
    atomic<bool> cancelled;
    
public:
    CancellableFuture() : cancelled(false) {
        future_obj = promise_obj.get_future();
    }
    
    void cancel() {
        cancelled = true;
        promise_obj.set_exception(make_exception_ptr(runtime_error("Cancelled")));
    }
    
    bool is_cancelled() const { return cancelled; }
    
    future<int>& get_future() { return future_obj; }
    
    void execute_async(function<int()> task) {
        thread([this, task]() {
            if (cancelled) return;
            
            try {
                int result = task();
                if (!cancelled) {
                    promise_obj.set_value(result);
                }
            }
            catch (...) {
                if (!cancelled) {
                    promise_obj.set_exception(current_exception());
                }
            }
        }).detach();
    }
};

void example_cancellable_future() {
    cout << "\n=== 12. CANCELLABLE FUTURE ===" << endl;
    
    CancellableFuture cf;
    
    cf.execute_async([]() {
        cout << "  Long task starting (5 seconds)..." << endl;
        for (int i = 0; i < 5; i++) {
            this_thread::sleep_for(1s);
            cout << "  Task progress: " << (i + 1) << "/5" << endl;
        }
        return 42;
    });
    
    this_thread::sleep_for(2500ms);
    cout << "Main: Cancelling task after 2.5 seconds..." << endl;
    cf.cancel();
    
    try {
        cout << "Main: Waiting for result..." << endl;
        int result = cf.get_future().get();
        cout << "Main: Got result: " << result << endl;
    }
    catch (const exception& e) {
        cout << "Main: Caught: " << e.what() << endl;
    }
    
    this_thread::sleep_for(1s);
}

// ===================================================================
// 13. STD::VARIANT (C++17)
// ===================================================================
void example_variant() {
    cout << "\n=== 13. STD::VARIANT (C++17) ===" << endl;
    
    variant<int, string, double> value;
    
    value = 42;
    cout << "Holds int: " << get<int>(value) << endl;
    
    value = "Hello World";
    cout << "Holds string: " << get<string>(value) << endl;
    
    value = 3.14159;
    cout << "Holds double: " << get<double>(value) << endl;
    
    // Visit pattern
    auto visitor = [](auto&& arg) {
        using T = decay_t<decltype(arg)>;
        if constexpr (is_same_v<T, int>) {
            cout << "Visited int: " << arg << endl;
        }
        else if constexpr (is_same_v<T, string>) {
            cout << "Visited string: " << arg << endl;
        }
        else if constexpr (is_same_v<T, double>) {
            cout << "Visited double: " << arg << endl;
        }
    };
    
    visit(visitor, value);
    
    value = "Test";
    if (holds_alternative<string>(value)) {
        cout << "Value holds a string" << endl;
    }
}

// ===================================================================
// 14. STD::OPTIONAL (C++17)
// ===================================================================
optional<int> divide(int a, int b) {
    if (b == 0) {
        return nullopt;
    }
    return a / b;
}

void example_optional() {
    cout << "\n=== 14. STD::OPTIONAL (C++17) ===" << endl;
    
    auto result1 = divide(10, 2);
    if (result1) {
        cout << "10 / 2 = " << *result1 << endl;
    }
    
    auto result2 = divide(10, 0);
    if (!result2) {
        cout << "10 / 0 = No result (division by zero)" << endl;
    }
    
    cout << "Result or default: " << result2.value_or(999) << endl;
    
    optional<string> name = "Alice";
    cout << "Name: " << name.value() << endl;
    
    optional<int> empty_opt;
    cout << "Empty optional has value: " << boolalpha << empty_opt.has_value() << endl;
}

// ===================================================================
// 15. STD::ANY (C++17)
// ===================================================================
void example_any() {
    cout << "\n=== 15. STD::ANY (C++17) ===" << endl;
    
    any value;
    
    value = 42;
    cout << "Any holds int: " << any_cast<int>(value) << endl;
    
    value = string("Hello");
    cout << "Any holds string: " << any_cast<string>(value) << endl;
    
    value = 3.14;
    cout << "Any holds double: " << any_cast<double>(value) << endl;
    
    if (value.type() == typeid(double)) {
        cout << "Value is a double" << endl;
    }
    
    try {
        cout << any_cast<int>(value) << endl;
    }
    catch (const bad_any_cast& e) {
        cout << "Caught exception: " << e.what() << endl;
    }
}

// ===================================================================
// 16. FILESYSTEM (C++17)
// ===================================================================
void example_filesystem() {
    cout << "\n=== 16. FILESYSTEM (C++17) ===" << endl;
    
    namespace fs = std::filesystem;
    
    cout << "Current path: " << fs::current_path() << endl;
    
    fs::create_directory("test_dir");
    cout << "Created directory: test_dir" << endl;
    
    {
        ofstream file("test_dir/test_file.txt");
        file << "Hello Filesystem!" << endl;
    }
    
    if (fs::exists("test_dir/test_file.txt")) {
        cout << "File exists" << endl;
        cout << "File size: " << fs::file_size("test_dir/test_file.txt") << " bytes" << endl;
    }
    
    cout << "\nDirectory contents:" << endl;
    for (const auto& entry : fs::directory_iterator("test_dir")) {
        cout << "  " << entry.path().filename() 
             << " (" << fs::file_size(entry) << " bytes)" << endl;
    }
    
    fs::remove_all("test_dir");
    cout << "Cleaned up test directory" << endl;
}

// ===================================================================
// 17. STRUCTURED BINDINGS (C++17)
// ===================================================================
void example_structured_bindings() {
    cout << "\n=== 17. STRUCTURED BINDINGS (C++17) ===" << endl;
    
    // With tuple
    tuple<int, string, double> tup = {42, "Alice", 3.14};
    auto [id, name, score] = tup;
    cout << "Tuple unpacked: " << id << ", " << name << ", " << score << endl;
    
    // With array
    array<int, 3> arr = {1, 2, 3};
    auto [a, b, c] = arr;
    cout << "Array unpacked: " << a << ", " << b << ", " << c << endl;
    
    // With struct
    struct Person {
        string name;
        int age;
        string city;
    };
    
    Person p = {"Bob", 30, "New York"};
    auto [person_name, age, city] = p;
    cout << "Person: " << person_name << ", " << age << ", " << city << endl;
    
    // With map
    map<string, int> scores = {{"Alice", 95}, {"Bob", 87}, {"Charlie", 92}};
    for (const auto& [student, score] : scores) {
        cout << student << ": " << score << endl;
    }
}

// ===================================================================
// 18. CONSTEXPR IF (C++17)
// ===================================================================
template<typename T>
auto process_value(T value) {
    if constexpr (is_integral_v<T>) {
        cout << "Processing integer: " << value * 2 << endl;
        return value * 2;
    }
    else if constexpr (is_floating_point_v<T>) {
        cout << "Processing float: " << value * 3.14 << endl;
        return value * 3.14;
    }
    else if constexpr (is_same_v<T, string>) {
        cout << "Processing string: " << value + " processed" << endl;
        return value + " processed";
    }
    else {
        cout << "Unknown type" << endl;
        return value;
    }
}

void example_constexpr_if() {
    cout << "\n=== 18. CONSTEXPR IF (C++17) ===" << endl;
    
    process_value(42);
    process_value(3.14);
    process_value(string("Hello"));
}

// ===================================================================
// 19. COMPREHENSIVE EXAMPLE: ASYNC FILE PROCESSOR
// ===================================================================
void example_comprehensive_async_processor() {
    cout << "\n=== 19. COMPREHENSIVE EXAMPLE: ASYNC FILE PROCESSOR ===" << endl;
    
    class AsyncFileProcessor {
    private:
        ThreadPool pool{4};
        
    public:
        future<vector<string>> process_files(const vector<string>& filenames) {
            vector<future<string>> futures;
            
            for (const auto& filename : filenames) {
                futures.push_back(pool.enqueue([filename]() -> string {
                    this_thread::sleep_for(chrono::milliseconds(100 + rand() % 400));
                    
                    if (filename.find("error") != string::npos) {
                        throw runtime_error("Error processing " + filename);
                    }
                    
                    return "Processed: " + filename + " (size: " 
                           + to_string(rand() % 1000) + " bytes)";
                }));
            }
            
            return async(launch::async, [futures = move(futures)]() mutable {
                vector<string> results;
                for (auto& f : futures) {
                    try {
                        results.push_back(f.get());
                    }
                    catch (const exception& e) {
                        results.push_back("ERROR: " + string(e.what()));
                    }
                }
                return results;
            });
        }
    };
    
    AsyncFileProcessor processor;
    
    vector<string> files = {
        "document1.txt",
        "image1.png",
        "error_file.txt",
        "data.csv",
        "config.json"
    };
    
    cout << "Processing " << files.size() << " files asynchronously..." << endl;
    
    auto start = high_resolution_clock::now();
    auto result_future = processor.process_files(files);
    
    cout << "Main thread doing other work..." << endl;
    this_thread::sleep_for(200ms);
    
    auto results = result_future.get();
    auto end = high_resolution_clock::now();
    
    cout << "\nProcessing complete in " 
         << duration_cast<milliseconds>(end - start).count() << "ms" << endl;
    cout << "\nResults:" << endl;
    for (const auto& result : results) {
        cout << "  " << result << endl;
    }
}

// ===================================================================
// 20. ADDITIONAL EXAMPLE: FUTURE WITH MULTIPLE CONTINUATIONS
// ===================================================================
void example_future_chaining() {
    cout << "\n=== 20. FUTURE CHAINING ===" << endl;
    
    auto future1 = async(launch::async, []() {
        cout << "  Step 1: Starting with 5" << endl;
        this_thread::sleep_for(200ms);
        return 5 + 10;  // 15
    });
    
    auto future2 = async(launch::async, [f1 = move(future1)]() mutable {
        int val = f1.get();
        cout << "  Step 2: Processing " << val << endl;
        this_thread::sleep_for(200ms);
        return val * 2;  // 30
    });
    
    auto future3 = async(launch::async, [f2 = move(future2)]() mutable {
        int val = f2.get();
        cout << "  Step 3: Finalizing " << val << endl;
        this_thread::sleep_for(200ms);
        return val - 5;  // 25
    });
    
    int result = future3.get();
    cout << "Final result: " << result << endl;
}

// ===================================================================
// MAIN FUNCTION
// ===================================================================
int main() {
    cout << "===================================================================" << endl;
    cout << "COMPLETE C++17 FEATURES EXAMPLES" << endl;
    cout << "===================================================================" << endl;
    
    srand(time(nullptr));
    
    try {
        example_basic_future_promise();
        example_future_exception();
        example_async();
        example_shared_future();
        example_future_timeout();
        example_packaged_task();
        example_parallel_computation();
        example_future_continuations();
        example_when_all();
        example_when_any();
        example_thread_pool();
        example_cancellable_future();
        example_variant();
        example_optional();
        example_any();
        example_filesystem();
        example_structured_bindings();
        example_constexpr_if();
        example_comprehensive_async_processor();
        example_future_chaining();
        
        cout << "\n===================================================================" << endl;
        cout << "ALL EXAMPLES COMPLETED SUCCESSFULLY!" << endl;
        cout << "===================================================================" << endl;
    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}