#ifndef PARAMS_HPP
#define PARAMS_HPP

// ******************************** Includes ******************************** //

#include <fstream>
#include <list>
#include <sstream>
#include <stddef.h>
#include <stdexcept>
#include <string>

// **************************** Header contents ***************************** //

namespace admission
{

/**
 * \defgroup Exceptions Exceptions 
 * \addtogroup Exceptions
 * @{
 */

/**\brief Thrown if a key that is not registered to the properties is
 * encountered while parsing.
 */
class KeyNotRegisteredError : public std::runtime_error
{
 public:
  KeyNotRegisteredError(std::string key, std::string list = "")
      : std::runtime_error(
            "The key \"" + key + "\" is not registered here!" +
            ((list == "") ? "" : " Known keys are:\n") + list)
  {}
};

/**\brief Thrown if the specified configfile is unreadable or not a file.
 */
class BadConfigFileError : public std::runtime_error
{
 public:
  BadConfigFileError()
      : std::runtime_error(
            "The specified config file is invalid or unreadable!")
  {}
};

/**
 * @}
 */

/** \brief Allows to parse Arguments to Properties from the command line and config files.
 *
 *  Uses a list of pointers to BasicPropertyInfo to be able to associate a type-safe class member of a
 *  class derived from Properties with a run-time string.
 *  The Properties class has basic -help and -h capabilities, ie. ie will print all possible keys of -h or -help are
 *  encountered. Also has the ability to switch from command line parameter reading to config file reading with
 *  -from-file=path.
 *  Functions can be registered to be executed with each put() in a PropertyInfo.
 *  @tparam CC The derived class; Each derived class must first be declared and then defined to inherit from Properties
 *  to enable correct dispatch to pointer-to-member-of-derived-class in register_property() functions.
 */
class Properties
{
 public:
  /**\brief Stores the key and a description of a property but no value.
   */
  class BasicPropertyInfo
  {
   public:
    virtual ~BasicPropertyInfo() {}

    /**\brief Will pipe from a stream into the member variable pointed to by PropertyInfo and invoke _on_read(c).
     */
    virtual void pipe(std::istream&) = 0;

    /**\brief Wraps around pipe.
     */
    void from_str(const std::string& s)
    {
      std::stringstream sst;
      sst << s;
      pipe(sst);
    }

    virtual const std::string& key()
    {
      return _key;
    }

    virtual const std::string& desc()
    {
      return _desc;
    }

    BasicPropertyInfo(std::string k, std::string d, void (*onr)(Properties*))
        : _key(k), _desc(d), _on_read(onr)
    {}

   private:
    /// Identifiers of this property as strings.
    std::string _key, _desc;
    /// Function pointer to execute something after reading the value.
    void (*_on_read)(Properties*);
    /// Let Properties access private stuff.
    friend Properties;
  };

  /**\brief Stores a property and its value.
   */
  template<typename T>
  class PropertyInfo : public BasicPropertyInfo
  {
   public:
    using type = T;

    /** Implements pipe function by deferencing stored pointer _ptr;
     */
    virtual void pipe(std::istream& i) override final
    {
      i >> *_ptr;
    }

    PropertyInfo(T* p, std::string k, std::string d, void (*onr)(Properties*))
        : BasicPropertyInfo(k, d, onr), _ptr(p)
    {}

   private:
    T* _ptr;
  };

  /**\brief Register a property.
   *
   * @tparam T Type of the value.
   * @param v Reference to the value.
   * @param key key Key to register the property under.
   * @param desc Description of the property to be printed when help flag is encountered.
   * @param on_read Pointer to function to be called after setting the property value.
   */
  template<typename T>
  void register_property(
      T& v, std::string key, std::string desc,
      void (*on_read)(Properties*) = [](Properties*) {})
  {
    _info.emplace_back(new PropertyInfo<T>(&v, key, desc, on_read));
  }

  ///\name Constructors and Destructor.
  ///@{

  /**\brief Default constructor for Properties, registeres defualt
   *        properties, such as help flag and swich-to-config-file flag.
   */
  Properties() {}

  Properties(const Properties&) = delete;

  ~Properties()
  {
    for (auto& i : _info)
    {
      delete i;
    }
  }

  ///@}

  /**\brief Finds the propertiy which is registered under key and pipes value into it\.
   *        Also executes the associated _on_read.
   *
   * @param key   The key into which to put the value.
   * @param value The value to write\. Will be converted from string.
   */
  void put(const std::string& key, const std::string& value)
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

  /**\brief Pareses a config file from a std::ifstream.
   *        First, checks if the file may be empty or invalid.
   */
  void parse_config(std::ifstream&& in)
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

  /**\brief Prints the keys and descriptions of all registeres properties in a structured way.
   */
  void print(std::ostream& o)
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

 private:
  /// Contains pointers to all registered properties.
  std::list<BasicPropertyInfo*> _info;

  /// Used to store the supplied nothing that comes with -help and -h into
  /// the waste. Therefore private.
  bool _waste;
};

}  // end namespace admission

#endif  // PARAMS_HPP
