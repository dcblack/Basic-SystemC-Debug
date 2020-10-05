#include "behavior.hpp"
#include "top.hpp"
#include <functional>

using namespace sc_core;

namespace {
  constexpr char const* const MSGID{ "/Doulos/Example/tracing" };
}

Behavior_module::Behavior_module( sc_core::sc_module_name instance )
: sc_module( instance )
{
  SC_HAS_PROCESS( Behavior_module );
  SC_THREAD( behavior_thread );
}

void Behavior_module::start_of_simulation()
{
  const auto& trace_file { Top_module::trace_file() };
  if( trace_file != nullptr ) {
    auto prefix = std::string(name()) + ".";
    sc_trace( trace_file, recv_value, prefix + "recv_value" );
    sc_trace( trace_file, send_value, prefix + "send_value" );
  }
}

void Behavior_module::behavior_thread()
{
  INFO( NONE, "Starting " << __PRETTY_FUNCTION__ );
  for(;;) {
    wait( recv_port->value_changed_event() );
    wait( 2.5_ns );
    recv_value = recv_port->read();
    wait( 2.5_ns );
    send_value = ~std::hash<Data_t>{}( recv_value ) & ~Data_t();
    send_port->write( send_value );
  }
}

// TAF!
