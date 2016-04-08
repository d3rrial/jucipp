#include "python_interpreter.h"
#include "notebook.h"
#include "config.h"
#include <iostream>
#include <pygobject.h>

inline pybind11::module pyobject_from_gobj(gpointer ptr){
  auto obj=G_OBJECT(ptr);
  if(obj)
    return pybind11::module(pygobject_new(obj), false);
  return pybind11::module(Py_None, false);
}

PythonInterpreter::PythonInterpreter(){
  auto init_juci_api=[](){
    pybind11::module(pygobject_init(-1,-1,-1), false);
    pybind11::module api("jucpp", "Python bindings for juCi++");
    api.def("get_current_text_buffer", [](){
      auto view=Notebook::get().get_current_view();
      if(view)
        return pyobject_from_gobj(view->gobj());
      return pybind11::module(Py_None, false);
    });
    return api.ptr();
  };
  PyImport_AppendInittab("jucipp", init_juci_api);
  Py_Initialize();
  Config::get().load();

  auto plugin_path=Config::get().python.plugin_directory;
  add_path(Config::get().python.site_packages);
  add_path(plugin_path);

  boost::filesystem::directory_iterator end_it;
  for(boost::filesystem::directory_iterator it(plugin_path);it!=end_it;it++){
    auto module_name=it->path().stem().string();
    if(module_name!="__pycache__"){
      auto module=import(module_name);
      if(!module){
        auto err=PythonError();
        if(err)
          std::cerr << std::string(err) << std::endl;
      }
    }
  }
}

pybind11::module PythonInterpreter::get_loaded_module(const std::string &module_name){
  return pybind11::module(PyImport_AddModule(module_name.c_str()), true);
}

pybind11::module PythonInterpreter::import(const std::string &module_name){
  return pybind11::module(PyImport_ImportModule(module_name.c_str()), false);
}

void PythonInterpreter::add_path(const boost::filesystem::path &path){
  std::wstring sys_path(Py_GetPath());
  if(!sys_path.empty())
#ifdef _WIN32
    sys_path += ';';
#else
    sys_path += ':';
#endif
  sys_path += path.generic_wstring();
  Py_SetPath(sys_path.c_str());
}

PythonInterpreter::~PythonInterpreter(){
  auto err=PythonError();
  if(Py_IsInitialized())
    Py_Finalize();
  if(err)
    std::cerr << std::string(err) << std::endl;
}

PythonError::PythonError(){
  pybind11::object error(PyErr_Occurred(), false);
  if(error){
    PyObject *exception,*value,*traceback;
    PyErr_Fetch(&exception,&value,&traceback);
    PyErr_NormalizeException(&exception,&value,&traceback);
    try{
      exp=std::string(pybind11::object(exception,false).str());
      val=std::string(pybind11::object(value,false).str());
      trace=std::string(pybind11::object(traceback,false).str());
    } catch (const std::runtime_error &e){
      exp=e.what();
    }
  }
}

PythonError::operator std::string(){
  return exp + "\n" + val + "\n" + trace;
}

PythonError::operator bool(){
  return !exp.empty();
}
