// $Id$
//
//    File: triggerData.h
// Created: Thu Jan 28 18:19:51 CET 2016
// Creator: celentan (on Linux apcx4 2.6.32-504.30.3.el6.x86_64 x86_64)
//

#ifndef _triggerData_
#define _triggerData_

#include <JANA/JObject.h>
#include <JANA/JFactory.h>

//#include <vector>
class triggerData:public jana::JObject{
	public:
		JOBJECT_PUBLIC(triggerData);
		
		std::vector<uint32_t> triggerWords;
		
		// This method is used primarily for pretty printing
		// the second argument to AddString is printf style format
		void toStrings(vector<pair<string,string> > &items)const{
			// AddString(items, "id", "%4d", id);
			// AddString(items, "E", "%f", E);
		}
		
};

#endif // _triggerData_

