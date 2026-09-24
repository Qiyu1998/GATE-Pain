# GATE-Pain

**Gamma-State Adaptive Thalamic Electrostimulation for Neuropathic Pain in Rats — Data and Code**

Data and analysis tools associated with *Closed-loop brain stimulation based on neural states bidirectionally modulates neuropathic pain in rats*.

## Contents

- `data/behavior_data/`: mechanical withdrawal thresholds in an Excel workbook.
- `data/LFPs/`: selected, preprocessed 60-second VPL local field potential (LFP) segments, organized by experimental condition, rat identifier, and stimulation phase.
- `code/online closed-loop state identification and recording/`: the supplied Qt/C++ platform for online neural-state detection, recording, and stimulation control.
- `code/offline data analysis/`: nine numbered MATLAB Live Script and Python notebook examples for LFP processing and analysis, with example inputs and figure previews. See the README in that directory for instructions and dependencies.

The original directory names and research files are retained. Files can be downloaded individually or the repository can be cloned with Git.

## Animals and experiments

The Control group contains eight rats. CCI recordings contain eight rats on postoperative day 3, eight on day 7, and five on day 14. Two animals contribute to both the Control and CCI conditions, giving 14 unique animal identifiers overall. Use the rat identifiers in the behavioral workbook to match modalities; do not infer pairing from alphabetical file order.

The same eight CCI rats underwent six stimulation strategies: open-loop (OL), gamma0-triggered closed-loop (CL), and gamma1-triggered CL stimulation, each at 40 and 130 Hz. Stimulation recordings are organized into Pre, Stim, and Post phases.

| LFP directory | Experiment | 60-s segments |
|---|---|---:|
| `01_Baseline` | Control and CCI day 3/day 7/day 14 | 396 |
| `02_40Hz_OL` | 40-Hz open-loop stimulation | 312 |
| `03_40Hz_gamma1_CL` | 40-Hz gamma1-triggered stimulation | 319 |
| `04_40Hz_gamma0_CL` | 40-Hz gamma0-triggered stimulation | 321 |
| `05_130Hz_OL` | 130-Hz open-loop stimulation | 332 |
| `06_130Hz_gamma1_CL` | 130-Hz gamma1-triggered stimulation | 323 |
| `07_130Hz_gamma0_CL` | 130-Hz gamma0-triggered stimulation | 324 |
| **Total** | | **2,327** |

## Data interpretation

- These LFP files contain selected preprocessed segments, not complete continuous raw recordings. Segment numbers do not establish temporal continuity; do not concatenate separate segments as a continuous recording.
- Read the sampling frequency from each MAT file. The released 60-second segments contain 18,000 samples at 300 Hz. Amplitude calibration is not encoded in these segment files; do not assume microvolt units.
- Residual stimulation-related spectral components may remain, including around 40 Hz and its harmonics. Preprocessing and segment selection do not establish that all residual artifacts are absent.
- Repeated segments and phases belong to the same rat and are not independent biological replicates. Primary animal-level summaries should respect the experimental pairing.
- The offline examples illustrate processing steps and include clearly identified synthetic teaching inputs where applicable. They are not a single script that reproduces every manuscript result.

The accompanying manuscript describes the experimental and statistical methods.
