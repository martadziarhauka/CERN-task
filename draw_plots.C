// draw_plots.C
// Opens analysis.root, draws all 32 histograms (one per canvas)
// and saves each plot as PNG and PDF in the folder "plots".
// Run with: root -l -b -q draw_plots.C

#include <string>
#include<iostream>
#include <TFile.h>
#include <TH1D.h>
#include <vector>
#include <TCanvas.h>
#include <TSystem.h>
#include <TLegend.h>
#include <TStyle.h>
#include <TLatex.h>



void draw_plots(){
    // Open the file with the histograms made by analyze.cc.
    TFile inputFile("analysis.root", "READ");
    if (inputFile.IsZombie()) {
        std::cerr << "File couldn't be opened" << std::endl;
        return;
    }


    // Particle categories and the text shown for them in the legend (same order).
    // Quantities and their axis titles (same order).
    const std::vector<std::string> categories = {"all", "ch", "pip", "pim", "Kp", "Km", "p", "pbar"};
    const std::vector<std::string> quantities = {"hN", "hPt", "hEta", "hPhi"};
    const std::vector<std::string> xTitles = {"Multiplicity N", "p_{T} [GeV]", "#eta", "#phi [rad]"};
    const std::vector<std::string> yTitles = {"Events", "Particles", "Particles", "Particles"};
    const std::vector<std::string> labels = {"all particles", "charged particles",
                                         "#pi^{+}", "#pi^{#minus}", "K^{+}", "K^{#minus}", "p", "#bar{p}"};


    // Folder for the images (no error if it already exists).
    gSystem->mkdir("plots", true);

    // Loop over all categories and quantities: 8 x 4 = 32 plots.
    for (size_t c = 0; c < categories.size(); c++) {
        for (size_t q = 0; q < quantities.size(); q++) {

            // Histogram name, e.g. "hPt_pip".
            std::string name = quantities[q] + "_" +categories[c];

            TH1D* h  = inputFile.Get<TH1D>(name.c_str());

            // Skip the histogram if it is not in the file.
            if (!h) {
                    std::cerr << "Histogram " << name << " not found" << std::endl;
                    continue;
                }

         
            // One canvas per histogram, with a wider left margin for the y axis title.
            TCanvas canvas(("c_"+ name).c_str(),"", 800, 600);
            canvas.SetLeftMargin(0.13);
            canvas.SetGridx();
            canvas.SetGridy();
            
            h->GetXaxis()->SetTitle(xTitles[q].c_str());
            h->GetYaxis()->SetTitle(yTitles[q].c_str());
            
            // N and pT change by many orders of magnitude, so we use a log scale.
            // eta and phi are almost flat: the y axis starts at 0 so they do not
            // look bumpy, and there is free space above the curve for the legend.
            if (quantities[q] == "hN" ||  quantities[q] == "hPt" ) {
                canvas.SetLogy();
            }else {
                h->SetMinimum(0);
                h->SetMaximum(1.4 * h->GetMaximum());
            }

            
            // We draw the title ourselves (below), so that all plots have
            // titles of the same size at the same height.
            std::string title = h->GetTitle();
            h->SetTitle("");

            // No statistics box, it would cover the legend.
            h->SetStats(0);
            h->Draw();

            TLatex titleText;
            titleText.SetNDC();
            titleText.SetTextAlign(21);
            titleText.SetTextSize(0.05);
            titleText.SetTextFont(42);
            titleText.DrawLatex(0.5, 0.93, title.c_str());

            // Legend: where the data come from and which particles are shown.
            TLegend legend(0.55, 0.75, 0.88, 0.88);
            legend.SetHeader("Pythia 8, pp #sqrt{s} = 13.6 TeV");
            legend.AddEntry(h, labels[c].c_str(), "l");
            legend.Draw();

            // Save the plot in two formats.
            canvas.SaveAs(("plots/"+ name+".png" ).c_str());
            canvas.SaveAs(("plots/"+ name+".pdf" ).c_str());

        }
    }
}





