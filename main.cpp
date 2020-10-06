
////////////////////////////////////////////////////////////////////////////////
#include "systemc.hpp"
#include <string>
#include <iomanip>
#include "top.hpp"
#include "commandline.hpp"
using namespace sc_core;

// Every file needs the following with appropriate adjustments
namespace {
  constexpr char const* const MSGID{ "/Doulos/Example/tracing/main" };
}

int sc_main( [[maybe_unused]]int argc, [[maybe_unused]]char* argv[] )
{
  // Built-in run-time help is often welcome
  if( Commandline::has_opt( "-help" )
   or Commandline::has_opt( "--help")
   or Commandline::has_opt( "-h" )
  ) {
    // Example of C++11 raw strings R"( .... )"
    std::cout << R"(
--------------------------------------------------------------------------------

Run-time options:
-----------------

| Option          | Description                                  |
| :-------------- | :------------------------------------------- |
| -help           | Displays this text and exits                 |
| -debug          | Increases verbosity to debug level (noisy)   |
| -debug=INSTANCE | Debug messages for instances named INSTANCE  |
| -debugall       | Debug messages for all instances             |
| -inject=PERCENT | Inject errors at a range of PERCENT (1..100) |
| -n=SAMPLE_SIZE  | Number of samples to generate (default 10)   |
| -quiet          | Decreases verbosity lowest level             |
| -trace          | Enables output of waveform data to dump.vcd  |

Note: If multiple verbosities are specified, the last one wins.

--------------------------------------------------------------------------------
  )";
    return 0;
  }

  SC_REPORT_INFO( MSGID, "Instantiating" );
  Top_module top{"top"};

  SC_REPORT_INFO( MSGID, "Starting simulator" );
  sc_start();

  // Clean up
  if ( not sc_end_of_simulation_invoked() )
  {
    SC_REPORT_ERROR( MSGID, "Simulation stopped without explicit sc_stop()");
    sc_stop(); //< invoke end_of_simulation() overrides
  }

  auto errors = sc_report_handler::get_count(SC_ERROR)
              + sc_report_handler::get_count(SC_FATAL);

  INFO( NONE, "\n" << std::string(80,'#') << "\nSummary for " << sc_argv()[0] << ":\n  "
    << std::setw(2) << sc_report_handler::get_count(SC_INFO)    << " informational messages" << "\n  "
    << std::setw(2) << sc_report_handler::get_count(SC_WARNING) << " warnings" << "\n  "
    << std::setw(2) << sc_report_handler::get_count(SC_ERROR)   << " errors"   << "\n  "
    << std::setw(2) << sc_report_handler::get_count(SC_FATAL)   << " fatals"   << "\n\n"
    << "Simulation " << (errors?"FAILED":"PASSED")
  );

  return (errors?1:0);
}

// TAF!
