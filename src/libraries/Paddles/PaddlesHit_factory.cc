// $Id$
//
//    File: ExtVetoHit_factory.cc
// Created: Wed Jan 13 21:06:44 CET 2016
// Creator: celentan (on Linux localhost.localdomain 2.6.32-504.30.3.el6.x86_64 x86_64)
//


#include <iostream>
#include <iomanip>
using namespace std;

#include <Paddles/PaddlesHit_factory.h>
#include <Paddles/PaddlesDigiHit.h>

#include <TT/TranslationTable.h>

using namespace jana;

//------------------
// init
//------------------
jerror_t PaddlesHit_factory::init(void)
{
	return NOERROR;
}

//------------------
// brun
//------------------
jerror_t PaddlesHit_factory::brun(jana::JEventLoop *eventLoop, int32_t runnumber)
{
	vector<vector < double> > m_enecalib;
	eventLoop->GetCalib("/Paddles/Ene", m_enecalib);
	m_ENE_gain.fillCalib(m_enecalib);
	return NOERROR;
}

//------------------
// evnt
//------------------
jerror_t PaddlesHit_factory::evnt(JEventLoop *loop, uint64_t eventnumber)
{

	// Code to generate factory data goes here. Add it like:
	//
	// ExtVetoHit *myExtVetoHit = new ExtVetoHit;
	// myExtVetoHit->x = x;
	// myExtVetoHit->y = y;
	// ...
	// _data.push_back(myExtVetoHit);
	//
	// Note that the objects you create here will be deleted later
	// by the system and the _data vector will be cleared automatically.

	vector <const PaddlesDigiHit *> m_data;
	vector <const PaddlesDigiHit *>::const_iterator m_it;


	loop->Get(m_data);

	vector<double> m_Ene;

	/*Create here the Hit from the Digi hit*/
	PaddlesHit *m_PaddlesHit=0;

	for (m_it=m_data.begin();m_it!=m_data.end();m_it++){
			m_PaddlesHit=new PaddlesHit;

			/*For now, very dummy!*/
			m_PaddlesHit->m_channel=(*m_it)->m_channel;

			m_ENE_gain.getCalib(m_PaddlesHit->m_channel,m_Ene);
			m_PaddlesHit->E=((*m_it)->Q)*m_Ene.at(0);
			m_PaddlesHit->T=(*m_it)->T;


			_data.push_back(m_PaddlesHit);
		}



	return NOERROR;
}

//------------------
// erun
//------------------
jerror_t PaddlesHit_factory::erun(void)
{
	return NOERROR;
}

//------------------
// fini
//------------------
jerror_t PaddlesHit_factory::fini(void)
{
	return NOERROR;
}

