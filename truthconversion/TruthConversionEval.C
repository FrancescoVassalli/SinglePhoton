#include "TruthConversionEval.h"

#include <fun4all/Fun4AllServer.h>

#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

#include <calotrigger/CaloTriggerInfo.h>

#include <calobase/RawCluster.h>
#include <calobase/RawClusterv1.h>

#include <g4eval/SvtxEvalStack.h>
#include <g4eval/SvtxTrackEval.h>


#include <iostream>
#include <math.h>

TruthConversionEval::TruthConversionEval(const std::string &name,const unsigned int runnumber, 
    const int particleEmbed, const int pythiaEmbed) : SubsysReco("TruthConversionEval"),
kRunNumber(runnumber),kParticleEmbed(particleEmbed), kPythiaEmbed(pythiaEmbed)
{
  _foutname = name;
}

TruthConversionEval::~TruthConversionEval(){
  delete _f;
}

int TruthConversionEval::InitRun(PHCompositeNode *topNode)
{
  _b_event=0;
  _f = new TFile( _foutname.c_str(), "RECREATE");
  _tree = new TTree("ttree","a succulent orange tree");
  _tree->SetAutoSave(300);
  _tree->Branch("runNumber",&kRunNumber);
  _tree->Branch("event",&_b_event); 
  _tree->Branch("nVtx", &_b_nVtx);
  _tree->Branch("nTpair", &_b_Tpair);
  _tree->Branch("nRpair", &_b_Rpair);
  _tree->Branch("rVtx", _b_rVtx,"rVtx[nVtx]/F");
  _tree->Branch("pythia", _b_pythia,"pythia[nVtx]/B");
  _tree->Branch("electron_pt", _b_electron_pt,"electron_pt[nVtx]/F");
  _tree->Branch("positron_pt", _b_positron_pt,"positron_pt[nVtx]/F");
  _tree->Branch("photon_pt",   _b_parent_pt    ,"photon_pt[nVtx]/F");
  _tree->Branch("photon_eta",  _b_parent_eta  ,"photon_eta[nVtx]/F");
  _tree->Branch("photon_phi",  _b_parent_phi  ,"photon_phi[nVtx]/F");

  return 0;
}

int TruthConversionEval::process_event(PHCompositeNode *topNode)
{
  _conversionClusters.Reset(); //clear the list of conversion clusters

  RawClusterContainer* mainClusterContainer = findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_CEMC");
  PHG4TruthInfoContainer* truthinfo = findNode::getClass<PHG4TruthInfoContainer>(topNode,"G4TruthInfo");
  PHG4TruthInfoContainer::Range range = truthinfo->GetParticleRange();
  SvtxEvalStack *stack = new SvtxEvalStack(topNode);
  SvtxTrackEval* trackeval = stack->get_track_eval();
  if (!trackeval)
  {
    cout<<"NULL track eval fatal error"<<endl;
    return 1;
  }
  //make a map of the conversions
  std::map<int,Conversion> mapConversions;
  for ( PHG4TruthInfoContainer::ConstIterator iter = range.first; iter != range.second; ++iter ) {
    PHG4Particle* g4particle = iter->second; 
    PHG4Particle* parent =truthinfo->GetParticle(g4particle->get_parent_id());
    float radius=0;
    if(!parent){ //if the parent point is null then the particle is primary 
      if(get_embed(g4particle,truthinfo)!=kParticleEmbed) continue;
    }
    else{ //if the particle is not primary find its vertex 
      if (get_embed(parent,truthinfo)==kPythiaEmbed)
      {
        cout<<"pythia hit\n";
      }
      //check that the parent is an embeded(2) photon or a pythia(3) photon that converts
      if(get_embed(parent,truthinfo)==kParticleEmbed
        ||(get_embed(parent,truthinfo)==kPythiaEmbed&&parent->get_pid()==22&&TMath::Abs(g4particle->get_pid())==11)){
        PHG4VtxPoint* vtx=truthinfo->GetVtx(g4particle->get_vtx_id()); //get the conversion vertex
        radius=sqrt(vtx->get_x()*vtx->get_x()+vtx->get_y()*vtx->get_y());
        if (radius<kTPCRADIUS) //limits to truth conversions within the tpc radius
        { 
          //initialize the conversion object -don't use constructor b/c setters have error handling
          std::cout<<"Conversion with radius [cm]:"<<radius<<'\n';
          (mapConversions[vtx->get_id()]).setElectron(g4particle);
          (mapConversions[vtx->get_id()]).setVtx(vtx);
          (mapConversions[vtx->get_id()]).setParent(parent);
          (mapConversions[vtx->get_id()]).setEmbed(get_embed(parent,truthinfo));
        }
      }
    }
  }
  //record event information 
  numUnique(&mapConversions,trackeval,mainClusterContainer);
  //std::queue<std::pair<int,int>> missingChildren= numUnique(&vtxList,&mapConversions,trackeval,mainClusterContainer);
  cout<<Name()<<"# conversion clusters="<<_conversionClusters.size()<<'\n';
  //findChildren(missingChildren,truthinfo);
  
  _tree->Fill();
  std::cout<<"Filled "<<_b_nVtx<<" vertices \n";
  _b_event++;
  delete stack;
  return 0;
}

std::queue<std::pair<int,int>> TruthConversionEval::numUnique(std::map<int,Conversion> *mymap=NULL,SvtxTrackEval* trackeval=NULL,RawClusterContainer *mainClusterContainer=NULL){

  _b_nVtx = 0;
  _b_Tpair=0;
  _b_Rpair=0;
  std::queue<std::pair<int,int>> missingChildren;
  for (std::map<int,Conversion>::iterator i = mymap->begin(); i != mymap->end(); ++i) {
    //fill the tree
    PHG4VtxPoint *vtx =i->second.getVtx(); //get the vtx
    _b_rVtx[_b_nVtx] = sqrt(vtx->get_x()*vtx->get_x()+vtx->get_y()*vtx->get_y()); //find the radius
    PHG4Particle *temp = i->second.getPhoton(); //set the photon
    TLorentzVector photonTrack,electronTrack,positronTrack; //make tlv for each particle 
    photonTrack.SetPxPyPzE(temp->get_px(),temp->get_py(),temp->get_pz(),temp->get_e()); //intialize
    //fill tree values
    _b_parent_pt[_b_nVtx] =photonTrack.Pt();
    _b_parent_phi[_b_nVtx]=photonTrack.Phi();
    _b_parent_eta[_b_nVtx]=photonTrack.Eta();
    temp=i->second.getElectron(); //set the first child 
    electronTrack.SetPxPyPzE(temp->get_px(),temp->get_py(),temp->get_pz(),temp->get_e());
    _b_electron_pt[_b_nVtx]=electronTrack.Pt(); //fill tree
    temp=i->second.getPositron();
    if(temp){ //this will be false for 1 track events
      cout<<"2 track event \n";
      positronTrack.SetPxPyPzE(temp->get_px(),temp->get_py(),temp->get_pz(),temp->get_e()); //init the tlv
      _b_positron_pt[_b_nVtx]=positronTrack.Pt(); //fill tree
      if (TMath::Abs(electronTrack.Eta())<kRAPIDITYACCEPT&&TMath::Abs(positronTrack.Eta())<kRAPIDITYACCEPT)
      {
        cout<<"In rapidity\n";
        _b_Tpair++;
        unsigned int nRecoTracks = i->second.setRecoTracks(trackeval); //find the reco tracks for this conversion
        int clustidtemp=-1;
        switch(nRecoTracks){
          case 2:
            _b_Rpair++;
            cout<<"reco pair \n";
            clustidtemp=i->second.get_cluster_id(); //get the cluster id of the current conversion
            break;
          case 1:
            cout<<"one reco track\n";
            clustidtemp=i->second.get_cluster_id(); //get the cluster id of the current conversion
            break;
          case 0:
            cout<<"no reco tracks\n";
            break;
          default:
            cout<<"Error setting reco tracks"<<endl;
        }
        RawCluster *clustemp =   dynamic_cast<RawCluster*>(mainClusterContainer->getCluster(clustidtemp)->clone());
        if(clustemp){
          clustemp->identify();
          _conversionClusters.AddCluster(clustemp); //add the calo cluster to the container
        }
      }
      else{
        cout<<"outside acceptance with electron eta:"<<electronTrack.Eta()<<" and positron:"<<positronTrack.Eta()<<'\n';
      }
    }
    else{ //fails the truth 2 track check
      cout<<"1 track event \n";
      temp=i->second.getElectron(); //go back to the first track 
      _b_positron_pt[_b_nVtx]=-1; //set the second track to null
      missingChildren.push(pair<int, int>(temp->get_parent_id(),temp->get_track_id())); //add the known ids to the list missing a child
      cout<<"No pair for:\n";
      temp->identify();
      cout<<"with parent:\n";
      i->second.getPhoton()->identify();
    }
    if(i->second.getEmbed()==3){ //decide if the conversion is from pythia
      _b_pythia[_b_nVtx]=true;
    }
    else{
      _b_pythia[_b_nVtx]=false;
    }
    _b_nVtx++; 
  }
  return missingChildren;
}

void TruthConversionEval::findChildren(std::queue<std::pair<int,int>> missingChildren,PHG4TruthInfoContainer* truthinfo){
  while(!missingChildren.empty()){
    for (PHG4TruthInfoContainer::ConstIterator iter = truthinfo->GetParticleRange().first; iter != truthinfo->GetParticleRange().second; ++iter)
    {
      if(iter->second->get_parent_id()==missingChildren.front().first&&iter->second->get_track_id()!=missingChildren.front().second){
        cout<<"Found Child:\n";
        iter->second->identify();
        cout<<"With mother:\n";

      }
    }
    missingChildren.pop();
  }
}


int TruthConversionEval::End(PHCompositeNode *topNode)
{
  std::cout<<"Writing truth tree"<<std::endl;
  _f->Write();
  _f->Close();
  return 0;
}



