#ifndef FACTORY_HPP
#define FACTORY_HPP

// ******************************** Includes ******************************** //

#include "properties.hpp"

#include <map>
#include <sstream>
#include <string>
#include <typeinfo>
#include <utility>
#include <vector>

// **************************** Header contents ***************************** //

namespace admission
{

/**\brief A creator class for instantiating objects
 *        of types that derive from the BaseT.
 *
 * @tparam BaseT Base type of the objects that can be instantiated.
 */
template<typename BaseT>
class AbstractCreator
{
 public:
  /**\brief Construct an instance. Must be implemented in derived class.
   * @returns BaseT* pointing to an instance of a type derived from BaseT.
   */
  virtual BaseT* construct() = 0;

  /**\brief Name of the class to be created. Has to implemented in derived class.
   * @returns std::string containing the name.
   */
  virtual const std::string& name() const = 0;

  virtual ~AbstractCreator() {}
};

/**\brief Can create an instance of class T, which is
 *        dientified by a string stored in this creator.
 *
 * @tparam T Class, objects of which can be created. T must define
 * a type T::AbstractBase.
 */
template<typename T>
class Creator : public AbstractCreator<typename T::AbstractBase>
{
 public:
  Creator() : _name(typeid(T).name()) {}

  typename T::AbstractBase* construct()
  {
    return new T;
  }

  virtual const std::string& name() const
  {
    return _name;
  }

 protected:
  std::string _name;
};

/**\brief Factory class to instantiate objects of types derived from
 * a common base class.
 *
 * The available types are identified by strings.
 * New types can be registered to this factory with the macro
 * ADM_REGISTER_TYPE(name string, typename)
 * @tparam T Common base class of the objects the factory can create.
 */
template<typename T>
class Factory
{
 public:
  /// Returns pointer to the single instance of the factory.
  static Factory* instance();

  /// Deletes the single instance of the factory.
  static void destroy();

  /// Registers a type to the factory by an identifying string.
  void register_type(const std::string& tag, AbstractCreator<T>* c);

  /// Removes a type from the list of available types by its ident. string.
  void unregister_type(const std::string& tag);

  /// Returns a pointer to an object of the type identified by the string.
  T* construct(const std::string& tag) const;

  /// Checks if a type is registered to this factory under the given string.
  bool knows_tag(const std::string& tag) const;

  /// Returns the string identifying the type of the object pointed to by e.
  const std::string& tag(const T* e) const;

  /// Returns a vector of strings with all types known to this factory.
  std::vector<std::string> known_types() const;

  /// Prints all types known to this factory to a std::ostream.
  std::ostream& print_types(std::ostream& os) const;

 protected:
  typedef std::map<std::string, AbstractCreator<T>*> CreatorMap;
  typedef std::map<std::string, std::string> TagLookup;

  Factory();
  ~Factory();

  /// Maps the strings identifying the registered types to instances of their corresponding creator classes.
  CreatorMap _creator;
  /// Maps the typeid of the registered classes back to the identifying strings.
  TagLookup _tag_lookup;

 private:
  /// Pointer to the single instance of this class.
  static Factory* _instance;
};

/**\brief Proxy that registers a type when instantiated.
 */
template<typename T>
class RegisterTypeProxy
{
 public:
  RegisterTypeProxy(const std::string& name) : _name(name)
  {
    _creator = new Creator<T>();
    Factory<typename T::AbstractBase>::instance()->register_type(
        _name, _creator);
  }

  ~RegisterTypeProxy()
  {
    Factory<typename T::AbstractBase>::instance()->unregister_type(_name);
    delete _creator;
  }

 private:
  std::string _name;
  AbstractCreator<typename T::AbstractBase>* _creator;
};

// This macro is used to automate registering types and forcing linkage
#define ADM_REGISTER_TYPE(name, classname)                                     \
  static admission::RegisterTypeProxy<classname>                               \
      g_type_proxy_vector##classname(#name);

template<typename T>
Factory<T>::Factory()
{}

template<typename T>
Factory<T>::~Factory()
{
  for (auto& i : _creator)
  {
    delete i.second;
  }

  _creator.clear();
  _tag_lookup.clear();
}

template<typename T>
Factory<T>* Factory<T>::instance()
{
  if (_instance == nullptr)
  {
    _instance = new Factory;
  }

  return _instance;
}

template<typename T>
T* Factory<T>::construct(const std::string& tag) const
{
  auto found_it = _creator.find(tag);
  if (found_it != _creator.end())
  {
    return found_it->second->construct();
  }
  else
  {
    throw admission::KeyNotRegisteredError(
        tag,
        [this]()
        {
          std::stringstream s;
          print_types(s);
          return s.str();
        }());
  }
  return nullptr;
}

template<typename T>
void Factory<T>::register_type(const std::string& tag, AbstractCreator<T>* c)
{
  _creator[tag] = c;
  _tag_lookup[c->name()] = tag;
}

template<typename T>
void Factory<T>::unregister_type(const std::string& tag)
{
  auto tagPosition = _creator.find(tag);

  if (tagPosition != _creator.end())
  {

    AbstractCreator<T>* c = tagPosition->second;

    // If we found it, remove the creator from the tag lookup map
    TagLookup::iterator classPosition = _tag_lookup.find(c->name());
    if (classPosition != _tag_lookup.end())
    {
      _tag_lookup.erase(classPosition);
    }
    _creator.erase(tagPosition);
  }
}

template<typename T>
const std::string& Factory<T>::tag(const T* e) const
{
  static std::string emptyStr("");
  auto foundIt = _tag_lookup.find(typeid(*e).name());
  if (foundIt != _tag_lookup.end())
  {
    return foundIt->second;
  }

  return emptyStr;
}

template<typename T>
std::vector<std::string> Factory<T>::known_types() const
{
  std::vector<std::string> types;

  for (auto it = _creator.begin(); it != _creator.end(); ++it)
  {
    types.push_back(it->first);
  }

  return types;
}

template<typename T>
std::ostream& Factory<T>::print_types(std::ostream& os) const
{
  for (auto it = _creator.begin(); it != _creator.end(); ++it)
  {
    os << (it->first) << std::endl;
  }
  return os;
}

template<typename T>
bool Factory<T>::knows_tag(const std::string& tag) const
{
  auto foundIt = _creator.find(tag);
  if (foundIt == _creator.end())
  {
    return false;
  }
  return true;
}

template<typename T>
void Factory<T>::destroy()
{
  delete _instance;
  _instance = nullptr;
}

// Set up the instances of factories
template<typename T>
Factory<T>* Factory<T>::_instance = nullptr;

}  // end namespace admission

#endif  // FACTORY_HPP
