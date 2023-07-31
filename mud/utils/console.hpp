#ifndef CONSOLE_HPP
#define CONSOLE_HPP

#include <functional>
#include <sstream>
#include <string>
#include <vector>

#include "text_input_buffer.hpp"

// command id

namespace mud::console
{
    enum class ParameterType
    {
        Number,
        Bool,
        String
    };

    struct ParameterInfo
    {
        ParameterType type;
        std::string name;
        std::string description;

        std::string getSignature() const
        {
            switch (type)
            {
            case ParameterType::Number:
                return name + " (number)";
            case ParameterType::Bool:
                return name + " (bool)";
            case ParameterType::String:
                return name + " (string)";
            }
            return name + "(UNSUPPORTED)";
        }
    };

    class Argument
    {
    public:

        Argument(const Argument & copy)
            : Argument(copy.m_info, copy.m_value)
        { }

        ~Argument()
        {
            switch (m_info.type)
            {
            case ParameterType::Number:
                delete reinterpret_cast<const double *>(m_value);
                break;
            
            case ParameterType::Bool:
                delete reinterpret_cast<const bool *>(m_value);
                break;

            case ParameterType::String:
                delete reinterpret_cast<const std::string *>(m_value);
                break;
            }
        }

        static Argument * tryParseString(const ParameterInfo & info, const std::string & string, std::string & error)
        {
            switch (info.type)
            {
            case ParameterType::Number:
                try
                {
                    double d = std::stod(string);
                    return new Argument(info, reinterpret_cast<const void *>(&d));
                }
                catch (const std::invalid_argument & e)
                {
                    error = "Expected number value";
                    break;
                }
                catch (const std::out_of_range & e)
                {
                    error = "Value out of range";
                    break;
                }

            case ParameterType::Bool:
                if (string == "true" || string == "1")
                {
                    const bool b = true;
                    return new Argument(info, reinterpret_cast<const void *>(&b));
                }
                else if (string == "false" || string == "0")
                {
                    const bool b = false;
                    return new Argument(info, reinterpret_cast<const void *>(&b));
                }
                else
                {
                    error = "Expected bool value (true | false | 1 | 0)";
                    break;
                }

            case ParameterType::String:
                return new Argument(info, reinterpret_cast<const void *>(&string));
            }
            
            return nullptr;
        }

        template <typename T>
        const T & getValue() const
        {
            return *reinterpret_cast<const T *>(m_value);
        }

    private:

        Argument(const ParameterInfo & info, const void * value)
            : m_info(info), m_value(nullptr)
        {
            switch (info.type)
            {
            case ParameterType::Number:
            {
                const double & numberValue = *reinterpret_cast<const double *>(value);
                m_value = new double(numberValue);
                break;
            }

            case ParameterType::Bool:
            {
                const bool & boolValue = *reinterpret_cast<const bool *>(value);
                m_value = new bool(boolValue);
                break;
            }

            case ParameterType::String:
            {
                const std::string & stringValue = *reinterpret_cast<const std::string *>(value);
                m_value = new std::string(stringValue);
                break;
            }
            }
        }

        ParameterInfo m_info;
        void * m_value;
    };

    class Command
    {
    public:

        using CommandFunc = std::function<std::string(const std::vector<Argument *> & arguments)>;

        Command(const std::string & id, const std::string & description, const std::vector<ParameterInfo> & parameters, CommandFunc function)
            : m_id(id), m_description(description), m_parameters(parameters), m_function(function)
        { }

        const std::string & getId() const
        {
            return m_id;
        }

        const std::string & getDescription() const
        {
            return m_description;
        }

        const std::vector<ParameterInfo> & getParameters() const
        {
            return m_parameters;
        }

        const std::string execute(const std::vector<Argument *> & arguments) const
        {
            return m_function(arguments);
        }

    private:

        const std::string m_id;
        const std::string m_description;
        const std::vector<ParameterInfo> m_parameters;
        CommandFunc m_function;
    };

    void init(Window & window);

    void destroy();
    
    TextInputBuffer * getInputBuffer();

    void registerCommand(const Command * command);

    void unregisterCommand(const Command * command);

    std::string execute(const std::string & commandString);
}

#endif