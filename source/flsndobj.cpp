#include "flsndobj.h"

flext_sndobj::flext_sndobj():
	inobjs(0),outobjs(0),
	inobj(NULL),tmpobj(NULL),outobj(NULL)
{}

flext_sndobj::~flext_sndobj()
{
	ClearObjs();
}

void flext_sndobj::ClearObjs()
{
	FreeObjs();

	if(inobj) {
		for(int i = 0; i < inobjs; ++i) { delete inobj[i]; delete tmpobj[i]; }
		delete[] inobj; inobj = NULL; 
		delete[] tmpobj; tmpobj = NULL;
		inobjs = 0;
	}
	if(outobj) {
		for(int i = 0; i < outobjs; ++i) delete outobj[i];
		delete[] outobj; outobj = NULL; outobjs = 0;
	}
}

void flext_sndobj::m_dsp(int n,t_sample *const *in,t_sample *const *out)
{
	// called on every rebuild of the dsp chain
	
	ClearObjs();

	// set up sndobjs for inlets and outlets
	int i;
	inobj = new Inlet *[inobjs = CntInSig()];
	tmpobj = new SndObj *[inobjs];
	for(i = 0; i < inobjs; ++i) {
		inobj[i] = new Inlet(in[i],Blocksize(),Samplerate());
		tmpobj[i] = new SndObj(NULL,Blocksize(),Samplerate());
	}
	outobj = new Outlet *[outobjs = CntInSig()];
	for(i = 0; i < outobjs; ++i) outobj[i] = new Outlet(out[i],Blocksize(),Samplerate());

	NewObjs();
}

void flext_sndobj::m_signal(int n,t_sample *const *in,t_sample *const *out)
{
	for(int i = 0; i < inobjs; ++i) *tmpobj[i] << *inobj[i];
	ProcessObjs();
}


flext_sndobj::Inlet::Inlet(const t_sample *b,int vecsz,float sr): 
  SndIO(1,sizeof(t_sample)*8,NULL,vecsz,sr),buf(b) 
{}

short flext_sndobj::Inlet::Read() 
{ 
	if(!m_error) { 
		for(m_vecpos = 0; m_vecpos < m_samples; m_vecpos++)
			m_output[m_vecpos] = buf[m_vecpos];
		return 1; 
	} 
	else return 0; 
}

short flext_sndobj::Inlet::Write() { return 0; }


flext_sndobj::Outlet::Outlet(t_sample *b,int vecsz,float sr): 
  SndIO(1,sizeof(t_sample)*8,NULL,vecsz,sr),buf(b) 
{}

short flext_sndobj::Outlet::Read() { return 0; }

short flext_sndobj::Outlet::Write() 
{ 
	if(!m_error) { 
		if(m_IOobjs[0])
			for(m_vecpos = 0; m_vecpos < m_samples; m_vecpos++)
				buf[m_vecpos] = m_IOobjs[0]->Output(m_vecpos);
		return 1; 
	} 
	else return 0; 
}


