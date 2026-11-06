
#include <iostream>
#include <vector>
#include <TFile.h>
#include <TTree.h>
#include <cmath>
#include <TH1D.h>


int main() {
    TFile inputFile("events.root", "READ");

    if(inputFile.IsZombie()) {
        std::cerr << "Cannot open events.root" << std::endl;
        return 1;
    }

    std::cout << "File is opened" << std::endl;

    TTree* tree = inputFile.Get<TTree>("events");
    if(!tree) {
        std::cerr << "Tree 'events' not found" << std::endl;
        return 1;
    }
    std::cout << "Events in file: " << tree->GetEntries() << std::endl;

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

    TFile outputFile("analysis.root", "RECREATE");

    for(int iEvent = 0; iEvent < tree->GetEntries(); iEvent++) {
        tree->GetEntry(iEvent);
        
        std::cout << "Event " << iEvent << ": " << pdg->size() << " particles" << std::endl;
    }

    return 0;

}