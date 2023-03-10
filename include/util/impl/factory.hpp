#ifndef ADM_INC_UTIL_IMPL_FACTORY_HPP_
#define ADM_INC_UTIL_IMPL_FACTORY_HPP_

// ################################ INCLUDES ################################ //

#include "util/factory.hpp"  // IWYU pragma: keep
#include "util/properties.hpp"

#include <typeinfo>
#include <utility>

// ###################### TEMPLATE AND INLINE CONTENTS ###################### //

namespace admission
{

// -------------------------------- Creator --------------------------------- //

/******************************************************************************
 * @brief Default constructor of Creator.
 *
 * Initializes the name with the typename.
 ******************************************************************************/
template<typename T>
Creator<T>::Creator() : _name(typeid(T).name())
{}

/******************************************************************************
 * @brief Constructs a dynamic instance of T and returns it as a pointer to
 *        its base class T::AbstractBase.
 *
 * @returns Pointer to an instance of the base class T::AbstractBase.
 ******************************************************************************/
template<typename T>
typename T::AbstractBase* Creator<T>::construct() const
{
  return new T;
}

/******************************************************************************
 * @brief Returns the name of the class to be created.
 *
 * @returns Name of the class to be created.
 ******************************************************************************/
template<typename T>
const std::string& Creator<T>::name() const
{
  return _name;
}

// -------------------------------- Factory --------------------------------- //

/******************************************************************************
 * @brief Returns pointer to the single instance of the factory.
 *
 * @returns Reference to the instance of the factory.
 ******************************************************************************/
template<typename T>
Factory<T>& Factory<T>::instance()
{
  static Factory<T> _instance;
  return _instance;
}

/******************************************************************************
 * @brief Registers a type to the factory by an identifying string.
 *
 * @param[in] tag Name of the type (doesn't need to be the actual typename).
 * @param[in] c Pointer to the type creator.
 ******************************************************************************/
template<typename T>
void Factory<T>::register_type(const std::string& tag, AbstractCreator<T>* c)
{
  _creator[tag] = c;
  _tag_lookup[c->name()] = tag;
}

/******************************************************************************
 * @brief Removes a type from the list of available types by its name.
 *
 * @param[in] tag Name of the type (doesn't need to be the actual typename).
 ******************************************************************************/
template<typename T>
void Factory<T>::unregister_type(const std::string& tag)
{
  auto tagPosition = _creator.find(tag);

  if (tagPosition != _creator.end())
  {
    AbstractCreator<T>* c = tagPosition->second;

    // If we found it, remove the creator from the tag lookup map
    auto classPosition = _tag_lookup.find(c->name());
    if (classPosition != _tag_lookup.end())
    {
      _tag_lookup.erase(classPosition);
    }
    _creator.erase(tagPosition);
  }
}

/******************************************************************************
 * @brief Returns a pointer to an object of the type identified by the name.
 *
 * @param[in] tag Name of the type (doesn't need to be the actual typename).
 ******************************************************************************/
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

/******************************************************************************
 * @brief Checks if a type is registered to this factory under the given name.
 *
 * @param[in] tag Name of the type (doesn't need to be the actual typename).
 ******************************************************************************/
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

/******************************************************************************
 * @brief Returns the name identifying the type of the object pointed to by e.
 *
 * Returns an empty string if the type is not registered.
 *
 * @returns Name of the type (doesn't need to be the actual typename).
 ******************************************************************************/
template<typename T>
const std::string& Factory<T>::tag(const T* e) const
{
  auto foundIt = _tag_lookup.find(typeid(*e).name());
  if (foundIt != _tag_lookup.end())
  {
    return foundIt->second;
  }

  static std::string emptyStr("");
  return emptyStr;
}

/******************************************************************************
 * @brief Returns a vector of strings with all types known to this factory.
 *
 * @param[in] tag Name of the type (doesn't need to be the actual typename).
 * @returns Vector of all registered type names.
 ******************************************************************************/
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

/******************************************************************************
 * @brief Prints all types known to this factory to an output stream.
 *
 * @param[in] os Reference to an output stream.
 * @returns The output stream.
 ******************************************************************************/
template<typename T>
std::ostream& Factory<T>::print_types(std::ostream& os) const
{
  for (auto it = _creator.begin(); it != _creator.end(); ++it)
  {
    os << (it->first) << std::endl;
  }
  return os;
}

/******************************************************************************
 * @brief Private destructor of the factory.
 ******************************************************************************/
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

// --------------------------- RegisterTypeProxy ---------------------------- //

/******************************************************************************
 * @brief Constructor taking the name.
 *
 * @param[in] name std::string The name to register the class template
 *                 parameter type T under.
 ******************************************************************************/
template<typename T>
RegisterTypeProxy<T>::RegisterTypeProxy(const std::string& name) : _name(name)
{
  _creator = new Creator<T>();
  Factory<typename T::AbstractBase>::instance().register_type(_name, _creator);
}

/******************************************************************************
 * @brief Destructor that unregisteres type T from the Factory.
 ******************************************************************************/
template<typename T>
RegisterTypeProxy<T>::~RegisterTypeProxy()
{
  Factory<typename T::AbstractBase>::instance().unregister_type(_name);
  delete static_cast<Creator<T>*>(_creator);
}

}  // end namespace admission

// ################################## EOF ################################### //

#endif  // ADM_INC_UTIL_IMPL_FACTORY_HPP_
