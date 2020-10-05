#pragma once

#include "systemc.hpp"
#include <string>

struct Commandline
{
  // Return true if a command-line argument beginning with opt exists
  inline static size_t has_opt( std::string opt )
  {
    for( int i = 1; i < sc_core::sc_argc(); ++i ) {
      std::string arg{ sc_core::sc_argv()[ i ] };
      if( arg.find( opt ) == 0 ) return i;
    }
    return 0;
  }
private:
  [[maybe_unused]]inline constexpr static char const * const
  MSGID{ "/Doulos/Example/Commandline" };
};
