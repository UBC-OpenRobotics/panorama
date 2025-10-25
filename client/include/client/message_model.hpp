#pragma once
#include <string>
#include <vector>
#include <mutex>
#include <functional>

class MessageModel {
public:
    void addMessage(const std::string& message);
    std::vector<std::string> getMessages() const;
    
    // Observer pattern for MVC
    using Observer = std::function<void()>;
    void addObserver(Observer observer);
    
private:
    mutable std::mutex mutex_;
    std::vector<std::string> messages_;
    std::vector<Observer> observers_;
    
    void notifyObservers();
};