// $Id$
//
//    File: CalorimeterDigiHit.h
// Created: Wed Jan 27 16:39:57 CET 2016
// Creator: celentan (on Linux apcx4 2.6.32-504.30.3.el6.x86_64 x86_64)
//

#ifndef _CalorimeterDigiHit_
#define _CalorimeterDigiHit_

#include <JANA/JObject.h>
#include <JANA/JFactory.h>
#include <TT/TranslationTable.h>
#include <Calorimeter/CalorimeterSiPMHit.h>

class CalorimeterDigiHit:public jana::JObject{
public:
	struct CalorimeterSiPMDigiHit{
		int readout;
		double Q;
		double Qs;
		double T;
		bool good_ped_RMS;
		CalorimeterSiPMHit::hit_type type;
	};
	JOBJECT_PUBLIC(CalorimeterDigiHit);

	// Add data members here. For example:
	// int id;
	// double E;

	// This method is used primarily for pretty printing
	// the second argument to AddString is printf style format
	void toStrings(vector<pair<string,string> > &items)const{
		// AddString(items, "id", "%4d", id);
		// AddString(items, "E", "%f", E);
	}
	TranslationTable::CALO_Index_t m_channel; //Detector-specific ID. Since this is a detector-based object, the readout field will be ==0
	vector <CalorimeterSiPMDigiHit> m_data; //keep in this way to be MC-compatible!

	uint64_t timestamp;
};

#endif // _CalorimeterDigiHit_

