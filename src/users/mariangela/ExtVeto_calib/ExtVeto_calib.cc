/*
 * ExtVeto_calib.cc
 *
 *  Created on: 18 feb 2016
 *      Author: Mariangela
 */
#include <iostream>
#include <iomanip>
using namespace std;

#include "ExtVeto_calib.h"
#include "system/BDXEventProcessor.h"

#include <DAQ/fa250Mode1CalibHit.h>

#include <TT/TranslationTable.h>

#include <DAQ/eventData.h>

#include <ExtVeto/ExtVetoPMTHit.h>
#include <ExtVeto/ExtVetoDigiHit.h>

#include <system/JROOTOutput.h>

#include "TH1D.h"
#include "TH2D.h"
#include "TTree.h"

// Routine used to create our JEventProcessor
#include <JANA/JApplication.h>
#include <JANA/JFactory.h>

using namespace jana;



extern "C"{
void InitPlugin(JApplication *app){
	InitJANAPlugin(app);
	app->AddProcessor(new ExtVeto_calib());
}
} // "C"


//------------------
// ExtVeto_SigDisplay (Constructor)
//------------------
ExtVeto_calib::ExtVeto_calib():m_isFirstCallToBrun(1),m_ROOTOutput(0)
{

}

//------------------
// ~ExtVeto_SigDisplay (Destructor)
//------------------
ExtVeto_calib::~ExtVeto_calib()
{

}
//------------------
// init
//------------------
jerror_t ExtVeto_calib::init(void)
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
//	h=new TH1D("h","h",100,-0.5,99.5);
	t=new TTree("tout","tout");

	t->Branch("component",&component);
    t->Branch("Q",&Q);
    t->Branch("ped",&ped);

    t->Branch("T",&T);
	t->Branch("eventN",&eventN);
	t->Branch("Tw",&Tw);


    //  t->Branch("h_wave",&h_wave);

	app->RootUnLock();
	return NOERROR;
}

//------------------
// brun
//------------------
jerror_t ExtVeto_calib::brun(JEventLoop *loop, int32_t runnumber)
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
//			m_ROOTOutput->AddObject(h);
			m_ROOTOutput->AddObject(t);
		}

	}

	return NOERROR;


}

//------------------
// evnt
//------------------
jerror_t ExtVeto_calib::evnt(JEventLoop *loop,uint64_t eventnumber)
{
	// This is called for every event. Use of common resources like writing
	// to a file or filling a histogram should be mutex protected. Using
	// loop->Get(...) to get reconstructed objects (and thereby activating the
	// reconstruction algorithm) should be done outside of any mutex lock
	// since multiple threads may call this method at the same time.
	// Here's an example:
	//
	vector<const ExtVetoPMTHit*> data;


	vector<const ExtVetoPMTHit*>::const_iterator data_it;
	const fa250Mode1CalibHit *fa;
	loop->Get(data);

	const eventData* tData;
	//has to be in a try-catch block, since if no trigger data is there (prestart - start - end events) trows it!
	try{
		loop->GetSingle(tData);
	}
	catch(unsigned long e){
		jout<<"No trig bank this event"<<endl;
		return 	OBJECT_NOT_AVAILABLE;
	}

	int tWord=tData->triggerWords.at(0);
//	jout<<eventnumber<<" tWord= "<<tWord<<endl;

	int isMPPC=0;
	for (int ii=0;ii<4;ii++){
		if ((((tWord)>>ii)&0x1)&&(ii==2)) isMPPC=1;
	}
//	if (!isMPPC) return OBJECT_NOT_AVAILABLE;

//	jout<<"****************************************************************"<<endl;
//	jout<<"Evt number="<< eventnumber<<" tWord= "<<tWord<<endl;
	japp->RootWriteLock();
	//  ... fill historgrams or trees ...

	for (data_it=data.begin();data_it<data.end();data_it++){



		const ExtVetoPMTHit *evhit = *data_it;


			// Get associated fa250Mode1CalibHit object
			fa = NULL;
			evhit->GetSingle(fa);


			if(!fa) continue; // need fa250Mode1CalibHit to continue


			eventN=eventnumber;
			component=evhit->m_channel.ext_veto.component;
	   	    Q=(*data_it)->Q;
	   	 T=(*data_it)->T;
	   	 ped=(*data_it)->pedestal;
			Tw=tData->triggerWords.at(0);



			t->Fill();


}

	japp->RootUnLock();

//	downthres:


	return NOERROR;
}

//------------------
// erun
//------------------
jerror_t ExtVeto_calib::erun(void)
{
	// This is called whenever the run number changes, before it is
	// changed to give you a chance to clean up before processing
	// events from the next run number.
	return NOERROR;
}

//------------------
// fini
//------------------
jerror_t ExtVeto_calib::fini(void)
{

	return NOERROR;
}





