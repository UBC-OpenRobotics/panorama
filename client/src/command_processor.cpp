#include "client/command_processor.hpp"
#include "client/DataBuffer.hpp"


/*
TO DO: add stop mechanism for command processor thread, currently it runs indefinitely and can only be stopped by exiting the program
TO DO: add command_processor in onExit() in main.cpp
TO DO: handle race conditions between command processor and tcp client both accessing data buffer using mutex
*/

CommandProcessor::CommandProcessor(std::shared_ptr<DataBuffer> dataBuffer)
    : dataBuffer_(dataBuffer) {
        
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
        conversion <data_type> <target_unit>
        add_offset <data_type> <offset_value>
    */
    std::string cmdType = command.substr(0, command.find(' '));

    //get the parameters from command
    size_t firstSpace = command.find(' ');
    size_t secondSpace = command.find(' ', firstSpace + 1);
    std::string firstParameter = command.substr(firstSpace + 1, secondSpace - firstSpace - 1);
    std::string secondParameter = command.substr(secondSpace + 1);

    if(cmdType == "conversion") {
        dataBuffer_->convertData(firstParameter, secondParameter);
    } else if (cmdType == "add_offset") {
        dataBuffer_->addOffset(firstParameter, std::stof(secondParameter));
    } else {
        std::cout << "Unknown command: " << command << std::endl;
    }
}