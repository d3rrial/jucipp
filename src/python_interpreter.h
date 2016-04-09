#ifndef JUCI_PYTHON_INTERPRETER_H_
#define JUCI_PYTHON_INTERPRETER_H_

#include <pybind11/pybind11.h>
#include <boost/filesystem/path.hpp>

#include <iostream>
using namespace std;
namespace Python {
  class Interpreter {
  private:
    Interpreter();
    ~Interpreter();
    wchar_t *argv;
  public:
    static Interpreter& get(){
      static Interpreter singleton;
      return singleton;
    }
    pybind11::module get_loaded_module(const std::string &module_name);
    pybind11::module import(const std::string &module_name);
    void add_path(const boost::filesystem::path &path);
  };

  class Error {
  public:
    Error();
    operator std::string();
    operator bool();
    std::string exp, val, trace;
  };
}  // namespace Python
#endif  // JUCI_PYTHON_INTERPRETER_H_