// C++ headers
#include <string>
using namespace std;

#include <JANA/JParameterManager.h>

// bdx headers
#include <system/JEventSourceEVIOGenerator.h>

#include <DAQ/JEventSourceEVIODAQ.h>
#include <MC/JEventSourceEVIOMC.h>

JEventSourceEvioGenerator::JEventSourceEvioGenerator() : isMC(0) {
	gPARMS->SetDefaultParameter("MC", isMC,"Flag to select if analysis is runned on a MC file");
}

// Describes the file format
const char* JEventSourceEvioGenerator::Description(void)
{
	return "Evio";
}

// Check if it's a valid file
double JEventSourceEvioGenerator::CheckOpenable(string source)
{
	/// This always returns 100% probability of 
	/// opening the source
	return 1.0;
}

// Make the file a JEventSource
JEventSource* JEventSourceEvioGenerator::MakeJEventSource(string source)
{
	/*Here is the trick for now. Since we do not have the same file format for MC and for Data,
	 * but both are evio, let's look at the option MC, and see if it is 0 or 1
	 */
	int m_isMC,m_verbose;
	m_isMC=0;
	m_verbose=0;
	gPARMS->GetParameter("MC",m_isMC);
	gPARMS->GetParameter("SYSTEM:VERBOSE",m_verbose);

	if (m_isMC==0){
		if (m_verbose>2) jout<<"JEventSourceEvioGenerator::MakeJEventSource for DAQ "<<endl;
		return new JEventSourceEvioDAQ(source.c_str());
	}
	else if (m_isMC==1){
		if (m_verbose>2) jout<<" JEventSourceEvioGenerator::MakeJEventSource for MC "<<endl;
		return new JEventSourceEvioMC(source.c_str());
	}

}


