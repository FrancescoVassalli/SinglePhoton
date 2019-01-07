#include "SinglePhotonAfter.h"

#include <fun4all/Fun4AllServer.h>

#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

#include <calotrigger/CaloTriggerInfo.h>

#include <g4eval/SvtxEvalStack.h>
#include <g4eval/SvtxTrackEval.h>


#include <iostream>
#include <sstream>
#include <math.h>

SinglePhotonAfter::SinglePhotonAfter(const std::string &name) : SubsysReco("TRUTH_CONVERSION_EVAL")
{
  _foutname = name;
}

SinglePhotonAfter::~SinglePhotonAfter(){
  delete _f;
}

int SinglePhotonAfter::InitRun(PHCompositeNode *topNode)
{
  _b_event=0;
  _f = new TFile( _foutname.c_str(), "RECREATE");
  _tree = new TTree("ttree","a succulent orange tree");
  _tree->SetAutoSave(300);
  _tree->Branch("event",&_b_event); 
  _tree->Branch("hash",&_b_hash);
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

int SinglePhotonAfter::process_event(PHCompositeNode *topNode)
{
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
  //make a list of the conversions
  std::list<int> vtxList;
  std::map<int,Conversion> mapConversions;
  for ( PHG4TruthInfoContainer::ConstIterator iter = range.first; iter != range.second; ++iter ) {
    //bool isPrimary;
    PHG4Particle* g4particle = iter->second; 
    PHG4Particle* parent =truthinfo->GetParticle(g4particle->get_parent_id());
    float radius=0;
    if(!parent){ //if the parent point is null then the particle is primary 
      //checking the embed ID to make sure it is a particle I made need to change the magic literal
      if(get_embed(g4particle,truthinfo)!=2) continue;
      //isPrimary=true;
    }
    else{ //if the particle is not primary find its vertex 
      if(get_embed(parent,truthinfo)==2||(get_embed(parent,truthinfo)==3&&parent->get_pid()==22&&TMath::Abs(g4particle->get_pid())==11)){
        PHG4VtxPoint* vtx=truthinfo->GetVtx(g4particle->get_vtx_id());
        radius=sqrt(vtx->get_x()*vtx->get_x()+vtx->get_y()*vtx->get_y());
        if (radius<kTPCRADIUS) //limits to truth conversions within the tpc radius
        { 
          std::cout<<radius<<'\n';
          vtxList.push_back(vtx->get_id());
          (mapConversions[vtx->get_id()]).setElectron(g4particle);
          (mapConversions[vtx->get_id()]).setVtx(vtx);
          (mapConversions[vtx->get_id()]).setParent(parent);
          (mapConversions[vtx->get_id()]).setEmbed(get_embed(parent,truthinfo));
        }
      }
      //isPrimary=false;
    }
  }
  //record event information 

  std::queue<std::pair<int,int>> missingChildren= numUnique(&vtxList,&mapConversions,trackeval,mainClusterContainer);
  cout<<"In truth system main RawClusterContainer::size="<<mainClusterContainer->size();
  cout<<"In truth system this RawClusterContainer::size="<<_conversionClusters.size();
  //findChildren(missingChildren,truthinfo);
  //make a hash of the event number and file number 
  std::stringstream ss;
  ss<<"-"<<_b_event;             //this is where the file number is 
  _b_hash=_foutname.substr(_foutname.length()-10,5)+ss.str();

  //currently my reco can only handle single conversion events

  _tree->Fill();
  std::cout<<"Filled "<<_b_nVtx<<" vertices \n";
  _b_event++;
  delete stack;
  return 0;
}

std::queue<std::pair<int,int>> SinglePhotonAfter::numUnique(std::list<int> *l,std::map<int,Conversion> *mymap=NULL,SvtxTrackEval* trackeval=NULL,RawClusterContainer *mainClusterContainer=NULL){
  
  l->sort();
  int last=-1;
  _b_nVtx = 0;
  _b_Tpair=0;
  _b_Rpair=0;
  std::queue<std::pair<int,int>> missingChildren;
  for (std::list<int>::iterator i = l->begin(); i != l->end(); ++i) {
    //make sure the conversions are not double counted 
    if(*i!=last){
      cout<<"unique \n";
      //fill the tree
      PHG4VtxPoint *vtx =(mymap->at(*i)).getVtx(); //get the vtx
      _b_rVtx[_b_nVtx] = sqrt(vtx->get_x()*vtx->get_x()+vtx->get_y()*vtx->get_y()); //find the radius
      PHG4Particle *temp = (mymap->at(*i)).getPhoton(); //set the photon
      TLorentzVector photonTrack,electronTrack,positronTrack; //make tlv for each particle 
      photonTrack.SetPxPyPzE(temp->get_px(),temp->get_py(),temp->get_pz(),temp->get_e()); //intialize
      //fill tree values
      _b_parent_pt[_b_nVtx] =photonTrack.Pt();
      _b_parent_phi[_b_nVtx]=photonTrack.Phi();
      _b_parent_eta[_b_nVtx]=photonTrack.Eta();
      temp=(mymap->at(*i)).getElectron(); //set the first child 
      electronTrack.SetPxPyPzE(temp->get_px(),temp->get_py(),temp->get_pz(),temp->get_e());
      _b_electron_pt[_b_nVtx]=electronTrack.Pt(); //fill tree
      temp=(mymap->at(*i)).getPositron();
      if(temp){ //this will be false for 1 track events
        cout<<"2 track event \n";
        positronTrack.SetPxPyPzE(temp->get_px(),temp->get_py(),temp->get_pz(),temp->get_e()); //init the tlv
        _b_positron_pt[_b_nVtx]=positronTrack.Pt(); //fill tree
        if (TMath::Abs(electronTrack.Eta())<kRAPIDITYACCEPT&&TMath::Abs(positronTrack.Eta())<kRAPIDITYACCEPT)
        {
          cout<<"In rapidity\n";
          _b_Tpair++;
          unsigned int nRecoTracks = mymap->at(*i).setRecoTracks(trackeval); //find the reco tracks for this conversion
          int clustidtemp=-1;
          switch(nRecoTracks){
            case 2:
              _b_Rpair++;
              cout<<"reco pair \n";
              clustidtemp=mymap->at(*i).get_cluster_id(); //get the cluster id of the current conversion
              break;
            case 1:
              cout<<"one reco track\n";
              clustidtemp=mymap->at(*i).get_cluster_id(); //get the cluster id of the current conversion
              break;
            case 0:
              cout<<"no reco tracks\n";
              break;
            default:
              cout<<"Error setting reco tracks"<<endl;
          }
          if (clustidtemp>=0)
          {
            _conversionClusters.AddCluster(mainClusterContainer->getCluster(clustidtemp)); //add the calo cluster to the container
          }
        }
        else{
          cout<<"outside acceptance with electron eta:"<<electronTrack.Eta()<<" and positron:"<<positronTrack.Eta()<<'\n';
        }
      }
      else{ //fails the truth 2 track check
        cout<<"1 track event \n";
        temp=(mymap->at(*i)).getElectron(); //go back to the first track 
        _b_positron_pt[_b_nVtx]=-1; //set the second track to null
        missingChildren.push(pair<int, int>(temp->get_parent_id(),temp->get_track_id())); //add the known ids to the list missing a child
        cout<<"No pair for:\n";
        temp->identify();
        cout<<"with parent:\n";
        (mymap->at(*i)).getPhoton()->identify();
      }
      last=*i; //update the loops position in the conversion list
      if((mymap->at(*i)).getEmbed()==3){ //decide if the conversion is from pythia
        _b_pythia[_b_nVtx]=true;
      }
      else{
        _b_pythia[_b_nVtx]=false;
      }
      _b_nVtx++; //if conversion is unique record it 
    }
  }
  return missingChildren;
}

void SinglePhotonAfter::findChildren(std::queue<std::pair<int,int>> missingChildren,PHG4TruthInfoContainer* truthinfo){
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


int SinglePhotonAfter::End(PHCompositeNode *topNode)
{
  std::cout<<"Writing truth tree"<<std::endl;
  _f->Write();
  _f->Close();
  return 0;
}



