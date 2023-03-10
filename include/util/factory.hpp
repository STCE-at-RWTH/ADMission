#ifndef ADM_INC_UTIL_FACTORY_HPP_
#define ADM_INC_UTIL_FACTORY_HPP_

// ################################ INCLUDES ################################ //

#include <map>
#include <sstream>
#include <string>
#include <vector>

// ############################# HEADER CONTENTS ############################ //

namespace admission
{

/******************************************************************************
 * @brief A creator class for instantiating objects
 *        of types that derive from the BaseT.
 *
 * @tparam BaseT Base type of the objects that can be instantiated.
 ******************************************************************************/
template<typename BaseT>
class AbstractCreator
{
 public:
  //! Default desctructor of AbstractCreator.
  virtual ~AbstractCreator() = default;

  //! Construct an instance. Must be implemented in derived class.
  virtual BaseT* construct() const = 0;

  //! Name of the class to be created. Must be implemented in derived class.
  virtual const std::string& name() const = 0;
};

/******************************************************************************
 * @brief Can create an instance of class T, which is
 *        identified by a string stored in this creator.
 *
 * @tparam T Class, objects of which can be created. T must define
 *           a type T::AbstractBase.
 ******************************************************************************/
template<typename T>
class Creator : public AbstractCreator<typename T::AbstractBase>
{
 public:
  //! Default constructor of Creator.
  Creator();

  //! Default desctructor of Creator.
  virtual ~Creator() override = default;

  //! Constructs a dynamic instance of T and returns it as a pointer to
  //! its base class T::AbstractBase.
  virtual typename T::AbstractBase* construct() const override final;

  //! Returns the typename of T.
  virtual const std::string& name() const override final;

 protected:
  std::string _name;
};

/******************************************************************************
 * @brief Factory class to instantiate objects of types derived from
 *        a common base class.
 *
 * The available types are identified by strings.
 * New types can be registered to this factory with the macro
 * ADM_REGISTER_TYPE(name string, typename).
 *
 * @tparam T Common base class of the objects the factory can create.
 ******************************************************************************/
template<typename T>
class Factory
{
 public:
  //! Returns pointer to the single instance of the factory.
  static Factory<T>& instance();

  //! Registers a type to the factory by an identifying string.
  void register_type(const std::string& tag, AbstractCreator<T>* c);

  //! Removes a type from the list of available types by its ident. string.
  void unregister_type(const std::string& tag);

  //! Returns a pointer to an object of the type identified by the string.
  T* construct(const std::string& tag) const;

  //! Checks if a type is registered to this factory under the given string.
  bool knows_tag(const std::string& tag) const;

  //! Returns the string identifying the type of the object pointed to by e.
  const std::string& tag(const T* e) const;

  //! Returns a vector of strings with all types known to this factory.
  std::vector<std::string> known_types() const;

  //! Prints all types known to this factory to a std::ostream.
  std::ostream& print_types(std::ostream& os) const;

 protected:
  //! Maps the strings identifying the registered types to instances of their
  //! corresponding creator classes.
  std::map<std::string, AbstractCreator<T>*> _creator;

  //! Enables to find the name (aka. tag) under which a class is registered by
  //! its typename.
  std::map<std::string, std::string> _tag_lookup;

 private:
  //! Default private constructor of the factory. Not callable by the user.
  Factory() = default;

  //! Private destructor of the factory.
  ~Factory();

  // Delete all copy & move constructors and assignment operators
  Factory(const Factory<T>& other) = delete;
  Factory(Factory<T>&& other) = delete;
  Factory<T>& operator=(const Factory<T>& other) = delete;
  Factory<T>& operator=(Factory<T>&& other) = delete;
};

/******************************************************************************
 * @brief Proxy that registers a type when instantiated.
 ******************************************************************************/
template<typename T>
class RegisterTypeProxy
{
 public:
  //! Constructor taking the name.
  RegisterTypeProxy(const std::string& name);

  //! Destructor that unregisteres type T from the Factory.
  ~RegisterTypeProxy();

  // Delete all copy & move constructors and assignment operators
  RegisterTypeProxy(const RegisterTypeProxy<T>& other) = delete;
  RegisterTypeProxy(RegisterTypeProxy<T>&& other) = delete;
  RegisterTypeProxy<T>& operator=(const RegisterTypeProxy<T>& other) = delete;
  RegisterTypeProxy<T>& operator=(RegisterTypeProxy<T>&& other) = delete;

 private:
  //! Name under which T is registered.
  std::string _name;

  //! Pointer to the creator.
  AbstractCreator<typename T::AbstractBase>* _creator;
};

}  // end namespace admission

/******************************************************************************
 * @brief This macro is used to automate registering types and forcing linkage.
 *
 * @param[in] name Name under which the type is registered.
 * @param[in] classname Actual classname of the type.
 ******************************************************************************/
#define ADM_REGISTER_TYPE(name, classname)                                     \
  static admission::RegisterTypeProxy<classname> adm_type_proxy_##classname(   \
      #name);

// ################ INCLUDE TEMPLATE AND INLINE DEFINITIONS ################# //

#include "util/impl/factory.hpp"  // IWYU pragma: export

// ################################## EOF ################################### //

#endif  // ADM_INC_UTIL_FACTORY_HPP_
