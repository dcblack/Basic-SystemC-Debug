#pragma once

#include "systemc.hpp"
#include "common.hpp"

struct Stimulus_module : sc_core::sc_module
{
  sc_core::sc_export<sc_core::sc_fifo_in_if<Data_t>> stim_export    { "stim_export" };
  sc_core::sc_export<sc_core::sc_signal_in_if<bool>> running_export { "running_export" };
  Stimulus_module( sc_core::sc_module_name instance );
  void start_of_simulation();
  void stimulus_thread();
private:
  sc_core::sc_fifo<Data_t> stimulus{ 4 };
  sc_core::sc_signal<bool> running;
  // Following are here only for tracing purposes
  uint16_t test_count{ 0 };
  Data_t   value{0};
};
