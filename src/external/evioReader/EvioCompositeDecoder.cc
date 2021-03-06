/**
 * Autogenerated Source File for EvioCompositeDecoder
 * \\file EvioCompositeDecoder.cc
 * \\author G.Gavalian
 * \\date 2014-09-19
 */
#include "EvioCompositeDecoder.h"


EvioCompositeDecoder::EvioCompositeDecoder()
{
}
/* 
EvioCompositeDecoder::EvioCompositeDecoder(const EvioCompositeDecoder &obj)
{
}
 */
EvioCompositeDecoder::~EvioCompositeDecoder()
{
}

uint8_t EvioCompositeDecoder::getInt8(char *data, int offset){
	uint8_t value = *(reinterpret_cast<uint8_t *>(&data[offset]));
	return value;
}

uint16_t EvioCompositeDecoder::getInt16(char *data, int offset){
	uint16_t value = *(reinterpret_cast<uint16_t *>(&data[offset]));
	return value;
}

uint32_t EvioCompositeDecoder::getInt32(char *data, int offset){
	uint32_t value = *(reinterpret_cast<uint32_t *>(&data[offset]));
	return value;
}

int64_t EvioCompositeDecoder::getInt64(char *data, int offset){
	int64_t value = *(reinterpret_cast<int64_t *>(&data[offset]));
	return value;
}

/* const EvioCompositeDecoder &EvioCompositeDecoder::operator=(const EvioCompositeDecoder &obj){} */
void    EvioCompositeDecoder::decode(vector<uint32_t> *vec, int dataSize){
	char *data = new char[dataSize*4];
	memcpy(data,reinterpret_cast<char *> (&(*vec)[0]),dataSize*4);




	int offset = 0;
	uint8_t bankSlot,chan;
	uint32_t samples;
	int bankNChannels,bankTrigger;
	unsigned long long bankTime,bankTimeTMP;
	int nchannels;
	adcSamples.clear();

	while(offset<dataSize*4){

		bankSlot          = getInt8 ( data, offset);
		bankTrigger       = getInt32( data, offset + 1);
		bankTimeTMP       = getInt64( data, offset + 1 + 4);
		bankTime          = 0;
		bankTime          = (bankTimeTMP&0xffffff000000)>>24;
		bankTime          = bankTime | (bankTimeTMP&0x000000ffffff)<<24;
		bankNChannels     = getInt32( data, offset + 1 + 4 + 8);
		offset = offset + 1 + 4 + 8 + 4;

		//printf("bankTimeTMP: %llx, bankTime: %llx \n",bankTimeTMP,bankTime);

		if (bankNChannels<=0) continue; //A.C. work-around
		if (bankNChannels>16) continue;  //A.C. work-around
		if ((bankSlot<=0)||(bankSlot>21)) continue;  //A.C. work-around


		/* cout << " decoding : SLOT = " << (unsigned int) bankSlot
	 << "  TRIGGER = " << bankTrigger 
	 << "  TIME = " << bankTime 
	 << " NCHAN = " << bankNChannels  << endl;*/


		nchannels = 0;


		while(nchannels<bankNChannels){

			chan    = getInt8(data,offset);
			samples = getInt32(data,offset+1);
			offset = offset + 1 + 4;
		//	cout << " CHANNEL = " << (unsigned int) chan << "  SAMPLES = " << (unsigned int) samples << "  " << endl;
			//cout << offset <<" "<<dataSize*4<<endl;
			//offset + samples*sizeof(short) << "  " << dataSize << endl;;
			if(offset + samples*sizeof(short) > dataSize*4) break;
			//cout << " scanning channels" << endl;
			CompositeADCRaw_t  adc;
			adc.slot = bankSlot;
			adc.trigger = bankTrigger;
			adc.time = bankTime;
			adc.channel = chan;
			adc.samples.clear();
			for(int loop = 0; loop < samples; loop++){
				short sp = getInt16(data, offset);
				adc.samples.push_back(sp);
				offset += 2;
				//	cout << sp << " " ;
			}
			adcSamples.push_back(adc);
			//  cout << endl;
			nchannels++;
		}
	}
	delete data;
}

void    EvioCompositeDecoder::decodePulse(vector<uint32_t> *vec, int dataSize){

	//cout << " decoding pulse " << endl;


	char *data = new char[dataSize*4];
	memcpy(data,reinterpret_cast<char *> (&(*vec)[0]),dataSize*4);


	int offset = 0;
	int sizeOfEntry = 2 + 4 + 2 + 2;

	uint8_t bankSlot,chan;
	uint32_t bankTrigger,samples;
	int bankNChannels;
	unsigned long long bankTime;



	int nchannels;

	adcSamplesPulse.clear();





	while(offset<dataSize*4){
		/* cout<<"HERE: "<<offset<<" "<<dataSize*4<<endl; */

		bankSlot          = getInt8 ( data, offset);
		bankTrigger       = getInt32( data, offset + 1);
		bankTime          = getInt64( data, offset + 1 + 4);
		bankNChannels     = getInt32( data, offset +1 + 4 + 8);
		offset = offset + 1 + 4 + 8 + 4;


		nchannels = 0;



		/*  cout << " decoding : SLOT = " << (unsigned int) bankSlot
    << "  TRIGGER = " << bankTrigger 
    << "  TIME = " << bankTime 
    << " NCHAN = " << bankNChannels  << endl;*/

		if (bankNChannels<=0) continue;
		if ((bankSlot<0)||(bankSlot>21)) continue;

		while(nchannels < bankNChannels){

			// cout<<"HERE1: "<<offset<<" "<<dataSize*4<<endl;
			chan    = getInt8(data,offset);
			samples = getInt32(data,offset+1);
			offset = offset + 1 + 4;


			//offset + samples*sizeof(short) << "  " << dataSize << endl;;
			if(offset + samples*sizeOfEntry > dataSize*4) break;
			//cout << " scanning channels" << endl;
			CompositeADCPulse_t  adc;
			adc.slot = bankSlot;
			adc.time = bankTime;
			adc.trigger = bankTrigger;
			adc.channel = chan;

			adc.pulses.clear();
			//    cout << "CHANNEL= " <<  (int)chan << " SAMPLES = " << (int)samples << endl;
			//printf(" channel = %d  samples = %d \n", chan, samples);
			for(int loop = 0; loop < samples; loop++){
				Pulse_t pulse;
				pulse.tdc = (int) getInt16(data, offset);
				pulse.adc = (int) getInt32(data, offset + 2);
				pulse.ped = (int) getInt16(data, offset + 2 + 4);
				pulse.max = (int) getInt16(data, offset + 2 + 4 + 2);

				//printf("%8d %8d %8d %8d\n",adc,tdc,ped,max);
				//cout << bankSlot << "  "  << chan << "  " << adcV << " " << tdcV << " " << pedV << "  " << maxV << endl;
				adc.pulses.push_back(pulse);
				offset += 2 + 4 + 2 + 2;
			}

			adcSamplesPulse.push_back(adc);
			nchannels++;
		}
	}
	delete data;
}


void    EvioCompositeDecoder::createStructures(){

}
