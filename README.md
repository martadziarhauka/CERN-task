# CERN-task
pythia-root-task 
# Simulation of pp collisions at √s = 13.6 TeV (Pythia 8 + ROOT)

This project simulates 100 000 minimum-bias proton-proton collisions at √s = 13.6 TeV
with the Pythia 8 event generator and stores the final-state particles of every event
in a ROOT tree. Then, for 8 groups of particles (all, charged, π⁺, π⁻, K⁺, K⁻, p, p̄),
it fills histograms of four quantities:

- **multiplicity N** – the number of particles in one event,
- **pT** – transverse momentum (momentum perpendicular to the beam),
- **η** – pseudorapidity (emission angle with respect to the beam),
- **φ** – azimuthal angle (direction around the beam).

This gives 32 histograms. They are saved in `analysis.root` ; the macro `draw_plots.C`
draws each of them on a separate canvas and saves it as PNG and PDF
(with `TCanvas::SaveAs`) in the folder `plots/`.



## Requirements

- ROOT 6
- Pythia 8
- a C++ compiler with C++17 support (clang or g++)
- `make`

The tools `root-config` and `pythia8-config` must be available in the terminal (in `PATH`):
the `Makefile` uses them to find where ROOT and Pythia are installed.

**Tested with:** macOS 26.7.1 (Apple Silicon, arm64), ROOT 6.40.04, Pythia 8.318
(both installed with Homebrew), Apple clang 21.0.0.




## Files

| File | What it does |
|---|---|
| `generate.cc` | generates 100 000 events with Pythia 8 and stores the final-state particles of each event in the tree `events` in `events.root` |
| `analyze.cc` | reads `events.root`, sorts the particles into 8 groups and fills 32 histograms; saves them in `analysis.root` |
| `draw_plots.C` | ROOT macro: draws every histogram on its own canvas with axis titles, a title and a legend, and saves it as PNG and PDF in `plots/` |
| `Makefile` | build rules for `generate` and `analyze` |
| `analysis.root` | the 32 histograms (result of the analysis) |
| `plots/` | the plots: 32 PNG and 32 PDF files |
| `events.root` | **not in the repository** (238 MB, larger than the GitHub limit); it is created by `./generate` |

### The tree `events`

One entry = one event. Each branch is a vector with one value per final-state particle.
Example – the first particles of event 1:

| pdg | chargeType | pt [GeV] | eta | phi [rad] |
|---|---|---|---|---|
| 2212 (p) | 3 | 0.278 | 10.80 | −0.99 |
| 321 (K⁺) | 3 | 0.109 | −8.51 | 0.15 |
| −321 (K⁻) | −3 | 0.448 | −6.93 | 1.26 |
| 211 (π⁺) | 3 | 0.307 | −4.47 | 1.08 |
| 22 (γ) | 0 | 0.476 | −4.51 | −0.75 |

`pdg` is the PDG particle code, `chargeType` is 3 × the electric charge.









