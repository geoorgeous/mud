#include "console.hpp"

#include "logger.hpp"

#include <unordered_map>

namespace mud::console
{
    namespace
    {
        TextInputBuffer::EventOnSubmit::TListenerCallbackFree * _inputBufferOnSubmitListener = nullptr;
        TextInputBuffer * _inputBuffer = nullptr;

        std::unordered_map<std::string, const Command *> _commands;

        Command * _helpCommand;
        Command * _logCommand;
    }

    void onConsoleInputBufferSubmit(TextInputBuffer & inputBuffer)
    {
        execute(inputBuffer.getContent());
        inputBuffer.reset();
    }

    void init(Window & window)
    {
        _inputBufferOnSubmitListener = new TextInputBuffer::EventOnSubmit::TListenerCallbackFree(&onConsoleInputBufferSubmit);
        _inputBuffer = new TextInputBuffer(window);
        _inputBuffer->getEventOnSubmit().addListener(_inputBufferOnSubmitListener);

        // command: log
        // description: The string to log
        // params:
        // - log message (string): The string to log

        _helpCommand = new Command("help", "Displays information about the console and existing commands", {}, [](const std::vector<Argument *> & arguments) -> std::string {
            std::stringstream ss;
            for (auto iter : _commands)
            {
                ss << "id: " << iter.second->getId() << '\n';
                ss << "description: " << iter.second->getDescription() << '\n';
                if (iter.second->getParameters().size() > 0)
                ss << "params:\n";
                for (const ParameterInfo & paramInfo : iter.second->getParameters())
                    ss << "- " << paramInfo.getSignature() << ": " << paramInfo.description << '\n';
                ss << '\n';
            }
            return ss.str();
        });
        registerCommand(_helpCommand);

        _logCommand = new Command("log", "Logs a string", { ParameterInfo{ParameterType::String, "log message", "The string to log"} }, [](const std::vector<Argument *> & arguments) -> std::string {
            log(LogLevel::Info, arguments[0]->getValue<std::string>() + '\n');
            return "";
        });
        registerCommand(_logCommand);
    }

    void destroy()
    {
        delete _inputBuffer;
        delete _inputBufferOnSubmitListener;

        delete _helpCommand;
        delete _logCommand;
    }

    TextInputBuffer * getInputBuffer()
    {
        return _inputBuffer;
    }

    void registerCommand(const Command * command)
    {
        auto iter = _commands.find(command->getId());
        if (iter != _commands.end())
            return;
        _commands[command->getId()] = command;
    }

    void unregisterCommand(const Command * command)
    {
        auto iter = _commands.find(command->getId());
        if (iter == _commands.end())
            return;
        _commands.erase(iter);
    }

    std::string execute(const std::string & commandString)
    {
        std::vector<std::string> parts;
        std::stringstream ss;
        for(size_t i=0; i< commandString.length(); i++)
        {
            char c = commandString[i];
            if( c == ' ' )
            {
                parts.push_back(ss.str());
                ss.str(std::string());
            }
            else if(c == '\"' || c == '\'')
            {
                i++;
                while(i < commandString.length() && commandString[i] != c)
                {
                    ss << commandString[i];
                    i++;
                }
            }
            else
                ss << c;
        }
        
        parts.push_back(ss.str());

        std::string response;

        if (parts.size() == 0)
            return response;

        const auto & iter = _commands.find(parts[0]);
        if (iter == _commands.end())
            response = fmt::format("No such command \"{0}\"", parts[0]);
        else
        {
            const Command & command = *iter->second;
            std::vector<Argument *> parsedArguments;

            for (size_t idx = 1; idx < parts.size(); idx++)
            {
                if (idx > command.getParameters().size())
                    break;

                std::string error;
                Argument * newArgument = Argument::tryParseString(command.getParameters()[idx - 1], parts[idx], error);

                if (newArgument == nullptr)
                {
                    response = fmt::format("Failed to parse argument \"{0}\" for parameter \"{1}\": {2}", parts[idx], command.getParameters()[idx - 1].getSignature(), error);
                    break;
                }

                parsedArguments.push_back(newArgument);
            }

            if (parsedArguments.size() != command.getParameters().size())
                response = fmt::format("Number of arguments provided does not match number of command parameters");
            else
                response = command.execute(parsedArguments);
            
            for (Argument * argument : parsedArguments)
                delete argument;
        }

        if (!response.empty())
            log(LogLevel::Info, fmt::format("{0}\n", response));
        
        return response;
    }
}