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

#include <random>
#include "commandline.hpp"
void Behavior_module::behavior_thread()
{
  INFO( NONE, "Starting " << __PRETTY_FUNCTION__ );

  // Manage error injection
  static std::default_random_engine gen;
  int weight = 50; // Percent
  int inject = Commandline::has_opt("-inject");
  if( inject != 0 ) {
    std::string arg = sc_argv()[ inject ];
    if( auto pos = arg.find_first_of("="); pos != std::string::npos ) {
      weight = std::stoi( arg.substr( pos + 1 ) );
      if( weight > 100 ) weight %= 100;
      if( weight <= 0  ) {
        REPORT( WARNING, "Weight should be a number 1..100" );
        weight = 1;
      }
    }
    INFO( NONE, "Inject bit-errors at " << weight << "%" );
  }
  std::uniform_int_distribution<int> rand_event(0,100);
  std::uniform_int_distribution<int> rand_bit(0,15);

  for(;;) {
    wait( recv_port->value_changed_event() );
    wait( 2.5_ns );
    recv_value = recv_port->read();
    wait( 2.5_ns );
    send_value = ~std::hash<Data_t>{}( recv_value ) & ~Data_t();
    // Check to see if an error should be injected
    if( inject != 0 and rand_event(gen) <= weight ) {
      // Perturb value by one bit
      auto bit = rand_bit(gen);
      DEBUG( "INJECTING bit " << bit );
      send_value ^= 1u<<bit;
    }
    send_port->write( send_value );
  }
}

// TAF!
