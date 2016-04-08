#ifndef JUCI_PYTHON_INTERPRETER_H_
#define JUCI_PYTHON_INTERPRETER_H_

#include <pybind11/pybind11.h>
#include <boost/filesystem/path.hpp>

#include <iostream>
using namespace std;

class PythonInterpreter {
private:
  PythonInterpreter();
  ~PythonInterpreter();
  wchar_t *argv;
public:
  static PythonInterpreter& get(){
    static PythonInterpreter singleton;
    return singleton;
  }
  pybind11::module get_loaded_module(const std::string &module_name);
  pybind11::module import(const std::string &module_name);
  void add_path(const boost::filesystem::path &path);
};

class PythonError {
public:
  PythonError();
  operator std::string();
  operator bool();
  std::string exp, val, trace;
};

#endif  // JUCI_PYTHON_INTERPRETER_H_