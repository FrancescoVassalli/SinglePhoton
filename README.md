#SinglePhoton
## This repository is used to generate and analyze photon conversion in G4 sPHENIX simulations.
Everything save some of the plotting macros in the analysis folder are designed to be run on RCF
- **gen**:Macros for running G4, ocaationally need to be updated from SPHENIX/coresoftware
- **condor**: scripts for running through condor and locally modules to create events and afterburners to process them
- **PhotonConversion**: WIP for finding the conversions only using reco information
-**analysis**: used to make and plot plots about the coversions 
-**pythia**:code for generating a photon-jet background 

**Background on Photon Conversion**
- I have been using ![this ATLAS paper](https://github.com/FrancescoVassalli/SinglePhoton/files/2742038/4layerRecopT.pdf) as a reference. The relevent information starts on page 136 of the pdf

**Conversion Efficiency** 
- At begin I studied the exiting efficiency of the sPHENIX simulation to recover converted photons. These two plots were made on 12/3/18 and describe the effiency relative to radius and pT for single photon events using SvtxTrack reconstruction. This is also known as the inside out method. 
![pT](https://github.com/FrancescoVassalli/SinglePhoton/files/2742038/4layerRecopT.pdf)
![radius](https://github.com/FrancescoVassalli/SinglePhoton/files/2742038/4layerRecoR.pdf)
It may be useful to compare this to the ![ATLAS effiency](https://user-images.githubusercontent.com/31448119/50920392-7f50c280-1402-11e9-9750-d93955f9af22.png)
Note that my photons were 5-30 GeV uniform distribution while the ATLAS study was fixed 20GeV.
