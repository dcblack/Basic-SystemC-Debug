#include "observer.hpp"
#include "objection.hpp"
#include "top.hpp"
#include "systemc.hpp"
#include <iomanip>
#include <string>

using namespace sc_core;

namespace {
  constexpr char const* const MSGID{ "/Doulos/Example/tracing" };
}

Observer_module::Observer_module( sc_module_name instance )
: sc_module( instance )
{
  SC_HAS_PROCESS( Observer_module );
  SC_THREAD( prepare_thread );
  SC_THREAD( checker_thread );
  actual_export.bind( actual_data );
}

void Observer_module::start_of_simulation()
{
  const auto& trace_file { Top_module::trace_file() };
  if( trace_file != nullptr ) {
    auto prefix = std::string(name()) + ".";
    sc_trace( trace_file, received_value, prefix + "received_value" );
    sc_trace( trace_file, expected_value, prefix + "expected_value" );
    sc_trace( trace_file, actual_value  , prefix + "actual_value" );
    sc_trace( trace_file, observed_count, prefix + "observed_count" );
    sc_trace( trace_file, failures_count, prefix + "failures_count" );
  }
}

void Observer_module::prepare_thread()
{
  INFO( NONE, "Starting " << __PRETTY_FUNCTION__ );
  // Obtain values sent out and convert into expected values
  for(;;) {
    wait( expect_port->value_changed_event() );
    received_value = expect_port->read();
    auto computed_value = ~std::hash<Data_t>{}( received_value ) & ~Data_t();
    DEBUG( "Computed " << std::hex << computed_value );
    expected_fifo.put( computed_value );
  }
}

void Observer_module::checker_thread()
{
  INFO( NONE, "Starting " << __PRETTY_FUNCTION__ );
  for(;;) {
    expected_value = expected_fifo.get(); // Wait for new expected value
    {
      Objection obj{ "Observing" }; // Raise objection on creation
      wait( actual_data.value_changed_event() );
      actual_value = actual_data.read();
      ++observed_count;
      // Do the values match?
      if( actual_value == expected_value ) {
        INFO( HIGH, "Good value 0x" << std::hex << actual_value );
      } else {
        REPORT(ERROR, std::hex << "Mismatch got 0x" << actual_value
                   << " != expected 0x" << expected_value );
        ++failures_count;
      }
    }
  }
}

// TAF!
