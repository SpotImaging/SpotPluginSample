#pragma once
#include <string>
#include <stdexcept>
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
        if (!PluginHost::ActionFunc(PluginHost::pluginHandle, SpotPluginApi::HostActionRequest::GetVariable, 0, &getVarMsg))
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
        if (!PluginHost::ActionFunc(PluginHost::pluginHandle, SpotPluginApi::HostActionRequest::SetVariable, 0, &setVarMsg))
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
        if (!PluginHost::ActionFunc(PluginHost::pluginHandle, SpotPluginApi::HostActionRequest::SetVariable, 0, &setVarMsg))
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
        if (!PluginHost::ActionFunc(PluginHost::pluginHandle, SpotPluginApi::HostActionRequest::GetVariable, 0, &getVarMsg))
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
        if (!PluginHost::ActionFunc(PluginHost::pluginHandle, SpotPluginApi::HostActionRequest::SetVariable, 0, &setVarMsg))
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
        if (!PluginHost::ActionFunc(PluginHost::pluginHandle, SpotPluginApi::HostActionRequest::GetVariable, 0, &getVarMsg))
            throw std::runtime_error(std::string("Error getting Boolean macro variable named ") + name);
        return getVarMsg.BoolValue != 0;
    }


    static inline void SaveVariable(const char* name, const char* fileName)
    {
        SpotPluginApi::msg_save_recall_variable_t saveMsg;
        saveMsg.FilePath = fileName;
        saveMsg.VariableName = name;
        if (!PluginHost::ActionFunc(PluginHost::pluginHandle, SpotPluginApi::HostActionRequest::SaveVariable, 0, &saveMsg))
            throw std::runtime_error(std::string("Error saving variable (").append(name).append(") to the file ").append(fileName));
    }

    static inline void RestoreVariableFromFile(const char* name, const char* fileName)
    {
        SpotPluginApi::msg_save_recall_variable_t restoreMsg;
        restoreMsg.FilePath = fileName;
        restoreMsg.VariableName = name;
        if (!PluginHost::ActionFunc(PluginHost::pluginHandle, SpotPluginApi::HostActionRequest::RecallVariable, 0, &restoreMsg))
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
        const std::string& ObjectId() const
        {
            if (!objectId)
                throw std::logic_error("null reference");
            return *objectId;
        }
        VariableType Type() const { return type;}
        ScopeFlags Scope() const { return scope; }
        bool IsReadOnly() const { return readOnly; }
        bool IsGlobal() const { return nullptr == objectId; }
        virtual void Update() = 0;
        virtual std::string ToString() { return std::string(name).append(", type:").append(std::to_string((int)type)).append(", {undefined value}"); }
    };
    
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /// Summary:
    ///
    class BoolVariable : public IVariable
    {
    protected:
        bool value;

    public:
        BoolVariable(const char* name, ScopeFlags scope = ScopeFlags::Unknown, bool isReadOnly=false) :
            IVariable(name, nullptr, VariableType::Text, scope, isReadOnly)
        {
            Update();
        }

        virtual void Update()
        {
            value = GetBoolVariable(Name().c_str());
        }
        
        virtual bool Value() const { return value;}

        virtual void Value(bool newValue)
        {
            if (IsReadOnly())
                throw std::runtime_error(std::string("Illegal operation. The variable (").append(name).append(") is a read only variable"));
            SetBoolVariable(Name().c_str(), newValue);
            value = newValue;
        }

        virtual std::string ToString() { return value ? "true" : "false"; }
    };


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /// Summary:
    ///
    class TextVariable : public IVariable
    {
    protected:
        std::string value;

    public:
        TextVariable(const char* name, ScopeFlags scope = ScopeFlags::Unknown, bool isReadOnly=false) :
            IVariable(name, nullptr, VariableType::Text, scope, isReadOnly)
        {
            Update();
        }

        virtual void Update()
        {
            value = GetTextVariable(Name().c_str());
        }
        
        virtual const std::string& Value() const { return value;}

        virtual void Value(const std::string& newValue)
        {
            if (IsReadOnly())
                throw std::runtime_error(std::string("Illegal operation. The variable (").append(name).append(") is a read only variable"));
            SetTextVariable(Name().c_str(), newValue);
            value = newValue;
        }

        virtual std::string ToString() { return value; }
    };

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /// Summary:
    ///
    class NumericVariable : public IVariable
    {
    protected:
        double value;

    public:
        NumericVariable(const char* name, ScopeFlags scope = ScopeFlags::Unknown, bool isReadOnly = false, bool isInteger = false) :
            IVariable(name, nullptr, isInteger? VariableType::Integer : VariableType::Numeric, scope, isReadOnly)
        {
            Update();
        }

        virtual void Update()
        {
            value = GetNumericVariable(Name().c_str());
        }
        
        virtual double Value() const { return value;}

        virtual void Value(int newValue)
        {
            using std::runtime_error;

            if (IsReadOnly())
                throw runtime_error(std::string("Illegal operation. The variable (").append(name).append(") is a read only variable"));
            double realVal = newValue;
            SetNumericVariable(Name().c_str(), realVal);
            value = realVal;
        }

        virtual void Value(double newValue)
        {
            using std::runtime_error;

            if (IsReadOnly())
                throw runtime_error(std::string("Illegal operation. The variable (").append(name).append(") is a read only variable"));
            if(type == VariableType::Integer)
                newValue = round_to_nearest_awayzero(newValue);
            SetNumericVariable(Name().c_str(), newValue);
            value = newValue;
        }

        virtual void Value(const std::string& textToParse, int base = 10)
        {
            using std::runtime_error;

            if (IsReadOnly())
                throw runtime_error(std::string("Illegal operation. The variable (").append(name).append(") is a read only variable"));
            double newValue = VariableType::Integer == type ? static_cast<double>(std::stoi(textToParse, nullptr, base)) : std::stod(textToParse);
            SetNumericVariable(Name().c_str(), newValue);
            value = newValue;
        }

        virtual std::string ToString()
        {
            return type == VariableType::Integer ? std::to_string(static_cast<int>(value)) : std::to_string(value); 
        }
    };

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "StandardHostVariables.h"

    class VariableManager
    {
        typedef std::map<std::string, std::unique_ptr<IVariable>> ivar_collection_t;
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
                item->Update();
            }
        }

        void UpdateAll()
        {
            for (auto& item : variableCollection)
                item.second->Update();
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

        IVariable* GetByName(const std::string& name) const
        {
            auto itemLocation = variableCollection.find(name);
            if (variableCollection.end() == itemLocation)
                return nullptr;
            return itemLocation->second.get();
        }

        template<typename T>
        T* GetByName(const std::string& name) const
        {
            return dynamic_cast<T*>(GetByName(name));
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
                        case VariableType::Numeric:
                            stdVars.Manage(new NumericVariable(item.szName, item.scope, item.readonly, item.type == VariableType::Integer));
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
