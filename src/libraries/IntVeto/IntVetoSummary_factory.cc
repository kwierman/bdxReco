// $Id$
//
//    File: IntVetoSummary_factory.cc
// Created: Tue Mar 29 15:07:39 CEST 2016
// Creator: celentan (on Linux apcx4 2.6.32-504.30.3.el6.x86_64 x86_64)
//


#include <iostream>
#include <iomanip>
using namespace std;

#include "IntVetoHit.h"
#include "IntVetoSummary_factory.h"
#include <TT/TranslationTable.h>
using namespace jana;

//------------------
// init
//------------------
jerror_t IntVetoSummary_factory::init(void)
{
	m_q_thr[0]=10;
	m_q_thr[1]=10;
	m_q_thr[2]=10;
	m_q_thr[3]=10;
	m_q_thr[4]=10;
	m_q_thr[5]=10;


	return NOERROR;
}

//------------------
// brun
//------------------
jerror_t IntVetoSummary_factory::brun(jana::JEventLoop *eventLoop, int32_t runnumber)
{
	return NOERROR;
}

//------------------
// evnt
//------------------
jerror_t IntVetoSummary_factory::evnt(JEventLoop *loop, uint64_t eventnumber)
{

	vector<const IntVetoHit*> m_intVetoHits;
	vector<const IntVetoHit*>::const_iterator it;
	const IntVetoHit* m_intVetoHit;
	IntVetoSummary *m_intVetoSummary;

	int sector;
	int component;
	int layer;

	loop->Get(m_intVetoHits);

	m_map.clear();
	/*We need to handle in a different way hits corresponding to different sectors*/
	for (it=m_intVetoHits.begin();it!=m_intVetoHits.end();it++){
		m_intVetoHit=(*it);
		sector=m_intVetoHit->m_channel.sector;
		layer=m_intVetoHit->m_channel.layer;
		component=m_intVetoHit->m_channel.component;

		/*Check if this sector is already in the map*/
		if (m_map.find(sector)==m_map.end()){ //NO
			m_intVetoSummary=new IntVetoSummary(); //Create it
			m_intVetoSummary->sector=sector;  //associate with the sector
			m_intVetoSummary->nHits=0;        //nhits at 0
			m_map[sector]=m_intVetoSummary;   //add it to the map, so that other hits with the same sector will find it.
		}
		else{ //YES, the object is already there
			m_intVetoSummary=m_map[sector];
		}
		/*Ok, from here on we have the m_intVetoSummary associated with the sector of this hit (if it was not existing, we create id. Otherwise, we got it)
		 * Need to check if the hit is above a given thr / is on time. If so:
		 * -need to increment the nHits,
		 * -add the hit index to the hits vector,
		 * -associate the object via AddAssociatedObject
		 */

		/*Dummy example: need to implement better selection*/
		if (m_intVetoHit->Q>m_q_thr[component]){
			m_intVetoSummary->nHits++;     //increment number of hits
			m_intVetoSummary->hits.push_back(m_intVetoHit->m_channel);  //add this hit channel to the list of channels
			m_intVetoSummary->AddAssociatedObject(m_intVetoHit); //add the hit to the associated objects.
		}

	}

	/*Publish all the objects now - one per sector!*/
	for (m_it=m_map.begin();m_it!=m_map.end();m_it++){
		_data.push_back(m_it->second);
	}

	return NOERROR;
}

//------------------
// erun
//------------------
jerror_t IntVetoSummary_factory::erun(void)
{
	return NOERROR;
}

//------------------
// fini
//------------------
jerror_t IntVetoSummary_factory::fini(void)
{
	return NOERROR;
}

