#include "cli.hpp"

#include "logger.hpp"

#include <unordered_map>

namespace mud::cli {

    namespace {

        std::unordered_map<std::string, const Command *> _registeredCommands;
    }

    Parameter::Parameter(const std::string & name, const std::string & description, ParameterType type)
        : m_name(name), m_description(description), m_type(type), m_forceUseSuggestions(false)
    { }

    const std::string & Parameter::getName() const
    {
        return m_name;
    }

    const std::string & Parameter::getDescription() const
    {
        return m_description;
    }

    ParameterType Parameter::getType() const
    {
        return m_type;
    }

    const std::vector<std::string> & Parameter::getSuggestions() const
    {
        return m_suggestions;
    }

    bool Parameter::getForceUseSuggestions() const
    {
        return m_forceUseSuggestions;
    }

    void Parameter::setSuggestions(const std::vector<std::string> & suggestions, bool forceUseSuggestions)
    {
        m_suggestions = suggestions;
        m_forceUseSuggestions = forceUseSuggestions;
    }

    bool Parameter::validateArgument(const std::string & argumentString, std::string & response) const
    {
        if (argumentString.empty())
        {
            response = "Invalid argument '" + argumentString + "' for parameter '" + m_name + "': Argument is empty";
            return false;
        }

        if (m_forceUseSuggestions)
        {
            for (size_t idx = 0; idx < m_suggestions.size(); ++idx)
                if (argumentString == m_suggestions[idx])
                    return true;
            
            response = "Invalid argument '" + argumentString + "' for parameter '" + m_name + "': Argument must be one of the suggested values";
            return false;
        }
        
        return true;
    }

    ParameterBool::ParameterBool(const std::string & name, const std::string & description)
        : Parameter(name, description, ParameterType::Bool)
    {
        setSuggestions({"true", "false", "TRUE", "FALSE"}, true);
    }

    ParameterNumber::ParameterNumber(const std::string & name, const std::string & description)
        : Parameter(name, description, ParameterType::Number)
    { }

    bool ParameterNumber::validateArgument(const std::string & argumentString, std::string & response) const
    {
        // todo: validate string is number: negative/floating point
        response = "Invalid argument '" + argumentString + "' for parameter '" + m_name + "': Argument is not a number";
        return false;
    }

    ParameterString::ParameterString(const std::string & name, const std::string & description)
        : Parameter(name, description, ParameterType::String)
    { }

    Command::Command(const std::string & name, const std::string & description, const std::vector<Parameter> & parameters, const std::vector<Parameter> & optionalParameters)
        : m_name(name), m_description(description), m_parameters(parameters), m_optionalParameters(optionalParameters)
    { }

    const std::string & Command::getName() const
    {
        return m_name;
    }

    const std::string & Command::getDescription() const
    {
        return m_description;
    }

    const std::vector<Parameter> & Command::getParameters() const
    {
        return m_parameters;
    }

    const std::vector<Parameter> & Command::getOptionalParameters() const
    {
        return m_optionalParameters;
    }

    std::string Command::getFullDescription() const
    {
        return m_name + m_description;  
    }

    bool Command::execute(const std::vector<std::string> & argumentStrings, std::string & response) const
    {
        for (size_t idx = 0; idx < m_parameters.size(); ++idx)
        {
            if (idx >= argumentStrings.size()) 
            {
                response = "Missing argument for parameter '" + m_parameters[idx].getName() + "'";
                return false;
            }
            
            if (!m_parameters[idx].validateArgument(argumentStrings[idx], response))
                return false;
        }

        for (size_t idx = m_parameters.size(); idx < m_parameters.size() + m_optionalParameters.size(); ++idx)
            if (idx < argumentStrings.size()) 
                if (!m_optionalParameters[idx - m_parameters.size()].validateArgument(argumentStrings[idx], response))
                    return false;

        if (argumentStrings.size() > m_parameters.size() + m_optionalParameters.size())
        {
            response = "Too many arguments provided";
            return false;
        }
        
        // todo: call function with typed args

        return true;
    }

    void registerCommand(const Command * command)
    {
        auto iter = _registeredCommands.find(command->getName());

        if (iter == _registeredCommands.end())
        {
            _registeredCommands[command->getName()] = command;
        }
        else
        {
            log(LogLevel::Warning, "Failed to register command '" + command->getName() + "': a command with this name has already been registered", "CLI");
        }
    }

    void unregisterCommand(const Command * command)
    {
        _registeredCommands.erase(_registeredCommands.find(command->getName()));
    }

    bool submit(const std::string & input, std::string & response)
    {
        size_t first = input.find_first_not_of("\t\n\r\f\v");
        size_t last = input.find_last_not_of("\t\n\r\f\v");
        if (input.substr(first, (last - first + 1)).empty())
        {
            response = "Invalid input";
            return false;
        }

        // todo: split input by ("), (') then by ( )
        // todo: fail if unmatching quotes (") or (')
        std::vector<std::string> inputSegments;

        auto iter = _registeredCommands.find(inputSegments[0]);

        if (iter == _registeredCommands.end())
        {
            response = "Command '" + inputSegments[0] + "' not found";
            return false;
        }

        return iter->second->execute(std::vector<std::string>(inputSegments.begin() + 1, inputSegments.end() - 1), response);
    }
}