#SingleParticle
## this repository is used to generate and analyze photon conversion in G4 sPHENIX simulations
**generation**: folders used to generate the events look at the readme's in the subfolder for better understanding 
- gen: the Fun4All control files used to generate events updated to the most recent coresoftware release that I have run on 
- condor: csh and submit scripts for condor to call the Fun4All modules to create events and afterburners to process them
**process**: the after burners to process the events from DSTs to useful roots 
- SinglePhotonAfter: records basic truth information in a TTree and records convesion assosiated clusters
- PhotonConversion: recover photons and record them in a vector and TTree
- ClusterShape: processes the data in the simulation to an easy TTree of cluster information which is formated by @ChaseSmith
**analysis**: used to make and plot plots about the coversions 
- analysis: contains the files used for extracting data from the root trees and plotting them
