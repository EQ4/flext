/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001-2005 Thomas Grill (gr@grrrr.org)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

#include "flext.h"
#include "flstk.h"

flext_stk::flext_stk():
    inobjs(0),outobjs(0),
    inobj(NULL),outobj(NULL),
    smprt(0),blsz(0)
{}

bool flext_stk::Init()
{
    bool ret = flext_dsp::Init();
    inobjs = CntInSig();
    outobjs = CntOutSig();
    return ret;
}

void flext_stk::Exit()
{
    ClearObjs();
    flext_dsp::Exit();
}

void flext_stk::ClearObjs()
{
    FreeObjs();

    if(inobj) {
        for(int i = 0; i < inobjs; ++i) delete inobj[i]; 
        delete[] inobj; inobj = NULL;
    }
    if(outobj) {
        for(int i = 0; i < outobjs; ++i) delete outobj[i];
        delete[] outobj; outobj = NULL;
    }
}

bool flext_stk::CbDsp()
{
    // called on every rebuild of the dsp chain

    int i;
    
    if(Blocksize() != blsz || Samplerate() != smprt) {
        // block size or sample rate has changed... rebuild all objects

        ClearObjs();

        smprt = Samplerate();
        blsz = Blocksize();
        Stk::setSampleRate(smprt); 

        // set up sndobjs for inlets and outlets
        if(inobjs) {
            inobj = new Input *[inobjs];
            for(i = 0; i < inobjs; ++i)
                inobj[i] = new Input(InSig(i),blsz);
        }
        if(outobjs) {
            outobj = new Output *[outobjs];
            for(i = 0; i < outobjs; ++i) 
                outobj[i] = new Output(OutSig(i),blsz);
        }

        if(!NewObjs()) ClearObjs();
    }
    else {
        // assign changed input/output vectors

        for(i = 0; i < inobjs; ++i) inobj[i]->SetBuf(InSig(i));
        for(i = 0; i < outobjs; ++i) outobj[i]->SetBuf(OutSig(i));
    }
    return true;
}

void flext_stk::CbSignal()
{
    if(inobjs || outobjs) ProcessObjs(blsz);
}


// inlet class

flext_stk::Input::Input(const t_sample *b,int v): 
    buf(b),vecsz(v),
    index(0)
{}

MY_FLOAT *flext_stk::Input::tick(MY_FLOAT *vector,unsigned int vectorSize)
{
    for(unsigned int i = 0; i < vectorSize; i++) vector[i] = tick();
    return vector;
}


// outlet class

flext_stk::Output::Output(t_sample *b,int v): 
    buf(b),vecsz(v),
    index(0)
{}

void flext_stk::Output::tick(const MY_FLOAT *vector,unsigned int vectorSize)
{
    for(unsigned int i = 0; i < vectorSize; i++) tick(vector[i]);
}

