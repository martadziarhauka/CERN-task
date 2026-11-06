#include <iostream>
#include <Pythia8/Pythia.h>
#include <vector>
#include <TFile.h>
#include <TTree.h>


int main(){
  Pythia8::Pythia pythia;
  
  pythia.readString("Beams:idA = 2212");
  pythia.readString("Beams:idB = 2212");
  pythia.readString("Beams:eCM = 13600.");
  pythia.readString("SoftQCD:inelastic = on");
  pythia.readString("Random:setSeed = on");
  pythia.readString("Random:seed = 12345");

    
  pythia.readString("Next:numberShowEvent = 0");
  pythia.readString("Next:numberShowInfo = 0");
  pythia.readString("Next:numberShowProcess = 0");
  pythia.readString("Next:numberCount = 10000");


  if (!pythia.init()) {
    std::cerr << "Pythia init failed" << std::endl;
    return 1;
  }

  TFile outputFile("events.root", "RECREATE");
  TTree tree("events", "Final-state particles");

  std::vector<int> pdg, chargeType;
  std::vector<float> pt, eta, phi;

  tree.Branch("pdg", &pdg);
  tree.Branch("chargeType", &chargeType);
  tree.Branch("pt", &pt);
  tree.Branch("eta", &eta);
  tree.Branch("phi", &phi);




  const int nEvents = 100000;
  int nAccepted = 0;
  int nFailed = 0;

  while (nAccepted < nEvents) {
    if (!pythia.next()) {
      nFailed++;
      if (nFailed > 1000) {
        std::cerr << "Too many failed events" << std::endl;
        return 1;
      }
      continue;
    }


    pdg.clear();
    chargeType.clear();
    pt.clear();
    eta.clear();
    phi.clear();

    
    for (int i = 0; i < pythia.event.size(); i++){
      if (pythia.event[i].isFinal()){
        pdg.push_back(pythia.event[i].id());
        chargeType.push_back(pythia.event[i].chargeType());
        pt.push_back(pythia.event[i].pT());
        eta.push_back(pythia.event[i].eta());
        phi.push_back(pythia.event[i].phi());
      }
    }


    tree.Fill();
    nAccepted++;


    
  }

  pythia.stat();
  std::cout << "Saved " << nAccepted << " events, failed: " << nFailed << std::endl;

  tree.Write();
  outputFile.Close();
  return 0;
}