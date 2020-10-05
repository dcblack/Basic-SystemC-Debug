#pragma once

#include "systemc.hpp"
#include "common.hpp"
#include <memory>

// Forward declarations
struct Objector_module;
struct Stimulus_module;
template<typename T> struct Splitter_module;
struct Behavior_module;
struct Observer_module;

struct Top_module: sc_core::sc_module
{
  std::unique_ptr<Objector_module>         objector;
  std::unique_ptr<Stimulus_module>         stimulus;
  std::unique_ptr<Splitter_module<Data_t>> splitter;
  std::unique_ptr<Behavior_module>         behavior;
  std::unique_ptr<Observer_module>         observer;
  // Constructor scans command-line and connects everything
  Top_module( sc_core::sc_module_name );
  ~Top_module();
  // Open trace file if needed
  void end_of_elaboration();
  // If not nullptr, then return an trace file handle
  static sc_core::sc_trace_file* trace_file();
private:
  sc_core::sc_trace_file*   m_trace { nullptr };
  inline static Top_module* s_self  { nullptr };
};
