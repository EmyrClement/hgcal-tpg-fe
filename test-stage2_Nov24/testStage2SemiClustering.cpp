/**********************************************************************
 Created on : 11/11/2024
 Purpose    : Read the TCs from ntuple and perform stage2 semi-emulation
 Author     : Indranil Das, Research Associate, Imperial
 Email      : indranil.das@cern.ch | indra.ehep@gmail.com
**********************************************************************/
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include <TROOT.h>
#include <TChain.h>
#include <TProfile.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TMath.h>
#include <TH2.h>

#include "TPGTriggerCellFloats.hh"
#include "Stage2.hh"

int main(int argc, char** argv)
{
  
  bool doPrint = 0;
  std::cout << "========= Run as : ./testStage2SemiClustering.exe $input_file $index $nofevents ==========" << std::endl;
  
  if(argc < 2){
    std::cerr << argv[0] << ": no input file has been specified specified" << std::endl;
    return false;
  }
  std::string inputfile;
  std::istringstream issInfile(argv[1]);
  issInfile >> inputfile;
  std::string index = "0";
  if(argc >= 3){
    std::istringstream issnIndex(argv[2]);
    issnIndex >> index;
  }
  uint64_t nofEvents(0);
  if(argc >= 4){
    std::istringstream issnEvents(argv[3]);
    issnEvents >> nofEvents;
  }
  
  TFile *fin = TFile::Open(Form(inputfile.c_str()));
  std::cout<<"Input filename : " << fin->GetName() << std::endl;
  TTree *tr  = (TTree*)fin->Get("l1tHGCalTriggerNtuplizer/HGCalTriggerNtuple");  
  tr->SetBranchStatus("*",0);
  Int_t event ;
  tr->SetBranchStatus("event",1);
  tr->SetBranchAddress("event"	, &event);

  std::vector<float>  *genpart_pt = 0 ;
  tr->SetBranchStatus("genpart_pt",1);
  tr->SetBranchAddress("genpart_pt" , &genpart_pt);
  
  std::vector<float>  *genpart_eta = 0 ;
  tr->SetBranchStatus("genpart_eta",1);
  tr->SetBranchAddress("genpart_eta" , &genpart_eta);

  std::vector<float>  *genpart_phi = 0 ;
  tr->SetBranchStatus("genpart_phi",1);
  tr->SetBranchAddress("genpart_phi" , &genpart_phi);

  std::vector<float>  *genjet_pt = 0 ;
  tr->SetBranchStatus("genjet_pt",1);
  tr->SetBranchAddress("genjet_pt" , &genjet_pt);

  std::vector<float>  *genjet_eta = 0 ;
  tr->SetBranchStatus("genjet_eta",1);
  tr->SetBranchAddress("genjet_eta" , &genjet_eta);

  std::vector<float>  *genjet_phi = 0 ;
  tr->SetBranchStatus("genjet_phi",1);
  tr->SetBranchAddress("genjet_phi" , &genjet_phi);

  std::vector<float>  *tc_pt = 0 ;
  tr->SetBranchStatus("tc_pt",1);
  tr->SetBranchAddress("tc_pt" , &tc_pt);
  
  std::vector<float>  *tc_eta = 0 ;
  tr->SetBranchStatus("tc_eta",1);
  tr->SetBranchAddress("tc_eta" , &tc_eta);

  std::vector<float>  *tc_phi = 0 ;
  tr->SetBranchStatus("tc_phi",1);
  tr->SetBranchAddress("tc_phi" , &tc_phi);

  std::vector<float>  *tc_layer = 0 ;
  tr->SetBranchStatus("tc_layer",1);
  tr->SetBranchAddress("tc_layer" , &tc_layer);
  
  std::vector<float>  *tc_x = 0 ;
  tr->SetBranchStatus("tc_x",1);
  tr->SetBranchAddress("tc_x" , &tc_x);
  
  std::vector<float>  *tc_y = 0 ;
  tr->SetBranchStatus("tc_y",1);
  tr->SetBranchAddress("tc_y" , &tc_y);
  
  std::vector<float>  *tc_z = 0 ;
  tr->SetBranchStatus("tc_z",1);
  tr->SetBranchAddress("tc_z" , &tc_z);

  std::vector<float>  *tc_energy = 0 ;
  tr->SetBranchStatus("tc_energy",1);
  tr->SetBranchAddress("tc_energy" , &tc_energy);

  TH1D *hEt = new TH1D("hEt","hEt",100,0,1000);
  TH1D *hPhi = new TH1D("hPhi","hPhi",2*TMath::TwoPi(),-1.0*TMath::TwoPi(),TMath::TwoPi());
  TH1D *hPhiDeg = new TH1D("hPhiDeg","hPhiDeg",2*TMath::RadToDeg()*TMath::TwoPi(),-1.0*TMath::RadToDeg()*TMath::TwoPi(),TMath::RadToDeg()*TMath::TwoPi());
  
  uint64_t totalEntries = tr->GetEntries();
  if(nofEvents==0) nofEvents = totalEntries;
  
  std::cout << "Total Entries : " << totalEntries << std::endl;
  std::cout << "Loop for: " << nofEvents << " entries"  << std::endl;
  
  TH2D *tcxy[6],*tcxyOverZ[6],*clusxyOverZ[6],*clusxy[6],*deltaGenclusSeg[6];
  TH2D *tcxyAll = new TH2D("TCXYAll","x-y distribution of TCs",1000,-500,500,1000,-500,500);
  for (uint32_t isect = 0 ; isect < 6 ; isect++ ) {
    tcxy[isect] = new TH2D(Form("TCXY_%u",isect),Form("x-y distribution of TCs for sector %u",isect),1000,-500,500,1000,-500,500);
    tcxyOverZ[isect] = new TH2D(Form("TCXYOverZ_%u",isect),Form("x/z and y/z distribution of TCs for sector %u",isect),1000,-0.6,0.6,1000,-0.6,0.6);
    clusxyOverZ[isect] = new TH2D(Form("CLUSXYOverZ_%u",isect),Form("x/z and y/z distribution of clusters for sector %u",isect),1000,-0.6,0.6,1000,-0.6,0.6);
    clusxy[isect] = new TH2D(Form("CLUSXY_%u",isect),Form("x-y distribution of clusters for sector %u",isect),1000,-0.6,0.6,1000,-0.6,0.6);
    deltaGenclusSeg[isect] = new TH2D(Form("deltaGenclus_%u",isect),Form("deltaGenclus_%u",isect),100,-0.25,0.25,100,-0.25,0.25);
  }
  TH2D *deltaGenclus = new TH2D("deltaGenclusAll", "deltaGenclusAll", 100, -0.25, 0.25, 100, -0.25, 0.25);
  TH2D *deltaTCclus = new TH2D("deltaTcclusAll", "deltaTCclusAll", 100, -0.25, 0.25, 100, -0.25, 0.25);
  TH2D *deltaGentc = new TH2D("deltaGentc", "deltaGentc", 100, -0.25, 0.25, 100, -0.25, 0.25);
  TH1D *hClusE = new TH1D("hClusE", "hClusE", 100, 0.0, 100.0);
  TH2D *hGenClusE = new TH2D("hGenClusE", "hGenClusE", 200, 0.0, 200.0, 200, 0.0, 200.0);

  TProfile *hEtRatioGenjetvsTC = new TProfile("hEtRatioGenjetvsTC", "hEtRatio Genjet-vs-TC", 200, 0., 200., 0., 200.);
  TProfile *hEtRatioTCvsClus = new TProfile("hEtRatioTCvsClus", "hEtRatio TC-vs-Cluster", 200, 0., 200., 0., 200.);
  TProfile *hEtRatioGenjetvsClus = new TProfile("hEtRatioGenjetvsClus", "hEtRatio Genjet-vs-Cluster", 200, 0., 200., 0., 200.);

  TH1D *hTCLayerEWt = new TH1D("hTCLayerEWt", "Energy weighted layer", 50, -0.5, 49.5);

  // Output ntuple for jet studies
  std::string outnameClusters = "clustersOut/Stage2EmulationOut_" + index;
  TFile *foutClusters = new TFile(Form("%s.root", outnameClusters.c_str()), "recreate");
  TTree *clusterTree = new TTree("clusters", "Clusters");
  std::vector<double> clusterE;
  std::vector<double> clusterPt;
  std::vector<double> clusterEta;
  std::vector<double> clusterPhi;
  clusterTree->Branch("clusEnergy", &clusterE);
  clusterTree->Branch("clusPt", &clusterPt);
  clusterTree->Branch("clusEta", &clusterEta);
  clusterTree->Branch("clusPhi", &clusterPhi);

  TTree *genJetTree = new TTree("genjets", "GenJets");
  std::vector<double> genJetPt;
  std::vector<double> genJetEta;
  std::vector<double> genJetPhi;
  genJetTree->Branch("genJetPt", &genJetPt);
  genJetTree->Branch("genJetEta", &genJetEta);
  genJetTree->Branch("genJetPhi", &genJetPhi);

  TPGTriggerCellFloats tcf0, tcf1;
  for (Long64_t ievent = 0; ievent < nofEvents; ievent++)
  {

    clusterE.clear();
    clusterPt.clear();
    clusterEta.clear();
    clusterPhi.clear();
    genJetPt.clear();
    genJetEta.clear();
    genJetPhi.clear();

    tr->GetEntry(ievent);

    if (doPrint)
      std::cout << "Event : " << ievent << ", nof TCs : " << tc_pt->size() << std::endl;
    if (ievent % 100 == 0)
      std::cout << "Event : " << ievent << ", nof TCs : " << tc_pt->size() << std::endl;

    float tot_tc_pt = 0.0, tot_tc_e = 0.0;
    std::vector<TPGTriggerCellWord> vTcw[6];
    //std::vector<TPGTriggerCellFloats> vTcw[6];    
    for(unsigned itc=0;itc<tc_pt->size();itc++){
      
      double z(fabs(tc_z->at(itc)));
      float phi_deg = TMath::RadToDeg() * tc_phi->at(itc) ;
      uint16_t sec0(7),sec1(7);

      //The following segmentation is applicable for towers
      // if((phi_deg>-15. and phi_deg<=0.) or (phi_deg>=0. and phi_deg<=5.)) {sec0 = 2; sec1 = 0;}
      // else if(phi_deg>5. and phi_deg<=105.) {sec0 = sec1 = 0;}
      // else if(phi_deg>105. and phi_deg<=125.) {sec0 = 0; sec1 = 1;}
      // else if((phi_deg>125. and phi_deg<=180.) or (phi_deg>=-180. and phi_deg<=-135.)) {sec0 = sec1 = 1;}
      // else if(phi_deg>-135. and phi_deg<=-115.) {sec0 = 1; sec1 = 2;}
      // else {sec0 = sec1 = 2;}
      
      //The following segmentation is applied for trigger cells
      if(phi_deg>=0. and phi_deg<=60.) {sec0 = 2; sec1 = 0;}
      else if(phi_deg>60. and phi_deg<=120.) {sec0 = sec1 = 0;}
      else if(phi_deg>120. and phi_deg<=180.) {sec0 = 0; sec1 = 1;}
      else if(phi_deg>=-180. and phi_deg<=-120.) {sec0 = sec1 = 1;}
      else if(phi_deg>-120. and phi_deg<=-60.) {sec0 = 1; sec1 = 2;}
      else {sec0 = sec1 = 2;}
      
      if(tc_z->at(itc)<0.){
	switch(sec0){
	case 0:
	  sec0 += 3;
	  break;
	case 1:
	  sec0 += 4;
	  break;
	case 2:
	  sec0 += 2;
	  break;
	default:
	  ;
	}
	switch(sec1){
	case 0:
	  sec1 += 3;
	  break;
	case 1:
	  sec1 += 4;
	  break;
	case 2:
	  sec1 += 2;
	  break;
	default:
	  ;
	}	
      }
      
      if(doPrint) std::cout << "ievent: " << ievent << ", eventId: " << event << ", itc: " << itc <<", z: " << tc_z->at(itc)
			    <<", zabs: " << z << ", x: " << tc_x->at(itc) << ", y: " << tc_y->at(itc)
			    << ", phi_deg: " << phi_deg << ", sec0: " <<  sec0 << ", sec1: " <<  sec1 << std::endl;
      
      tcxyAll->Fill(tc_x->at(itc),tc_y->at(itc));
      tcxy[sec0]->Fill(tc_x->at(itc),tc_y->at(itc));      
      if(sec0!=sec1) tcxy[sec1]->Fill(tc_x->at(itc),tc_y->at(itc));	
      
      for (uint32_t isect = 0 ; isect < 3 ; isect++ ){
	uint32_t addisect = 0;
	if(tc_z->at(itc)>0.0) addisect = 3;
	tcf0.setZero();
	tcf0.setXYOverZF(tc_x->at(itc)/z,tc_y->at(itc)/z,isect+addisect);
	if(tcf0.getXOverZF()>=0.0){
	  tcf0.setEnergyGeV(tc_pt->at(itc));
	  tcf0.setLayer(tc_layer->at(itc));
	  //if(doPrint) tcf0.print();
	  vTcw[isect+addisect].push_back(tcf0);
	}
      }
      
      tot_tc_pt += tc_pt->at(itc);
      tot_tc_e += tc_energy->at(itc);
      hPhi->Fill(tc_phi->at(itc));
      hPhiDeg->Fill(phi_deg);
    }//end of TC loop    
    if(doPrint) std::cout<<"tot_tc_pt : "<< tot_tc_pt << std::endl;
    for (uint32_t isect = 0 ; isect < 6 ; isect++ )
      for(TPGTriggerCellFloats const& tcf : vTcw[isect]) tcxyOverZ[isect]->Fill(tcf.getXOverZF(),tcf.getYOverZF());

    std::vector<TPGClusterData> vCld[6];    
    TPGStage2Emulation::Stage2 s2Clustering;
    for (uint32_t isect = 0 ; isect < 6 ; isect++ ) {
      s2Clustering.run(vTcw[isect],vCld[isect]);
      if(doPrint) std::cout << isect << ", Size of Tcs: " << vTcw[isect].size() << ", Size of Clusters: " << vCld[isect].size() << std::endl;
    }
    for (uint32_t isect = 0; isect < 6; isect++)
    {
      for (TPGClusterFloats const &clf : vCld[isect])
      {
        clusterE.push_back(clf.getEnergyGeV());
        clusterPt.push_back(clf.getEnergyGeV());  // ....no pt, just energy
        clusterEta.push_back(clf.getGlobalEtaRad(isect));
        clusterPhi.push_back(clf.getGlobalPhiRad(isect));

        clusxyOverZ[isect]->Fill(clf.getLocalXOverZF(), clf.getLocalYOverZF());
        clusxy[isect]->Fill(clf.getGlobalXOverZF(isect), clf.getGlobalYOverZF(isect));
      }
    }
    for (int ijet = 0; ijet < genjet_eta->size(); ijet++)
    {

      genJetPt.push_back(genjet_pt->at(ijet));
      genJetEta.push_back(genjet_eta->at(ijet));
      genJetPhi.push_back(genjet_phi->at(ijet));

      for (uint32_t isect = 0; isect < 6; isect++)
      {
        for (TPGClusterFloats const &clf : vCld[isect])
        {
          hClusE->Fill(clf.getEnergyGeV());
          if (fabs(clf.getGlobalEtaRad(isect) - genjet_eta->at(ijet)) < 0.05 and fabs(clf.getGlobalPhiRad(isect) - genjet_phi->at(ijet)) < 0.05)
          {
            hGenClusE->Fill(genjet_pt->at(ijet), clf.getEnergyGeV());
            hEtRatioGenjetvsClus->Fill(genjet_pt->at(ijet), clf.getEnergyGeV());
          }
          if (clf.getEnergyGeV() > 10.0)
          {
            deltaGenclus->Fill(clf.getGlobalEtaRad(isect) - genjet_eta->at(ijet), clf.getGlobalPhiRad(isect) - genjet_phi->at(ijet));
            deltaGenclusSeg[isect]->Fill(clf.getGlobalEtaRad(isect) - genjet_eta->at(ijet), clf.getGlobalPhiRad(isect) - genjet_phi->at(ijet));
          }
        }
      } // isect loop
      for (unsigned itc = 0; itc < tc_pt->size(); itc++)
      {
        deltaGentc->Fill(tc_eta->at(itc) - genjet_eta->at(ijet), tc_phi->at(itc) - genjet_phi->at(ijet));
        if (fabs(tc_eta->at(itc) - genjet_eta->at(ijet)) < 0.05 and fabs(tc_phi->at(itc) - genjet_phi->at(ijet)) < 0.05)
          hEtRatioGenjetvsTC->Fill(genjet_pt->at(ijet), tc_phi->at(itc));
      }
    }
    
    for(unsigned itc=0;itc<tc_pt->size();itc++){
      for (uint32_t isect = 0 ; isect < 6 ; isect++ ){
	for(TPGClusterFloats const& clf : vCld[isect]){
	  if(fabs(clf.getGlobalEtaRad(isect)-tc_eta->at(itc))<0.05 and fabs(clf.getGlobalPhiRad(isect) - tc_phi->at(itc))<0.05){
	    hEtRatioTCvsClus->Fill(tc_pt->at(itc), clf.getEnergyGeV());
	  }
	  if(clf.getEnergyGeV()>10.0) {
	    deltaTCclus->Fill(clf.getGlobalEtaRad(isect)-tc_eta->at(itc), clf.getGlobalPhiRad(isect) - tc_phi->at(itc));
	  }
	}	
      }
      hTCLayerEWt->Fill(tc_layer->at(itc),tc_energy->at(itc)/tot_tc_e);
    }
    
    hEt->Fill(tot_tc_pt);

    genpart_pt->clear();
    genpart_eta->clear();
    genpart_phi->clear();
    genjet_pt->clear();
    genjet_eta->clear();
    genjet_phi->clear();
    tc_pt->clear();
    tc_eta->clear();
    tc_phi->clear();
    tc_layer->clear();
    tc_x->clear();
    tc_y->clear();
    tc_z->clear();
    tc_energy->clear();

    clusterTree->Fill();
    genJetTree->Fill();
  } // event loop

  std::string outname = "teststage2histos_" + index;
  // hEt->SetTitle(outname.c_str());
  // hEt->GetXaxis()->SetTitle("total p_{T} trigger cells (in GeV)");
  // TCanvas *c1 = new TCanvas("c1",outname.c_str());
  // hEt->Draw();
  // c1->Update();
  
  TFile *fout = new TFile(Form("%s.root",outname.c_str()),"recreate");
  hEt->Write();
  hPhi->Write();
  hPhiDeg->Write();
  tcxyAll->Write();
  for (uint32_t isect = 0 ; isect < 6 ; isect++ ) tcxy[isect]->Write();
  for (uint32_t isect = 0 ; isect < 6 ; isect++ ) tcxyOverZ[isect]->Write();
  for (uint32_t isect = 0 ; isect < 6 ; isect++ ) clusxyOverZ[isect]->Write();
  for (uint32_t isect = 0 ; isect < 6 ; isect++ ) clusxy[isect]->Write();
  deltaGenclus->Write();
  deltaGentc->Write();
  hClusE->Write();
  hGenClusE->Write();
  for (uint32_t isect = 0 ; isect < 6 ; isect++ ) deltaGenclusSeg[isect]->Write();
  hEtRatioGenjetvsTC->Write();
  hEtRatioTCvsClus->Write();
  hEtRatioGenjetvsClus->Write();
  deltaTCclus->Write();
  hTCLayerEWt->Write();
  fout->Close();
  delete fout;

  fin->Close();
  delete fin;

  foutClusters->cd();
  clusterTree->Write();
  genJetTree->Write();
  foutClusters->Close();
  return true;
}
