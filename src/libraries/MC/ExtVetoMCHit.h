// $Id$
//
//    File: genParticle.h
// Created: Tue Mar  1 00:59:14 CET 2016
// Creator: celentan (on Linux localhost.localdomain 2.6.32-573.12.1.el6.x86_64 x86_64)
//

#ifndef _JExtVetoMCHit_
#define _JExtVetoMCHit_

#include <JANA/JObject.h>
#include <JANA/JFactory.h>
#include <MC/VetoMCHit.h>

/*This class is basically a place-holder for the data coming from EVIO file,
 * for the raw and digitized banks
 */
class ExtVetoMCHit:public VetoMCHit{
	public:
		JOBJECT_PUBLIC(ExtVetoMCHit);

		// Add data members here. For example:
		// int id;
		// double E;

		// This method is used primarily for pretty printing
		// the second argument to AddString is printf style format
		void toStrings(vector<pair<string,string> > &items)const{
			 AddString(items, "id", "%4d", id);
		}

		//dgtz banks
		int adc,tdc;

		//raw banks
		double totEdep;
};

#endif

