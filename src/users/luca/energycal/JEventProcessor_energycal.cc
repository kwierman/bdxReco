// $Id$
//
//    File: JEventProcessor_energycal.cc
// Created: Fri Mar  4 10:46:51 CET 2016
// Creator: lmarsicano (on Linux apcx14 2.6.32-573.el6.x86_64 x86_64)
//

#include "JEventProcessor_energycal.h"
#include "system/BDXEventProcessor.h"
using namespace jana;

#include <Calorimeter/CalorimeterSiPMHit.h>
#include <Calorimeter/CalorimeterDigiHit.h>
#include <Calorimeter/CalorimeterHit.h>

#include <Paddles/PaddlesDigiHit.h>
#include <Paddles/PaddlesHit.h>

#include <MC/CalorimeterMCHit.h>

// Routine used to create our JEventProcessor
#include <JANA/JApplication.h>
#include <JANA/JFactory.h>

#include <system/JROOTOutput.h>

#include "TH1D.h"
#include "TH2D.h"
#include "TTree.h"
#include "TF1.h"

extern "C"{
void InitPlugin(JApplication *app){
	InitJANAPlugin(app);
	app->AddProcessor(new JEventProcessor_energycal());
}
} // "C"


//------------------
// JEventProcessor_energycal (Constructor)
//------------------
JEventProcessor_energycal::JEventProcessor_energycal():m_ROOTOutput(0)
{
	isMC=0;

}

//------------------
// ~JEventProcessor_energycal (Destructor)
//------------------
JEventProcessor_energycal::~JEventProcessor_energycal()
{

}

//------------------
// init
//------------------
jerror_t JEventProcessor_energycal::init(void)
{
	// This is called once at program startup. If you are creating
	// and filling historgrams in this plugin, you should lock the
	// ROOT mutex like this:
	//
	// japp->RootWriteLock();
	//  ... fill historgrams or trees ...
	// japp->RootUnLock();
	//

	gPARMS->GetParameter("MC", isMC);

	app->RootWriteLock();

	jout<<"energycal::init is called"<<std::endl;
	hp1 =new TH1D("hp1","hp1",5000,-0.5,20.5);
	hp2 = new TH1D("hp2","hp2",5000,-0.5,20.5);
	hp1_2 = new TH2D("hp1_2","hp1_2",5000,-0.5,20.5,5000,-0.5,20.5);
	hc1 = new TH1D("hc1","hc1",5000,0,750);
	hc2 = new TH1D("hc2","hc2",5000,0,750);
	hctot = new TH1D("hctot","hctot",5000,0,1500);
	hc1_2 = new TH2D("hc1_2","hc1_2",5000,0,750,5000,0,750);

	t=new TTree("energycal","energycal");
	t->Branch("eventN",&eventN);
	t->Branch("Qc1",&Qc1);
	t->Branch("Qc2",&Qc2);
	t->Branch("Qctot",&Qctot);
	t->Branch("Tc1",&Tc1);
	t->Branch("Tc2",&Tc2);
	t->Branch("Ep1",&Ep1);
	t->Branch("Ep2",&Ep2);
	t->Branch("Qp1",&Qp1);
	t->Branch("Qp2",&Qp2);
	t->Branch("Tp1",&Tp1);
	t->Branch("Tp2",&Tp2);
	t->Branch("Tpdiff",&Tpdiff);

	//Create always MC branch
	t->Branch("Ec_MC",&Ec_MC);

	app->RootUnLock();
	return NOERROR;
}

//------------------
// brun
//------------------
jerror_t JEventProcessor_energycal::brun(JEventLoop *eventLoop, int32_t runnumber)
{
	// This is called whenever the run number changes
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
			m_ROOTOutput->AddObject(hp2);
			m_ROOTOutput->AddObject(hp1);
			m_ROOTOutput->AddObject(hp1_2);
			m_ROOTOutput->AddObject(hc1);
			m_ROOTOutput->AddObject(hc2);
			m_ROOTOutput->AddObject(hc1_2);
			m_ROOTOutput->AddObject(hctot);
			m_ROOTOutput->AddObject(t);
		}

	}









	return NOERROR;
}

//------------------
// evnt
//------------------
jerror_t JEventProcessor_energycal::evnt(JEventLoop *loop, uint64_t eventnumber)
{
	// This is called for every event. Use of common resources like writing
	// to a file or filling a histogram should be mutex protected. Using
	// loop->Get(...) to get reconstructed objects (and thereby activating the
	// reconstruction algorithm) should be done outside of any mutex lock
	// since multiple threads may call this method at the same time.
	// Here's an example:
	//
	// vector<const MyDataClass*> mydataclasses;
	// loop->Get(mydataclasses);
	//
	// japp->RootWriteLock();
	//  ... fill historgrams or trees ...
	// japp->RootUnLock();

	vector<const PaddlesHit*> data;
	vector<const PaddlesHit*>::const_iterator data_it;
	loop->Get(data);


	vector<const CalorimeterHit*> cdata;
	vector<const CalorimeterHit*>::const_iterator cdata_it;
	loop->Get(cdata);


	vector<const CalorimeterMCHit*> mc_data;

	app->RootWriteLock();
	double E,T;
	Qc1=-1;
	Qc2=-1;
	Tp1=-1;
	Tp2=-1;
	Ep1=-1;
	Ep2=-1;


	for (data_it=data.begin();data_it<data.end();data_it++){

		const PaddlesHit *evhit = *data_it;
		vector<const PaddlesDigiHit*> digiphits;
		evhit->Get(digiphits);
		//jout << evhit->m_channel.id<<endl;
		E = evhit->E;
		T = evhit->T;
		if(evhit->m_channel.id==0){
			Ep1=E;
			Tp1=T;
			Qp1=digiphits[0]->Q;
		}
		if(evhit->m_channel.id==1){
			Ep2=E;
			Tp2=T;
			Qp2=digiphits[0]->Q;

		}

	}

	for (cdata_it=cdata.begin();cdata_it<cdata.end();cdata_it++){
		const CalorimeterHit *evchit= *cdata_it;
		Qc1=0;
		Qc2=0;
		for (int idata=0;idata<evchit->m_data.size();idata++){
			switch (evchit->m_data[idata].readout){
			case (1):
								Qc1 = evchit->m_data[idata].Q;
								Tc1 = evchit->m_data[idata].T;
			break;
			case (2):
								Qc2 = evchit->m_data[idata].Q;
								Tc2 = evchit->m_data[idata].T;
			break;
			default:
				break;

			}
		}

		Ec_MC=0;
		if (isMC){
				evchit->Get(mc_data); //use a vector since it is re-iterating!
				for (int imc=0;imc<mc_data.size();imc++){
					Ec_MC+=mc_data[imc]->totEdep;
				}
			}
			else{
				Ec_MC=-1;
			}
	}




	Qctot = Qc1+Qc2;
	Tpdiff= Tp1-Tp2;

	if (Ep1!=-1)	hp1->Fill(Ep1);
	if (Ep2!=-1)    hp2->Fill(Ep2);
	if (Ep2!=-1 && Ep1!=-1)    hp1_2->Fill(Ep1,Ep2);

	if (Qc1!=-1)  hc1->Fill(Qc1);
	if (Qc2!=-1)  hc2->Fill(Qc2);
	if (Qc1!=-1 && Qc2!=-1) {
		hctot->Fill(Qctot);
		hc1_2->Fill(Qc1,Qc2);
	}
	eventN=eventnumber;
	if((Ep1 > 0.5) && (Ep2 > 0.5)) t->Fill();

	app->RootUnLock();



	return NOERROR;
}

//------------------
// erun
//------------------
jerror_t JEventProcessor_energycal::erun(void)
{
	// This is called whenever the run number changes, before it is
	// changed to give you a chance to clean up before processing
	// events from the next run number.
	return NOERROR;
}

//------------------
// fini
//------------------
jerror_t JEventProcessor_energycal::fini(void)
{
	// Called before program exit after event processing is finished.
	return NOERROR;
}

