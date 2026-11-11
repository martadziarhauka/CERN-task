// analyze.cc
// Reads final-state particles from events.root (TTree "events", written by generate.cc)
// and fills histograms of multiplicity N, pT, eta and |phi| for 8 particle categories.
// The 32 histograms are saved to analysis.root.

#include <iostream>
#include <vector>
#include <TFile.h>
#include <TTree.h>
#include <cmath>
#include <TH1D.h>
#include <string>

// Returns true if a particle with this PDG code and charge belongs to the category.
// chargeType is 3 * electric charge, so any non-zero value means a charged particle.
bool inCategory(const std::string& category, int pdg, int chargeType) {
    if (category == "all") return true;
    if (category == "ch") return chargeType != 0;
    if (category == "pip") return pdg == 211;
    if (category == "pim") return pdg == -211;
    if (category == "Kp") return pdg == 321;
    if (category == "Km") return pdg == -321;
    if (category == "p") return pdg == 2212;
    if (category == "pbar") return pdg == -2212;
    return false;
}

int main() {
    // Open the input file and find the tree with the generated events.
    TFile inputFile("events.root", "READ");

    if (inputFile.IsZombie()) {
        std::cerr << "Cannot open events.root" << std::endl;
        return 1;
    }

    std::cout << "File is opened" << std::endl;

    
    TTree* tree = inputFile.Get<TTree>("events");
    if (!tree) {
        std::cerr << "Tree 'events' not found" << std::endl;
        return 1;
    }
    std::cout << "Events in file: " << tree->GetEntries() << std::endl;

    // Connect the tree columns to variables. After tree->GetEntry(i) these vectors
    // hold the particles of event i (one element per particle).
    std::vector<int>* pdg = nullptr;
    std::vector<int>* chargeType = nullptr;
    std::vector<float>* pt = nullptr;
    std::vector<float>* eta = nullptr;
    std::vector<float>* phi = nullptr;

    tree->SetBranchAddress("pdg", &pdg);
    tree->SetBranchAddress("chargeType", &chargeType);
    tree->SetBranchAddress("pt", &pt);
    tree->SetBranchAddress("eta", &eta);
    tree->SetBranchAddress("phi", &phi);

    // Output file. Histograms created after this line are automatically stored in it.
    TFile outputFile("analysis.root", "RECREATE");

    // Particle categories: short names (used in histogram names) and labels for plot titles.
    // The same index c refers to the same category in all lists below.
    const std::vector<std::string> categories = {"all", "ch", "pip", "pim", "Kp", "Km", "p", "pbar"};
    const std::vector<std::string> labels = {"all particles", "charged particles",
                                             "#pi^{+}", "#pi^{#minus}", "K^{+}", "K^{#minus}", "p", "#bar{p}"};
                            
    std::vector<TH1D*> hN, hPt, hEta, hPhi;

    // Create 4 histograms for each category: hN_<name>, hPt_<name>, hEta_<name>, hPhi_<name>.
    // Bin numbers and ranges are those required by the task.
    for (size_t c = 0; c < categories.size(); c++) {
        std::string name = categories[c];
        std::string label = labels[c];

        hN.push_back(new TH1D( ("hN_" + name).c_str(), ("Multiplicity, " + label).c_str(), 400, 0, 400));
        hPt.push_back(new TH1D( ("hPt_" + name).c_str(), ("p_{T}, " + label).c_str(), 100, 0, 10));
        hEta.push_back(new TH1D( ("hEta_" + name).c_str(), ("#eta, " + label).c_str(), 100, -5, 5));
        hPhi.push_back(new TH1D( ("hPhi_" + name).c_str(), ("#phi, " + label).c_str(), 64, 0, M_PI));
    }

    // Event loop: read each event once and sort its particles into categories.
    for (int iEvent = 0; iEvent < tree->GetEntries(); iEvent++) {
        tree->GetEntry(iEvent);

        // Number of particles of each category in this event (reset for every event).
        std::vector<int> counts(categories.size(), 0);

        for (size_t i = 0; i < pdg->size(); i++) {

            // A particle can belong to several categories (e.g. pi+ is in all, ch and pip).
            for (size_t c = 0; c < categories.size(); c++) {
                if (inCategory(categories[c], pdg->at(i), chargeType->at(i))) {
                    counts[c]++;
                    hPt[c]->Fill(pt->at(i));
                    hEta[c]->Fill(eta->at(i));
                    hPhi[c]->Fill(std::abs(phi->at(i))); // |phi|: phi is in (-pi, pi], histogram range is 0-pi
                }
            }
        }
        // Multiplicity: one entry per event and category (also when the count is 0).
        for (size_t c = 0; c < categories.size(); c++) {
            hN[c]->Fill(counts[c]);
        }
    }
    // Save all histograms to analysis.root.
    outputFile.Write();
    outputFile.Close();

    return 0;
}
