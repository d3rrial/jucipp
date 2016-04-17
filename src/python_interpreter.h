#ifndef JUCI_PYTHON_INTERPRETER_H_
#define JUCI_PYTHON_INTERPRETER_H_

#include <pybind11/pybind11.h>
#include <boost/filesystem/path.hpp>

#include <iostream>
using namespace std;

class Python {
  public:
  class Interpreter {
  private:
    Interpreter();
    ~Interpreter();
    wchar_t *argv;
    void add_path(const boost::filesystem::path &path);
  public:
    static Interpreter& get(){
      static Interpreter singleton;
      return singleton;
    }
  };

  pybind11::module static get_loaded_module(const std::string &module_name);
  pybind11::module static import(const std::string &module_name);
  pybind11::module static reload(pybind11::module &module);

  class Error {
  public:
    Error();
    operator std::string();
    operator bool();
    pybind11::object exp, val, trace;
    enum Type {Syntax,Attribute,Import};
  };

  class SyntaxError : Error{
  public:
    SyntaxError();
    operator std::string();
    std::string exp, text;
    int line_number, line_offset;
  };

  bool static thrown_exception_matches(Error::Type exception_type);

private:
  pybind11::object static error_occured();
};

#endif  // JUCI_PYTHON_INTERPRETER_H_
