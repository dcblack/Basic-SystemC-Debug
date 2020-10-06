#include "stimulus.hpp"
#include "top.hpp"
#include "objection.hpp"
#include <random>

using namespace sc_core;

namespace {
  constexpr char const* const MSGID{ "/Doulos/Example/tracing" };
}

Stimulus_module::Stimulus_module( sc_module_name instance )
  : sc_module( instance )
{
  SC_HAS_PROCESS( Stimulus_module );
  SC_THREAD( stimulus_thread );
  stim_export.bind( stimulus );
  running_export.bind( running );
}

void Stimulus_module::start_of_simulation()
{
  const auto& trace_file { Top_module::trace_file() };

  if ( trace_file != nullptr ) {
    auto prefix = std::string( name() ) + ".";
    sc_trace( trace_file, running, prefix + "running" );
    sc_trace( trace_file, test_count, prefix + "test_count" );
    sc_trace( trace_file, value, prefix + "value" );
  }

  Objection::set_drain_time( 2_ns );
}

void Stimulus_module::stimulus_thread()
{
  INFO( NONE, "Starting " << __PRETTY_FUNCTION__ );

  // Determine sample size
  auto sample_size = 10; // Default

  if ( int i = Commandline::has_opt( "-n=" ); i != 0 ) {
    std::string arg = sc_argv()[ i ];
    sample_size = std::stoi( arg.substr( 3 ) );

    if ( sample_size <= 1 ) {
      REPORT( WARNING, "Sample size (-n) should be a number 1..100" );
      sample_size = 1;
    }
  }

  INFO( NONE, "Generating " << sample_size << " samples." );

  static std::default_random_engine    gen;
  static std::uniform_int_distribution dist( 0, ~value );

  // Generate samples
  running.write( true );
  Objection o{ "Stimulus" };

  while ( sample_size-- ) {
    value = dist( gen );
    wait( 10_ns );
    DEBUG( "Sending 0x" << std::hex << value );
    ++test_count;
    stimulus.write( value );
  }

  INFO( NONE, "Stimulus sent " << test_count <<  " samples" );
  running.write( false );
}

// TAF!
