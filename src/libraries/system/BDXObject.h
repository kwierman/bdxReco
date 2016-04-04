/*
 * BDXObject.h
 *
 *  Created on: Apr 3, 2016
 *      Author: celentan
 */

#ifndef SRC_LIBRARIES_SYSTEM_BDXOBJECT_H_
#define SRC_LIBRARIES_SYSTEM_BDXOBJECT_H_

#include <JANA/JObject.h>
#include <JANA/JFactory.h>

using namespace jana;
using namespace std;

class TCanvas;

class BDXObject : public jana::JObject{
public:
	BDXObject();
	virtual ~BDXObject();
	virtual	TCanvas* Draw(){return m_canvas;}

protected:
	TCanvas *m_canvas;


};

#endif /* SRC_LIBRARIES_SYSTEM_BDXOBJECT_H_ */
