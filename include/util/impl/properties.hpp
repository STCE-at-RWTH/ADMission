#ifndef ADM_INC_UTIL_IMPL_PROPERTIES_HPP_
#define ADM_INC_UTIL_IMPL_PROPERTIES_HPP_

// ################################ INCLUDES ################################ //

#include "admission_config.hpp"
#include "util/properties.hpp"  // IWYU pragma: keep

#include <fstream>
#include <list>
#include <sstream>
#include <stddef.h>
#include <stdexcept>
#include <string>

// ###################### TEMPLATE AND INLINE CONTENTS ###################### //

namespace admission
{

// ------------------------- KeyNotRegisteredError -------------------------- //

/******************************************************************************
 * @brief Constructor of KeyNotRegisteredError.
 *
 * @param[in] key Name of the key that is not registered.
 * @param[in] list List of known keys.
 ******************************************************************************/
ADM_ALWAYS_INLINE KeyNotRegisteredError::KeyNotRegisteredError(
    const std::string& key, const std::string& list)
    : std::runtime_error(
          "The key \"" + key + "\" is not registered here!" +
          ((list == "") ? "" : " Known keys are:\n") + list)
{}

// --------------------------- BadConfigFileError --------------------------- //

/******************************************************************************
 * @brief Constructor of BadConfigFileError.
 ******************************************************************************/
ADM_ALWAYS_INLINE BadConfigFileError::BadConfigFileError()
    : std::runtime_error("The specified config file is invalid or unreadable!")
{}

// --------------------- Properties - BasicPropertyInfo --------------------- //

/******************************************************************************
 * @brief Default constructor of BasicPropertyInfo.
 *
 * @param[in] k std::string Key to register the property under.
 * @param[in] d std::string Description of the property.
 * @param[in] onr void (*)(Properties*) Function to be executed on reading
 *                this property.
 ******************************************************************************/
ADM_ALWAYS_INLINE Properties::BasicPropertyInfo::BasicPropertyInfo(
    const std::string& k, const std::string& d, void (*onr)(Properties*))
    : _key(k), _desc(d), _on_read(onr)
{}

/******************************************************************************
 * @brief Wraps around pipe.
 *
 * @param[in] String to initialize the property from.
 ******************************************************************************/
ADM_ALWAYS_INLINE void Properties::BasicPropertyInfo::from_str(
    const std::string& s)
{
  std::stringstream sst;
  sst << s;
  pipe(sst);
}

/******************************************************************************
 * @brief Get the key of the property.
 *
 * @returns The key of the property.
 ******************************************************************************/
ADM_ALWAYS_INLINE const std::string& Properties::BasicPropertyInfo::key() const
{
  return _key;
}

/******************************************************************************
 * @brief Get the description of the property.
 *
 * @returns The description of the property.
 ******************************************************************************/
ADM_ALWAYS_INLINE const std::string& Properties::BasicPropertyInfo::desc() const
{
  return _desc;
}

// ----------------------- Properties - PropertyInfo ------------------------ //

/******************************************************************************
 * @brief Implements pipe function by deferencing stored pointer _ptr.
 *
 * @param[in] i std::ifstream& The stream to intialize the property from.
 ******************************************************************************/
template<typename T>
ADM_ALWAYS_INLINE void Properties::PropertyInfo<T>::pipe(std::istream& i)
{
  i >> *_ptr;
}

/******************************************************************************
 * @brief Constructor, delegating most of its work to that of
 *        BasicPropertyInfo, just initializing the pointer.
 *
 * @param[in] p pointer to T, the value of this property.
 * @param[in] k std::string Key to register the property under.
 * @param[in] d std::string Description of the property.
 * @param[in] onr void (*)(Properties*) Function to be executed on reading
 *                this property.
 ******************************************************************************/
template<typename T>
ADM_ALWAYS_INLINE Properties::PropertyInfo<T>::PropertyInfo(
    T* p, const std::string& k, const std::string& d, void (*onr)(Properties*))
    : BasicPropertyInfo(k, d, onr), _ptr(p)
{}

// ------------------------------- Properties ------------------------------- //

/******************************************************************************
 * @brief Register a property.
 *
 * @tparam T Type of the value.
 * @param[in] v Reference to the value.
 * @param[in] key key Key to register the property under.
 * @param[in] desc Description of the property to be printed when help
 *                 flag is encountered.
 * @param[in] on_read Pointer to function to be called after setting
 *                    the property value.
 ******************************************************************************/
template<typename T>
ADM_ALWAYS_INLINE void Properties::register_property(
    T& v, const std::string& key, const std::string& desc,
    void (*on_read)(Properties*))
{
  _info.emplace_back(new PropertyInfo<T>(&v, key, desc, on_read));
}

/******************************************************************************
 * @brief Destructor takes care of deleting dynamic memory pointed to
 *        by entries of _info.
 ******************************************************************************/
ADM_ALWAYS_INLINE Properties::~Properties()
{
  for (auto& i : _info)
  {
    delete i;
  }
}

/******************************************************************************
 * @brief Finds the propertiy which is registered under key and pipes value
 *        into it. Also executes the associated _on_read.
 *
 * @param[in] key The key into which to put the value.
 * @param[in] value The value to write\. Will be converted from string.
 ******************************************************************************/
ADM_ALWAYS_INLINE void Properties::put(
    const std::string& key, const std::string& value)
{
  for (auto& pi : _info)
  {
    if (key == pi->key())
    {
      pi->from_str(value);
      pi->_on_read(this);
      return;
    }
  }
  throw KeyNotRegisteredError(key);
}

/******************************************************************************
 * @brief Parses a config file from a std::ifstream.
 *
 * First checks if the file may be empty or invalid.
 *
 * @param[in] in std::ifstream from which to read the properties.
 ******************************************************************************/
ADM_ALWAYS_INLINE void Properties::parse_config(std::ifstream&& in)
{
  if (in.eof() || in.fail() || !in.good())
  {
    throw BadConfigFileError();
  }

  std::string key, value;
  in >> key;
  while (!in.eof())
  {
    in >> value;
    put(key, value);
    in >> key;
  }
}

/******************************************************************************
 * @brief Prints the keys and descriptions of all registeres properties in a
 *        structured way.
 *
 * @param[in] o Reference to std::ofstream to print the description
 *              of the properties.
 ******************************************************************************/
ADM_ALWAYS_INLINE void Properties::print(std::ostream& o)
{
  size_t l = 0;
  for (auto& pi : _info)
  {
    l = (pi->key().length() > l) ? pi->key().length() : l;
  }

  for (auto& pi : _info)
  {
    o.width(l);
    o << pi->key();
    o << ": " << pi->desc() << std::endl;
  }
}

}  // end namespace admission

// ################################## EOF ################################### //

#endif  // ADM_INC_UTIL_IMPL_PROPERTIES_HPP_
