#!gdb

set pagination off

define skip_systemc
  # Skip all templated constructors/destructors in std::
  skip -rfu ^std::([a-zA-z0-9_]+)<.*>::~?\1
  # Skip SystemC and TLM namespaces
  skip -rfu ^sc_core::.*
  skip -rfu ^sc_dt::.*
  skip -rfu ^tlm::.*
  skip wait
end

skip_systemc
break sc_main
rbreak .*::.*_thread[^_]
rbreak .*[^c]_module::before_end_of_elaboration
rbreak .*[^c]_module::end_of_elaboration
rbreak .*[^c]_module::start_of_simulation
rbreak .*[^c]_module::end_of_simulation
info breakpoints
set args -debug=splitter -trace
show args
set pagination on
