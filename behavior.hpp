#pragma once

#include "systemc.hpp"
#include "common.hpp"

struct Behavior_module : sc_core::sc_module
{
  sc_core::sc_in<Data_t>  recv_port { "recv_port" };
  sc_core::sc_out<Data_t> send_port { "send_port" };
  Behavior_module( sc_core::sc_module_name instance );
  void start_of_simulation();
  void behavior_thread();
private:
  Data_t recv_value{ 0 };
  Data_t send_value{ 0xFFFFu };
};
