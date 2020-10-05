// The following makes time specification nicer:
//    15.3_ns, 0.1_us, 0_sec, 10_min, 4.5_hr 2_day
//

#ifndef SC_TIME_LITERAL_HPP
#define SC_TIME_LITERAL_HPP

#if __cplusplus >= 201103L

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma GCC   diagnostic push
#pragma GCC   diagnostic ignored "-Wunused-parameter"
#include <systemc>
#pragma clang diagnostic pop
#pragma GCC   diagnostic pop

inline sc_core::sc_time operator "" _day (long double val)        { return sc_core::sc_time( val        , sc_core::SC_SEC )*3600*24; }
inline sc_core::sc_time operator "" _day (unsigned long long val) { return sc_core::sc_time( double(val), sc_core::SC_SEC )*3600*24; }
inline sc_core::sc_time operator "" _hr  (long double val)        { return sc_core::sc_time( val        , sc_core::SC_SEC )*3600;    }
inline sc_core::sc_time operator "" _hr  (unsigned long long val) { return sc_core::sc_time( double(val), sc_core::SC_SEC )*3600;    }
inline sc_core::sc_time operator "" _min (long double val)        { return sc_core::sc_time( val        , sc_core::SC_SEC )*60;      }
inline sc_core::sc_time operator "" _min (unsigned long long val) { return sc_core::sc_time( double(val), sc_core::SC_SEC )*60;      }
inline sc_core::sc_time operator "" _sec (long double val)        { return sc_core::sc_time( val        , sc_core::SC_SEC );         }
inline sc_core::sc_time operator "" _sec (unsigned long long val) { return sc_core::sc_time( double(val), sc_core::SC_SEC );         }
inline sc_core::sc_time operator "" _ms  (long double val)        { return sc_core::sc_time( val        , sc_core::SC_MS  );         }
inline sc_core::sc_time operator "" _ms  (unsigned long long val) { return sc_core::sc_time( double(val), sc_core::SC_MS  );         }
inline sc_core::sc_time operator "" _us  (long double val)        { return sc_core::sc_time( val        , sc_core::SC_US  );         }
inline sc_core::sc_time operator "" _us  (unsigned long long val) { return sc_core::sc_time( double(val), sc_core::SC_US  );         }
inline sc_core::sc_time operator "" _ns  (long double val)        { return sc_core::sc_time( val        , sc_core::SC_NS  );         }
inline sc_core::sc_time operator "" _ns  (unsigned long long val) { return sc_core::sc_time( double(val), sc_core::SC_NS  );         }
inline sc_core::sc_time operator "" _ps  (long double val)        { return sc_core::sc_time( val        , sc_core::SC_PS  );         }
inline sc_core::sc_time operator "" _ps  (unsigned long long val) { return sc_core::sc_time( double(val), sc_core::SC_PS  );         }
inline sc_core::sc_time operator "" _fs  (long double val)        { return sc_core::sc_time( val        , sc_core::SC_FS  );         }
inline sc_core::sc_time operator "" _fs  (unsigned long long val) { return sc_core::sc_time( double(val), sc_core::SC_FS  );         }

#endif

#endif/*SC_TIME_LITERAL_HPP*/
