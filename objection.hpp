#pragma once

/** @class Objection

@brief  UVM-like mechanism to stop simulation when the last objection is removed.

Usage
-----

### Step 1

Instantiate `Objector_module` somewhere (e.g. under the top-most module)

### Step 2

Establish a drain-time using `Objection::set_drain_time(sc_time)`. The
drain-time is how long after all objections are missing to wait before
shutting down the simulation. If an objection is raised during the
drain-time, then the mechanism is reset.

Note that drain-time will start being monitored after the first objection
is raised.

### Step 3

Simply `Objection("name")` in threads at points where activity is starting.
Make sure the objection object has a well-defined life-time. The
construction string provided provides and identifier for the specific
objection.

Note: If verbosity is set to SC_DEBUG, then messages will be produced
as objections are raised and dropped.

Simulation will stop after destruction of the last objection.

Example
-------

```c++
void Stimulus_module::main_thread()
{
  Objection stim{ "stimulus" };
  ...Generate test stimulus data...
}
```

********************************************************************************
*/
#include "systemc.hpp"
#include "report.hpp"
#include <string>
#include <set>

////////////////////////////////////////////////////////////////////////////////
struct Objection
{
  Objection( const std::string& name ) ///< Create an objection
  : m_name( name )
  {
    sc_assert( name.size() > 0 );
    sc_assert( ready );
    objections.insert( m_name );
    std::string note{ "Raising objection " };
    note += m_name;
    INFO( DEBUG, note );
    ++created;
  }
  ~Objection() ///< Remove an objection
  {
    auto elt = objections.find( m_name );
    sc_assert( elt != objections.end() );
    objections.erase( elt );
    std::string note{ "Dropping objection " };
    note += m_name;
    INFO( DEBUG, note );
    if( objections.empty() and sc_core::sc_is_running() ) 
    {
      INFO( DEBUG, "No objections remain" );
      stop_reason = note;
      stop_event.notify(sc_core::SC_ZERO_TIME);
      wait( sc_core::SC_ZERO_TIME );
    }
  }
  static size_t total() { return created; } ///< Return total times used
  static size_t count() { return objections.size(); } ///< Return the outstanding objections
  static void set_drain_time( sc_core::sc_time delay ) { Objection::drain_time = delay; }
  static sc_core::sc_time get_drain_time() { return Objection::drain_time; }
  void set_timeout( sc_core::sc_time delay )
  {
    timeout = delay;
    timeout_event.notify(sc_core::SC_ZERO_TIME);
    wait( sc_core::SC_ZERO_TIME );
  }
private:
  friend struct Objector_module;
  std::string m_name;
  // Static stuff
  static constexpr const char* const       MSGID { "/Doulos/Objection" };
  inline static sc_core::sc_time           drain_time{ sc_core::SC_ZERO_TIME };
  inline static size_t                     created{ 0u };
  inline static sc_core::sc_process_handle timer_handle;
  inline static std::set<std::string>      objections;
  inline static sc_core::sc_event          stop_event;
  inline static std::string                stop_reason{ "unknown" };
  inline static sc_core::sc_event          timeout_event;
  inline static sc_core::sc_time           timeout{ sc_core::SC_ZERO_TIME };
  inline static bool                       ready{ false };
};

////////////////////////////////////////////////////////////////////////////////
// Module to shutdown SystemC on request
struct Objector_module: sc_core::sc_module
{
  Objector_module( sc_core::sc_module_name instance )
  : sc_module( instance )
  {
    SC_HAS_PROCESS( Objector_module );
    SC_THREAD( objection_thread );
    SC_THREAD( timeout_thread );
    if( Objection::get_drain_time() == sc_core::SC_ZERO_TIME ) {
      Objection::set_drain_time( sc_core::sc_time( 10, sc_core::SC_NS ));
    }
    Objection::ready = true;
  }
private:
  // Shuts down if last objection raised
  void objection_thread()
  {
    auto& MSGID{ Objection::MSGID };
    for(;;) {
      wait( Objection::stop_event );
      // Grab reason while waiting
      auto reason = std::exchange( Objection::stop_reason, "unknown"s );
      // Allow drainage
      DEBUG( "Draining" );
      wait( Objection::drain_time );
      if( Objection::objections.empty() ) {
        std::string note{ "Shutting down " };
        note += reason;
        INFO( NONE, note );
        sc_core::sc_stop();
      }
    }
  }
  void timeout_thread()
  {
    auto& MSGID{ Objection::MSGID };
    for(;;) {
      wait( Objection::timeout_event );
      auto stop_time = sc_core::sc_time_stamp() + Objection::timeout;
      sc_core::wait( Objection::timeout, Objection::timeout_event );
      if( sc_core::sc_time_stamp() == stop_time ) {
        SC_REPORT_WARNING( MSGID, "Timed out - shutting down" );
        sc_core::sc_stop();
      }
    }
  }
};

//TAF!
