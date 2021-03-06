// $Id$
//
//    File: fa250Mode1PedSubHit_factory.cc
// Created: Wed Feb  3 07:38:15 EST 2016
// Creator: davidl (on Darwin harriet.jlab.org 13.4.0 i386)
//


#include <iostream>
#include <iomanip>
using namespace std;

#include "fa250Mode1CalibHit.h"
#include "fa250Mode1CalibPedSubHit_factory.h"
#include <TT/TranslationTable.h>
using namespace jana;

//------------------
// init
//------------------
jerror_t fa250Mode1CalibPedSubHit_factory::init(void)
{
	m_pedestals=new DAQCalibrationHandler("/DAQ/pedestals");
	this->mapCalibrationHandler(m_pedestals);
	return NOERROR;
}

//------------------
// brun
//------------------
jerror_t fa250Mode1CalibPedSubHit_factory::brun(jana::JEventLoop *eventLoop, int32_t runnumber)
{
	// Here, we would normally get this from the CalibPedSubration DB.
	// For now, we hard code it...
	LSB=0.4884;


	this->updateCalibrationHandler(m_pedestals,eventLoop);



	return NOERROR;
}

//------------------
// evnt
//------------------
jerror_t fa250Mode1CalibPedSubHit_factory::evnt(JEventLoop *loop, uint64_t eventnumber)
{
	vector<const fa250Mode1CalibHit*> hits;
	vector<double> DAQdata;
	double pedestal,RMS;
	double sample=0;
	TranslationTable::csc_t index;
	loop->Get(hits);



	for(uint32_t i=0; i<hits.size(); i++){

		const fa250Mode1CalibHit *hit = hits[i];

		// Create new fa250Mode1PedSubHit
		fa250Mode1CalibPedSubHit *CalibPedSubHit = new fa250Mode1CalibPedSubHit;

		// Copy the fa250Hit part (crate, slot, channel, ...)
		// doing it this way allow one to modify fa250 later and
		// not have to change this code.
		fa250Hit *a = CalibPedSubHit;
		const fa250Hit *b = hit;
		*a = *b;

		// Copy all samples, applying PedSubration constant as we go
		DAQdata=m_pedestals->getCalib(hit->m_channel);
		pedestal=DAQdata[0];
		RMS=DAQdata[1];
		pedestal*=LSB;
		RMS*=LSB;
		for(uint32_t j=0; j<hit->samples.size(); j++){
			sample = (double)hit->samples[j];
			sample = sample - pedestal;
			CalibPedSubHit->samples.push_back(sample);
		}
		CalibPedSubHit->m_ped=pedestal;
		CalibPedSubHit->m_RMS=RMS;
		// Add original as associated object 
		CalibPedSubHit->AddAssociatedObject(hit);
		_data.push_back(CalibPedSubHit);
	}

	return NOERROR;
}

//------------------
// erun
//------------------
jerror_t fa250Mode1CalibPedSubHit_factory::erun(void)
{

	this->clearCalibrationHandler(m_pedestals);



	return NOERROR;
}

//------------------
// fini
//------------------
jerror_t fa250Mode1CalibPedSubHit_factory::fini(void)
{
	if (m_pedestals) delete m_pedestals;
	return NOERROR;
}

