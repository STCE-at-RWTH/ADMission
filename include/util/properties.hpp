#ifndef ADM_INC_UTIL_PROPERTIES_HPP_
#define ADM_INC_UTIL_PROPERTIES_HPP_

// ################################ INCLUDES ################################ //

#include "misc/doxygen.hpp"

#include <fstream>
#include <list>
#include <stdexcept>
#include <string>

// ############################# HEADER CONTENTS ############################ //

namespace admission
{

DOXYGEN_MODULE_BEGIN(Exceptions)

/******************************************************************************
 * @brief Thrown if a key that is not registered to the properties is
 *        encountered while parsing.
 ******************************************************************************/
class KeyNotRegisteredError : public std::runtime_error
{
 public:
  //! Constructor of KeyNotRegisteredError.
  KeyNotRegisteredError(const std::string& key, const std::string& list = "");
};

/******************************************************************************
 * @brief Thrown if the specified configfile is unreadable or not a file.
 ******************************************************************************/
class BadConfigFileError : public std::runtime_error
{
 public:
  //! Constructor of BadConfigFileError.
  BadConfigFileError();
};

DOXYGEN_MODULE_END(Exceptions)

/******************************************************************************
 * @brief Allows to parse Arguments to Properties from the command line and
 *        config files.
 *
 * Uses a list of pointers to BasicPropertyInfo to be able to associate a
 * type-safe class member of a class derived from Properties with a run-time
 * string. The Properties class has basic -help and -h capabilities, ie. ie will
 * print all possible keys of -h or -help are encountered. Also has the ability
 * to switch from command line parameter reading to config file reading with
 * -from-file=path. Functions can be registered to be executed with each put()
 * in a PropertyInfo.
 *
 * @todo Where is CC?
 * @tparam CC The derived class; Each derived class must first be declared and
 * then defined to inherit from Properties to enable correct dispatch to
 * pointer-to-member-of-derived-class in register_property() functions.
 ******************************************************************************/
class Properties
{
 public:
  /****************************************************************************
   * @brief Stores the key and a description of a property but no value.
   ****************************************************************************/
  class BasicPropertyInfo
  {
   public:
    //! Default constructor of BasicPropertyInfo.
    BasicPropertyInfo(
        const std::string& k, const std::string& d, void (*onr)(Properties*));

    //! Default destructor of BasicPropertyInfo.
    virtual ~BasicPropertyInfo() = default;

    //! Will pipe from a stream into the member variable pointed to by
    //! PropertyInfo and invoke _on_read(c).
    virtual void pipe(std::istream&) = 0;

    //! Wraps around pipe.
    void from_str(const std::string& s);

    //! Get the key of the property.
    virtual const std::string& key() const;

    //! Get the description of the property.
    virtual const std::string& desc() const;

   private:
    //! Identifiers of this property as strings.
    std::string _key, _desc;
    //! Function pointer to execute something after reading the value.
    void (*_on_read)(Properties*);
    //! Let Properties access private stuff.
    friend Properties;
  };

  /******************************************************************************
   * @brief Stores a property and its value.
   *
   * @tparam T Type of the stored value.
   ******************************************************************************/
  template<typename T>
  class PropertyInfo : public BasicPropertyInfo
  {
   public:
    //! The type of the value stored in here.
    using type = T;

    //! Implements pipe function by deferencing stored pointer _ptr.
    virtual void pipe(std::istream& i) override final;

    //! Constructor, delegating most of its work to that of
    //! BasicPropertyInfo, just initializing the pointer.
    PropertyInfo(
        T* p, const std::string& k, const std::string& d,
        void (*onr)(Properties*));

   private:
    T* _ptr;
  };

  //! Register a property.
  template<typename T>
  void register_property(
      T& v, const std::string& key, const std::string& desc,
      void (*on_read)(Properties*) = [](Properties*) {});

  //! Default constructor for Properties, used for default
  //! properties, such as help flag and swich-to-config-file flag.
  Properties() = default;

  //! The copy constructor is disabled.
  Properties(const Properties&) = delete;

  //! Destructor takes care of deleting dynamic memory pointed to
  //! by entries of _info.
  ~Properties();

  //! Finds the propertiy which is registered under key and pipes value
  //! into it. Also executes the associated _on_read.
  void put(const std::string& key, const std::string& value);

  //! Parses a config file from a std::ifstream.
  void parse_config(std::ifstream&& in);

  //! Prints the keys and descriptions of all registeres properties in a
  //! structured way.
  void print(std::ostream& o);

 private:
  //! Contains pointers to all registered properties.
  std::list<BasicPropertyInfo*> _info;

  //! Used to store the supplied nothing that comes with -help and -h into
  //! the waste. Therefore private.
  bool _waste;
};

}  // end namespace admission

// ################ INCLUDE TEMPLATE AND INLINE DEFINITIONS ################# //

#include "util/impl/properties.hpp"  // IWYU pragma: export

// ################################## EOF ################################### //

#endif  // ADM_INC_UTIL_PROPERTIES_HPP_
