#include "SignalHandler.h"

void SignalHandler::setSignalHandler(SignalType sig ,SignalFunction func )
{
    assert(func);
    switch(sig)
        {
	        case Abort: 
		        fSIGABRT = func ;
		        signal(Abort,fSIGABRT);
		        break;

	        case FloatingPointException:
		        fSIGFPE = func ; 
		        signal(FloatingPointException,fSIGFPE);
		        break;

	        case InvalidInstruction:
		        fSIGILL = func ; 
		        signal(InvalidInstruction,fSIGILL);
		        break;

	        case Interupt: 
		        fSIGINT = func ;
		        signal(Interupt,fSIGINT);
		        break;

	        case SegmentationFault: 
		        fSIGSEGV = func ;
		        signal(SegmentationFault,fSIGSEGV );
		        break;

	        case Terminate: 
		        fSIGTERM = func ;  
		        signal(Terminate,fSIGTERM); 
		        break;

	        case Continue: 
		        fSIGCONT = func ;  
		        signal(Continue,fSIGCONT); 
		        break;

            default:
                break;
	    }

}

void SignalHandler::setIgnoreSignal(SignalType sig)
{
    switch(sig)
        {
	        case Abort: 
		        signal(Abort,SIG_IGN);
		        break;

	        case FloatingPointException:
		        signal(FloatingPointException,SIG_IGN);
		        break;

	        case InvalidInstruction:
		        signal(InvalidInstruction,SIG_IGN);
		        break;

	        case Interupt: 
		        signal(Interupt,SIG_IGN);
		        break;

	        case SegmentationFault: 
		        signal(SegmentationFault,SIG_IGN);
		        break;

	        case Terminate: 
		        signal(Terminate,SIG_IGN);
		        break;

	        case Continue: 
		        signal(Continue,SIG_IGN);
		        break;

            default:
                break;
	    }
}

void SignalHandler::setDefaultHandler(SignalType sig)
{
    switch(sig)
        {
	        case Abort: 
		        signal(Abort,SIG_DFL);
		        break;

	        case FloatingPointException:
		        signal(FloatingPointException,SIG_DFL);
		        break;

	        case InvalidInstruction:
		        signal(InvalidInstruction,SIG_DFL);
		        break;

	        case Interupt: 
		        signal(Interupt,SIG_DFL);
		        break;

	        case SegmentationFault: 
		        signal(SegmentationFault,SIG_DFL);
		        break;

	        case Terminate: 
		        signal(Terminate,SIG_DFL);
		        break;

	        case Continue: 
		        signal(Continue,SIG_DFL);
		        break;

            default:
                break;
	    }
}
