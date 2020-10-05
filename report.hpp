#pragma once

/** @file report.hpp

@brief Reporting macro extensions

This is a simplified version of David's reporting extensions.

The basic idea is to simplify syntax when reporting on messages more
complex than a basic string. Also, simplifies specification of the
message type (aka message identifier). See instructions for usage.

There are three macros, several requirements, one limitation, and
some instructions below. Read on...

Macros
------

1. REPORT(type,stream) -- trivially improves SC_REPORT_(INFO,WARNING,ERROR,FATAL)
2. INFO(level,stream) -- improves on SC_REPORT_INFO_VERB
3. DEBUG(stream) -- allows sc_object specific debugging

Requirements
------------

1. Assumes SystemC
2. Define MSGID string anytime these macros are used.
3. If using the DEBUG macro, then commandline.hpp must be available
4. To disable the DEBUG macro, define NDEBUG

Limitations
-----------

If your streaming expression (see instructions) contains commas, then
you must surround that element with parentheses due to limitations of
pre-processor macros.

Instructions
------------

### Step 1

Include this at the top of any file where the macros are used.

```c++
#include "report.cpp"
```

### Step 2

Define the message type with a file local variable, MSGID as follows:

In modules, channels, classes or structs:

```c++
  inline static constexpr char const * const MSGID
  { "/COMPANY/PROJECT/MODULE" };
```

Or files without a class:

```c++
namespace {
  constexpr char const * const MSGID
  { "/COMPANY/PROJECT/MODULE" };
}
```

### Step 3

Insert the macros where you would normally use SC_REPORT_* macros. Here are
some simple examples:

REPORT( WARNING, "Possible problem detected" );
INFO( NONE, "Version " << version );
DEBUG( "Sending packet " << packet );

### Step 4 (for DEBUG macro)

Make sure you have set the verbosity level to SC_DEBUG. You might consider
using the Commandline::has_opt("-debug") to conditionallly set this.

At run-time, add command-line arguments to specify the instances you want to
debug:

% run.x -debug=observer -debug=splitter # debugs only for specified elements
% run.x -debugall # turns on all DEBUG messages

********************************************************************************
*/

#include "systemc.hpp"
#include <string>
#include <iomanip>
#include <sstream>
struct Report {
  inline static std::ostringstream mout;
};

// For type: WARNING, ERROR, FATAL
#define REPORT(type,stream)                            \
do {                                                   \
  Report::mout << std::dec << stream << std::ends;     \
  auto str = Report::mout.str(); Report::mout.str(""); \
  SC_REPORT_##type( MSGID, str.c_str() );              \
} while (0)
// For level: NONE, LOW, MEDIUM, HIGH, DEBUG
#define INFO(level,stream)                                                     \
do {                                                                           \
  if( sc_core::sc_report_handler::get_verbosity_level()                        \
        >= (sc_core::SC_##level) ) {                                           \
    Report::mout << std::dec << stream;                                        \
    auto now = sc_core::sc_time_stamp();                                       \
    if( now > sc_core::SC_ZERO_TIME                                            \
        or sc_core::sc_get_status() >= sc_core::SC_START_OF_SIMULATION ) {     \
      Report::mout << std::dec << " at " << now;                               \
    }                                                                          \
    Report::mout << std::ends;                                                 \
    if( (sc_core::SC_##level) > sc_core::SC_DEBUG ) {                          \
      std::string id{"DEBUG("};                                                \
      id+=__FILE__ ; id+=":"; id+=std::to_string(__LINE__)+")";                \
      size_t p0=id.find("/"),p1=id.find_last_of("/");                          \
      if(p1!=std::string::npos) id.erase(p0,p1-p0+1);                          \
      auto str = Report::mout.str(); Report::mout.str("");                     \
      SC_REPORT_INFO_VERB( id.c_str(), str.c_str(), (sc_core::SC_##level) );   \
    } else {                                                                   \
      auto str = Report::mout.str(); Report::mout.str("");                     \
      SC_REPORT_INFO_VERB( MSGID, str.c_str(), (sc_core::SC_##level) );        \
    }                                                                          \
  }                                                                            \
} while (0)

#ifdef NDEBUG
#define DEBUG()
#else
#include "commandline.hpp"
#define DEBUG(stream) do {                                                     \
  if( sc_core::sc_report_handler::get_verbosity_level() >= sc_core::SC_DEBUG   \
  and ( Commandline::has_opt("-debugall")                                      \
     or Commandline::has_opt("-debug="s + basename() ) ) ) {                   \
     INFO(DEBUG,stream);                                                       \
  }                                                                            \
} while(0)
#endif

// TAF!
