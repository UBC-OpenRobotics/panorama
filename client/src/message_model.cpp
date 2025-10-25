#include "client/message_model.hpp"

void MessageModel::addMessage(const std::string& message) {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        messages_.push_back(message);
    }
    notifyObservers();
}

std::vector<std::string> MessageModel::getMessages() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return messages_;
}

void MessageModel::addObserver(Observer observer) {
    observers_.push_back(observer);
}

void MessageModel::notifyObservers() {
    for (auto& observer : observers_) {
        observer();
    }
}