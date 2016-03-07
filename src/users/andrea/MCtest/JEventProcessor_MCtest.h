// $Id$
//
//    File: JEventProcessor_MCtest.h
// Created: Mon Mar  7 11:23:10 CET 2016
// Creator: celentan (on Linux apcx4 2.6.32-504.30.3.el6.x86_64 x86_64)
//

#ifndef _JEventProcessor_MCtest_
#define _JEventProcessor_MCtest_

#include <JANA/JEventProcessor.h>

class JEventProcessor_MCtest:public jana::JEventProcessor{
	public:
		JEventProcessor_MCtest();
		~JEventProcessor_MCtest();
		const char* className(void){return "JEventProcessor_MCtest";}

	private:
		jerror_t init(void);						///< Called once at program start.
		jerror_t brun(jana::JEventLoop *eventLoop, int32_t runnumber);	///< Called everytime a new run number is detected.
		jerror_t evnt(jana::JEventLoop *eventLoop, uint64_t eventnumber);	///< Called every event.
		jerror_t erun(void);						///< Called everytime run number changes, provided brun has been called.
		jerror_t fini(void);						///< Called after last event of last event source has been processed.
};

#endif // _JEventProcessor_MCtest_

