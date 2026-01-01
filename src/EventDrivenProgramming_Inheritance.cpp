#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <queue>

// ===================================================================
// EVENT-DRIVEN PROGRAMMING: TRADITIONAL INHERITANCE-BASED APPROACH
// ===================================================================
// This file demonstrates traditional event-driven programming using:
// - Abstract base classes
// - Virtual functions
// - Inheritance hierarchies
// - Classic OOP patterns
// ===================================================================

// ===================================================================
// 1. OBSERVER PATTERN (INHERITANCE-BASED)
// ===================================================================

// Abstract observer interface
class IObserver {
public:
    virtual ~IObserver() = default;
    virtual void on_notify(const std::string& event) = 0;
};

// Concrete observer implementations
class ConsoleObserver : public IObserver {
private:
    std::string name;
    
public:
    ConsoleObserver(std::string n) : name(std::move(n)) {}
    
    void on_notify(const std::string& event) override {
        std::cout << "    [" << name << "] Received: " << event << std::endl;
    }
};

class CountingObserver : public IObserver {
private:
    int count = 0;
    
public:
    void on_notify(const std::string& event) override {
        count++;
        std::cout << "    [Counter] Event #" << count << ": " << event << std::endl;
    }
    
    int get_count() const { return count; }
};

class Subject {
private:
    std::vector<IObserver*> observers;  // Raw pointers or shared_ptr
    
public:
    void attach(IObserver* observer) {
        observers.push_back(observer);
    }
    
    void detach(IObserver* observer) {
        observers.erase(
            std::remove(observers.begin(), observers.end(), observer),
            observers.end()
        );
    }
    
    void notify(const std::string& event) {
        std::cout << "  [Subject] Notifying " << observers.size() << " observers" << std::endl;
        for (auto* observer : observers) {
            observer->on_notify(event);
        }
    }
};

void example_inheritance_observer() {
    std::cout << "\n=== 1. OBSERVER PATTERN (INHERITANCE-BASED) ===" << std::endl;
    
    Subject subject;
    
    // Need to create concrete observer objects
    ConsoleObserver obs1("Observer1");
    ConsoleObserver obs2("Observer2");
    CountingObserver counter;
    
    subject.attach(&obs1);
    subject.attach(&obs2);
    subject.attach(&counter);
    
    subject.notify("User logged in");
    subject.notify("Data updated");
    
    std::cout << "\n❌ DISADVANTAGES:" << std::endl;
    std::cout << "  • Must define observer classes" << std::endl;
    std::cout << "  • Need inheritance hierarchy" << std::endl;
    std::cout << "  • More boilerplate code" << std::endl;
    std::cout << "  • Lifetime management issues (pointers)" << std::endl;
}

// ===================================================================
// 2. EVENT HIERARCHY (INHERITANCE-BASED)
// ===================================================================

// Abstract event base class
class Event {
public:
    virtual ~Event() = default;
    virtual std::string get_name() const = 0;
    virtual void print() const = 0;
};

class MouseClickEvent : public Event {
private:
    int x, y;
    std::string button;
    
public:
    MouseClickEvent(int x, int y, std::string btn)
        : x(x), y(y), button(std::move(btn)) {}
    
    std::string get_name() const override { return "MouseClick"; }
    
    void print() const override {
        std::cout << "    Mouse clicked: " << button 
                  << " at (" << x << "," << y << ")" << std::endl;
    }
    
    int get_x() const { return x; }
    int get_y() const { return y; }
    const std::string& get_button() const { return button; }
};

class KeyPressEvent : public Event {
private:
    char key;
    bool ctrl;
    
public:
    KeyPressEvent(char k, bool c) : key(k), ctrl(c) {}
    
    std::string get_name() const override { return "KeyPress"; }
    
    void print() const override {
        std::cout << "    Key pressed: '" << key << "'"
                  << (ctrl ? " (Ctrl)" : "") << std::endl;
    }
    
    char get_key() const { return key; }
    bool has_ctrl() const { return ctrl; }
};

// Abstract event handler
class IEventHandler {
public:
    virtual ~IEventHandler() = default;
    virtual void handle_event(const Event& event) = 0;
};

class EventLogger : public IEventHandler {
public:
    void handle_event(const Event& event) override {
        std::cout << "    [Logger] Handling " << event.get_name() << std::endl;
        event.print();
    }
};

class MouseHandler : public IEventHandler {
public:
    void handle_event(const Event& event) override {
        // Need to use dynamic_cast to access specific event data
        if (auto* mouse_event = dynamic_cast<const MouseClickEvent*>(&event)) {
            std::cout << "    [MouseHandler] Processing mouse at ("
                      << mouse_event->get_x() << "," << mouse_event->get_y() << ")" << std::endl;
        }
        // Ignore other event types
    }
};

class EventDispatcher {
private:
    std::vector<IEventHandler*> handlers;
    
public:
    void subscribe(IEventHandler* handler) {
        handlers.push_back(handler);
    }
    
    void dispatch(const Event& event) {
        for (auto* handler : handlers) {
            handler->handle_event(event);
        }
    }
};

void example_event_hierarchy() {
    std::cout << "\n=== 2. EVENT HIERARCHY (INHERITANCE-BASED) ===" << std::endl;
    
    EventDispatcher dispatcher;
    
    EventLogger logger;
    MouseHandler mouse_handler;
    
    dispatcher.subscribe(&logger);
    dispatcher.subscribe(&mouse_handler);
    
    std::cout << "\nDispatching events:" << std::endl;
    MouseClickEvent click(100, 200, "left");
    dispatcher.dispatch(click);
    
    KeyPressEvent key('A', true);
    dispatcher.dispatch(key);
    
    std::cout << "\n❌ DISADVANTAGES:" << std::endl;
    std::cout << "  • Need event class hierarchy" << std::endl;
    std::cout << "  • Must use dynamic_cast (RTTI)" << std::endl;
    std::cout << "  • Pointer semantics required" << std::endl;
    std::cout << "  • More complex type checking" << std::endl;
}

// ===================================================================
// 3. LISTENER PATTERN (CLASSIC GUI APPROACH)
// ===================================================================

class IButtonListener {
public:
    virtual ~IButtonListener() = default;
    virtual void on_button_clicked(const std::string& button_id) = 0;
};

class Button {
private:
    std::string id;
    std::vector<IButtonListener*> listeners;
    
public:
    Button(std::string button_id) : id(std::move(button_id)) {}
    
    void add_listener(IButtonListener* listener) {
        listeners.push_back(listener);
    }
    
    void click() {
        std::cout << "  [Button '" << id << "'] Clicked!" << std::endl;
        for (auto* listener : listeners) {
            listener->on_button_clicked(id);
        }
    }
};

class SaveHandler : public IButtonListener {
public:
    void on_button_clicked(const std::string& button_id) override {
        std::cout << "    [SaveHandler] Saving file..." << std::endl;
    }
};

class LogHandler : public IButtonListener {
public:
    void on_button_clicked(const std::string& button_id) override {
        std::cout << "    [LogHandler] Logging click on " << button_id << std::endl;
    }
};

void example_listener_pattern() {
    std::cout << "\n=== 3. LISTENER PATTERN (CLASSIC GUI) ===" << std::endl;
    
    Button save_btn("SaveButton");
    
    SaveHandler save_handler;
    LogHandler log_handler;
    
    save_btn.add_listener(&save_handler);
    save_btn.add_listener(&log_handler);
    
    save_btn.click();
    
    std::cout << "\n❌ DISADVANTAGES:" << std::endl;
    std::cout << "  • Need listener interface classes" << std::endl;
    std::cout << "  • Can't define behavior inline" << std::endl;
    std::cout << "  • More classes to maintain" << std::endl;
    std::cout << "  • Harder to share state" << std::endl;
}

// ===================================================================
// 4. COMMAND PATTERN (INHERITANCE-BASED)
// ===================================================================

class ICommand {
public:
    virtual ~ICommand() = default;
    virtual void execute() = 0;
    virtual void undo() = 0;
    virtual std::string get_description() const = 0;
};

class TextCommand : public ICommand {
private:
    std::string& text_ref;
    std::string new_text;
    std::string old_text;
    
public:
    TextCommand(std::string& text, std::string new_val)
        : text_ref(text), new_text(std::move(new_val)), old_text(text) {}
    
    void execute() override {
        std::cout << "    Execute: Set text to '" << new_text << "'" << std::endl;
        text_ref = new_text;
    }
    
    void undo() override {
        std::cout << "    Undo: Restore text to '" << old_text << "'" << std::endl;
        text_ref = old_text;
    }
    
    std::string get_description() const override {
        return "Change text to '" + new_text + "'";
    }
};

class CommandManager {
private:
    std::vector<std::unique_ptr<ICommand>> history;
    size_t current = 0;
    
public:
    void execute(std::unique_ptr<ICommand> cmd) {
        cmd->execute();
        
        // Clear redo history
        history.erase(history.begin() + current, history.end());
        
        history.push_back(std::move(cmd));
        current = history.size();
    }
    
    void undo() {
        if (current > 0) {
            current--;
            history[current]->undo();
        }
    }
    
    void redo() {
        if (current < history.size()) {
            history[current]->execute();
            current++;
        }
    }
};

void example_command_pattern() {
    std::cout << "\n=== 4. COMMAND PATTERN (INHERITANCE-BASED) ===" << std::endl;
    
    std::string document = "Original";
    CommandManager manager;
    
    std::cout << "\nExecuting commands:" << std::endl;
    manager.execute(std::make_unique<TextCommand>(document, "Modified 1"));
    std::cout << "      Text: " << document << std::endl;
    
    manager.execute(std::make_unique<TextCommand>(document, "Modified 2"));
    std::cout << "      Text: " << document << std::endl;
    
    std::cout << "\nUndoing commands:" << std::endl;
    manager.undo();
    std::cout << "      Text: " << document << std::endl;
    
    manager.undo();
    std::cout << "      Text: " << document << std::endl;
    
    std::cout << "\n❌ DISADVANTAGES:" << std::endl;
    std::cout << "  • Need command class for each operation" << std::endl;
    std::cout << "  • More verbose class definitions" << std::endl;
    std::cout << "  • State management in constructors" << std::endl;
    std::cout << "  • Can't define commands inline" << std::endl;
}

// ===================================================================
// 5. CALLBACK INTERFACE (OLD-SCHOOL)
// ===================================================================

class ICallback {
public:
    virtual ~ICallback() = default;
    virtual void on_success(int result) = 0;
    virtual void on_error(const std::string& error) = 0;
};

class AsyncOperation {
private:
    ICallback* callback = nullptr;
    
public:
    void set_callback(ICallback* cb) {
        callback = cb;
    }
    
    void execute(bool should_fail = false) {
        if (callback) {
            if (should_fail) {
                callback->on_error("Operation failed!");
            } else {
                callback->on_success(42);
            }
        }
    }
};

class MyCallback : public ICallback {
public:
    void on_success(int result) override {
        std::cout << "    Success! Result: " << result << std::endl;
    }
    
    void on_error(const std::string& error) override {
        std::cout << "    Error: " << error << std::endl;
    }
};

void example_callback_interface() {
    std::cout << "\n=== 5. CALLBACK INTERFACE (OLD-SCHOOL) ===" << std::endl;
    
    AsyncOperation op;
    MyCallback callback;
    
    op.set_callback(&callback);
    
    std::cout << "\n--- Success case ---" << std::endl;
    op.execute(false);
    
    std::cout << "\n--- Error case ---" << std::endl;
    op.execute(true);
    
    std::cout << "\n❌ DISADVANTAGES:" << std::endl;
    std::cout << "  • Must define callback class" << std::endl;
    std::cout << "  • Can't chain callbacks easily" << std::endl;
    std::cout << "  • Only one callback per operation" << std::endl;
    std::cout << "  • Less readable than fluent API" << std::endl;
}

// ===================================================================
// MAIN FUNCTION
// ===================================================================

int main() {
    std::cout << "\n================================================================" << std::endl;
    std::cout << "  EVENT-DRIVEN PROGRAMMING: TRADITIONAL INHERITANCE APPROACH" << std::endl;
    std::cout << "================================================================" << std::endl;
    
    example_inheritance_observer();
    example_event_hierarchy();
    example_listener_pattern();
    example_command_pattern();
    example_callback_interface();
    
    std::cout << "\n================================================================" << std::endl;
    std::cout << "  SUMMARY: PROBLEMS WITH INHERITANCE-BASED APPROACH" << std::endl;
    std::cout << "================================================================" << std::endl;
    
    std::cout << "\n❌ DISADVANTAGES OF INHERITANCE-BASED APPROACH:" << std::endl;
    
    std::cout << "\n1. BOILERPLATE CODE" << std::endl;
    std::cout << "   • Must define interface classes (IObserver, IListener, etc.)" << std::endl;
    std::cout << "   • Must define concrete implementations" << std::endl;
    std::cout << "   • Each behavior needs a new class" << std::endl;
    std::cout << "   • Lots of virtual functions" << std::endl;
    
    std::cout << "\n2. INFLEXIBILITY" << std::endl;
    std::cout << "   • Can't define behavior inline" << std::endl;
    std::cout << "   • Hard to share state between callbacks" << std::endl;
    std::cout << "   • Difficult to compose behaviors" << std::endl;
    std::cout << "   • Rigid class hierarchies" << std::endl;
    
    std::cout << "\n3. LIFETIME MANAGEMENT" << std::endl;
    std::cout << "   • Raw pointers cause dangling reference bugs" << std::endl;
    std::cout << "   • shared_ptr adds complexity and overhead" << std::endl;
    std::cout << "   • Must manage attach/detach carefully" << std::endl;
    std::cout << "   • Who owns the observer objects?" << std::endl;
    
    std::cout << "\n4. POOR READABILITY" << std::endl;
    std::cout << "   • Behavior scattered across multiple files" << std::endl;
    std::cout << "   • Must jump between class definitions" << std::endl;
    std::cout << "   • Harder to see full picture" << std::endl;
    std::cout << "   • Intent hidden in class names" << std::endl;
    
    std::cout << "\n5. RTTI AND CASTING" << std::endl;
    std::cout << "   • Need dynamic_cast for event hierarchies" << std::endl;
    std::cout << "   • Runtime type checking (slower)" << std::endl;
    std::cout << "   • Can fail at runtime" << std::endl;
    std::cout << "   • Not as type-safe" << std::endl;
    
    std::cout << "\n6. MAINTENANCE BURDEN" << std::endl;
    std::cout << "   • More files to maintain" << std::endl;
    std::cout << "   • More classes in codebase" << std::endl;
    std::cout << "   • Harder to refactor" << std::endl;
    std::cout << "   • Changes ripple through hierarchy" << std::endl;
    
    std::cout << "\n✅ WHEN INHERITANCE IS JUSTIFIED:" << std::endl;
    std::cout << "   • Polymorphic object collections" << std::endl;
    std::cout << "   • Complex state machines" << std::endl;
    std::cout << "   • Need object identity (not just behavior)" << std::endl;
    std::cout << "   • Multiple related virtual methods" << std::endl;
    std::cout << "   • Performance critical (no std::function overhead)" << std::endl;
    std::cout << "   • Plugin architectures" << std::endl;
    
    std::cout << "\n💡 MODERN ALTERNATIVE:" << std::endl;
    std::cout << "   Use EventDrivenProgramming_Lambdas.cpp for:" << std::endl;
    std::cout << "   ✓ std::function + lambdas (no inheritance)" << std::endl;
    std::cout << "   ✓ std::variant (no event hierarchy)" << std::endl;
    std::cout << "   ✓ Inline behavior definitions" << std::endl;
    std::cout << "   ✓ Automatic state capture" << std::endl;
    std::cout << "   ✓ Much less boilerplate" << std::endl;
    
    std::cout << "\n📊 COMPARISON:" << std::endl;
    std::cout << "   Inheritance approach: ~200 lines for basic observer" << std::endl;
    std::cout << "   Lambda approach:      ~50 lines for same functionality" << std::endl;
    std::cout << "   Code reduction:       75% less code!" << std::endl;
    
    std::cout << "\n================================================================\n" << std::endl;
    
    return 0;
}
