#include "IntVetoHit.h"
#include <DAQ/fa250Mode1CalibPedSubHit.h>
#include <IntVeto/IntVetoSiPMHit.h>
#include "TCanvas.h"
#include "TH1D.h"


TCanvas* IntVetoHit::Draw(int id)const{
	int Nwaves;
	int Nhits;
	int Nsamples;
	if (m_canvas==0){
		if (id==0){
			m_canvas=new TCanvas();
		}
		else{
			m_canvas=new TCanvas(Form("c%i_%i_%i",m_channel.sector,m_channel.layer,m_channel.component),200,200,id);
		}
	}
	m_canvas->Clear();
	m_canvas->cd();
	m_canvas->Divide(2,2);

	vector <const IntVetoSiPMHit*> hits;
	const fa250Mode1CalibPedSubHit* wave;
	this->Get(hits);


	Nhits=hits.size();
	/*Clear previous histograms and create new ones*/
	for (int ih=0;ih<hWaves.size();ih++){
		if (hWaves[ih]!=0) delete hWaves[ih];
	}
	hWaves.clear();

	for (int ih=0;ih<Nhits;ih++){
		hits[ih]->GetSingle(wave);
		Nsamples=wave->samples.size();
		hWaves.push_back(new TH1D(Form("h%i_%i_%i_%i",hits[ih]->m_channel.int_veto->sector,hits[ih]->m_channel.int_veto->layer,hits[ih]->m_channel.int_veto->component,hits[ih]->m_channel.int_veto->readout),Form("h%i_%i_%i_%i",hits[ih]->m_channel.int_veto->sector,hits[ih]->m_channel.int_veto->layer,hits[ih]->m_channel.int_veto->component,hits[ih]->m_channel.int_veto->readout),Nsamples,-0.5,Nsamples-0.5));
		wave->toHisto(hWaves[ih]);
		m_canvas->cd(ih+1);
		hWaves[ih]->Draw();
	}



	return m_canvas;
}
