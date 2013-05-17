#pragma once
#include <string>
#include <stdexcept>
#include <unordered_map>
#include <map>
#include <type_traits>
#include <cassert>
#include "SpotPlugin.h"
#include "PluginHost.h"
#include "Utilities.h"


namespace HostInterop
{

    enum class VariableType
    {
        Bool,
        Text,
        Numeric,
        Integer 
    };

    enum class ScopeFlags
    {
        Unknown             = 0x00,
        ImageMetaData       = 0x01,
        CameraSetting       = 0x02,
        UserSetting         = 0x04,
        CaseInformation     = 0x08,
        FilePath            = 0x10,
        Measurment          = 0x20,
        Reporting           = 0x40,
        ApplicationState    = 0x80
    };

    inline ScopeFlags operator | (ScopeFlags a, ScopeFlags b)
    { return static_cast<ScopeFlags>(static_cast<std::underlying_type<ScopeFlags>::type>(a) | static_cast<std::underlying_type<ScopeFlags>::type>(b));}

    inline ScopeFlags operator & (ScopeFlags a, ScopeFlags b)
    { return static_cast<ScopeFlags>(static_cast<std::underlying_type<ScopeFlags>::type>(a) & static_cast<std::underlying_type<ScopeFlags>::type>(b));}

namespace internal // implementation specific namespace not for general usage
{
    template<size_t MaxReadLength>
    static inline std::string _GetTextVariable(const char* name)
    {
        static_assert(MaxReadLength < 65536u, "Warning - potential stack overflow detected. Consider using a different method to retrieve a variable of this size.");
        char szTextBuffer[MaxReadLength + 1];
        SpotPluginApi::msg_get_set_variable_t getVarMsg;
        getVarMsg.DataType = SpotPluginApi::msg_get_set_variable_t::Text;
        getVarMsg.VariableName = name;
        getVarMsg.TextValue = SpotPluginApi::make_text_variable(szTextBuffer, MaxReadLength);
        if (!PluginHost::DoAction( SpotPluginApi::HostActionRequest::GetVariable, 0, &getVarMsg))
            throw std::runtime_error(std::string("Error getting text macro variable named ") + name);
        getVarMsg.TextValue.UpdateLength();
        return std::string(getVarMsg.TextValue.c_str());
    }
} // end namespace internal

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    /// Summary:
    ///     Sets a global text variable with a matching name to a new value.
    /// Arguments:
    ///     name  - A null terminated string of the name of the target variable
    ///     value - The value to set the variable to.
    /// Returns:
    ///     void
    /// Throws:
    ///     runtime_error if unable to set the variable value
    static void SetTextVariable(const char* name, const std::string& value)
    {
        SpotPluginApi::msg_get_set_variable_t setVarMsg;
        setVarMsg.DataType = SpotPluginApi::msg_get_set_variable_t::Text;
        setVarMsg.VariableName = name;
        setVarMsg.TextValue = SpotPluginApi::make_text_variable(value);
        if (!PluginHost::DoAction( SpotPluginApi::HostActionRequest::SetVariable, 0, &setVarMsg))
            throw std::runtime_error(std::string("Error setting text macro variable named ") + name);
    }

    /// Summary:
    ///     Returns a string with the current value of a global text variable with a matching name.
    /// Arguments:
    ///     name - A null terminated string of the name of the target variable
    /// Template Arguments:
    ///     MaxReadLength - The maximum length of the variable string to return.
    ///                     Make sure there is enough stack space available to allocate a char buffer of this size.
    /// Returns:
    ///     A std::string that is set to the current value of the global variable.
    /// Throws:
    ///     runtime_error if unable to get the variable value
    template<size_t MaxReadLength>
    static std::string GetTextVariable(const char* name)
    { return internal::_GetTextVariable<MaxReadLength>(name); }

    
    /// Summary:
    ///     Returns a string with the current value of a global text variable with a matching name.
    /// Arguments:
    ///     name - A null terminated string of the name of the target variable
    /// Returns:
    ///     A std::string that is set to the current value of the global variable up to the first 1024 characters.
    /// Throws:
    ///     runtime_error if unable to get the variable value
    static inline std::string GetTextVariable(const char* name)
    { return internal::_GetTextVariable<1024>(name); }

    
    /// Summary:
    ///     Sets a global numeric variable with a matching name to a new value.
    /// Arguments:
    ///     name  - A null terminated string of the name of the target variable
    ///     value - The value to set the variable to.
    /// Returns:
    ///     void
    /// Throws:
    ///     runtime_error if unable to set the variable value
    static inline void SetNumericVariable(const char* name, double value)
    {
        SpotPluginApi::msg_get_set_variable_t setVarMsg;
        setVarMsg.DataType = SpotPluginApi::msg_get_set_variable_t::Numeric;
        setVarMsg.VariableName = name;
        setVarMsg.NumericValue = value;
        if (!PluginHost::DoAction( SpotPluginApi::HostActionRequest::SetVariable, 0, &setVarMsg))
            throw std::runtime_error(std::string("Error setting numeric macro variable named ") + name);
    }
    
    /// Summary:
    ///     Returns the current value of a global numeric variable with a matching name.
    /// Arguments:
    ///     name - A null terminated string of the name of the target variable
    /// Returns:
    ///     A double precision float set to the current value of the global variable
    /// Throws:
    ///     runtime_error if unable to get the variable value
    static inline double GetNumericVariable(const char* name)
    {
        SpotPluginApi::msg_get_set_variable_t getVarMsg;
        getVarMsg.DataType = SpotPluginApi::msg_get_set_variable_t::Numeric;
        getVarMsg.VariableName = name;
        if (!PluginHost::DoAction( SpotPluginApi::HostActionRequest::GetVariable, 0, &getVarMsg))
            throw std::runtime_error(std::string("Error getting numeric macro variable named ") + name);
        return getVarMsg.NumericValue;
    }

    /// Summary:
    ///     Sets a global Boolean variable with a matching name to a new value.
    /// Arguments:
    ///     name  - A null terminated string of the name of the target variable
    ///     value - The value to set the variable to.
    /// Returns:
    ///     void
    /// Throws:
    ///     runtime_error if unable to set the variable value
    static inline void SetBoolVariable(const char* name, bool value)
    {
        SpotPluginApi::msg_get_set_variable_t setVarMsg;
        setVarMsg.DataType = SpotPluginApi::msg_get_set_variable_t::Bool;
        setVarMsg.VariableName = name;
        setVarMsg.BoolValue = value;
        if (!PluginHost::DoAction( SpotPluginApi::HostActionRequest::SetVariable, 0, &setVarMsg))
            throw std::runtime_error(std::string("Error setting Boolean macro variable named ") + name);
    }
    
    /// Summary:
    ///     Returns the current value of a global Boolean variable with a matching name.
    /// Arguments:
    ///     name - A null terminated string of the name of the target variable
    /// Returns:
    ///     A bool set to the current value of the global variable
    /// Throws:
    ///     runtime_error if unable to get the variable value
    static inline bool GetBoolVariable(const char* name)
    {
        SpotPluginApi::msg_get_set_variable_t getVarMsg;
        getVarMsg.DataType = SpotPluginApi::msg_get_set_variable_t::Bool;
        getVarMsg.VariableName = name;
        if (!PluginHost::DoAction( SpotPluginApi::HostActionRequest::GetVariable, 0, &getVarMsg))
            throw std::runtime_error(std::string("Error getting Boolean macro variable named ") + name);
        return getVarMsg.BoolValue != 0;
    }


    static inline void SaveVariable(const char* name, const char* fileName)
    {
        SpotPluginApi::msg_save_recall_variable_t saveMsg;
        saveMsg.FilePath = fileName;
        saveMsg.VariableName = name;
        if (!PluginHost::DoAction( SpotPluginApi::HostActionRequest::SaveVariable, 0, &saveMsg))
            throw std::runtime_error(std::string("Error saving variable (").append(name).append(") to the file ").append(fileName));
    }

    static inline void RestoreVariableFromFile(const char* name, const char* fileName)
    {
        SpotPluginApi::msg_save_recall_variable_t restoreMsg;
        restoreMsg.FilePath = fileName;
        restoreMsg.VariableName = name;
        if (!PluginHost::DoAction( SpotPluginApi::HostActionRequest::RecallVariable, 0, &restoreMsg))
            throw std::runtime_error(std::string("Error reading variable (").append(name).append(") from file ").append(fileName));
    }
    
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /// Summary:
    ///
    class IVariable
    {
    protected:
        std::string name;
        std::shared_ptr<std::string> objectId; // The name of the object that owns the variable
        VariableType type;
        ScopeFlags scope;
        bool readOnly;

        IVariable(std::string name, std::shared_ptr<std::string> objectId, VariableType type, ScopeFlags scope, bool readOnly) :
            name(name), objectId(objectId), type(type), scope(scope), readOnly(readOnly) {}
    public:
        virtual ~IVariable() {};
        const std::string& Name() const { return name; }
        const std::shared_ptr<std::string> ObjectId() const { return objectId; }
        VariableType Type() const { return type;}
        ScopeFlags Scope() const { return scope; }
        bool IsReadOnly() const { return readOnly; }
        bool IsGlobal() const { return nullptr == objectId; }
        virtual std::string ToString() { return std::string(name).append(", type:").append(std::to_string((int)type)).append(", {undefined value}"); }
    };
    
    template<typename T>
    class Variable : public IVariable
    {
    public:
        Variable(std::string name, std::shared_ptr<std::string> objectId, VariableType type, ScopeFlags scope, bool readOnly) :
            IVariable(name, objectId, type, scope, readOnly)
        {}
        virtual ~Variable() {}
        virtual T Value() const = 0;
        virtual Variable& Value(const T& value) = 0;
    };

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /// Summary:
    ///
    class BoolVariable : public Variable<bool>
    {
    public:
        BoolVariable(const char* name, ScopeFlags scope = ScopeFlags::Unknown, bool isReadOnly=false) :
            Variable<bool>(name, nullptr, VariableType::Text, scope, isReadOnly)
        {   }

        virtual bool Value() const { return GetBoolVariable(name.c_str()); }

        virtual Variable<bool>& Value(const bool& newValue)
        {
            if (IsReadOnly())
                throw std::runtime_error(std::string("Illegal operation. The variable (").append(name).append(") is a read only variable"));
            SetBoolVariable(name.c_str(), newValue);
            return *this;
        }

        virtual std::string ToString() { return Value() ? "true" : "false"; }
    };


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /// Summary:
    ///
    class TextVariable : public Variable<std::string>
    {
    public:
        TextVariable(const char* name, ScopeFlags scope = ScopeFlags::Unknown, bool isReadOnly=false) :
            Variable<std::string>(name, nullptr, VariableType::Text, scope, isReadOnly)
        {  }

        virtual std::string Value() const { return GetTextVariable(name.c_str()); }

        virtual Variable<std::string>& Value(const std::string& newValue)
        {
            if (IsReadOnly())
                throw std::runtime_error(std::string("Illegal operation. The variable (").append(name).append(") is a read only variable"));
            SetTextVariable(name.c_str(), newValue);
            return *this;
        }

        virtual std::string ToString() { return Value(); }
    };

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /// Summary:
    ///
    class NumericVariable : public Variable<double>
    {

    public:
        NumericVariable(const char* name, ScopeFlags scope = ScopeFlags::Unknown, bool isReadOnly = false) :
            Variable(name, nullptr, VariableType::Numeric, scope, isReadOnly)
        { }

        virtual double Value() const { return GetNumericVariable(name.c_str());}

        virtual Variable<double>& Value(int newValue)
        {
            if (IsReadOnly())
                throw std::runtime_error(std::string("Illegal operation. The variable (").append(name).append(") is a read only variable"));
            SetNumericVariable(name.c_str(), newValue);
            return *this;
        }

        virtual Variable<double>& Value(const double& newValue)
        {
            if (IsReadOnly())
                throw std::runtime_error(std::string("Illegal operation. The variable (").append(name).append(") is a read only variable"));
            SetNumericVariable(name.c_str(), newValue);
            return *this;
        }

        virtual Variable<double>& Value(const std::string& textToParse)
        {
            if (IsReadOnly())
                throw std::runtime_error(std::string("Illegal operation. The variable (").append(name).append(") is a read only variable"));
            SetNumericVariable(name.c_str(), std::stod(textToParse));
            return *this;
        }

        virtual std::string ToString()
        {
            return std::to_string(Value()); 
        }
    };

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /// Summary:
    ///
    class IntegerVariable : public Variable<int>
    {

    public:
        IntegerVariable(const char* name, ScopeFlags scope = ScopeFlags::Unknown, bool isReadOnly = false) :
            Variable(name, nullptr, VariableType::Integer, scope, isReadOnly)
        { }

        virtual int Value() const { return static_cast<int>(GetNumericVariable(name.c_str()));}

        virtual Variable<int>& Value(const int& newValue)
        {
            if (IsReadOnly())
                throw std::runtime_error(std::string("Illegal operation. The variable (").append(name).append(") is a read only variable"));
            double realVal = newValue;
            SetNumericVariable(name.c_str(), realVal);
            return *this;
        }

        virtual Variable<int>& Value(double newValue)
        {
            if (IsReadOnly())
                throw std::runtime_error(std::string("Illegal operation. The variable (").append(name).append(") is a read only variable"));
            newValue = round_to_nearest_awayzero(newValue);
            SetNumericVariable(name.c_str(), newValue);
            return *this;
        }

        virtual Variable<int>& Value(const std::string& textToParse, int base = 10)
        {
            if (IsReadOnly())
                throw std::runtime_error(std::string("Illegal operation. The variable (").append(name).append(") is a read only variable"));
            SetNumericVariable(name.c_str(), static_cast<double>(std::stoi(textToParse, nullptr, base)));
            return *this;
        }

        virtual std::string ToString()
        {
            return std::to_string(Value()); 
        }
    };

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "StandardHostVariables.h"

    class VariableManager
    {
        typedef std::unordered_map<std::string, std::shared_ptr<IVariable>> ivar_collection_t;
        ivar_collection_t variableCollection;

    public:
        VariableManager()
        {
        }

        ~VariableManager()
        {
        }

        size_t Size() const
        {
            return variableCollection.size();
        }

        void SaveAll(const std::string& fileName)
        {
            for(auto& item : variableCollection)
                SaveVariable(item.first.c_str(), fileName.c_str());
        }

        void RestoreAll(const std::string& fileName)
        {
            for(auto item : AllMutable())
            {
                RestoreVariableFromFile(item->Name().c_str(), fileName.c_str());           
            }
        }

        std::vector<IVariable*> MatchingAll(ScopeFlags withScope) const
        {
            std::vector<IVariable*> matching;
            for( auto& item : variableCollection)
            {
                if (item.second && item.second->Scope() == withScope)
                    matching.push_back(item.second.get());
            }
            return matching;
        }

        template<typename T>
        std::vector<T*> MatchingAll(ScopeFlags withScope) const
        {
            std::vector<T*> matching;
            for( auto& item : variableCollection)
            {
                if (item.second && item.second->Scope() == withScope && dynamic_cast<T*>(item.second.get()))
                    matching.push_back(static_cast<T*>(item.second.get()));
            }
            return matching;
        }

        std::vector<IVariable*> MatchingAny(ScopeFlags withScope) const
        {
            std::vector<IVariable*> matching;
            for( auto& item : variableCollection)
            {
                if (item.second && bool(item.second->Scope() & withScope))
                    matching.push_back(item.second.get());
            }
            return matching;
        }

        template<typename T>
        std::vector<T*> MatchingAny(ScopeFlags withScope) const 
        {
            std::vector<T*> matching;
            for( auto& item : variableCollection)
            {
                if (item.second && bool(item.second->Scope() & withScope) && dynamic_cast<T*>(item.second.get()))
                    matching.push_back(static_cast<T*>(item.second.get()));
            }
            return matching;
        }

        std::vector<IVariable*> AllMutable() const
        {
            std::vector<IVariable*> matching;
            for( auto& item : variableCollection)
            {
                if (item.second && !item.second->IsReadOnly())
                    matching.push_back(item.second.get());
            }
            return matching;
        }

        template<typename T>
        std::vector<T*> AllMutable() const 
        {
            std::vector<T*> matching;
            for( auto& item : variableCollection)
            {
                if (item.second && !item.second->IsReadOnly() && dynamic_cast<T*>(item.second.get()))
                    matching.push_back(static_cast<T*>(item.second.get()));
            }
            return matching;
        }

        std::vector<IVariable*> AllImmutable() const
        {
            std::vector<IVariable*> matching;
            for( auto& item : variableCollection)
            {
                if (item.second && item.second->IsReadOnly())
                    matching.push_back(item.second.get());
            }
            return matching;
        }

        template<typename T>
        std::vector<T*> AllImmutable() const 
        {
            std::vector<T*> matching;
            for( auto& item : variableCollection)
            {
                if (item.second && item.second->IsReadOnly() && dynamic_cast<T*>(item.second.get()))
                    matching.push_back(static_cast<T*>(item.second.get()));
            }
            return matching;
        }
        

        void Manage(IVariable* variable)
        {
            variableCollection[variable->Name()].reset(variable);
        }

        bool ContainsVariable(const std::string& name)
        {
            return variableCollection.find(name) != variableCollection.end();
        }

        template<typename T>
        bool ContainsVariable(const std::string& name)
        {
            auto item = variableCollection.find(name);
            return (item != variableCollection.end() && dynamic_cast<Variable<T*>>(item->second.get()) != nullptr);

        }
        
        IVariable& GetByName(const std::string& name) const
        {
            auto itemLocation = variableCollection.find(name);
            if (variableCollection.end() == itemLocation)
                throw std::invalid_argument(std::string("No variable with the name (").append(name).append(") exists"));
            return *(itemLocation->second.get());
        }

        template<typename T>
        T& GetByName(const std::string& name) const
        {
            auto var = dynamic_cast<T*>(&GetByName(name));
            if (nullptr == var)
                throw std::invalid_argument(std::string("No variable with the name (").append(name).append(") exists for type ").append(typeid(T).name()));
            return *var;
        }

        template<typename T>
        void SetValue(const std::string& name, const T& value)
        {
            GetByName<Variable<T>>(name).Value(value);
        }

        // Return a reference to a VariableManager that includes all the standard variables available by the host application.
        static VariableManager& StandardVars()
        {
            static VariableManager stdVars; // Singleton

            if (stdVars.Size() == 0)
            {
                for(auto &item : internal::std_vars_build_script)
                {
                    try
                    {
                        switch(item.type)
                        {
                        case VariableType::Bool:
                            stdVars.Manage(new BoolVariable(item.szName, item.scope, item.readonly));
                            break;
                        case VariableType::Integer:
                            stdVars.Manage(new IntegerVariable(item.szName, item.scope, item.readonly));
                            break;
                        case VariableType::Numeric:
                            stdVars.Manage(new NumericVariable(item.szName, item.scope, item.readonly));
                            break;
                        case VariableType::Text:
                            stdVars.Manage(new TextVariable(item.szName, item.scope, item.readonly));
                            break;
                        default:
                            assert(false); 
                            throw std::logic_error("application logic error in file " AT_FILE_LOCATION);
                        }
                    }
                    catch(std::runtime_error&)
                    { /* ignore load failures */ }
                }
            }
            return stdVars;
        }

    };


} // end namespace HostInterop
