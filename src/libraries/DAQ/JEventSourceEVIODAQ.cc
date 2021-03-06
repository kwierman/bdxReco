// JANA headers
#include <JANA/JEvent.h>
#include <JANA/JParameterManager.h>
#include <system/JEventSourceEVIOGenerator.h>
#include "JEventSourceEVIODAQ.h"
// bdx headers
#include "CLHEP/Units/PhysicalConstants.h"
using namespace CLHEP;

// C++ headers
#include <iostream>
using namespace std;

//DAQ
#include <DAQ/fa250Mode1Hit.h>
#include <DAQ/fa250Mode7Hit.h>
#include <DAQ/eventData.h>


// Constructor
JEventSourceEvioDAQ::JEventSourceEvioDAQ(const char* source_name):JEventSource(source_name),
		chan(0),EDT(0),vme_mother_tag(0),child_mode1_tag(0),child_mode7_tag(0),eventHeader_tag(0),
		curRunNumber(0),curEventNumber(0)
{




	vme_mother_tag=0x1;
	child_mode1_tag=0xe101;
	child_mode7_tag=0xe102;
	child_trigger_tag=0xe118;
	eventHeader_tag=0xE10F;
	eventHeader_CODA_tag=0xC000;
	prestart_tag=0x11;  //decimal 17

	overwriteRunNumber=-1;


	gPARMS->SetDefaultParameter("DAQ:VME_MOTHER_TAG",vme_mother_tag);
	gPARMS->SetDefaultParameter("DAQ:CHILD_MODE1_TAG",child_mode1_tag);
	gPARMS->SetDefaultParameter("DAQ:CHILD_MODE7_TAG",child_mode7_tag);
	gPARMS->SetDefaultParameter("DAQ:CHILD_TRIGGER_TAG",child_trigger_tag);
	gPARMS->SetDefaultParameter("DAQ:EVENTHEADER_TAG",eventHeader_tag);
	gPARMS->SetDefaultParameter("DAQ:PRESTART_TAG",prestart_tag);
	gPARMS->SetDefaultParameter("DAQ:RUN_NUMBER",overwriteRunNumber);

	// open EVIO file - buffer is hardcoded at 3M... that right?
	jout << " Opening input file " << source_name << "." << endl;

	try{
		//		chan = new evioFileChannel(source_name, "r", 300000);
		chan = new evioFileChannel(source_name, "r", 10804);
		chan->open();
	}
	catch(evioException *e){
		chan = 0;
		jerr<<e->toString()<<endl;
	}
}


// Destructor
JEventSourceEvioDAQ::~JEventSourceEvioDAQ()
{
	cout << " Closing input file " << source_name << "." << endl;
	chan->close();
	delete chan;
}

// GetEvent
jerror_t JEventSourceEvioDAQ::GetEvent(JEvent &event)
{

	event.SetRef(NULL);


	if(chan->read())
	{
		EDT=new evioDOMTree(chan);

		//	Mevent *this_evt = new Mevent(EDT, hitTypes, &banksMap, 0);

		event.SetJEventSource(this);
		event.SetRef(EDT);


		//This part is fine for real data @ catania
		evio::evioDOMNodeListP fullList     = EDT->getNodeList();
		evio::evioDOMNodeList::const_iterator iter;

		for(iter=fullList->begin(); iter!=fullList->end(); iter++) {
			if(((*iter)->tag==prestart_tag)&&(overwriteRunNumber==-1)){
				const evio::evioCompositeDOMLeafNode *leaf = static_cast<const evio::evioCompositeDOMLeafNode*>(*iter);
				//				int leafSize = leaf->getSize();
				vector<uint32_t> *pData = const_cast<vector<uint32_t> *>(&(leaf->data));
				curRunNumber=pData->at(1);
				jout<<"JEventSourceEvioDAQ::GetEvent new run number: "<<curRunNumber<<endl;
			}
			if((*iter)->tag==eventHeader_CODA_tag){ /*To be compatible also with data taken without header*/
				const evio::evioCompositeDOMLeafNode *leaf = static_cast<const evio::evioCompositeDOMLeafNode*>(*iter);
				//				int leafSize = leaf->getSize();
				vector<uint32_t> *pData = const_cast<vector<uint32_t> *>(&(leaf->data));
				event.SetEventNumber(pData->at(0));
			}
			else if((*iter)->tag==eventHeader_tag){
				const evio::evioCompositeDOMLeafNode *leaf = static_cast<const evio::evioCompositeDOMLeafNode*>(*iter);
				//				int leafSize = leaf->getSize();
				vector<uint32_t> *pData = const_cast<vector<uint32_t> *>(&(leaf->data));
				event.SetEventNumber(pData->at(2));
				curRunNumber=pData->at(1);
			}
		}
		if (overwriteRunNumber!=-1) event.SetRunNumber(overwriteRunNumber);
		else event.SetRunNumber(curRunNumber);
		return NOERROR;
	}
	else{
		jout<<"Source done"<<endl;
		return NO_MORE_EVENTS_IN_SOURCE;
	}
}

// FreeEvent
void JEventSourceEvioDAQ::FreeEvent(JEvent &event)
{
	delete (evioDOMTree*)event.GetRef();
	//	delete (Mevent*)event.GetRef();
}

// GetObjects
jerror_t JEventSourceEvioDAQ::GetObjects(JEvent &event, JFactory_base *factory)
{
	/// This gets called through the virtual method of the
	/// JEventSource base class. It creates the objects of the type
	/// which factory is based.

	/// Example: DCsegment needs DCHit. If DCHit doesn't exist already, then
	/// it will be read here.

	// We must have a factory to hold the data
	if(!factory)throw RESOURCE_UNAVAILABLE;

	// Get name of data class we're trying to extract
	string dataClassName = factory->GetDataClassName();

	//As suggested by David, do a check on the factory type to decide what to do
	JFactory<fa250Mode1Hit> *fac_fa250Mode1hit = dynamic_cast<JFactory<fa250Mode1Hit>*>(factory);
	JFactory<fa250Mode7Hit> *fac_fa250Mode7hit = dynamic_cast<JFactory<fa250Mode7Hit>*>(factory);
	JFactory<eventData> 	*fac_eventData = dynamic_cast<JFactory<eventData>*>(factory);

	if(fac_fa250Mode1hit != NULL){

		vector<fa250Mode1Hit*> data;
		evioDOMTree* local_EDT=(evioDOMTree*)event.GetRef();
		//	jout<<local_EDT->toString()<<endl;



		evio::evioDOMNodeListP fullList     = local_EDT->getNodeList();
		evio::evioDOMNodeList::const_iterator iter;
		evio::evioDOMNodeList::const_iterator branch;
		EvioCompositeDecoder decoder;



		for(iter=fullList->begin(); iter!=fullList->end(); iter++) {
			if((*iter)->tag==vme_mother_tag){

				evio::evioDOMNodeList *leafList = (*iter)->getChildList();

				for(branch=leafList->begin();branch!=leafList->end(); branch++){
					if( (*branch)->tag==child_mode1_tag){
						const evio::evioCompositeDOMLeafNode *leaf = static_cast<const evio::evioCompositeDOMLeafNode*>(*branch);
						int leafSize = leaf->getSize();
						vector<uint32_t> *pData = const_cast<vector<uint32_t> *>(&(leaf->data));
						decoder.decode(pData,leafSize);
						if(decoder.getData().size()>0){
							try{
								vector<CompositeADCRaw_t>  decdata = decoder.getData();
								for(int loop = 0; loop < decdata.size();loop++){
									fa250Mode1Hit *hit=new fa250Mode1Hit();
									/*Why I do not save directly the CompositeADCRaw_t and use it as the "Jobject in jana:\
									 * 1) CompositeADCRaw_t is just a simple structure to old data
									 * 2) CompositeADCRaw_t is just for evio-readout
									 * 3) fa250Mode1Hit is a more complex class, used within JANA
									 * */

									hit->m_channel.rocid=0;  ///TODO better
									hit->m_channel.slot=decdata[loop].slot;
									hit->m_channel.channel=decdata[loop].channel;
									for (int isample=0;isample<decdata[loop].samples.size();isample++){
										hit->samples.push_back(decdata[loop].samples.at(isample));
									}

									hit->trigger=decdata[loop].trigger;
									hit->timestamp=decdata[loop].time;
								//	jout<<hit->m_channel.rocid<<" "<<hit->m_channel.slot<<" "<<hit->m_channel.channel<<" "<<hit->samples.size()<<" "<<hit->trigger<<" "<<hit->timestamp<<endl;
									data.push_back(hit);
								}
							} catch (exception e){
								cout << "ERROR" << endl;
							}
						}
					}
				}
			}
		}
		fac_fa250Mode1hit->CopyTo(data);
		return NOERROR;
	}


	else if(fac_fa250Mode7hit != NULL){
		vector<fa250Mode7Hit*> data;
		evioDOMTree* local_EDT=(evioDOMTree*)event.GetRef();
		//	jout<<local_EDT->toString()<<endl;



		evio::evioDOMNodeListP fullList     = local_EDT->getNodeList();
		evio::evioDOMNodeList::const_iterator iter;
		evio::evioDOMNodeList::const_iterator branch;
		EvioCompositeDecoder decoder;



		for(iter=fullList->begin(); iter!=fullList->end(); iter++) {
			if((*iter)->tag==vme_mother_tag){

				evio::evioDOMNodeList *leafList = (*iter)->getChildList();

				for(branch=leafList->begin();branch!=leafList->end(); branch++){
					if( (*branch)->tag==child_mode7_tag){
						const evio::evioCompositeDOMLeafNode *leaf = static_cast<const evio::evioCompositeDOMLeafNode*>(*branch);
						int leafSize = leaf->getSize();
						vector<uint32_t> *pData = const_cast<vector<uint32_t> *>(&(leaf->data));
						decoder.decode(pData,leafSize);
						if(decoder.getData().size()>0){
							try{
								vector<CompositeADCPulse_t>  decdata = decoder.getDataPulse();
								for(int loop = 0; loop < decdata.size();loop++){
									fa250Mode7Hit *hit=new fa250Mode7Hit();
									/*Why I do not save directly the CompositeADCPulse_t and use it as the "Jobject in jana:\
									 * 1) CompositeADCPulse_t is just a simple structure to old data
									 * 2) CompositeADCPulse_t is just for evio-readout
									 * 3) fa250Mode7Hit is a more complex class, used within JANA
									 * */

									hit->m_channel.rocid=0;  ///TODO better
									hit->m_channel.slot=decdata[loop].slot;
									hit->m_channel.channel=decdata[loop].channel;
									hit->trigger=decdata[loop].trigger;
									hit->timestamp=decdata[loop].time;
									for (int ipulse=0;ipulse<decdata[loop].pulses.size();ipulse++){
										fa250Mode7Hit::pulse_t pulse;
										pulse.tdc=decdata[loop].pulses[ipulse].tdc;
										pulse.adc=decdata[loop].pulses[ipulse].adc;
										pulse.ped=decdata[loop].pulses[ipulse].ped;
										pulse.max=decdata[loop].pulses[ipulse].max;
										hit->pulses.push_back(pulse);
									}
									data.push_back(hit);
								}
							} catch (exception e){
								cout << "ERROR" << endl;
							}
						}
					}
				}
			}
		}
		fac_fa250Mode7hit->CopyTo(data);
		return NOERROR;
	}
	else if(fac_eventData != NULL){
		vector<eventData*> data;
		evioDOMTree* local_EDT=(evioDOMTree*)event.GetRef();

		evio::evioDOMNodeListP fullList     = local_EDT->getNodeList();
		evio::evioDOMNodeList::const_iterator iter;
		evio::evioDOMNodeList::const_iterator branch;

		eventData *this_eventData=new eventData();

		for(iter=fullList->begin(); iter!=fullList->end(); iter++) {
			if((*iter)->tag==vme_mother_tag){
				evio::evioDOMNodeList *leafList = (*iter)->getChildList();

				for(branch=leafList->begin();branch!=leafList->end(); branch++){
					if( (*branch)->tag==child_trigger_tag){
						const evio::evioCompositeDOMLeafNode *leaf = static_cast<const evio::evioCompositeDOMLeafNode*>(*branch);
						int leafSize = leaf->getSize();
						vector<uint32_t> *pData = const_cast<vector<uint32_t> *>(&(leaf->data));
						if (leafSize>0){

							for (int itrigWord=0;itrigWord<pData->size();itrigWord++){
								this_eventData->triggerWords.push_back(pData->at(itrigWord));
							}

						}

					}

					else if( (*branch)->tag==eventHeader_tag){
						const evio::evioCompositeDOMLeafNode *leaf = static_cast<const evio::evioCompositeDOMLeafNode*>(*branch);
						int leafSize = leaf->getSize();
						vector<uint32_t> *pData = const_cast<vector<uint32_t> *>(&(leaf->data));
						if (leafSize!=5){  //should have 5 words in head bank
							jerr<<"Incompatible number of words in head bank: got "<<leafSize<<endl;
						}
						else{
							this_eventData->eventN=pData->at(2);
							this_eventData->runN=pData->at(1);
							this_eventData->time=pData->at(3);
						}
					}
				}
			}
		}
	//	jout<<"Done: "<<this_eventData->eventN<<" "<<this_eventData->runN<<" "<<this_eventData->time<<endl;
		if (this_eventData->triggerWords.size()!=0){
			data.push_back(this_eventData);
			fac_eventData->CopyTo(data);
		}
		return NOERROR;
	}

	/*Mevent* this_evt = (Mevent*)event.GetRef();



	if(dataClassName == "CTOFhit")
	{
		// getting EVIO bank
		vector<hitOutput> bankDgt = this_evt->dgtBanks["ctof"];

		vector<CTOFhit*> ctofhits;
		for(unsigned int ih=0; ih<bankDgt.size(); ih++)
		{
			CTOFhit *ctofhit = new CTOFhit;
			ctofhit->paddle = bankDgt[ih].getIntDgtVar("paddle");
			ctofhit->ADCL   = bankDgt[ih].getIntDgtVar("ADCL");
			ctofhit->ADCR   = bankDgt[ih].getIntDgtVar("ADCR");
			ctofhits.push_back(ctofhit);
		}

		// publish the CTOFhit
		JFactory<CTOFhit> *fac = dynamic_cast<JFactory<CTOFhit>*>(factory);
		fac->CopyTo(ctofhits);
		return NOERROR;

	}
	 */




	// Just return. The _data vector should already be reset to have zero objects
	return OBJECT_NOT_AVAILABLE;
}









