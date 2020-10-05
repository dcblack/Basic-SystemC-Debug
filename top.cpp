#include "top.hpp"
#include "objection.hpp"
#include "stimulus.hpp"
#include "splitter.hpp"
#include "behavior.hpp"
#include "observer.hpp"
#include "commandline.hpp"

using namespace sc_core;

// stimulus -- splitter -- behavior -- observer
//                     \______________/

Top_module::Top_module( sc_module_name instance )
: sc_module( instance )
, objector( std::make_unique<Objector_module>         ("objector") )
, stimulus( std::make_unique<Stimulus_module>         ("stimulus") )
, splitter( std::make_unique<Splitter_module<Data_t>> ("splitter") )
, behavior( std::make_unique<Behavior_module>         ("behavior") )
, observer( std::make_unique<Observer_module>         ("observer") )
{
  //----------------------------------------------------------------------------
  // Parse command-line
  if( Commandline::has_opt( "-trace" ) > 0 and s_self == nullptr ) {
    s_self = this;
  }
  if( Commandline::has_opt( "-quiet" ) > 0 ) {
    sc_report_handler::set_verbosity_level( SC_NONE );
  }
  if( Commandline::has_opt( "-verbose" ) > 0 ) {
    sc_report_handler::set_verbosity_level( SC_HIGH );
  }
  if( Commandline::has_opt( "-debug" ) > 0 ) {
    sc_report_handler::set_verbosity_level( SC_DEBUG );
  }
  sc_report_handler::set_actions( SC_ERROR, SC_DISPLAY | SC_LOG );

  //----------------------------------------------------------------------------
  // Connect everything up
  splitter->fifo_port.bind    ( stimulus->stim_export    );
  behavior->recv_port.bind    ( splitter->sig1_export    );
  behavior->send_port.bind    ( observer->actual_export  );
  observer->expect_port.bind  ( splitter->sig2_export    );
  observer->running_port.bind ( stimulus->running_export );
}

Top_module::~Top_module() = default;

void Top_module::end_of_elaboration()
{

  // If tracing, s_self will point to top-module and
  // we should open the waveform data file.
  if( s_self != nullptr ) {
    m_trace = sc_create_vcd_trace_file( "dump" );
    sc_assert( m_trace != nullptr );
  }
}

sc_core::sc_trace_file* Top_module::trace_file()
{
  if( s_self == nullptr ) return nullptr;
  return s_self->m_trace;
}

// TAF!
