# Simulation of pp collisions at √s = 13.6 TeV (Pythia 8 + ROOT)

This project simulates 100 000 minimum-bias proton-proton collisions at √s = 13.6 TeV
with the Pythia 8 event generator and stores the final-state particles of every event
in a ROOT tree. Then, for 8 groups of particles (all, charged, π⁺, π⁻, K⁺, K⁻, p, p̄),
it fills histograms of four quantities:

- **multiplicity N** – the number of particles in one event,
- **pT** – transverse momentum (momentum perpendicular to the beam),
- **η** – pseudorapidity (emission angle with respect to the beam),
- **φ** – azimuthal angle (direction around the beam).

This gives 32 histograms. They are saved in `analysis.root`; the macro `draw_plots.C`
draws each of them on a separate canvas and saves it as PNG and PDF
(with `TCanvas::SaveAs`) in the folder `plots/`.

## Requirements

- ROOT 6
- Pythia 8
- a C++ compiler with C++17 support (the `Makefile` uses `clang++`)
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
| `draw_plots.C` | ROOT macro: draws every histogram on its own canvas with axis titles, a title and a legend, and saves it as PNG and PDF in `plots/`; at the end it prints a table of the mean N and mean pT for each category in the terminal |
| `Makefile` | build rules for `generate` and `analyze` |
| `analysis.root` | the 32 histograms (result of the analysis) |
| `plots/` | the plots: 32 PNG and 32 PDF files |
| `events.root` | **not in the repository** (238 MB, larger than the GitHub limit); it is created by `./generate` |

### The tree `events`

One entry = one event. Each branch is a vector with one value per final-state particle.
Example for one event: the first 5 particles of event 1 (this event has 49 particles in total,
so the full entry has 49 values in each column):

| pdg | chargeType | pt [GeV] | eta | phi [rad] |
|---|---|---|---|---|
| 2212 (p) | 3 | 0.278 | 10.80 | −0.99 |
| 321 (K⁺) | 3 | 0.109 | −8.51 | 0.15 |
| −321 (K⁻) | −3 | 0.448 | −6.93 | 1.26 |
| 211 (π⁺) | 3 | 0.307 | −4.47 | 1.08 |
| 22 (γ) | 0 | 0.476 | −4.51 | −0.75 |

`pdg` is the PDG particle code, `chargeType` is 3 × the electric charge.

## How to build and run

Run all commands in the project folder.

```bash
make generate analyze           # 1. build the programs
./generate                      # 2. generate events  -> events.root   (~3 min)
./analyze                       # 3. fill histograms  -> analysis.root (~15 s)
root -l -b -q draw_plots.C      # 4. draw plots       -> plots/*.png, plots/*.pdf
```

| Step | Command | Output | Time |
|---|---|---|---|
| 1 | `make generate analyze` | programs `generate` and `analyze` | a few seconds |
| 2 | `./generate` | `events.root` (100 000 events, ~238 MB) | ~3 min |
| 3 | `./analyze` | `analysis.root` (32 histograms) | ~15 s |
| 4 | `root -l -b -q draw_plots.C` | `plots/`: 32 PNG + 32 PDF; table of mean N and mean pT in the terminal | a few seconds |

`analysis.root` is already in the repository, so to only redraw the plots you can run step 4 directly.

## Analysis choices

- **Process:** `SoftQCD:inelastic` – all inelastic collisions (non-diffractive and diffractive).
  Elastic scattering is not included: it leaves only the two beam protons and would give
  an artificial peak at N = 2.
- **Final-state particles only:** only particles with `isFinal()` are stored;
  intermediate particles that have already decayed are not used.
- **Categories:** 8 groups of particles, see the table below.
- **Angle φ:** Pythia gives φ from −π to π, but the histogram range required by the task is 0–π.
  The histograms are therefore filled with |φ|; otherwise about half of the particles would fall
  outside the range. The φ distribution is flat, so this does not change its shape.
- **Multiplicity N:** filled once per event for each category, also when the category has no particles in the event (N = 0).
- **Reproducibility:** fixed random seed (`Random:seed = 12345`). Running the chain again gives
  the same events and the same histograms (checked: all 32 histograms are identical bin by bin).

### Particle categories

The short names are used in the histogram names, e.g. `hPt_pip` is pT of π⁺.

| Category | Particles | Selection |
|---|---|---|
| `all` | all final-state particles | – |
| `ch` | charged particles | `chargeType != 0` |
| `pip`, `pim` | π⁺, π⁻ | PDG = 211, −211 |
| `Kp`, `Km` | K⁺, K⁻ | PDG = 321, −321 |
| `p`, `pbar` | p, p̄ | PDG = 2212, −2212 |

## Notes

- **N > 400:** in `hN_all` about 13% of the events (12 690 of 100 000) have more than 400 particles
  and go to the overflow bin (80 events in `hN_ch`). The range 0–400 is fixed by the task.
  Because of this, `GetMean()` of `hN_all` gives 144.5, while the true mean is 192.7 particles per event.
- **|η| > 5:** about 26% of all particles fly almost along the beam and are outside the range −5…5
  of the `hEta_*` histograms (underflow/overflow).
- **Even number of charged particles:** the number of charged particles in an event is always even
  (charge is conserved: the two protons give +2). So all bins with odd N in `hN_ch` are empty,
  which looks like a "comb" on the log scale.
- **No detector:** all final-state particles are used, at any η and pT, without the acceptance
  of a real detector. The numbers are therefore not directly comparable to LHC measurements.
- **C++17 and ROOT:** ROOT from Homebrew is built with C++20, so building with C++17 prints
  a warning about different standards. In our tests it caused no problems: the whole chain
  was built and run with C++17, and the results are reproducible.
