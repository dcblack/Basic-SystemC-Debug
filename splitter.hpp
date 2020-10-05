#pragma once

/** @class Splitter_module

@brief Splits an incoming data stream into two or three parts.

Simplified block diagram
------------------------

```
fifo_port<T> _____               ____ fifo_export<T>
                  \             /
                  Splitter_module --- sig1_export<T>
signal_port<T> ___/             \____ sig2_export<T>
```

Reads input from either a connected FIFO or Signal.

Write to any connected output.

********************************************************************************
*/

#include "systemc.hpp"
#include "top.hpp"
#include "report.hpp"

template< typename T>
struct Splitter_module : sc_core::sc_module
{
  constexpr static const char* MSGID = "/Doulos/Example/splitter";
  constexpr static auto BIND = sc_core::SC_ZERO_OR_MORE_BOUND;
  sc_core::sc_port<sc_core::sc_signal_in_if<T>,1,BIND> signal_port { "signal_port" };
  sc_core::sc_port<sc_core::sc_fifo_in_if<T>,1,BIND>   fifo_port   { "fifo_port" };
  sc_core::sc_export<sc_core::sc_fifo_in_if<T>>        fifo_export { "fifo_export" };
  sc_core::sc_export<sc_core::sc_signal_in_if<T>>      sig1_export { "sig1_export" };
  sc_core::sc_export<sc_core::sc_signal_in_if<T>>      sig2_export { "sig2_export" };
  Splitter_module( sc_core::sc_module_name instance )
  : sc_module( instance )
  {
    SC_HAS_PROCESS( Splitter_module );
    SC_THREAD( transfer );
    fifo_export.bind( fifo );
    sig1_export.bind( sig1 );
    sig2_export.bind( sig2 );
  }
  void start_of_simulation();
  void transfer();
  sc_core::sc_fifo<T>   fifo{ 1 };
  sc_core::sc_signal<T> sig1{ "sig1" };
  sc_core::sc_signal<T> sig2{ "sig2" };
  // Following are here only for tracing purposes
  T xfer_value{};
private:
  bool is_connected( sc_core::sc_object*    self
                   , sc_core::sc_object*    channel
                   , sc_core::sc_interface* chan_if );
  bool fifo_connected{ false };
  bool sig1_connected{ false };
  bool sig2_connected{ false };
  void end_of_elaboration();
};

template< typename T>
bool Splitter_module<T>::is_connected( sc_core::sc_object*    self
                                     , sc_core::sc_object*    channel
                                     , sc_core::sc_interface* chan_if )
{
  sc_assert( self->get_parent_object() != nullptr );
  DEBUG( "Looking for connections to " << channel->name() );
  auto parent = self->get_parent_object();
  using port_ptr = sc_core::sc_port_base*;
  using export_ptr = sc_core::sc_export_base*;
  // Examine parent's children for modules
  for( const auto sibling : parent->get_child_objects() ) {
    if( sibling != self and "sc_module"s == sibling->kind() ) {
      // Example siblings ports
      DEBUG( "Considering " << sibling->name() );
      for( const auto obj : sibling->get_child_objects() ) {
        // Too many kinds of ports to consider, so just check the base class
        if( auto port = dynamic_cast<port_ptr>(obj); port != nullptr ) {
          DEBUG( "Considering " << port->name() );
          if( port->get_interface() == chan_if ) {
            DEBUG( port->name() << " connected to " << channel->name() );
            return true;
          }
        }
      }
    }
    // Continue up tree if exported
    if( "sc_export"s == sibling->kind() ) {
      if( dynamic_cast<export_ptr>(sibling)->get_interface() == chan_if ) {
        DEBUG( "Moving to grandparent" );
        if( is_connected( parent, channel, chan_if ) ) return true;
      }
    }
  }
  return false;
}

template< typename T>
void Splitter_module<T>::end_of_elaboration()
{
  if( fifo_port.size() == signal_port.size() ) {
    SC_REPORT_FATAL( MSGID, "Exactly one input must be connected" );
  }
  // Note: In the following, we pass the channel address in twice due
  // to multiple inheritance. First, to find a pointer to the channel
  // aspect and second to find a pointer to the interface aspect. They
  // are distinctly different within the object and upcasting will do
  // the right thing in this situation.
  fifo_connected = is_connected( this, &fifo, &fifo );
  sig1_connected = is_connected( this, &sig1, &sig1 );
  sig2_connected = is_connected( this, &sig2, &sig2 );
  if ( not ( fifo_connected || sig1_connected || sig2_connected ) ) {
    SC_REPORT_WARNING( MSGID, "No outputs are connected" );
  }
  if ( ( fifo_connected != (sig1_connected != sig2_connected) ) 
     and not ( fifo_connected && sig1_connected && sig2_connected ) ) {
    SC_REPORT_WARNING( MSGID, "Only one output is connected" );
  }
}

template< typename T>
void Splitter_module<T>::start_of_simulation()
{
  const auto& trace_file { Top_module::trace_file() };
  if( trace_file != nullptr ) {
    auto prefix = std::string(name()) + ".";
    sc_trace( trace_file, xfer_value, prefix + "xfer_value" );
  }
}

template< typename T>
void Splitter_module<T>::transfer()
{
  if( fifo_port.size() != 0 ) {
    for(;;) {
      xfer_value = fifo_port->read();
      DEBUG( "Transferring " <<  xfer_value );
      if( sig1_connected ) sig1.write( xfer_value );
      if( sig2_connected ) sig2.write( xfer_value );
      if( fifo_connected ) fifo.nb_write( xfer_value );
    }
  }
  if ( signal_port.size() != 0 ) {
    for(;;) {
      wait( signal_port->value_changed_event() );
      xfer_value = signal_port->read();
      DEBUG( "Transferring " <<  xfer_value );
      if( sig1_connected ) sig1.write( xfer_value );
      if( sig2_connected ) sig2.write( xfer_value );
      if( fifo_connected ) fifo.nb_write( xfer_value );
    }
  }
  sc_assert( false ); //< Paranoia check
}
