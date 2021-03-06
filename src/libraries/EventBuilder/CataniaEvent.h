// $Id$
//
//    File: CataniaEvent.h
// Created: Mon Apr 25 23:25:44 CEST 2016
// Creator: celentan (on Linux dhcp-visitor-enr-116-196.slac.stanford.edu 2.6.32-573.12.1.el6.x86_64 x86_64)
//

#ifndef _CataniaEvent_
#define _CataniaEvent_

#include <JANA/JObject.h>
#include <JANA/JFactory.h>
#include "TObject.h"

#include <TT/TranslationTable.h>

class CataniaEvent:public jana::JObject,public TObject{
	public:
		JOBJECT_PUBLIC(CataniaEvent);
		
		// Add data members here. For example:
		// int id;
		// double E;
		
		// This method is used primarily for pretty printing
		// the second argument to AddString is printf style format
		void toStrings(vector<pair<string,string> > &items)const{
			AddString(items,"runN","%i",runN);
			AddString(items,"eventN","%i",eventN);
			AddString(items,"time","%i",time);
			AddString(items,"timestamp","%lld",(long long)timestamp);
			AddString(items,"tWord","%i",tWord);
			AddString(items,"RMSflag","%d",1*flag_RMS);
			AddString(items,"RMSflag1","%d",1*flag_RMS1);
			AddString(items,"RMSflag2","%d",1*flag_RMS2);
			AddString(items,"Ec1","%f",Ec1);
			AddString(items,"Ec2","%f",Ec2);
			AddString(items,"Ec3","%f",Ec3);
		}

		double Qc1,Qc2,Qc3,Qcs1,Qcs2,Qcs3;
		double Ec1,Ec2,Ec3;


		double E1,T1,E2,T2,E,T;
		bool flag_RMS,flag_RMS1,flag_RMS2;


		int nExtVetoHits;
		int nExtVetoHitsCoincidence;
		vector <TranslationTable::EXT_VETO_Index_t> vExtVetoHits;
		vector <bool> vExtVetoHitsIsInCoincidence;
		vector <double>	vExtVetoHitsT;
		vector <double>	vExtVetoHitsE;


		int nIntVetoHits;
		int nIntVetoHitsCoincidence;
		vector <TranslationTable::INT_VETO_Index_t> vIntVetoHits;
		vector <bool> vIntVetoHitsIsInCoincidence;
		vector <double>	vIntVetoHitsT;
		vector <double>	vIntVetoHitsQ;


		double Ep1,Ep2;

		int time; //in s
		int tWord;

		int eventN,runN;

		uint64_t timestamp;


#if (!defined(__APPLE__))
		ClassDef(CataniaEvent,1);
#endif
};

#endif // _CataniaEvent_

