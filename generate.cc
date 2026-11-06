// generate.cc
// Generates 100000 minimum-bias pp collisions at sqrt(s) = 13.6 TeV with Pythia 8
// and saves the final-state particles of each event to events.root (TTree "events").

#include <iostream>
#include <Pythia8/Pythia.h>
#include <vector>
#include <TFile.h>
#include <TTree.h>

int main() {
  Pythia8::Pythia pythia;
  
  // Physics: proton-proton collisions at 13.6 TeV, minimum bias (inelastic soft QCD).
  pythia.readString("Beams:idA = 2212");
  pythia.readString("Beams:idB = 2212");
  pythia.readString("Beams:eCM = 13600.");
  pythia.readString("SoftQCD:inelastic = on");

  // Fixed random seed, so every run produces exactly the same events.
  pythia.readString("Random:setSeed = on");
  pythia.readString("Random:seed = 12345");

  // Short log: no event listings, progress every 10000 events.
  pythia.readString("Next:numberShowEvent = 0");
  pythia.readString("Next:numberShowInfo = 0");
  pythia.readString("Next:numberShowProcess = 0");
  pythia.readString("Next:numberCount = 10000");

  if (!pythia.init()) {
    std::cerr << "Pythia init failed" << std::endl;
    return 1;
  }

  // Output file with one tree entry per event.
  // Each vector holds one value per final-state particle of that event.
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

  // Event loop: failed events are skipped and do not count towards nEvents.
  while (nAccepted < nEvents) {
    if (!pythia.next()) {
      nFailed++;
      if (nFailed > 1000) {
        std::cerr << "Too many failed events" << std::endl;
        return 1;
      }
      continue;
    }

    // Reset the vectors before filling them with the new event.
    pdg.clear();
    chargeType.clear();
    pt.clear();
    eta.clear();
    phi.clear();

    // Keep only final-state particles; intermediate ones have already decayed.
    for (int i = 0; i < pythia.event.size(); i++) {
      if (pythia.event[i].isFinal()) {
        pdg.push_back(pythia.event[i].id());
        chargeType.push_back(pythia.event[i].chargeType()); // 3 * electric charge
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
