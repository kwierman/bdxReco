// $Id$
//
//    File: JEventProcessor_intVeto_digi.cc
// Created: Tue Feb  9 16:51:03 CET 2016
// Creator: celentan (on Linux apcx4 2.6.32-504.30.3.el6.x86_64 x86_64)
//

#include "JEventProcessor_IntVeto_digi.h"
using namespace jana;

// Routine used to create our JEventProcessor
#include <JANA/JApplication.h>
#include <JANA/JFactory.h>



#include "JEventProcessor_test.h"
#include <system/BDXEventProcessor.h>
#include <system/JROOTOutput.h>

#include <DAQ/fa250Mode1Hit.h>
#include <DAQ/eventData.h>

#include <TT/TranslationTable.h>


#include <IntVeto/IntVetoDigiHit.h>
#include <IntVeto/IntVetoSiPMHit.h>

#include <ExtVeto/ExtVetoPMTHit.h>
#include <ExtVeto/ExtVetoDigiHit.h>

#include <Calorimeter/CalorimeterHit.h>
#include <Paddles/PaddlesHit.h>

#include <MC/IntVetoMCHit.h>

#include "TH1D.h"
#include "TH2D.h"
#include "TTree.h"





extern "C"{
void InitPlugin(JApplication *app){
	InitJANAPlugin(app);
	app->AddProcessor(new JEventProcessor_IntVeto_digi());
}
} // "C"


//------------------
// JEventProcessor_intVeto_digi (Constructor)
//------------------
JEventProcessor_IntVeto_digi::JEventProcessor_IntVeto_digi():
								m_ROOTOutput(0)
{
	m_isMC=0;
}

//------------------
// ~JEventProcessor_intVeto_digi (Destructor)
//------------------
JEventProcessor_IntVeto_digi::~JEventProcessor_IntVeto_digi()
{

}

//------------------
// init
//------------------
jerror_t JEventProcessor_IntVeto_digi::init(void)
{

	//
	japp->RootWriteLock();


	gPARMS->GetParameter("MC", m_isMC);

	t=new TTree("IntVeto_digi","IntVeto_digi");

	t->Branch("sector",&m_sector);
	t->Branch("layer",&m_layer);
	t->Branch("component",&m_component);
	t->Branch("readout",&m_readout);

	t->Branch("Q0",&Q0);
	t->Branch("Q0_1",&Q0_1);
	t->Branch("Q0_2",&Q0_2);
	t->Branch("Q0_3",&Q0_3);
	t->Branch("Q0_4",&Q0_4);

	t->Branch("Q1",&Q1);
	t->Branch("Q1_1",&Q1_1);
	t->Branch("Q1_2",&Q1_2);
	t->Branch("Q1_3",&Q1_3);
	t->Branch("Q1_4",&Q1_4);

	t->Branch("Q2",&Q2);
	t->Branch("Q2_1",&Q2_1);
	t->Branch("Q2_2",&Q2_2);
	t->Branch("Q2_3",&Q2_3);
	t->Branch("Q2_4",&Q2_4);

	t->Branch("Q3",&Q3);
	t->Branch("Q3_1",&Q3_1);
	t->Branch("Q3_2",&Q3_2);
	t->Branch("Q3_3",&Q3_3);
	t->Branch("Q3_4",&Q3_4);

	t->Branch("Q4",&Q4);
	t->Branch("Q5",&Q5);

	t->Branch("Qc1",&Qc1);
	t->Branch("Qc2",&Qc2);
	t->Branch("Ep1",&Ep1);
	t->Branch("Ep2",&Ep2);

	t->Branch("Emc",&Emc);

	t->Branch("tWord",&tWord);


	t->Branch("eventN",&eventNumber);
	japp->RootUnLock();


	return NOERROR;
}

//------------------
// brun
//------------------
jerror_t JEventProcessor_IntVeto_digi::brun(JEventLoop *eventLoop, int32_t runnumber)
{

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
			m_ROOTOutput->AddObject(t);
		}
	}
	return NOERROR;
}

//------------------
// evnt
//------------------
jerror_t JEventProcessor_IntVeto_digi::evnt(JEventLoop *loop, uint64_t eventnumber)
{
	vector<const IntVetoDigiHit*> data;
	vector<const IntVetoDigiHit*>::const_iterator data_it;

	vector<const CalorimeterHit*> cdata;
	vector<const CalorimeterHit*>::const_iterator cdata_it;

	vector<const PaddlesHit*> pdata;
	vector<const PaddlesHit*>::const_iterator pdata_it;


	vector<const IntVetoMCHit*> mcdata;

	const eventData* tData;
	if (m_isMC==0){
		try{
			loop->GetSingle(tData);
		}
		catch(unsigned long e){
			jout<<"No trig bank this event"<<endl;
			return 	OBJECT_NOT_AVAILABLE;
		}
	}

	if (m_isMC==1){
		loop->Get(data,"MC");
	}
	else{
		loop->Get(data);
	}
	loop->Get(cdata);
	loop->Get(pdata);

	japp->RootWriteLock();
	if (m_isMC==0){
		tWord=tData->triggerWords.at(0);
	}
	Q0_1=-1;
	Q0_2=-1;
	Q0_3=-1;
	Q0_4=-1;
	Q0=-1;

	Q1_1=-1;
	Q1_2=-1;
	Q1_3=-1;
	Q1_4=-1;
	Q1=-1;

	Q2_1=-1;
	Q2_2=-1;
	Q2_3=-1;
	Q2_4=-1;
	Q2=-1;

	Q3_1=-1;
	Q3_2=-1;
	Q3_3=-1;
	Q3_4=-1;
	Q3=-1;

	Q4=-1;
	Q5=-1;

	for (data_it=data.begin();data_it<data.end();data_it++){
		m_sector=(*data_it)->m_channel.sector;
		m_layer=(*data_it)->m_channel.layer;
		m_component=(*data_it)->m_channel.component;
		m_readout=(*data_it)->m_channel.readout;

		//	T=(*data_it)->T;
		//	Q=(*data_it)->Q;
		eventNumber=eventnumber;
		//(*data_it)->Get(associated_data,"",1);
		switch(m_component){
		case (0):
		Q0_1=(*data_it)->m_data[0].Q;
		Q0_2=(*data_it)->m_data[1].Q;
		Q0_3=(*data_it)->m_data[2].Q;
		Q0_4=(*data_it)->m_data[3].Q;
		Q0=Q0_1+Q0_2+Q0_3+Q0_4;
		if (m_isMC){
			Emc=0;
			(*data_it)->Get(mcdata);
			for (int imc=0;imc<mcdata.size();imc++){
				Emc+=mcdata[imc]->totEdep;
			}
		}
		break;
		case (1):
													Q1_1=(*data_it)->m_data[0].Q;
		Q1_2=(*data_it)->m_data[1].Q;
		Q1_3=(*data_it)->m_data[2].Q;
		Q1_4=(*data_it)->m_data[3].Q;
		Q1=Q1_1+Q1_2+Q1_3+Q1_4;
		break;
		case (2):
													Q2_1=(*data_it)->m_data[0].Q;
		Q2_2=(*data_it)->m_data[1].Q;
		Q2_3=(*data_it)->m_data[2].Q;
		Q2_4=(*data_it)->m_data[3].Q;
		Q2=Q2_1+Q2_2+Q2_3+Q2_4;
		break;
		case (3):
													Q3_1=(*data_it)->m_data[0].Q;
		Q3_2=(*data_it)->m_data[1].Q;
		Q3_3=(*data_it)->m_data[2].Q;
		Q3_4=(*data_it)->m_data[3].Q;
		Q3=Q3_1+Q3_2+Q3_3+Q3_4;
		break;
		case (4):
													Q4=(*data_it)->m_data[0].Q;
		break;
		case (5):
													Q5=(*data_it)->m_data[0].Q;
		break;
		}

	}



	/*Calorimeter*/
	Qc1=-1;
	Qc2=-1;
	for (cdata_it=cdata.begin();cdata_it!=cdata.end();cdata_it++){
		const CalorimeterHit *evchit= *cdata_it;

		for (int idata=0;idata<evchit->m_data.size();idata++){
			switch (evchit->m_data[idata].readout){
			case (1):
									Qc1 = evchit->m_data[idata].Q;
			break;
			case (2):
									Qc2 = evchit->m_data[idata].Q;
			break;
			default:
				break;

			}
		}
	}

	/*Paddles*/
	Ep1=-1;Ep2=-1;
	for (pdata_it=pdata.begin();pdata_it!=pdata.end();pdata_it++){

		const PaddlesHit *evhit = *pdata_it;
		//jout << evhit->m_channel.id<<endl;

		if(evhit->m_channel.id==0){
			Ep1=evhit->E;

		}
		else if(evhit->m_channel.id==1){
			Ep2=evhit->E;
		}
	}


	t->Fill();
	japp->RootUnLock();

	return NOERROR;
}

//------------------
// erun
//------------------
jerror_t JEventProcessor_IntVeto_digi::erun(void)
{
	// This is called whenever the run number changes, before it is
	// changed to give you a chance to clean up before processing
	// events from the next run number.
	return NOERROR;
}

//------------------
// fini
//------------------
jerror_t JEventProcessor_IntVeto_digi::fini(void)
{
	// Called before program exit after event processing is finished.
	return NOERROR;
}

