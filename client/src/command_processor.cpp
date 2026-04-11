#include "client/command_processor.hpp"
#include "client/DataBuffer.hpp"

/*
TO DO: add stop mechanism for command processor thread, currently it runs indefinitely and can only be stopped by exiting the program
TO DO: add command_processor in onExit() in main.cpp
TO DO: handle race conditions between command processor and tcp client both accessing data buffer using mutex
*/

CommandProcessor::CommandProcessor(std::shared_ptr<DataBuffer> dataBuffer, std::shared_ptr<PostProcessing> postProcessor)
    : dataBuffer_(dataBuffer), postProcessor_(postProcessor) {
        
}

void CommandProcessor::start() {
    while(running_) {
            std::cout << "Enter command (type 'exit' to quit): ";
            std::getline(std::cin, command);

            if (command == "exit") {
                break;
            }

            
            processCommand(command);
        }
}

void CommandProcessor::stop() {
    running_ = false;
}

void CommandProcessor::processCommand(const std::string& command) {
    /*
    command:
        <function> <parameter1> <parameter2>
    */
    std::string cmdType = command.substr(0, command.find(' '));

    //get the parameters from command
    size_t firstSpace = command.find(' ');
    size_t secondSpace = command.find(' ', firstSpace + 1);
    std::string firstParameter = command.substr(firstSpace + 1, secondSpace - firstSpace - 1);
    std::string secondParameter = command.substr(secondSpace + 1);

    if(cmdType == "printAll") {
        dataBuffer_->printAll();
        
    } else if (cmdType == "clear") {
        dataBuffer_->clear();

    } else if (cmdType == "size"){
        std::cout << "Buffer size: " << dataBuffer_->size() << std::endl;
    } else if (cmdType == "reset"){
        //reset all post processing parameters to default values
        postProcessor_->addOffset(0.0);
        postProcessor_->addScaling(1.0);
        //postProcessor_->reset();
        //std::cout << "Post processing parameters reset to default values." << std::endl;
    } else if (cmdType == "setOffset"){
        float offset = std::stof(firstParameter);
        postProcessor_->addOffset(offset);
        //std::cout << "Offset set to: " << offset << std::endl;
    } else if (cmdType == "setScale"){
        float scaleFactor = std::stof(firstParameter);
        postProcessor_->addScaling(scaleFactor);
        //std::cout << "Scale factor set to: " << scaleFactor << std::endl;
    } else {
        std::cout << "Unknown command: " << command << std::endl;
    }

    //add more commands as needed
}