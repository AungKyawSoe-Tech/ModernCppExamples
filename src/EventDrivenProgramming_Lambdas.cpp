#include <iostream>
#include <functional>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <variant>
#include <algorithm>
#include <queue>
#include <typeindex>

// ===================================================================
// EVENT-DRIVEN PROGRAMMING: MODERN LAMBDA-BASED APPROACH
// ===================================================================
// This file demonstrates event-driven programming using:
// - std::function
// - Lambdas with captures
// - std::variant
// - No inheritance hierarchies needed!
// ===================================================================

// ===================================================================
// 1. OBSERVER PATTERN (LAMBDA-BASED)
// ===================================================================

class Subject {
private:
    std::vector<std::function<void(const std::string&)>> observers;
    
public:
    void subscribe(std::function<void(const std::string&)> observer) {
        observers.push_back(std::move(observer));
    }
    
    void notify(const std::string& event) {
        std::cout << "  [Subject] Notifying " << observers.size() << " observers" << std::endl;
        for (auto& observer : observers) {
            observer(event);
        }
    }
};

void example_lambda_observer() {
    std::cout << "\n=== 1. OBSERVER PATTERN (LAMBDA-BASED) ===" << std::endl;
    
    Subject subject;
    
    // No observer class needed! Just pass lambdas
    subject.subscribe([](const std::string& event) {
        std::cout << "    Lambda Observer 1: " << event << std::endl;
    });
    
    // Can capture state inline
    int count = 0;
    subject.subscribe([&count](const std::string& event) {
        std::cout << "    Lambda Observer 2 (count=" << ++count << "): " << event << std::endl;
    });
    
    // Can define behavior at subscription time
    std::string prefix = "CustomPrefix";
    subject.subscribe([prefix](const std::string& event) {
        std::cout << "    [" << prefix << "] " << event << std::endl;
    });
    
    subject.notify("User logged in");
    subject.notify("Data updated");
    
    std::cout << "\n✓ ADVANTAGES:" << std::endl;
    std::cout << "  • No observer interface/base class needed" << std::endl;
    std::cout << "  • Inline behavior definition" << std::endl;
    std::cout << "  • Can capture local state" << std::endl;
    std::cout << "  • Less boilerplate code" << std::endl;
}

// ===================================================================
// 2. SIGNAL/SLOT PATTERN (MODERN)
// ===================================================================

template<typename... Args>
class Signal {
private:
    std::vector<std::function<void(Args...)>> slots;
    
public:
    void connect(std::function<void(Args...)> slot) {
        slots.push_back(std::move(slot));
    }
    
    void emit(Args... args) {
        for (auto& slot : slots) {
            slot(args...);
        }
    }
    
    void operator()(Args... args) { emit(args...); }
};

class Button {
public:
    Signal<const std::string&> clicked;
    Signal<int, int> positionChanged;
    
    std::string id;
    
    Button(std::string id) : id(std::move(id)) {}
};

void example_signal_slot() {
    std::cout << "\n=== 2. SIGNAL/SLOT PATTERN (MODERN) ===" << std::endl;
    
    Button btn("SaveButton");
    
    // Connect signals to lambdas (no slot classes needed!)
    btn.clicked.connect([](const std::string& id) {
        std::cout << "    Handler 1: Button " << id << " was clicked" << std::endl;
    });
    
    btn.clicked.connect([](const std::string& id) {
        std::cout << "    Handler 2: Logging click on " << id << std::endl;
    });
    
    btn.positionChanged.connect([](int x, int y) {
        std::cout << "    Position handler: Moved to (" << x << "," << y << ")" << std::endl;
    });
    
    std::cout << "\nTriggering signals:" << std::endl;
    btn.clicked.emit("SaveButton");
    btn.positionChanged.emit(150, 200);
    
    std::cout << "\n✓ ADVANTAGES:" << std::endl;
    std::cout << "  • Type-safe (template-based)" << std::endl;
    std::cout << "  • No slot base class needed" << std::endl;
    std::cout << "  • Multiple handlers per signal" << std::endl;
    std::cout << "  • Clean, declarative syntax" << std::endl;
}

// ===================================================================
// 3. EVENT DISPATCHER WITH std::variant
// ===================================================================

struct MouseClick { int x; int y; std::string button; };
struct KeyPress { char key; bool ctrl; bool shift; };
struct WindowResize { int width; int height; };

using UIEvent = std::variant<MouseClick, KeyPress, WindowResize>;

class EventDispatcher {
private:
    std::vector<std::function<void(const UIEvent&)>> handlers;
    
public:
    void subscribe(std::function<void(const UIEvent&)> handler) {
        handlers.push_back(std::move(handler));
    }
    
    void dispatch(const UIEvent& event) {
        for (auto& handler : handlers) {
            handler(event);
        }
    }
};

void example_variant_dispatcher() {
    std::cout << "\n=== 3. EVENT DISPATCHER WITH std::variant ===" << std::endl;
    
    EventDispatcher dispatcher;
    
    // Subscribe with lambda that uses std::visit
    dispatcher.subscribe([](const UIEvent& event) {
        std::visit([](const auto& e) {
            using T = std::decay_t<decltype(e)>;
            
            if constexpr (std::is_same_v<T, MouseClick>) {
                std::cout << "    Mouse clicked: " << e.button 
                          << " at (" << e.x << "," << e.y << ")" << std::endl;
            } else if constexpr (std::is_same_v<T, KeyPress>) {
                std::cout << "    Key pressed: '" << e.key << "'"
                          << (e.ctrl ? " (Ctrl)" : "")
                          << (e.shift ? " (Shift)" : "") << std::endl;
            } else if constexpr (std::is_same_v<T, WindowResize>) {
                std::cout << "    Window resized: " << e.width << "x" << e.height << std::endl;
            }
        }, event);
    });
    
    // Another handler that only cares about mouse clicks
    dispatcher.subscribe([](const UIEvent& event) {
        if (auto* click = std::get_if<MouseClick>(&event)) {
            std::cout << "    [Logger] Mouse event at (" << click->x << "," << click->y << ")" << std::endl;
        }
    });
    
    std::cout << "\nDispatching events:" << std::endl;
    dispatcher.dispatch(MouseClick{100, 200, "left"});
    dispatcher.dispatch(KeyPress{'A', true, false});
    dispatcher.dispatch(WindowResize{1920, 1080});
    
    std::cout << "\n✓ ADVANTAGES:" << std::endl;
    std::cout << "  • No event hierarchy needed" << std::endl;
    std::cout << "  • Type-safe with std::variant" << std::endl;
    std::cout << "  • Compile-time dispatch with std::visit" << std::endl;
    std::cout << "  • Value semantics (no pointers)" << std::endl;
}

// ===================================================================
// 4. CALLBACK COMPOSITION (FLUENT API)
// ===================================================================

class AsyncTask {
private:
    std::function<void(int)> success_callback;
    std::function<void(const std::string&)> error_callback;
    std::function<void()> finally_callback;
    
public:
    AsyncTask& on_success(std::function<void(int)> callback) {
        success_callback = std::move(callback);
        return *this;
    }
    
    AsyncTask& on_error(std::function<void(const std::string&)> callback) {
        error_callback = std::move(callback);
        return *this;
    }
    
    AsyncTask& finally(std::function<void()> callback) {
        finally_callback = std::move(callback);
        return *this;
    }
    
    void execute(bool should_fail = false) {
        try {
            if (should_fail) {
                if (error_callback) error_callback("Task failed!");
            } else {
                if (success_callback) success_callback(42);
            }
        } catch (...) {
            if (error_callback) error_callback("Exception occurred");
        }
        
        if (finally_callback) finally_callback();
    }
};

void example_callback_composition() {
    std::cout << "\n=== 4. CALLBACK COMPOSITION (FLUENT API) ===" << std::endl;
    
    std::cout << "\n--- Success case ---" << std::endl;
    AsyncTask task1;
    task1.on_success([](int result) {
            std::cout << "    Success! Result: " << result << std::endl;
        })
        .on_error([](const std::string& error) {
            std::cout << "    Error: " << error << std::endl;
        })
        .finally([]() {
            std::cout << "    Cleanup completed" << std::endl;
        })
        .execute(false);
    
    std::cout << "\n--- Error case ---" << std::endl;
    AsyncTask task2;
    task2.on_success([](int result) {
            std::cout << "    Success! Result: " << result << std::endl;
        })
        .on_error([](const std::string& error) {
            std::cout << "    Error: " << error << std::endl;
        })
        .finally([]() {
            std::cout << "    Cleanup completed" << std::endl;
        })
        .execute(true);
    
    std::cout << "\n✓ ADVANTAGES:" << std::endl;
    std::cout << "  • No callback interface classes" << std::endl;
    std::cout << "  • Readable, sequential setup" << std::endl;
    std::cout << "  • Similar to JavaScript Promises" << std::endl;
    std::cout << "  • Fluent API (method chaining)" << std::endl;
}

// ===================================================================
// 5. REACTIVE OBSERVABLE
// ===================================================================

template<typename T>
class Observable {
private:
    T value;
    std::vector<std::function<void(const T&)>> subscribers;
    
public:
    Observable(T initial) : value(std::move(initial)) {}
    
    void subscribe(std::function<void(const T&)> subscriber) {
        subscriber(value);  // Immediate notification
        subscribers.push_back(std::move(subscriber));
    }
    
    void set(T new_value) {
        if (value != new_value) {
            value = std::move(new_value);
            for (auto& subscriber : subscribers) {
                subscriber(value);
            }
        }
    }
    
    const T& get() const { return value; }
};

void example_reactive_observable() {
    std::cout << "\n=== 5. REACTIVE OBSERVABLE ===" << std::endl;
    
    Observable<int> counter(0);
    
    // Subscribe with lambdas
    counter.subscribe([](int value) {
        std::cout << "    Subscriber 1: Counter = " << value << std::endl;
    });
    
    counter.subscribe([](int value) {
        if (value > 0 && value % 2 == 0) {
            std::cout << "    Subscriber 2: Even number " << value << std::endl;
        }
    });
    
    std::cout << "\nUpdating counter:" << std::endl;
    counter.set(1);
    counter.set(2);
    counter.set(3);
    counter.set(4);
    
    std::cout << "\n✓ ADVANTAGES:" << std::endl;
    std::cout << "  • Automatic propagation" << std::endl;
    std::cout << "  • No observer base class" << std::endl;
    std::cout << "  • Declarative subscriptions" << std::endl;
    std::cout << "  • Great for UI data binding" << std::endl;
}

// ===================================================================
// 6. EVENT QUEUE WITH LAMBDAS
// ===================================================================

class EventQueue {
private:
    std::queue<std::function<void()>> events;
    
public:
    void post(std::function<void()> event) {
        events.push(std::move(event));
    }
    
    void process_all() {
        while (!events.empty()) {
            auto event = std::move(events.front());
            events.pop();
            event();
        }
    }
    
    size_t size() const { return events.size(); }
};

void example_event_queue() {
    std::cout << "\n=== 6. EVENT QUEUE WITH LAMBDAS ===" << std::endl;
    
    EventQueue queue;
    
    // Post events as lambdas (no event classes needed!)
    queue.post([]() {
        std::cout << "    Event 1: Initialize" << std::endl;
    });
    
    queue.post([]() {
        std::cout << "    Event 2: Load config" << std::endl;
    });
    
    int data = 42;
    queue.post([data]() {
        std::cout << "    Event 3: Process data: " << data << std::endl;
    });
    
    std::cout << "\nProcessing " << queue.size() << " events:" << std::endl;
    queue.process_all();
    
    std::cout << "\n✓ ADVANTAGES:" << std::endl;
    std::cout << "  • No event class hierarchy" << std::endl;
    std::cout << "  • Can capture context in closure" << std::endl;
    std::cout << "  • Extremely flexible" << std::endl;
    std::cout << "  • Minimal boilerplate" << std::endl;
}

// ===================================================================
// 7. COMMAND PATTERN WITH LAMBDAS
// ===================================================================

class Command {
private:
    std::function<void()> execute_func;
    std::function<void()> undo_func;
    std::string description;
    
public:
    Command(std::function<void()> exec, std::function<void()> undo, std::string desc)
        : execute_func(std::move(exec))
        , undo_func(std::move(undo))
        , description(std::move(desc)) {}
    
    void execute() {
        std::cout << "    Execute: " << description << std::endl;
        if (execute_func) execute_func();
    }
    
    void undo() {
        std::cout << "    Undo: " << description << std::endl;
        if (undo_func) undo_func();
    }
};

void example_lambda_command() {
    std::cout << "\n=== 7. COMMAND PATTERN WITH LAMBDAS ===" << std::endl;
    
    std::string text = "Original";
    std::vector<Command> history;
    
    // Create commands with lambdas (no command classes!)
    history.emplace_back(
        [&text]() { text = "Modified 1"; },
        [&text]() { text = "Original"; },
        "Change to 'Modified 1'"
    );
    
    history.emplace_back(
        [&text]() { text = "Modified 2"; },
        [&text]() { text = "Modified 1"; },
        "Change to 'Modified 2'"
    );
    
    std::cout << "\nExecuting commands:" << std::endl;
    for (auto& cmd : history) {
        cmd.execute();
        std::cout << "      Text: " << text << std::endl;
    }
    
    std::cout << "\nUndoing commands:" << std::endl;
    for (auto it = history.rbegin(); it != history.rend(); ++it) {
        it->undo();
        std::cout << "      Text: " << text << std::endl;
    }
    
    std::cout << "\n✓ ADVANTAGES:" << std::endl;
    std::cout << "  • No command class hierarchy" << std::endl;
    std::cout << "  • Define behavior inline" << std::endl;
    std::cout << "  • Capture state automatically" << std::endl;
    std::cout << "  • Very concise" << std::endl;
}

// ===================================================================
// MAIN FUNCTION
// ===================================================================

int main() {
    std::cout << "\n================================================================" << std::endl;
    std::cout << "  EVENT-DRIVEN PROGRAMMING: MODERN LAMBDA-BASED APPROACH" << std::endl;
    std::cout << "================================================================" << std::endl;
    
    example_lambda_observer();
    example_signal_slot();
    example_variant_dispatcher();
    example_callback_composition();
    example_reactive_observable();
    example_event_queue();
    example_lambda_command();
    
    std::cout << "\n================================================================" << std::endl;
    std::cout << "  SUMMARY: WHY LAMBDAS ARE BETTER" << std::endl;
    std::cout << "================================================================" << std::endl;
    
    std::cout << "\n✅ ADVANTAGES OF LAMBDA-BASED APPROACH:" << std::endl;
    std::cout << "\n1. LESS BOILERPLATE" << std::endl;
    std::cout << "   • No need to define observer/listener classes" << std::endl;
    std::cout << "   • No inheritance hierarchies" << std::endl;
    std::cout << "   • Behavior defined at point of use" << std::endl;
    
    std::cout << "\n2. MORE FLEXIBLE" << std::endl;
    std::cout << "   • Can capture local state [=], [&]" << std::endl;
    std::cout << "   • Different behavior per subscription" << std::endl;
    std::cout << "   • Easy to compose and chain" << std::endl;
    
    std::cout << "\n3. BETTER READABILITY" << std::endl;
    std::cout << "   • Callback logic near subscription point" << std::endl;
    std::cout << "   • No jumping between class definitions" << std::endl;
    std::cout << "   • Intent is clear and local" << std::endl;
    
    std::cout << "\n4. TYPE SAFETY" << std::endl;
    std::cout << "   • std::function provides type checking" << std::endl;
    std::cout << "   • Template parameters enforce signatures" << std::endl;
    std::cout << "   • Compile-time errors for mismatches" << std::endl;
    
    std::cout << "\n5. MODERN C++ FEATURES" << std::endl;
    std::cout << "   • std::function (C++11)" << std::endl;
    std::cout << "   • Lambda expressions (C++11)" << std::endl;
    std::cout << "   • std::variant (C++17)" << std::endl;
    std::cout << "   • if constexpr (C++17)" << std::endl;
    std::cout << "   • Move semantics" << std::endl;
    
    std::cout << "\n⚠️ THINGS TO WATCH OUT FOR:" << std::endl;
    std::cout << "   • Lambda lifetime: Don't capture [&] if object may be destroyed" << std::endl;
    std::cout << "   • std::function has overhead: Consider templates for hot paths" << std::endl;
    std::cout << "   • Circular references: Can cause memory leaks" << std::endl;
    std::cout << "   • Check if std::function is empty before calling" << std::endl;
    
    std::cout << "\n💡 WHEN TO USE INHERITANCE INSTEAD:" << std::endl;
    std::cout << "   • Need polymorphic object identity" << std::endl;
    std::cout << "   • Complex state management" << std::endl;
    std::cout << "   • Multiple virtual methods needed" << std::endl;
    std::cout << "   • Performance-critical (avoid std::function overhead)" << std::endl;
    
    std::cout << "\n================================================================\n" << std::endl;
    
    return 0;
}
