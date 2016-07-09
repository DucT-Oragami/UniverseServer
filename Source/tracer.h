// A simple debug function used to print the current File, Function, and Line
// This is for when your program crashes, and you're trying to narrow down where exactly it does so
// Please get rid of any dTrace() calls you make when you are done with them
// Note: The "Function" is called dTrace() (Debug Trace) to ensure that there are no name conflicts with anything else

#ifndef _tracer_h
#define _tracer_h

// Uncomment below line to make all dTrace() calls irrelavent
//#define noTrace
#ifdef noTrace
#define dTrace()

#else
// In the future, uncomment below line to log all traces in a file
//#define logTrace
#ifdef logTrace
// ToDo: Log traces
#else
#define dTrace() fprintf(stderr, "\nTracer:\nFILE: %s\nFUNCTION: %s\nLINE: %d\n", __FILE__, __FUNCTION__, __LINE__);

#endif
#endif
#endif