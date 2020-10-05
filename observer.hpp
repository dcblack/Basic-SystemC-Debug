#pragma once

#include "systemc.hpp"
#include "tlm.hpp"
#include "common.hpp"

struct Observer_module : sc_core::sc_module
{
  sc_core::sc_export<sc_core::sc_signal_out_if<Data_t>> actual_export { "actual_export" };
  sc_core::sc_port<sc_core::sc_signal_in_if<Data_t>>    expect_port   { "expect_port" };
  sc_core::sc_port<sc_core::sc_signal_in_if<bool>>      running_port  { "running_port" };
  Observer_module( sc_core::sc_module_name instance );
  void start_of_simulation();
  void prepare_thread();
  void checker_thread();
private:
  uint16_t observed_count{ 0 };
  uint16_t failures_count{ 0 };
  sc_core::sc_signal<Data_t> actual_data;
  tlm::tlm_fifo<Data_t> expected_fifo{ -1 }; // unbounded
  // Following are here only for tracing purposes
  Data_t received_value{};
  Data_t expected_value{};
  Data_t actual_value{};
};
