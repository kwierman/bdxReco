
#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <time.h>
using namespace std;

#include "MCEvent_factory.h"

#include <Calorimeter/CalorimeterHit.h>
#include <Calorimeter/CalorimeterCluster.h>
#include <IntVeto/IntVetoHit.h>
#include <ExtVeto/ExtVetoHit.h>
#include <Paddles/PaddlesHit.h>


using namespace jana;

//------------------
// init
//------------------
jerror_t MCEvent_factory::init(void)
{

	m_EC2_cut=20; //MeV
	m_ExtVeto_timeWindows=250; //ns
	m_IntVeto_timeWindows=250; //ns
	gPARMS->SetDefaultParameter("CATANIAEVENT:EC2_CUT",m_EC2_cut,"Cut on MPPC#2 energy in MeV");
	gPARMS->SetDefaultParameter("CATANIAEVENT:EXTVETO_TIME_WINDOW",m_ExtVeto_timeWindows,"ExtVeto time window (ns)");
	gPARMS->SetDefaultParameter("CATANIAEVENT:INTVETO_TIME_WINDOW",m_ExtVeto_timeWindows,"IntVeto time window (ns)");
	srand((unsigned)time(NULL));



	return NOERROR;
}

//------------------
// brun
//------------------
jerror_t MCEvent_factory::brun(jana::JEventLoop *eventLoop, int32_t runnumber)
{
	return NOERROR;
}

//------------------
// evnt
//------------------
jerror_t MCEvent_factory::evnt(JEventLoop *loop, uint64_t eventnumber)
{

	double inefficiency_iv=1;		// % on single detector
	double inefficiency_ev=1.;		// % on single detector

	double Ene_thr=0;		// Energy threshold on single Crystal

	vector <const CalorimeterHit*> chits;
	vector <const CalorimeterHit*>::const_iterator chits_it;

	vector <const CalorimeterCluster*> cclusters;
	vector <const CalorimeterCluster*>::const_iterator cclusters_it;

	vector <const IntVetoHit*> ivhits;
	vector <const IntVetoHit*>::const_iterator ivhits_it;

	vector <const ExtVetoHit*> evhits;
	vector <const ExtVetoHit*>::const_iterator evhits_it;



	loop->Get(chits);
	loop->Get(cclusters);
	loop->Get(ivhits);
	loop->Get(evhits);



	MCEvent *m_event=new MCEvent();
	m_event->E=0;
	m_event->E_single_crys=0;
	m_event->E1=0;
	m_event->E2=0;
	m_event->phe1=0;
	m_event->phe2=0;
	m_event->T=0;
	m_event->nCalorimeterHits=0;
	m_event->nCalorimeterHits_ext_layer=0;


	m_event->nCalorimeterHits_S0=0;
	m_event->nCalorimeterHits_S1=0;
	m_event->nCalorimeterHits_S2=0;
	m_event->nCalorimeterHits_S3=0;
	m_event->nCalorimeterHits_S4=0;
	m_event->nCalorimeterHits_S5=0;
	m_event->nCalorimeterHits_S6=0;
	m_event->nCalorimeterHits_S7=0;

	m_event->Eseed=0;
	m_event->xseed=0;
	m_event->yseed=0;
	m_event->Nhit_cluster=0;
	m_event->E_cluster=0;
	m_event->T_cluster=0;


	m_event->flag_RMS=false;
	double T1,T2;
	bool flag1,flag2;

//	int take_event=0;

	T1=T2=0;
	flag1=false;
	flag2=false;

	double e_crys_max=0;


	for (chits_it=chits.begin();chits_it!=chits.end();chits_it++){				// loop over the fired crystals
		const CalorimeterHit *hit=(*chits_it);

		if(hit->T<0) continue;
		else{
		for (int ihit=0;ihit<hit->m_data.size();ihit++){			// loop over the 2 SiPMs
				switch (hit->m_data[ihit].readout){
				case 1:
				//	m_event->phe1 +=(hit->m_data[ihit].Q);   	// Q-> number of p.e.
					m_event->E1=(hit->m_data[ihit].E); 		// energy calibration for muons
				//	m_event->E1=(hit->m_data[ihit].Q)/9.5; // energy calibration from data : 16 MeV  protons at 12cm from the SiPM
					break;
				case 2:
				//	m_event->phe2 +=(hit->m_data[ihit].Q);  	// Q-> number of p.e.
					m_event->E2=(hit->m_data[ihit].E); 	// energy calibration for muons
				//	m_event->E2=(hit->m_data[ihit].Q)/17; // energy calibration from data : 16 MeV  protons at 12cm from the SiPM
					break;
				}
			}

		if(   ((m_event->E1 +m_event->E2)/2)<Ene_thr     ) continue;


         if(((m_event->E1 +m_event->E2)/2)>=50) m_event->nCalorimeterHits_thr++;    // number of crystals above the threshold

		if ( ((m_event->E1 +m_event->E2)/2)>e_crys_max ) {
			e_crys_max=((m_event->E1 +m_event->E2)/2);
			m_event->y_crys_max=hit->m_channel.x;		// x and y are inverted in the simulazion (?)
			m_event->x_crys_max=hit->m_channel.y;
		}


		if(hit->m_channel.x==0&&hit->m_channel.y==5&&hit->m_channel.sector==5)m_event->E_single_crys = (m_event->E1 +m_event->E2)/2;			// Energy of a single cryslal

		m_event->E += (m_event->E1 +m_event->E2)/2;				// sum the energies of all the crystals fired


		 m_event->nCalorimeterHits++;
	    if((hit->m_channel.x==0||hit->m_channel.x==9)&&(hit->m_channel.y==0||hit->m_channel.y==9))m_event->nCalorimeterHits_ext_layer++;		// Multiplicity of the external crystals
	    m_event->vCalorimeterHits.push_back(hit->m_channel);

	    switch (hit->m_channel.sector){
	     case 0:
	    	 m_event->nCalorimeterHits_S0++;
	    	 break;

	     case 1:
	    	  m_event->nCalorimeterHits_S1++;
	    	  break;

	     case 2:
	    	  m_event->nCalorimeterHits_S2++;
	    	  break;

	     case 3:
	    	  m_event->nCalorimeterHits_S3++;
	    	  break;

	     case 4:
	    	  m_event->nCalorimeterHits_S4++;
	    	  break;

	     case 5:
	    	  m_event->nCalorimeterHits_S5++;
	    	  break;

	     case 6:
	    	  m_event->nCalorimeterHits_S6++;
	    	  break;

	     case 7:
	    	  m_event->nCalorimeterHits_S7++;
	    	  break;

	                                   }



							}   //end else on Time >0

	              }			// end loop over the fired crystals

   // if (take_event<2) m_event->E = -99;

 //          jout << "S0= "<< m_event->nCalorimeterHits_S0<<endl;
 //          jout << "S1= "<< m_event->nCalorimeterHits_S1<<endl;
 //          jout << "S2= "<< m_event->nCalorimeterHits_S2<<endl;
 //          jout << "S3= "<< m_event->nCalorimeterHits_S3<<endl;
 //          jout << "S4= "<< m_event->nCalorimeterHits_S4<<endl;
 //          jout << "S5= "<< m_event->nCalorimeterHits_S5<<endl;
 //          jout << "S6= "<< m_event->nCalorimeterHits_S6<<endl;
 //          jout << "S7= "<< m_event->nCalorimeterHits_S7<<endl;


    //// number of sector involved in the EM shower ///

m_event->sector_EM=0;
map<int,vector<const CalorimeterHit*> > sector_hits;  /*key: sector, data: all hits in that sector*/
map<int,vector<const CalorimeterHit*> >::const_iterator sector_hits_it;  /*key: sector, data: all hits in that sector*/


vector<const CalorimeterHit*> all_hits;
vector<const CalorimeterHit*>::const_iterator all_hits_it;
loop->Get(all_hits);


/*1: divide hits in different sectors. At this stage, do not apply yet any cut*/
for (all_hits_it=all_hits.begin();all_hits_it!=all_hits.end();all_hits_it++){
sector_hits[(*all_hits_it)->m_channel.sector].push_back(*all_hits_it);  //the [] operator in a map create a new data if key does not exists.
}

/*2: start the loop on the sectors*/
for (sector_hits_it=sector_hits.begin();sector_hits_it!=sector_hits.end();sector_hits_it++){
int this_sector;
vector<const CalorimeterHit*> this_sector_hits;
vector<const CalorimeterHit*>::iterator this_sector_hits_it;

vector<const CalorimeterHit*> this_sector_hits_temp;      // mr
vector<const CalorimeterHit*>::iterator this_sector_hits_temp_it; //mr


this_sector=(sector_hits_it)->first;
this_sector_hits_temp=(sector_hits_it)->second;

int ii=0;
for (this_sector_hits_temp_it=this_sector_hits_temp.begin();this_sector_hits_temp_it!=this_sector_hits_temp.end();this_sector_hits_temp_it++){
if ((*this_sector_hits_temp_it)->E > 20) ii++;

                                             	}
if(ii>0) m_event->sector_EM++;
        }



	       double seed_temp=0;
	       double seed = 0;
	       double x_cl_t[m_event->sector_EM];
	       double y_cl_t[m_event->sector_EM];
	       double sector_cl_t[m_event->sector_EM];
	       double x_cl=0;
           double y_cl=0;
	       double E_cl=0;
	       double sector_cl = 0;
	       int Nhits_cl=0;
	       int Nhits_cl_near_seed=0;
           double gamma=-99;



 int jj=0;
 	 jout<<"£££££££££££££££££££££££££££"<<endl;
	for (cclusters_it=cclusters.begin();cclusters_it!=cclusters.end();cclusters_it++){			// loop over clusters
			const CalorimeterCluster *hit=(*cclusters_it);



			seed = hit->Eseed;

			x_cl_t[jj] = hit->x;
            y_cl_t[jj] = hit->y;
            sector_cl_t[jj]=hit->sector;
     	    m_event->Block=hit->sector;

			E_cl += hit->E;
			Nhits_cl += hit->Nhits;

			if (seed > seed_temp ){
				seed_temp = seed;
				x_cl = hit->x;
				y_cl = hit->y;
				sector_cl = hit->sector;
				Nhits_cl_near_seed=hit->Nhits_near_seed;
			                      }
             jj++;

	                           }
	   m_event->Nblock=jj;
	   double idir=0;
	   double jdir=0;
	   double kdir=0;
	   double M=0;

	   if (jj>=2){

        idir = (y_cl_t[1] - y_cl_t[0])*5;		// x 5 cm
        jdir = (x_cl_t[1] - x_cl_t[0])*5;		// x 5 cm
        kdir = (sector_cl_t[1] - sector_cl_t[0])*50;		//x 40 cm

        jout<< " X2= "<<x_cl_t[1]<< " X1= "<<x_cl_t[0]<<endl;
        jout<< " Y2= "<<y_cl_t[1]<< " Y1= "<<y_cl_t[0]<<endl;
        jout<< " Z2= "<<sector_cl_t[1]<< " Z1= "<<sector_cl_t[0]<<endl;
        jout << idir << " "<< jdir << " "<<kdir<<endl;

        M = sqrt(pow(idir,2) + pow(jdir,2) + pow(kdir,2) );

        jout << M << endl;
        gamma = acos(kdir/M);

        jout << "Theta= "<<(180/3.1415)*gamma<< endl;

	    }

       m_event->theta = (180/3.1415)*gamma;

			m_event->Eseed=seed_temp;
	        m_event->xseed=x_cl;
			m_event->yseed=y_cl;
			m_event->Nhit_cluster=Nhits_cl;
			m_event->Nhit_cluster_near_seed=Nhits_cl_near_seed;
			m_event->E_cluster=E_cl;
			m_event->sectorseed=sector_cl;

//			jout << "sono il seed principale "<< m_event->Eseed<<endl;

			//	m_event->T_cluster;

			/*Now loop on inner veto hits*/
				m_event->nIntVetoHits=0;
				m_event->nIntVetoHitsCoincidence=0;
				m_event->nIntVetoHits_0=0;
				for (ivhits_it=ivhits.begin();ivhits_it!=ivhits.end();ivhits_it++){
					const IntVetoHit *hit=(*ivhits_it);
					 random =((double)rand()/(double)RAND_MAX) * 100;
					if (hit->T<0 || random<inefficiency_iv) continue; //The IntVeto condition for a "good" hit
					else{
						m_event->nIntVetoHits++;
						m_event->vIntVetoHits.push_back(hit->m_channel);
                        if(hit->m_channel.component==0){

                        	m_event->nIntVetoHits_0++;      //nIntVetoHits: multiplicity of IV -top
                        }


					}
				}



	/*Now loop on external veto hits*/
	m_event->nExtVetoHits=0;
	m_event->nExtVetoHitsCoincidence=0;

	for (evhits_it=evhits.begin();evhits_it!=evhits.end();evhits_it++){			// loop over the fired EV
		const ExtVetoHit *hit=(*evhits_it);
		 random =((double)rand()/(double)RAND_MAX) * 100;
//		jout << "Sector= "<<hit->m_channel.sector<< "Component  "<<hit->m_channel.component<<endl;

		if (hit->E<=0||hit->T<0 || random<inefficiency_ev) continue; //The ExtVeto condition for a "good" hit

		else{
			m_event->nExtVetoHits++;
			m_event->vExtVetoHits.push_back(hit->m_channel);



		}
	}


	_data.push_back(m_event);

	return NOERROR;
}

//------------------
// erun
//------------------
jerror_t MCEvent_factory::erun(void)
{
	return NOERROR;
}

//------------------
// fini
//------------------
jerror_t MCEvent_factory::fini(void)
{
	return NOERROR;
}

