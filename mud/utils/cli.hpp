// map a function pointer to a name that can be entered in to a cli along with following parameters
// try and call function and parse parameters and pass in to function call

// eg

// new command "print" = print(string)
// call command: print andhereissomestring
// try and call print() passing the appended option in as arg???

#include <string>
#include <vector>

namespace mud::cli {

    enum class ParameterType
    {
        Bool,
        Number,
        String
    };

    class Parameter
    {
    public:

        Parameter(const std::string & name, const std::string & description, ParameterType type);

        const std::string & getName() const;

        const std::string & getDescription() const;

        ParameterType getType() const;

        const std::vector<std::string> & getSuggestions() const;

        bool getForceUseSuggestions() const;

        void setSuggestions(const std::vector<std::string> & suggestions, bool forceUseSuggestions = false);

        virtual bool validateArgument(const std::string & argumentString, std::string & response) const;

    protected:

        const std::string m_name;

        const std::string m_description;

        const ParameterType m_type;

        std::vector<std::string> m_suggestions;

        bool m_forceUseSuggestions;
    };

    class ParameterBool : public Parameter
    {
    public:

        ParameterBool(const std::string & name, const std::string & description);
    };

    class ParameterNumber : public Parameter
    {
    public:

        ParameterNumber(const std::string & name, const std::string & description);

        bool validateArgument(const std::string & argumentString, std::string & response) const override;
    };

    class ParameterString : public Parameter
    {
    public:

        ParameterString(const std::string & name, const std::string & description);
    };

    class Command
    {
    public:

        Command(const std::string & name, const std::string & description, const std::vector<Parameter> & parameters = {}, const std::vector<Parameter> & optionalParameters = {});

        const std::string & getName() const;

        const std::string & getDescription() const;

        const std::vector<Parameter> & getParameters() const;

        const std::vector<Parameter> & getOptionalParameters() const;

        std::string getFullDescription() const;

        bool execute(const std::vector<std::string> & argumentStrings, std::string & response) const;

    private:

        const std::string m_name;

        const std::string m_description;

        const std::vector<Parameter> m_parameters;

        const std::vector<Parameter> m_optionalParameters;
    };

    void registerCommand(const Command * command);

    void unregisterCommand(const Command * command);

    void submit(const std::string & input);
}