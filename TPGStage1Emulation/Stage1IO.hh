#ifndef Stage1IO_h
#define Stage1IO_h

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cstdint>
#include <cstring>
#include <cassert>
#include <random>

//#include "UnpackerOutputStream.hh"
#include "TPGBEDataformat.hh"
//#include "CspLink.hh"
//#include "TcRawData.hh"
#include "LpGbtData.hh"
#include "Stage1IOFwCfg.hh"

namespace TPGStage1Emulation {

class Stage1IO {
public:
  Stage1IO() : _runCall(0) {
  }
  
  static void convertElinksToTcRawData(TPGFEDataformat::TcRawData::Type type, unsigned nTc,
				       const uint32_t *v,
				       TPGFEDataformat::TcRawDataPacket &vTcrdp) {
    
    std::vector<TPGFEDataformat::TcRawData> &vTc(vTcrdp.second);

    bool doPrint(false);

    if(doPrint) {      
      for(unsigned i(0);i<2;i++) {
	std::cout << "Elink " << i << " = "
		  << std::hex << std::setw(8) << v[i]
		  << std::dec << std::endl;
      }
    }
    
    //if(v.size()==0) return;
    //if(v.size()>=4) return; // FIXME

    unsigned bx(v[0]>>28);

    bool bitMap(false);
    if(type==TPGFEDataformat::TcRawData::BestC) bitMap=(nTc>7);
    
    assert(nTc>0);
    vTc.resize(0);
    
    unsigned lastWord(0);
    unsigned lastBit(28);
    uint64_t d(v[0]);
      
    if(type==TPGFEDataformat::TcRawData::BestC) {
      lastBit-=8;
      vTc.push_back(TPGFEDataformat::TcRawData((d>>lastBit)&0xff));
      if(false) vTc.back().print();
    }
      
    if(!bitMap) {
      for(unsigned tc(0);tc<nTc;tc++) {
	if(lastBit<6) {
	  d=(d<<32);
	  lastWord++;
	  //if(v.size()>lastWord) d|=v[lastWord];
	  d|=v[lastWord];
	  lastBit+=32;
	}
	
	unsigned mult(0);
	if(type==TPGFEDataformat::TcRawData::BestC) {
	  lastBit-=6;
	  // std::cout<< std::hex
	  // 	   <<", d-word : 0x" << std::setfill('0') << std::setw(8) << (d>>lastBit)
	  // 	   <<", masked-d-word : 0x" << std::setfill('0') << std::setw(8) << ((d>>lastBit)&0x3f)
	  // 	   << std::dec << std::setfill(' ')
	  // 	   <<std::endl;
	  vTc.push_back(TPGFEDataformat::TcRawData(type,mult*tc+((d>>lastBit)&0x3f),0));
	}
	if(type==TPGFEDataformat::TcRawData::STC4A) {
	  lastBit-=2;
	  mult=4;
	  mult=0;
	  vTc.push_back(TPGFEDataformat::TcRawData(type,mult*tc+((d>>lastBit)&0x03),0));
	}
	if(type==TPGFEDataformat::TcRawData::STC4B) {
	  lastBit-=2;
	  mult=4;
	  mult=0;
	  vTc.push_back(TPGFEDataformat::TcRawData(type,mult*tc+((d>>lastBit)&0x03),0));
	}
	if(type==TPGFEDataformat::TcRawData::STC16) {
	  lastBit-=4;
	  mult=16;
	  mult=0;
	  vTc.push_back(TPGFEDataformat::TcRawData(type,mult*tc+((d>>lastBit)&0x0f),0));
	}
      }

    } else {
      for(unsigned tc(0);tc<48;tc++) {
	if(lastBit<1) {
	  d=(d<<32);
	  lastWord++;
	  //if(v.size()>lastWord) d|=v[lastWord];
	  d|=v[lastWord];
	  lastBit+=32;
	}
	
	lastBit-=1;
	if(((d>>lastBit)&0x1)!=0) {
	  vTc.push_back(TPGFEDataformat::TcRawData(type,tc,0));
	  if(doPrint) vTc.back().print();
	}
      }
      if(doPrint) std::cout << "vTc.size() = " << vTc.size() << ", nTc = " << nTc << std::endl;
      assert(vTc.size()==nTc+1);
    }
      
    for(unsigned tc(0);tc<nTc;tc++) {
      if(lastBit<9) {
	d=(d<<32);
	lastWord++;
	//if(v.size()>lastWord) d|=v[lastWord];
	d|=v[lastWord];
	lastBit+=32;
      }

      if(type==TPGFEDataformat::TcRawData::BestC) {
	lastBit-=7;
	vTc[tc+1]=TPGFEDataformat::TcRawData(type,vTc[tc+1].address(),(d>>lastBit)&0x7f);
      } else if(type==TPGFEDataformat::TcRawData::STC4A) {
	lastBit-=7;
	vTc[tc]=TPGFEDataformat::TcRawData(type,vTc[tc].address(),(d>>lastBit)&0x7f);
      } else {
	lastBit-=9;
	vTc[tc]=TPGFEDataformat::TcRawData(type,vTc[tc].address(),(d>>lastBit)&0x1ff);
      }
    }

    //if(bx==0xf && doPrint) {
    if(doPrint) {
      std::cout << "TcRawData words = " << vTc.size() << std::endl;
      for(unsigned i(0);i<vTc.size();i++) {
	vTc[i].print();
      }      
    }
    
  }

  static void convertTcRawDataToUnpackerOutputStreamPair(unsigned bx,
							 const TPGFEDataformat::TcRawDataPacket &vTcrdp,
							 TPGBEDataformat::UnpackerOutputStreamPair &up) {

    const std::vector<TPGFEDataformat::TcRawData> &vTc(vTcrdp.second);

    bool doPrint(false);

    up.setZero();

    bool doubleUos(vTc.size()>7);

    unsigned tc(0);

    if(vTc[0].isModuleSum()) {
      up.setModuleSum(0,bx,vTc[0].moduleSum());
      if(doubleUos) up.setModuleSum(1,bx,vTc[0].moduleSum());
      tc++;
      
    } else {
      up.setBx(0,bx);
      if(doubleUos) up.setBx(1,bx);
    }

    unsigned nTc(0);
    for(;tc<vTc.size();tc++) {
      uint8_t ch(vTc[tc].address());
      if(vTc[tc].type()==TPGFEDataformat::TcRawData::STC4A) ch+= 4*nTc;
      if(vTc[tc].type()==TPGFEDataformat::TcRawData::STC4B) ch+= 4*nTc;
      if(vTc[tc].type()==TPGFEDataformat::TcRawData::STC16) ch+=16*nTc;
      
      uint16_t en(vTc[tc].energy());
      if(vTc[tc].is4E3M()) en=TPGBEDataformat::UnpackerOutputStreamPair::pack5E4MFrom4E3M(en);
      
      if(doubleUos) up.setTriggerCell(nTc%2,nTc/2,ch,en);
      else          up.setTriggerCell(    0,nTc  ,ch,en);
      
      nTc++;
    }
    
    if(doPrint) {
      up.print();
      //up[1].print();
    }
  
    assert(up.checkFormat());
    //assert(up[1].checkFormat());
  }

  ////////////////////////////////////////////////////////

  // Direct access to set values (e.g. from beam test file)
  TPGBEDataformat::UnpackerOutputStreamPair&  unpackerOutputStreamPair(unsigned lp, unsigned up) {
    assert(lp<64);
    assert(up<6);
    return _vUos[lp][up];
  }
  
  // Chain W = lpGBT pairs to elinks
  void run(const std::vector<LpGbtData> &vPair,
	   std::vector<TPGFEDataformat::OrderedElinkPacket> &vElink) {
    assert(false);
  }
  
  // Chain X = elinks to TC raw data
  unsigned run(const std::vector<TPGFEDataformat::OrderedElinkPacket> &vElink,
	       std::vector<TPGFEDataformat::TcRawDataPacket> &vTc) {

    bool doPrint(false);
    
    if(doPrint) std::cout << std::endl
			  << "run called with elink vector size = "
			  << vElink.size() << std::endl;
    
    unsigned bx(vElink[0][0]>>28);


    // CONFIG
    TPGFEDataformat::TcRawData::Type type[5]={TPGFEDataformat::TcRawData::BestC,
			     TPGFEDataformat::TcRawData::BestC,
			     TPGFEDataformat::TcRawData::STC4A,
			     TPGFEDataformat::TcRawData::STC16,
			     TPGFEDataformat::TcRawData::STC16};
    unsigned nTc[5]={9,4,10,3,3};

    unsigned nElink(0);
    unsigned nArray(0);
    
    for(unsigned lp(0);lp<Stage1IOFwCfg::MaximumLpgbtPairs;lp++) {
      for(unsigned up(0);up<Stage1IOFwCfg::MaximumUnpackersPerLpgbtPair;up++) {
	if(_cfgPtr->connected(lp,up)) {
	  assert(nElink<vElink.size());
	  //assert(vElink[nElink].size()==_cfgPtr->numberOfElinks(lp,up));
	  assert((vElink[nElink][0]>>28)==bx);
	  
	  vTc.push_back(TPGFEDataformat::TcRawDataPacket());

	  //std::array<uint32_t,14> oElink;
	  //for(unsigned i(0);i<vElink[nElink].size();i++) oElink[i]=vElink[nElink][i];
	  
	  convertElinksToTcRawData(_cfgPtr->type(lp,up),
				   _cfgPtr->numberOfTCs(lp,up),
				   vElink[nElink].data()+nArray,vTc.back());
	  //oElink.data(),vTc.back());
	  nArray+=_cfgPtr->numberOfElinks(lp,up);
	    
	  if(doPrint) {
	    std::cout << "Loop with lp,up = " << lp << ", " << up
		      << ", nElink = " << nElink
		      << std::endl;
	    for(unsigned j(0);j<vTc.back().second.size() && doPrint;j++) {
	      vTc.back().second[j].print();
	    }
	    std::cout << std::endl;
	  }
	  
	  nElink++;
	}
      }
    }
    
    if(doPrint) std::cout << "nElink = " << nElink << std::endl;
    assert(nElink==vElink.size());

    return bx;
  }
  
  // Chain Y = TC raw data to unpacker output
  void run(unsigned bx,
	   const std::vector<TPGFEDataformat::TcRawDataPacket> &vTc) {
    
    bool doPrint(false);
    if(doPrint) std::cout << "Entering run: chain Y" << std::endl;

    unsigned nUos(0);
    
    for(unsigned lp(0);lp<Stage1IOFwCfg::MaximumLpgbtPairs;lp++) {
      for(unsigned up(0);up<Stage1IOFwCfg::MaximumUnpackersPerLpgbtPair;up++) {
	_vUos[lp][up].setZero();
	//_vUos[lp][up][1].setZero();
	
	if(_cfgPtr->connected(lp,up)) {
	  assert(nUos<vTc.size());
	  
	  bool doubleUos(vTc[nUos].second.size()>7);

	  unsigned tc(0);
	  if(vTc[nUos].second[0].isModuleSum()) {
	    unsigned modsum = vTc[nUos].second[0].moduleSum();
	    if(vTc[nUos].second[0].type()==TPGFEDataformat::TcRawData::STC16) modsum = 1;
	    _vUos[lp][up].setModuleSum(bx,modsum);
	    //if(doubleUos) _vUos[lp][up][1].setModuleSum(bx,vTc[nUos][0].moduleSum());
	    tc++;
	    
	  } else {
	    _vUos[lp][up].setBx(bx);
	    //if(doubleUos) _vUos[lp][up][1].setBx(bx);
	  }

	  unsigned nTc(0);
	  for(;tc<vTc[nUos].second.size();tc++) {
	    uint8_t ch(vTc[nUos].second[tc].address());
	    if(vTc[nUos].second[tc].type()==TPGFEDataformat::TcRawData::STC4A) ch+= 4*nTc;
	    if(vTc[nUos].second[tc].type()==TPGFEDataformat::TcRawData::STC4B) ch+= 4*nTc;
	    if(vTc[nUos].second[tc].type()==TPGFEDataformat::TcRawData::STC16) ch+=16*nTc;

	    uint16_t en(vTc[nUos].second[tc].energy());
	    if(vTc[nUos].second[tc].is4E3M()) en=TPGBEDataformat::UnpackerOutputStreamPair::pack5E4MFrom4E3M(en);
	  
	    if(doubleUos) {
	      _vUos[lp][up].setTriggerCell(nTc%2,nTc/2,ch,en);
	    } else {
	      _vUos[lp][up].setTriggerCell(    0,nTc  ,ch,en);
	    }
	
	    nTc++;
	  }

	  if(doPrint) {
	    _vUos[lp][up].print();
	    //_vUos[lp][up][1].print();
	  }
	}
    
	assert(_vUos[lp][up].checkFormat());
	//assert(_vUos[lp][up][1].checkFormat());
      }
    }
    
    if(doPrint) std::cout << "Exiting  run: chain Y" << std::endl;
  }
  
  // Chain Z = Unpacker output to Stage 1 output in inherited class
  //void run(const std::vector<UnpackerOutputStreamPair> &vUos,
  //	   std::vector<TPGBEDataformat::Stage1ToStage2Data*> &vS12) {
  //}
  
  virtual void runAlgorithm() = 0;
  
  // Chain Z = Input D = unpacker output
  void run(std::vector<TPGBEDataformat::Stage1ToStage2Data*> &vS12) {
    assert(vS12.size()==6);

    uint64_t crc(0xc);

    bool leadingBit((_runCall/18)%2==1);

    bool distort(true);

    // Copy bits in right order

    for(unsigned link(0);link<6;link++) {
      std::array<uint64_t,108> &d(vS12[link]->accessData());

      std::cout << "End link = " << link << std::endl;
      for(unsigned frame(0);frame<108;frame++) {
	std::cout << std::setw(3) << frame << ": "
		  << std::hex << std::setw(16) << _packerInputTc[link][frame]
		  << " " << std::setw(4) << _packerInputPtt[link][frame]
		  << std::dec << std::endl;
	
	if(leadingBit) d[frame]=(uint64_t(0x2)<<62)|crc;
	else d[frame]=crc;
      }
      
      // TCs
      unsigned tcLink(link);
      if(link==2) tcLink=1;
      if(link==3) tcLink=2;
      if(link==4) tcLink=3;
      if(link==5) tcLink=3;
      
      for(unsigned group(0);group<18;group++) {
	for(unsigned sub(0);sub<6;sub++) {
	  
	  if(distort) {
	    for(unsigned bit(0);bit<42;bit++) {
	      unsigned nBit(42*sub+bit);
	      if((_packerInputTc[tcLink][6*group+sub][bit/14]&(uint64_t(1)<<(bit%14)))!=0) d[102-6*group+((nBit+7)%6)]|=uint64_t(1)<<((nBit/6)+20);
	    }
	  } else {
	    d[107-6*group-sub]|=uint64_t(_packerInputTc[tcLink][6*group+sub][0])<<20;
	    d[107-6*group-sub]|=uint64_t(_packerInputTc[tcLink][6*group+sub][1])<<34;
	    d[107-6*group-sub]|=uint64_t(_packerInputTc[tcLink][6*group+sub][2])<<48;
	  }
	}	  
      }
      
      // pTTs
      for(unsigned group(0);group<18;group++) {
	for(unsigned sub(0);sub<6;sub++) {

	  if(distort) {
	    for(unsigned bit(0);bit<16;bit++) {
	      unsigned nBit(16*sub+bit);
	      if((_packerInputPtt[link][6*group+sub][bit/8]&(1<<(bit%8)))!=0) d[102-18*group+((nBit+7)%6)-6*(bit/16)]|=uint64_t(1)<<(((nBit/6)%16)+4);
	    }
	  } else {
	    d[107-6*group-sub]|=uint64_t(_packerInputPtt[link][6*group+sub][0])<<4;
	    d[107-6*group-sub]|=uint64_t(_packerInputPtt[link][6*group+sub][1])<<12;
	  }
	}
      }
    }
    _runCall++;
  }

  // Input A = lpGBT pairs
  void run(const std::vector<LpGbtData> &vPair,
	   std::vector<TPGBEDataformat::Stage1ToStage2Data*> &vS12) {
    std::vector<TPGFEDataformat::OrderedElinkPacket> vElink;
    run(vPair,vElink);
    run(vElink,vS12);
  }
  
  // Input B = elinks
  void run(const std::vector<TPGFEDataformat::OrderedElinkPacket> &vElink,
	   std::vector<TPGBEDataformat::Stage1ToStage2Data*> &vS12) {

    std::vector<TPGFEDataformat::TcRawDataPacket> vTc;
    unsigned bx=run(vElink,vTc);
    run(bx,vTc,vS12);
  }
  
  // Input C = TC raw data
  void run(unsigned bx,
	   const std::vector<TPGFEDataformat::TcRawDataPacket> &vTc,
	   std::vector<TPGBEDataformat::Stage1ToStage2Data*> &vS12) {

    run(bx,vTc);
    runAlgorithm();
    run(vS12);
  }
  
  void setHardwareConfiguration(const Stage1IOFwCfg *p) {
    _cfgPtr=p;
    _cfgPtr->print();
  }

protected:
  TPGBEDataformat::UnpackerOutputStreamPair _vUos[64][6];
  //uint64_t _packerInputTc[4][108];
  uint16_t _packerInputTc[4][108][3];
  //uint16_t _packerInputPtt[6][108];
  uint8_t _packerInputPtt[6][108][2];
  
  unsigned _runCall;
  
  const Stage1IOFwCfg *_cfgPtr;
};

}
#endif
