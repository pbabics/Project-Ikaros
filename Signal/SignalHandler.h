#include <signal.h>
#include <assert.h>

#ifndef __Signal
#define __Signal

enum SignalType
{
    Abort = SIGABRT,
    FloatingPointException = SIGFPE,
    InvalidInstruction = SIGILL,
    Interupt = SIGINT,
    SegmentationFault = SIGSEGV,
    Terminate = SIGTERM,
    Continue = SIGCONT
};

class SignalHandler
{
    public:
        typedef void (*SignalFunction)(int p);

        SignalFunction fSIGABRT;
        SignalFunction fSIGFPE;
        SignalFunction fSIGILL;
        SignalFunction fSIGINT;
        SignalFunction fSIGSEGV;
        SignalFunction fSIGTERM;
        SignalFunction fSIGCONT;

        void setSignalHandler(SignalType sig, SignalFunction func);
        void setIgnoreSignal(SignalType sig);
        void setDefaultHandler(SignalType sig);
};

#endif
