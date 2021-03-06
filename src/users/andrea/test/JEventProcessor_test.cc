// $Id$
//
//    File: JEventProcessor_test.cc
// Created: Mon Dec 28 15:48:21 CET 2015
// Creator: celentan (on Linux apcx4 2.6.32-504.30.3.el6.x86_64 x86_64)
//

#include <iostream>
#include <iomanip>
using namespace std;

#include "JEventProcessor_test.h"
#include "system/BDXEventProcessor.h"

#include <DAQ/fa250Mode1CalibHit.h>
#include <DAQ/fa250Mode1CalibPedSubHit.h>

#include <TT/TranslationTable.h>

#include <DAQ/eventData.h>

#include <IntVeto/IntVetoHit.h>
#include <IntVeto/IntVetoDigiHit.h>
#include <IntVeto/IntVetoSiPMHit.h>

#include <ExtVeto/ExtVetoPMTHit.h>
#include <ExtVeto/ExtVetoDigiHit.h>

#include <Calorimeter/CalorimeterSiPMHit.h>
#include <Calorimeter/CalorimeterHit.h>

#include <system/JROOTOutput.h>

#include "TH1D.h"
#include "TH2D.h"
#include "TTree.h"
#include "TF1.h"
// Routine used to create our JEventProcessor
#include <JANA/JApplication.h>
#include <JANA/JFactory.h>

using namespace jana;



double JEventProcessor_test::HH(double x){
	if (x>0) return 1;
	else return 0;
}

double JEventProcessor_test::fun(double t,double t0){
	double ret;
	if ((t-t0)<0) ret=0;
	else{
		ret=(t-t0)*(t-t0)*exp(-(t-t0)/tau);
		ret/=(2*tau*tau*tau); //normalization
	}
	return ret;
}

double JEventProcessor_test::integrand(double *x,double *par){
	double t=x[0];
	double t0=par[0];
	return fun(t,t0)*hGlob->GetBinContent(hGlob->FindBin(t));
}

double JEventProcessor_test::integrated(double t){
	double ret;
	fGlob->SetParameter(0,t);
	ret = fGlob->Integral(t,t+5*tau);
	return ret;
}

extern "C"{
void InitPlugin(JApplication *app){
	InitJANAPlugin(app);
	app->AddProcessor(new JEventProcessor_test());
}
} // "C"


//------------------
// JEventProcessor_test (Constructor)
//------------------
JEventProcessor_test::JEventProcessor_test():m_isFirstCallToBrun(1)
{
	tau=3.5;
	T0=500;
	fGlob=new TF1("fGlob",this,&JEventProcessor_test::integrand,-10*tau,T0+10*tau,1);
	fGlob->SetNpx(1000);
}

//------------------
// ~JEventProcessor_test (Destructor)
//------------------
JEventProcessor_test::~JEventProcessor_test()
{

}
//------------------
// init
//------------------
jerror_t JEventProcessor_test::init(void)
{
	// This is called once at program startup. If you are creating
	// and filling historgrams in this plugin, you should lock the
	// ROOT mutex like this:
	//
	// japp->RootWriteLock();
	//  ... fill historgrams or trees ...
	// japp->RootUnLock();
	//

	app->RootWriteLock();

	jout<<"test::init is called"<<std::endl;
	h=new TH1D("h","h",500,-0.5,499.5);
	h1=new TH1D("h1","h1",1000,-0.5,499.5);
	t=new TTree("tout","tout");

	t->Branch("component",&component);
	t->Branch("Q",&Q);
	t->Branch("Q1",&Q1);
	t->Branch("Q2",&Q2);
	t->Branch("eventN",&eventN);

	app->RootUnLock();
	return NOERROR;
}

//------------------
// brun
//------------------
jerror_t JEventProcessor_test::brun(JEventLoop *loop, int32_t runnumber)
{

	// This is called whenever the run number changes
	/* To get access to the ROOT output, use exactly the following code */
	if (m_isFirstCallToBrun){
		string class_name,this_class_name;
		string joutput_name;
		BDXEventProcessor *m_BDXEventProcessor;
		vector<JEventProcessor*> m_processors=app->GetProcessors();
		vector<JEventProcessor*>::iterator m_processors_it;

		m_isFirstCallToBrun=0;
		class_name="BDXEventProcessor";
		joutput_name="JROOTOutput";
		//Now I need to determine which processor is the one holding the output. Discussing with David, he suggested just to check the class name, since
		//a dynamic cast may not work with plugins
		for (m_processors_it=m_processors.begin();m_processors_it!=m_processors.end();m_processors_it++){
			if ((*m_processors_it)!=0){
				this_class_name=string((*m_processors_it)->className());
				if (this_class_name==class_name){
					m_BDXEventProcessor=(BDXEventProcessor*)(*m_processors_it);
					if (m_BDXEventProcessor->getOutput()==0){
						jerr<<"BDXEventProcessor JOutput is null!"<<endl;
						break;
					}
					if (string(m_BDXEventProcessor->getOutput()->className())==joutput_name){
						m_ROOTOutput=(JROOTOutput*)(m_BDXEventProcessor->getOutput());
						jout<<"Got JROOTOutput!"<<endl;
					}
					else{
						jerr<<"BDXEventProcessor JOutput is not null BUT class is: "<<m_BDXEventProcessor->getOutput()->className()<<endl;
					}
				}
			}
		}
		/*For ALL objects you want to add to ROOT file, use the following:*/
		if (m_ROOTOutput){
			m_ROOTOutput->AddObject(h);
			m_ROOTOutput->AddObject(h1);
			m_ROOTOutput->AddObject(t);
		}

	}








	return NOERROR;


}

//------------------
// evnt
//------------------
jerror_t JEventProcessor_test::evnt(JEventLoop *loop,uint64_t eventnumber)
{
	// This is called for every event. Use of common resources like writing
	// to a file or filling a histogram should be mutex protected. Using
	// loop->Get(...) to get reconstructed objects (and thereby activating the
	// reconstruction algorithm) should be done outside of any mutex lock
	// since multiple threads may call this method at the same time.
	// Here's an example:
	//

	if ((eventnumber==33571)||(eventnumber==362954)||(eventnumber==41649)||(eventnumber==3606108)){



		vector<const fa250Mode1Hit*> cdata;
		vector<const fa250Mode1Hit*>::const_iterator cdata_it;

		loop->Get(cdata);

	


		japp->RootWriteLock();
		//  ... fill historgrams or trees ...
		for (cdata_it=cdata.begin();cdata_it!=cdata.end();cdata_it++){
			h->Reset();
			h->SetName(Form("h_%lld_%i_%i",eventnumber,(*cdata_it)->m_channel.slot,(*cdata_it)->m_channel.channel));

			for (int ii=0;ii<(*cdata_it)->samples.size();ii++){
				h->Fill(ii,(*cdata_it)->samples.at(ii));
			}
			h->Write();

		}



		japp->RootUnLock();

	}


	return NOERROR;
}

//------------------
// erun
//------------------
jerror_t JEventProcessor_test::erun(void)
{
	// This is called whenever the run number changes, before it is
	// changed to give you a chance to clean up before processing
	// events from the next run number.
	return NOERROR;
}

//------------------
// fini
//------------------
jerror_t JEventProcessor_test::fini(void)
{

	return NOERROR;
}

