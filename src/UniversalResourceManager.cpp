// ===================================================================
// UNIVERSAL RESOURCE MANAGER - CUSTOM DELETERS IN C++
// ===================================================================
// Comprehensive guide to using custom deleters with smart pointers
// to manage ANY type of resource (not just heap memory).
//
// KEY INSIGHT: Custom deleters turn smart pointers from simple memory
// managers into universal RAII resource managers for:
//   • File handles (FILE*, fds)
//   • Network connections (sockets, SSL contexts)
//   • Database connections
//   • GPU resources (OpenGL, DirectX, Vulkan, CUDA)
//   • OS handles (Windows HANDLE, POSIX fds)
//   • Shared memory, memory-mapped files
//   • Locks, mutexes, semaphores
//   • Custom allocators and memory pools
//
// Build: g++ -std=c++20 -O2 -o UniversalResourceManager UniversalResourceManager.cpp
// ===================================================================

#include <iostream>
#include <memory>
#include <cstdio>
#include <vector>
#include <string>
#include <cstring>
#include <cstdlib>

using namespace std;

// ===================================================================
// PART 1: WHY CUSTOM DELETERS?
// ===================================================================

void explain_the_problem() {
    cout << "\n╔══════════════════════════════════════════════════════════════════╗\n";
    cout << "║         WHY DO WE NEED CUSTOM DELETERS?                         ║\n";
    cout << "╚══════════════════════════════════════════════════════════════════╝\n";
    
    cout << "\n📌 THE FUNDAMENTAL PROBLEM:\n";
    cout << "   Default unique_ptr/shared_ptr call 'delete ptr' or 'delete[] ptr'\n";
    cout << "   But NOT ALL resources are heap-allocated C++ objects!\n";
    
    cout << "\n🎯 RESOURCE TYPES THAT NEED CUSTOM CLEANUP:\n";
    cout << "   1. C Library Resources:    FILE* → fclose()\n";
    cout << "   2. OS Handles:             HANDLE → CloseHandle()\n";
    cout << "   3. Network:                socket → close(), shutdown()\n";
    cout << "   4. Database:               connection → disconnect(), rollback()\n";
    cout << "   5. GPU:                    texture → glDeleteTextures()\n";
    cout << "   6. Shared Memory:          mapping → munmap(), shm_unlink()\n";
    cout << "   7. Custom Allocators:      pool → return_to_pool()\n";
    cout << "   8. Legacy C++ Code:        Cannot modify destructor\n";
    
    cout << "\n✅ SOLUTION: Custom Deleters\n";
    cout << "   Transform smart pointers into universal resource managers\n";
    cout << "   Apply RAII to ANY resource, not just memory\n";
}

// ===================================================================
// PART 2: REAL-WORLD EXAMPLES
// ===================================================================

// Example 1: FILE HANDLING
void file_example() {
    cout << "\n=== EXAMPLE 1: FILE HANDLING ===\n";
    
    cout << "❌ WITHOUT custom deleter (DANGEROUS):\n";
    {
        FILE* raw_file = fopen("test.txt", "w");
        if (raw_file) {
            fputs("Hello World\n", raw_file);
            // What if exception occurs here? File won't be closed!
            // Must manually remember to call fclose()
            fclose(raw_file);
        }
    }
    
    cout << "\n✅ WITH custom deleter (SAFE):\n";
    {
        auto file_closer = [](FILE* f) {
            if (f) {
                cout << "   Auto-closing file\n";
                fclose(f);
            }
        };
        
        unique_ptr<FILE, decltype(file_closer)> 
            file(fopen("test.txt", "w"), file_closer);
        
        if (file) {
            fputs("Auto-closed!\n", file.get());
            // Exception-safe! File always closed
        }
    }
}

// Example 2: DATABASE CONNECTIONS
void database_example() {
    cout << "\n=== EXAMPLE 2: DATABASE CONNECTIONS ===\n";
    
    struct DatabaseConnection {
        string connection_id;
        bool is_open;
        
        DatabaseConnection(string id) : connection_id(id), is_open(true) {
            cout << "   📡 Connected to: " << connection_id << endl;
        }
        
        void execute_query(const string& query) {
            if (is_open) {
                cout << "   🔍 Executing: " << query << endl;
            }
        }
    };
    
    auto db_closer = [](DatabaseConnection* db) {
        if (db && db->is_open) {
            cout << "   🔒 Closing connection: " << db->connection_id << endl;
            db->is_open = false;
            // Real code: db->rollback(), db->disconnect()
        }
        delete db;
    };
    
    {
        unique_ptr<DatabaseConnection, decltype(db_closer)> 
            db(new DatabaseConnection("prod_db"), db_closer);
        
        db->execute_query("SELECT * FROM users");
    }  // Auto-closed, even if exception!
}

// Example 3: NETWORK SOCKETS
void socket_example() {
    cout << "\n=== EXAMPLE 3: NETWORK SOCKETS ===\n";
    
    struct Socket {
        int socket_fd;
        bool is_connected;
        
        Socket() : socket_fd(42), is_connected(true) {
            cout << "   🌐 Socket created, FD: " << socket_fd << endl;
        }
        
        void send_data(const string& data) {
            if (is_connected) {
                cout << "   📤 Sending: " << data << endl;
            }
        }
    };
    
    auto socket_cleanup = [](Socket* sock) {
        if (sock) {
            cout << "   🔧 Cleaning up socket:\n";
            if (sock->is_connected) {
                cout << "      • Flushing send buffer\n";
                cout << "      • Shutting down connection\n";
                cout << "      • Closing FD: " << sock->socket_fd << endl;
                // Real: shutdown(fd, SHUT_RDWR); close(fd);
            }
            delete sock;
        }
    };
    
    {
        unique_ptr<Socket, decltype(socket_cleanup)> 
            socket(new Socket(), socket_cleanup);
        socket->send_data("Hello server!");
    }
}

// Example 4: GPU/OPENGL RESOURCES
void gpu_resource_example() {
    cout << "\n=== EXAMPLE 4: GPU RESOURCES ===\n";
    
    struct Texture {
        unsigned int texture_id;
        int width, height;
        
        Texture(int w, int h) : texture_id(1001), width(w), height(h) {
            cout << "   🎮 GPU Texture: ID=" << texture_id 
                 << " (" << w << "x" << h << ")" << endl;
            // Real: glGenTextures(1, &texture_id);
        }
        
        void bind() {
            cout << "   📌 Binding texture: " << texture_id << endl;
        }
    };
    
    auto texture_deleter = [](Texture* tex) {
        if (tex) {
            cout << "   🗑️  Deleting GPU texture: " << tex->texture_id << endl;
            cout << "      • Unbinding texture\n";
            cout << "      • Freeing GPU memory\n";
            // Real: glBindTexture(GL_TEXTURE_2D, 0);
            //       glDeleteTextures(1, &tex->texture_id);
            delete tex;
        }
    };
    
    {
        unique_ptr<Texture, decltype(texture_deleter)> 
            texture(new Texture(1024, 1024), texture_deleter);
        texture->bind();
    }
}

// Example 5: SHARED MEMORY
void shared_memory_example() {
    cout << "\n=== EXAMPLE 5: SHARED MEMORY ===\n";
    
    struct SharedMemory {
        void* memory;
        size_t size;
        string name;
        
        SharedMemory(const string& n, size_t s) 
            : memory(malloc(s)), size(s), name(n) {
            cout << "   💾 Shared memory: " << name << " (" << s << " bytes)" << endl;
        }
        
        void write(const string& data) {
            if (memory && data.size() <= size) {
                memcpy(memory, data.data(), data.size());
                cout << "   ✍️  Written: " << data << endl;
            }
        }
    };
    
    auto shared_mem_deleter = [](SharedMemory* sm) {
        if (sm) {
            cout << "   🧹 Cleaning up: " << sm->name << endl;
            cout << "      • Syncing with other processes\n";
            cout << "      • Unmapping memory\n";
            cout << "      • Closing handle\n";
            // Real: munmap(), shm_unlink() (POSIX)
            //       UnmapViewOfFile(), CloseHandle() (Windows)
            if (sm->memory) free(sm->memory);
            delete sm;
        }
    };
    
    {
        unique_ptr<SharedMemory, decltype(shared_mem_deleter)> 
            shmem(new SharedMemory("app_data", 4096), shared_mem_deleter);
        shmem->write("Shared data");
    }
}

// Example 6: RESOURCE POOL (Return to pool instead of delete!)
void resource_pool_example() {
    cout << "\n=== EXAMPLE 6: RESOURCE POOL (Don't Delete, Reuse!) ===\n";
    
    struct DatabaseConnection {
        int connection_id;
        bool in_use;
        
        DatabaseConnection(int id) : connection_id(id), in_use(true) {
            cout << "   🔌 Connection " << id << " created (expensive!)" << endl;
        }
        
        void query() {
            if (in_use) {
                cout << "   🔍 Query on connection " << connection_id << endl;
            }
        }
    };
    
    static vector<DatabaseConnection*> pool;
    
    // DON'T delete! Return to pool for reuse
    auto pool_deleter = [](DatabaseConnection* conn) {
        if (conn) {
            cout << "   ♻️  Returning conn " << conn->connection_id 
                 << " to pool (NOT deleting)" << endl;
            conn->in_use = false;
            pool.push_back(conn);  // Reuse later!
        }
    };
    
    auto get_connection = [&](int id) {
        if (!pool.empty()) {
            auto conn = pool.back();
            pool.pop_back();
            conn->in_use = true;
            cout << "   ♻️  Reusing connection " << conn->connection_id << endl;
            return unique_ptr<DatabaseConnection, decltype(pool_deleter)>(conn, pool_deleter);
        }
        auto conn = new DatabaseConnection(id);
        return unique_ptr<DatabaseConnection, decltype(pool_deleter)>(conn, pool_deleter);
    };
    
    // Initialize pool
    pool.push_back(new DatabaseConnection(1));
    pool.push_back(new DatabaseConnection(2));
    
    {
        auto conn1 = get_connection(3);  // Reuses from pool
        auto conn2 = get_connection(4);  // Reuses from pool
        auto conn3 = get_connection(5);  // Creates new
        
        conn1->query();
        conn2->query();
        conn3->query();
    }  // Returned to pool
    
    cout << "   📊 Pool size: " << pool.size() << " connections\n";
    
    // Cleanup at program end
    for (auto conn : pool) delete conn;
    pool.clear();
}

// Example 7: DEBUG ALLOCATOR with statistics
void allocator_with_stats() {
    cout << "\n=== EXAMPLE 7: DEBUG ALLOCATOR (Tracking & Logging) ===\n";
    
    struct AllocatedBlock {
        void* memory;
        size_t size;
        string allocation_site;
        
        AllocatedBlock(size_t s, const string& site) 
            : memory(malloc(s)), size(s), allocation_site(site) {
            cout << "   🆕 Allocated " << s << " bytes at: " << site << endl;
        }
    };
    
    static int total_allocs = 0;
    static size_t total_memory = 0;
    
    auto debug_deleter = [](AllocatedBlock* block) {
        if (block) {
            cout << "   🗑️  Freeing " << block->size << " bytes from: " 
                 << block->allocation_site << endl;
            total_allocs--;
            total_memory -= block->size;
            cout << "      📊 Stats: " << total_allocs << " blocks, " 
                 << total_memory << " bytes\n";
            if (block->memory) free(block->memory);
            delete block;
        }
    };
    
    auto debug_allocate = [&](size_t size, const string& site) {
        auto block = new AllocatedBlock(size, site);
        total_allocs++;
        total_memory += size;
        return unique_ptr<AllocatedBlock, decltype(debug_deleter)>(block, debug_deleter);
    };
    
    {
        auto block1 = debug_allocate(100, "function_a()");
        auto block2 = debug_allocate(200, "function_b()");
    }  // Auto-tracked cleanup
    
    cout << "   ✅ Final: " << total_allocs << " blocks remaining\n";
}

// Example 8: COMPOSITE DELETER (Multiple sub-resources)
void composite_deleter_example() {
    cout << "\n=== EXAMPLE 8: COMPOSITE DELETER (Multiple Resources) ===\n";
    
    struct ComplexResource {
        FILE* log_file;
        void* shared_memory;
        int network_socket;
        
        ComplexResource() 
            : log_file(fopen("app.log", "w"))
            , shared_memory(malloc(1024))
            , network_socket(42) {
            cout << "   🏗️  Complex resource created (3 sub-resources)" << endl;
        }
    };
    
    // Single deleter handles ALL sub-resources in correct order
    auto composite_deleter = [](ComplexResource* res) {
        if (res) {
            cout << "   🧹 Cleanup sequence:\n";
            if (res->log_file) {
                cout << "      1. Close log file\n";
                fclose(res->log_file);
            }
            if (res->shared_memory) {
                cout << "      2. Free shared memory\n";
                free(res->shared_memory);
            }
            if (res->network_socket > 0) {
                cout << "      3. Close socket: " << res->network_socket << endl;
            }
            delete res;
        }
    };
    
    {
        unique_ptr<ComplexResource, decltype(composite_deleter)> 
            resource(new ComplexResource(), composite_deleter);
    }  // All 3 resources cleaned up!
}

// ===================================================================
// PART 3: CUSTOM DELETER PATTERNS
// ===================================================================

void deleter_patterns() {
    cout << "\n╔══════════════════════════════════════════════════════════════════╗\n";
    cout << "║         CUSTOM DELETER PATTERNS & BEST PRACTICES                ║\n";
    cout << "╚══════════════════════════════════════════════════════════════════╝\n";
    
    cout << "\n📋 PATTERN 1: Simple Lambda (Most Common)\n";
    cout << "   auto deleter = [](Resource* r) { cleanup(r); delete r; };\n";
    cout << "   unique_ptr<Resource, decltype(deleter)> ptr(res, deleter);\n";
    
    cout << "\n📋 PATTERN 2: Stateful Lambda (Captures context)\n";
    cout << "   auto logger_deleter = [&logger](Resource* r) {\n";
    cout << "       logger.log(\"Cleanup\");\n";
    cout << "       cleanup(r); delete r;\n";
    cout << "   };\n";
    
    cout << "\n📋 PATTERN 3: Function Object (For shared_ptr)\n";
    cout << "   struct Deleter {\n";
    cout << "       void operator()(Resource* r) { cleanup(r); delete r; }\n";
    cout << "   };\n";
    cout << "   shared_ptr<Resource> ptr(res, Deleter{});\n";
    
    cout << "\n📋 PATTERN 4: Function Pointer (C-style)\n";
    cout << "   void cleanup_func(Resource* r) { cleanup(r); delete r; }\n";
    cout << "   unique_ptr<Resource, void(*)(Resource*)> ptr(res, cleanup_func);\n";
    
    cout << "\n📋 PATTERN 5: Templated Deleter Factory\n";
    cout << "   template<typename Cleanup>\n";
    cout << "   auto make_managed(T* ptr, Cleanup cleanup) {\n";
    cout << "       return unique_ptr<T, Cleanup>(ptr, cleanup);\n";
    cout << "   }\n";
}

// ===================================================================
// PART 4: DRAWBACKS & PITFALLS
// ===================================================================

void drawbacks_and_pitfalls() {
    cout << "\n╔══════════════════════════════════════════════════════════════════╗\n";
    cout << "║         DRAWBACKS, PITFALLS & COMMON MISTAKES                   ║\n";
    cout << "╚══════════════════════════════════════════════════════════════════╝\n";
    
    cout << "\n⚠️  PITFALL 1: Type Bloat (unique_ptr)\n";
    cout << "   Problem: Deleter type is part of unique_ptr type!\n";
    cout << "   unique_ptr<int, DeleterA> != unique_ptr<int, DeleterB>\n";
    cout << "   Impact: Cannot store in same container, harder to pass around\n";
    cout << "   Solution: Use shared_ptr (type-erases deleter) or function pointers\n";
    
    cout << "\n⚠️  PITFALL 2: Size Overhead (unique_ptr with stateful deleters)\n";
    cout << "   sizeof(unique_ptr<int>) = 8 bytes\n";
    cout << "   sizeof(unique_ptr<int, LambdaDeleter>) = 16+ bytes (deleter stored)\n";
    cout << "   Impact: Increases memory usage, cache misses\n";
    cout << "   Solution: Prefer stateless deleters, or use shared_ptr\n";
    
    cout << "\n⚠️  PITFALL 3: Don't Forget nullptr Check!\n";
    cout << "   ❌ BAD:  auto del = [](T* p) { cleanup(p); delete p; }\n";
    cout << "   ✅ GOOD: auto del = [](T* p) { if(p) { cleanup(p); delete p; } }\n";
    cout << "   Why: Smart pointer may be reset() or moved-from\n";
    
    cout << "\n⚠️  PITFALL 4: Don't Capture by Reference (for shared_ptr)\n";
    cout << "   ❌ BAD:  shared_ptr<T>(ptr, [&logger](T* p) { logger.log(); });\n";
    cout << "   ✅ GOOD: shared_ptr<T>(ptr, [logger=&logger](T* p) { logger->log(); });\n";
    cout << "   Why: shared_ptr may outlive captured references (dangling!)\n";
    
    cout << "\n⚠️  PITFALL 5: Exception Safety in Deleters\n";
    cout << "   Deleters should be noexcept (or handle exceptions internally)\n";
    cout << "   ❌ BAD:  auto del = [](T* p) { may_throw(); delete p; };\n";
    cout << "   ✅ GOOD: auto del = [](T* p) noexcept { try_cleanup(p); delete p; };\n";
    cout << "   Why: Exception in destructor = std::terminate()\n";
    
    cout << "\n⚠️  PITFALL 6: Don't Delete Twice!\n";
    cout << "   ❌ DANGEROUS:\n";
    cout << "      T* raw = new T();\n";
    cout << "      unique_ptr<T, Deleter> p1(raw, deleter);\n";
    cout << "      unique_ptr<T, Deleter> p2(raw, deleter);  // Double delete!\n";
    
    cout << "\n⚠️  PITFALL 7: Don't Use delete on Non-Heap!\n";
    cout << "   ❌ CRASH:\n";
    cout << "      int stack_var = 42;\n";
    cout << "      shared_ptr<int>(&stack_var, [](int* p) { delete p; });  // BOOM!\n";
    cout << "   ✅ CORRECT:\n";
    cout << "      shared_ptr<int>(&stack_var, [](int*) {}); // No-op deleter\n";
    
    cout << "\n⚠️  PITFALL 8: Performance Cost\n";
    cout << "   Every smart pointer deallocation calls deleter (virtual call for shared_ptr)\n";
    cout << "   Impact: Slower than raw delete, especially in tight loops\n";
    cout << "   Mitigation: Batch allocations, use object pools\n";
    
    cout << "\n⚠️  PITFALL 9: Deleter Must Match Allocation!\n";
    cout << "   new    → delete      ✅\n";
    cout << "   new[]  → delete[]    ✅\n";
    cout << "   malloc → free        ✅\n";
    cout << "   new    → free        ❌ UNDEFINED BEHAVIOR!\n";
    cout << "   new[]  → delete      ❌ MEMORY LEAK & CORRUPTION!\n";
    
    cout << "\n⚠️  PITFALL 10: Thread Safety (shared_ptr only)\n";
    cout << "   shared_ptr ref counting is thread-safe\n";
    cout << "   BUT custom deleter execution is NOT synchronized\n";
    cout << "   If deleter modifies shared state, YOU must synchronize!\n";
}

// ===================================================================
// PART 5: WHEN TO USE WHAT
// ===================================================================

void when_to_use_what() {
    cout << "\n╔══════════════════════════════════════════════════════════════════╗\n";
    cout << "║         DECISION TREE: WHEN TO USE CUSTOM DELETERS?             ║\n";
    cout << "╚══════════════════════════════════════════════════════════════════╝\n";
    
    cout << "\n✅ USE unique_ptr with custom deleter when:\n";
    cout << "   • Single ownership\n";
    cout << "   • Resource is NOT heap memory (FILE*, socket, handle)\n";
    cout << "   • Need custom cleanup (fclose, close, CloseHandle)\n";
    cout << "   • Don't need to share ownership\n";
    cout << "   • Type can vary per deleter (okay with type bloat)\n";
    
    cout << "\n✅ USE shared_ptr with custom deleter when:\n";
    cout << "   • Shared ownership needed\n";
    cout << "   • Want type erasure (all shared_ptr<T> same type)\n";
    cout << "   • Need to store in containers with different deleters\n";
    cout << "   • Resource may be shared across threads\n";
    cout << "   • Don't mind small overhead (control block)\n";
    
    cout << "\n❌ DON'T USE custom deleters when:\n";
    cout << "   • Standard delete/delete[] works fine\n";
    cout << "   • Resource is simple heap memory\n";
    cout << "   • Performance is critical (use raw pointers + manual RAII)\n";
    cout << "   • Can use standard containers (vector, string, etc.)\n";
    
    cout << "\n🎯 PREFER alternatives:\n";
    cout << "   • vector<T> instead of unique_ptr<T[]>\n";
    cout << "   • string instead of unique_ptr<char[]>\n";
    cout << "   • RAII wrapper class for complex resources\n";
    cout << "   • Standard library types (fstream, thread, mutex)\n";
}

// ===================================================================
// PART 6: SIMPLIFIED PATTERNS
// ===================================================================

void simplified_patterns() {
    cout << "\n╔══════════════════════════════════════════════════════════════════╗\n";
    cout << "║         SIMPLIFIED PATTERNS FOR COMMON CASES                     ║\n";
    cout << "╚══════════════════════════════════════════════════════════════════╝\n";
    
    cout << "\n📦 PATTERN: Generic RAII Wrapper (Simplest!)\n";
    cout << "   template<typename T, auto Cleanup>\n";
    cout << "   struct RAIIWrapper {\n";
    cout << "       T resource;\n";
    cout << "       RAIIWrapper(T r) : resource(r) {}\n";
    cout << "       ~RAIIWrapper() { if(resource) Cleanup(resource); }\n";
    cout << "       operator T() { return resource; }\n";
    cout << "   };\n";
    cout << "   \n";
    cout << "   Usage: RAIIWrapper<FILE*, fclose> file(fopen(\"f.txt\", \"r\"));\n";
    
    cout << "\n📦 PATTERN: Factory Function (Hide deleter type)\n";
    cout << "   auto make_file(const char* name, const char* mode) {\n";
    cout << "       return unique_ptr<FILE, decltype(&fclose)>(\n";
    cout << "           fopen(name, mode), &fclose);\n";
    cout << "   }\n";
    cout << "   \n";
    cout << "   Usage: auto file = make_file(\"data.txt\", \"r\");\n";
    
    cout << "\n📦 PATTERN: Type Alias (Reusable)\n";
    cout << "   using FilePtr = unique_ptr<FILE, decltype(&fclose)>;\n";
    cout << "   \n";
    cout << "   FilePtr make_file(const char* name) {\n";
    cout << "       return FilePtr(fopen(name, \"r\"), &fclose);\n";
    cout << "   }\n";
}

// ===================================================================
// MAIN - COMPREHENSIVE DEMO
// ===================================================================

int main() {
    cout << "╔══════════════════════════════════════════════════════════════════╗\n";
    cout << "║     UNIVERSAL RESOURCE MANAGER - CUSTOM DELETERS IN C++         ║\n";
    cout << "║     Transform Smart Pointers Into Universal RAII Managers       ║\n";
    cout << "╚══════════════════════════════════════════════════════════════════╝\n";
    
    // Part 1: Understanding the problem
    explain_the_problem();
    
    // Part 2: Real-world examples
    cout << "\n\n";
    cout << "╔══════════════════════════════════════════════════════════════════╗\n";
    cout << "║              REAL-WORLD EXAMPLES & USE CASES                     ║\n";
    cout << "╚══════════════════════════════════════════════════════════════════╝\n";
    
    file_example();
    database_example();
    socket_example();
    gpu_resource_example();
    shared_memory_example();
    resource_pool_example();
    allocator_with_stats();
    composite_deleter_example();
    
    // Part 3: Patterns
    deleter_patterns();
    
    // Part 4: Pitfalls
    drawbacks_and_pitfalls();
    
    // Part 5: Decision guide
    when_to_use_what();
    
    // Part 6: Simplified patterns
    simplified_patterns();
    
    // Summary
    cout << "\n\n";
    cout << "╔══════════════════════════════════════════════════════════════════╗\n";
    cout << "║                         KEY TAKEAWAYS                            ║\n";
    cout << "╚══════════════════════════════════════════════════════════════════╝\n";
    
    cout << "\n✅ BENEFITS:\n";
    cout << "   1. RAII for ANY resource (not just memory)\n";
    cout << "   2. Exception-safe cleanup\n";
    cout << "   3. No manual cleanup code\n";
    cout << "   4. Prevents resource leaks\n";
    cout << "   5. Self-documenting (cleanup is obvious)\n";
    cout << "   6. Composable (combine multiple resources)\n";
    
    cout << "\n⚠️  WATCH OUT FOR:\n";
    cout << "   1. Type bloat with unique_ptr\n";
    cout << "   2. Size overhead with stateful deleters\n";
    cout << "   3. nullptr checks in deleters\n";
    cout << "   4. Exception safety (use noexcept)\n";
    cout << "   5. Matching allocation/deallocation\n";
    cout << "   6. Thread safety for shared state\n";
    
    cout << "\n🎯 BEST PRACTICES:\n";
    cout << "   1. Prefer simple lambdas for deleters\n";
    cout << "   2. Make deleters noexcept\n";
    cout << "   3. Always check for nullptr\n";
    cout << "   4. Use factory functions to hide deleter type\n";
    cout << "   5. Consider RAII wrapper classes for complex cases\n";
    cout << "   6. Document deleter behavior clearly\n";
    cout << "   7. Test cleanup paths (use sanitizers!)\n";
    
    cout << "\n💡 REMEMBER:\n";
    cout << "   Custom deleters turn smart pointers into UNIVERSAL resource\n";
    cout << "   managers. Use them to apply RAII to ANY resource type!\n";
    
    cout << "\n╔══════════════════════════════════════════════════════════════════╗\n";
    cout << "║              ALL EXAMPLES COMPLETED SUCCESSFULLY!                ║\n";
    cout << "╚══════════════════════════════════════════════════════════════════╝\n\n";
    
    return 0;
}
