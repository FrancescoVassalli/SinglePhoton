#include "SVReco.h"
/*#include <trackbase_historic/SvtxCluster.h>
#include <trackbase_historic/SvtxClusterMap.h>
#include <trackbase_historic/SvtxHitMap.h>*/
#include <trackbase_historic/SvtxTrack.h>
#include <trackbase_historic/SvtxVertex.h>
#include <trackbase_historic/SvtxTrackMap.h>
#include <trackbase_historic/SvtxVertexMap.h>

#include <trackbase/TrkrClusterContainer.h>
#include <trackbase/TrkrClusterv1.h>
#include <trackbase/TrkrHitSet.h>
#include <trackbase/TrkrHitSetContainer.h>
#include <trackbase/TrkrClusterHitAssoc.h>
#include <mvtx/MvtxDefs.h>
#include <intt/InttDefs.h>

#include <g4jets/JetMap.h>

#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/PHTFileServer.h>

//#include <g4detectors/PHG4CellContainer.h>
#include <g4detectors/PHG4Cell.h>
#include <g4detectors/PHG4CylinderGeomContainer.h>
#include <mvtx/CylinderGeom_Mvtx.h>
#include <intt/CylinderGeomIntt.h>

#include <g4main/PHG4Hit.h>
#include <g4main/PHG4HitContainer.h>
#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4Particle.h>

#include <phgenfit/Fitter.h>
#include <phgenfit/PlanarMeasurement.h>
#include <phgenfit/Track.h>
#include <phgenfit/SpacepointMeasurement.h>

#include <phool/getClass.h>
#include <phool/phool.h>
#include <phool/PHCompositeNode.h>
#include <phool/PHIODataNode.h>
#include <phool/PHNodeIterator.h>

#include <phgeom/PHGeomUtility.h>
#include <phfield/PHFieldUtility.h>

#include <GenFit/FieldManager.h>
#include <GenFit/GFRaveVertex.h>
#include <GenFit/GFRaveVertexFactory.h>
#include <GenFit/MeasuredStateOnPlane.h>
#include <GenFit/RKTrackRep.h>
#include <GenFit/StateOnPlane.h>
#include <GenFit/Track.h>

//#include <HFJetTruthGeneration/HFJetDefs.h>

#include <TClonesArray.h>
#include <TMatrixDSym.h>
#include <TTree.h>
#include <TVector3.h>
#include <TRandom3.h>
#include <TRotation.h>

#include <iostream>
#include <utility>
#include <memory>

#define LogDebug(exp)		std::cout<<"DEBUG: "  <<__FILE__<<": "<<__LINE__<<": "<< exp <<std::endl
#define LogError(exp)		std::cout<<"ERROR: "  <<__FILE__<<": "<<__LINE__<<": "<< exp <<std::endl
#define LogWarning(exp)	std::cout<<"WARNING: "<<__FILE__<<": "<<__LINE__<<": "<< exp <<std::endl

using namespace std;

//Rave
#include <rave/Version.h>
#include <rave/Track.h>
#include <rave/VertexFactory.h>
#include <rave/ConstantMagneticField.h>

//GenFit
#include <GenFit/GFRaveConverters.h>


/*
 * Constructor
 */
SVReco::SVReco(const string &name) :
  SubsysReco(name),
  _mag_field_file_name("/phenix/upgrades/decadal/fieldmaps/sPHENIX.2d.root"),
  _mag_field_re_scaling_factor(1.4 / 1.5),
  _reverse_mag_field(false),
  _fitter(NULL),
  _track_fitting_alg_name("DafRef"),
  _n_maps_layer(3),
  _n_intt_layer(4),
  _primary_pid_guess(211),
  _cut_jet(true),
  _cut_Ncluster(false),
  _cut_min_pT(0.1),
  _cut_dca(5.0),
  _cut_chi2_ndf(5),
  _cut_jet_pT(20.0),
  _cut_jet_eta(0.6),
  _cut_jet_R(0.4),
  _use_ladder_geom(false),
  _vertex_finder(NULL),
  //_vertexing_method("avf"),
  _vertexing_method("avf-smoothing:1"),
  _clustermap(NULL),
  _trackmap(NULL),
  _vertexmap(NULL),
  _do_eval(false),
  _eval_outname("SVReco_eval_tree.root"),
  _jetmap_name("AntiKt_Truth_r04"),
  _eval_tree(NULL),
  _do_evt_display(false){

    _event = 0;

  }

/*
 * Init
 */
int SVReco::Init(PHCompositeNode *topNode) {


  return Fun4AllReturnCodes::EVENT_OK;
}

/*
 * Init run
 */
int SVReco::InitRun(PHCompositeNode *topNode) {

  CreateNodes(topNode);

  TGeoManager* tgeo_manager = PHGeomUtility::GetTGeoManager(topNode);
  PHField * field = PHFieldUtility::GetFieldMapNode(nullptr, topNode);

  _fitter = PHGenFit::Fitter::getInstance(tgeo_manager, field,
      _track_fitting_alg_name, "RKTrackRep", _do_evt_display);

  if (!_fitter) {
    cerr << PHWHERE << endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  _vertex_finder = new genfit::GFRaveVertexFactory(Verbosity());
  _vertex_finder->setMethod(_vertexing_method.data());

  if (!_vertex_finder) {
    cerr << PHWHERE << endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  if (_do_eval) {
    if(Verbosity() >= 1)
      cout << PHWHERE << " Openning file: " << _eval_outname << endl;
    PHTFileServer::get().open(_eval_outname, "RECREATE");
    init_eval_tree();
  }

  return Fun4AllReturnCodes::EVENT_OK;
}
/*
 * process_event():
 *  Call user instructions for every event.
 *  This function contains the analysis structure.
 *
 */
int SVReco::process_event(PHCompositeNode *topNode) {
  _event++;
  if (_event % 1000 == 0)
    cout << PHWHERE << "Events processed: " << _event << endl;

  GetNodes(topNode);

  int njets = 0; 
  if (_cut_jet){
    for (JetMap::ConstIter iter=_jetmap->begin(); iter!=_jetmap->end(); ++iter)
    {
      Jet *jet = iter->second;
      if ( jet->get_pt()<_cut_jet_pT ) continue; 
      if ( fabs(jet->get_eta())>_cut_jet_eta ) continue; 
      njets++;
    }

    if (njets<1) return Fun4AllReturnCodes::ABORTEVENT;
  }

  if (_do_eval)
    reset_eval_variables();

  //! stands for Refit_GenFit_Tracks
  vector<genfit::Track*> rf_gf_tracks;
  rf_gf_tracks.clear();
  vector<PHGenFit::Track*> rf_phgf_tracks;
  rf_phgf_tracks.clear();

  svtxtrk_gftrk_map.clear();
  svtxtrk_wt_map.clear();
  svtxtrk_id.clear();

  SvtxVertex *vertex = _vertexmap->get(0);
  //iterate over all tracks
  for (SvtxTrackMap::Iter iter = _trackmap->begin(); iter != _trackmap->end();
      ++iter) {
    SvtxTrack* svtx_track = iter->second;
    // do track cuts
    if (!svtx_track)
      continue;
    if ( svtx_track->get_ndf()<40 )
      continue;
    if (!(svtx_track->get_pt()>_cut_min_pT))
      continue;
    if ((svtx_track->get_chisq()/svtx_track->get_ndf())>_cut_chi2_ndf)
      continue;
    if (fabs(svtx_track->get_dca3d_xy())>_cut_dca || fabs(svtx_track->get_dca3d_z())>_cut_dca )
      continue;

    int n_MVTX = 0, n_INTT = 0, n_TPC = 0;
    for (SvtxTrack::ConstClusterIter iter2 = svtx_track->begin_clusters(); iter2!=svtx_track->end_clusters(); iter2++) {
      //this line is buggy
      TrkrDefs::cluskey cluster_key = *iter2;
      float layer = (float) TrkrDefs::getLayer(cluster_key);

      if (layer<_n_maps_layer) n_MVTX++;
      else if (layer<_n_maps_layer+_n_intt_layer) n_INTT++;
      else n_TPC++;
    }

    if ( _cut_Ncluster && (n_MVTX<2 || n_INTT<2) ){
      continue;
    }


    if ( n_TPC<25 ) continue;


    //cout << (svtx_track->get_chisq()/svtx_track->get_ndf()) << ", " << n_TPC << ", " << svtx_track->get_pt() << endl;
    //cout << svtx_track->get_ndf() << ", " << svtx_track->size_clusters() << endl;


    PHGenFit::Track* rf_phgf_track = MakeGenFitTrack(topNode, svtx_track, vertex);

    //cout << "DONE" << endl;

    if (rf_phgf_track) {
      svtxtrk_id.push_back(svtx_track->get_id());
      svtxtrk_gftrk_map[svtx_track->get_id()] = rf_phgf_tracks.size();
      rf_phgf_tracks.push_back(rf_phgf_track);

      rf_gf_tracks.push_back(rf_phgf_track->getGenFitTrack());

    }
  }

  //! find vertex using tracks
  std::vector<genfit::GFRaveVertex*> rave_vertices;
  rave_vertices.clear();
  //!
  _vertex_finder->setMethod(_vertexing_method.data());
  if (rf_gf_tracks.size()>=2){
    try {
      _vertex_finder->findVertices(&rave_vertices, rf_gf_tracks);
    }catch (...){
      std::cout << PHWHERE << "GFRaveVertexFactory::findVertices failed!";
    }
  }

  FillVertexMap(rave_vertices, rf_gf_tracks);

  //! change the vertex finding method
  _vertex_finder->setMethod("avr-smoothing:1");
  //_vertex_finder->setMethod("avr");
  vector<genfit::GFRaveVertex*> rave_vertices_jet;
  rave_vertices_jet.clear();

  //! scan jetmap
  //here
  for (JetMap::ConstIter iter=_jetmap->begin(); iter!=_jetmap->end(); iter++)
  {
    Jet *jet = iter->second;

    float jet_pT = jet->get_pt();
    float jet_eta = jet->get_eta();
    float jet_phi = jet->get_phi();

    float jet_px = jet->get_px(); 
    float jet_py = jet->get_py();
    float jet_pz = jet->get_pz();

    TVector3 vec_jet(jet_px,jet_py,jet_pz);

    if ( jet_pT<_cut_jet_pT ) continue;
    if ( fabs(jet_eta)>_cut_jet_eta ) continue;

    int counter_r10 = 0, counter_miss = 0;

    vector<genfit::Track*> rf_gf_tracks_jet;
    rf_gf_tracks_jet.clear();
    vector<genfit::Track*> rf_gf_tracks_jet_pT05;
    rf_gf_tracks_jet_pT05.clear();
    vector<genfit::Track*> rf_gf_tracks_jet_pT10;
    rf_gf_tracks_jet_pT10.clear();
    vector<genfit::Track*> rf_gf_tracks_jet_pT15;
    rf_gf_tracks_jet_pT15.clear();
    vector<genfit::Track*> rf_gf_tracks_jet_pT20;
    rf_gf_tracks_jet_pT20.clear();

    //cout << "JET ETA: " << jet_eta << ", JET PHI: " << jet_phi << endl;

    for (SvtxTrackMap::ConstIter iter2=_trackmap->begin(); iter2!=_trackmap->end(); iter2++)
    {
      SvtxTrack* svtx_track = iter2->second;

      float trk_phi = svtx_track->get_phi();
      float trk_eta = svtx_track->get_eta();
      float trk_pT = svtx_track->get_pt();

      float sin_phi = sin(jet_phi - trk_phi);
      float cos_phi = cos(jet_phi - trk_phi);
      float dphi = atan2(sin_phi, cos_phi);

      /*
         if ( fabs(dphi-(jet_phi-trk_phi))>0.01 ){
         cout << "dphi: " << jet_phi - trk_phi << ", " << dphi << endl;
         }
         */
      //cout << "TRK ETA: " << trk_eta << ", TRK PHI: " << trk_phi << endl;

      if (sqrt((jet_eta-trk_eta)*(jet_eta-trk_eta) + dphi*dphi)<1.0)
      {
        counter_r10++;
      }

      if (sqrt((jet_eta-trk_eta)*(jet_eta-trk_eta) + dphi*dphi)>_cut_jet_R) continue;

      if (svtxtrk_gftrk_map.find(svtx_track->get_id())!=svtxtrk_gftrk_map.end()){
        unsigned int trk_index = svtxtrk_gftrk_map[svtx_track->get_id()];
        //unsigned int nclus_mvtx = svtxtrk_nmvtx_map[svtx_track->get_id()];
        //unsigned int nclus_intt = svtxtrk_nintt_map[svtx_track->get_id()];
        //unsigned int nfirst = svtxtrk_nfirst_map[svtx_track->get_id()];

        //cout << "NCLUS MVTX: " << nclus_mvtx << endl;

        PHGenFit::Track* rf_phgf_track = rf_phgf_tracks[trk_index];

        rf_gf_tracks_jet.push_back(rf_phgf_track->getGenFitTrack());
        if (trk_pT>0.5) rf_gf_tracks_jet_pT05.push_back(rf_phgf_track->getGenFitTrack());
        if (trk_pT>1.0) rf_gf_tracks_jet_pT10.push_back(rf_phgf_track->getGenFitTrack());
        if (trk_pT>1.5) rf_gf_tracks_jet_pT15.push_back(rf_phgf_track->getGenFitTrack());
        if (trk_pT>2.0) rf_gf_tracks_jet_pT20.push_back(rf_phgf_track->getGenFitTrack());
      }else{
        counter_miss++;
      }
    }//trackmap

    //! SV reco
    //!
    if (rf_gf_tracks_jet.size()>1){
      try{
        _vertex_finder->findVertices(&rave_vertices_jet, rf_gf_tracks_jet);
      }catch (...){
        std::cout << PHWHERE << "GFRaveVertexFactory::findVertices failed!";
      }
    }

    //cout << "N MISS: " << counter_miss << endl;
    cout << "JET PT: " << jet_pT << ", N TRK: " << int(jet->size_comp()) << ", CUT10: " << counter_r10 << ", CUT04: " << rf_gf_tracks_jet.size() << ", N VTX: " << rave_vertices_jet.size() << endl;

    if (_do_eval){
      rv_sv_pT00_nvtx[rv_sv_njets] = rave_vertices_jet.size();
      for (int ivtx=0; ivtx<int(rave_vertices_jet.size()); ivtx++){
        genfit::GFRaveVertex* rave_vtx = rave_vertices_jet[ivtx];
        rv_sv_pT00_vtx_x[rv_sv_njets][ivtx] = rave_vtx->getPos().X();
        rv_sv_pT00_vtx_y[rv_sv_njets][ivtx] = rave_vtx->getPos().Y();
        rv_sv_pT00_vtx_z[rv_sv_njets][ivtx] = rave_vtx->getPos().Z();

        rv_sv_pT00_vtx_ex[rv_sv_njets][ivtx] = sqrt(rave_vtx->getCov()[0][0]);
        rv_sv_pT00_vtx_ey[rv_sv_njets][ivtx] = sqrt(rave_vtx->getCov()[1][1]);
        rv_sv_pT00_vtx_ez[rv_sv_njets][ivtx] = sqrt(rave_vtx->getCov()[2][2]);

        rv_sv_pT00_vtx_ntrk[rv_sv_njets][ivtx] = (int)rave_vtx->getNTracks();

        float vtx_mass, vtx_px, vtx_py, vtx_pz;
        int ntrk_good_pv = 0;
        rv_sv_pT00_vtx_ntrk_good[rv_sv_njets][ivtx] = GetSVMass_mom(rave_vtx,vtx_mass,vtx_px,vtx_py,vtx_pz,ntrk_good_pv);
        rv_sv_pT00_vtx_ntrk_good_pv[rv_sv_njets][ivtx] = ntrk_good_pv;

        //cout << "N TRK: " << rv_sv_pT00_vtx_ntrk[rv_sv_njets][ivtx] << ", GOOD: " << rv_sv_pT00_vtx_ntrk_good[rv_sv_njets][ivtx] << endl;

        TVector3 vec1(vtx_px, vtx_py, vtx_pz);
        TVector3 vec2(rv_sv_pT00_vtx_x[rv_sv_njets][ivtx]-rv_prim_vtx[0], rv_sv_pT00_vtx_y[rv_sv_njets][ivtx]-rv_prim_vtx[1], rv_sv_pT00_vtx_z[rv_sv_njets][ivtx]-rv_prim_vtx[2]);
        float theta = vec1.Angle(vec2);
        float A = vec1.Mag()*sin(theta);
        float vtx_mass_corr = sqrt(vtx_mass*vtx_mass + A*A) + A; 

        rv_sv_pT00_vtx_mass[rv_sv_njets][ivtx] = vtx_mass;
        rv_sv_pT00_vtx_mass_corr[rv_sv_njets][ivtx] = vtx_mass_corr;
        rv_sv_pT00_vtx_pT[rv_sv_njets][ivtx] = sqrt(vtx_px*vtx_px + vtx_py*vtx_py);

        float theta_jet = vec_jet.Angle(vec2);
        rv_sv_pT00_vtx_jet_theta[rv_sv_njets][ivtx] = theta_jet;

        rv_sv_pT00_vtx_chi2[rv_sv_njets][ivtx] = rave_vtx->getChi2();
        rv_sv_pT00_vtx_ndf[rv_sv_njets][ivtx] = rave_vtx->getNdf();
      }
    }

    for (genfit::GFRaveVertex* vertex: rave_vertices_jet){
      delete vertex;
    }
    rave_vertices_jet.clear();

    //! 
    if (rf_gf_tracks_jet_pT05.size()>1){
      try{
        _vertex_finder->findVertices(&rave_vertices_jet, rf_gf_tracks_jet_pT05);
      }catch (...){
        std::cout << PHWHERE << "GFRaveVertexFactory::findVertices failed!";
      }
    }

    if (_do_eval){
      rv_sv_pT05_nvtx[rv_sv_njets] = rave_vertices_jet.size();
      for (unsigned int ivtx=0; ivtx<rave_vertices_jet.size(); ++ivtx){
        genfit::GFRaveVertex* rave_vtx = rave_vertices_jet[ivtx];
        rv_sv_pT05_vtx_x[rv_sv_njets][ivtx] = rave_vtx->getPos().X();
        rv_sv_pT05_vtx_y[rv_sv_njets][ivtx] = rave_vtx->getPos().Y();
        rv_sv_pT05_vtx_z[rv_sv_njets][ivtx] = rave_vtx->getPos().Z();

        rv_sv_pT05_vtx_ex[rv_sv_njets][ivtx] = sqrt(rave_vtx->getCov()[0][0]);
        rv_sv_pT05_vtx_ey[rv_sv_njets][ivtx] = sqrt(rave_vtx->getCov()[1][1]);
        rv_sv_pT05_vtx_ez[rv_sv_njets][ivtx] = sqrt(rave_vtx->getCov()[2][2]);

        rv_sv_pT05_vtx_ntrk[rv_sv_njets][ivtx] = (int)rave_vtx->getNTracks();

        float vtx_mass, vtx_px, vtx_py, vtx_pz;
        int ntrk_good_pv = 0;
        rv_sv_pT05_vtx_ntrk_good[rv_sv_njets][ivtx] = GetSVMass_mom(rave_vtx,vtx_mass,vtx_px,vtx_py,vtx_pz,ntrk_good_pv);
        rv_sv_pT05_vtx_ntrk_good_pv[rv_sv_njets][ivtx] = ntrk_good_pv;

        TVector3 vec1(vtx_px, vtx_py, vtx_pz);
        TVector3 vec2(rv_sv_pT05_vtx_x[rv_sv_njets][ivtx]-rv_prim_vtx[0], rv_sv_pT05_vtx_y[rv_sv_njets][ivtx]-rv_prim_vtx[1], rv_sv_pT05_vtx_z[rv_sv_njets][ivtx]-rv_prim_vtx[2]);
        float theta = vec1.Angle(vec2);
        float A = vec1.Mag()*sin(theta);
        float vtx_mass_corr = sqrt(vtx_mass*vtx_mass + A*A) + A; 

        rv_sv_pT05_vtx_mass[rv_sv_njets][ivtx] = vtx_mass;
        rv_sv_pT05_vtx_mass_corr[rv_sv_njets][ivtx] = vtx_mass_corr;
        rv_sv_pT05_vtx_pT[rv_sv_njets][ivtx] = sqrt(vtx_px*vtx_px + vtx_py*vtx_py);

        float theta_jet = vec_jet.Angle(vec2);
        rv_sv_pT05_vtx_jet_theta[rv_sv_njets][ivtx] = theta_jet;

        rv_sv_pT05_vtx_chi2[rv_sv_njets][ivtx] = rave_vtx->getChi2();
        rv_sv_pT05_vtx_ndf[rv_sv_njets][ivtx] = rave_vtx->getNdf();
      }
    }

    for (genfit::GFRaveVertex* vertex: rave_vertices_jet){
      delete vertex;
    }
    rave_vertices_jet.clear();

    //! 
    if (rf_gf_tracks_jet_pT10.size()>1){
      try{
        _vertex_finder->findVertices(&rave_vertices_jet, rf_gf_tracks_jet_pT10);
      }catch (...){
        std::cout << PHWHERE << "GFRaveVertexFactory::findVertices failed!";
      }
    }

    if (_do_eval){
      rv_sv_pT10_nvtx[rv_sv_njets] = rave_vertices_jet.size();
      for (unsigned int ivtx=0; ivtx<rave_vertices_jet.size(); ++ivtx){
        genfit::GFRaveVertex* rave_vtx = rave_vertices_jet[ivtx];
        rv_sv_pT10_vtx_x[rv_sv_njets][ivtx] = rave_vtx->getPos().X();
        rv_sv_pT10_vtx_y[rv_sv_njets][ivtx] = rave_vtx->getPos().Y();
        rv_sv_pT10_vtx_z[rv_sv_njets][ivtx] = rave_vtx->getPos().Z();

        rv_sv_pT10_vtx_ex[rv_sv_njets][ivtx] = sqrt(rave_vtx->getCov()[0][0]);
        rv_sv_pT10_vtx_ey[rv_sv_njets][ivtx] = sqrt(rave_vtx->getCov()[1][1]);
        rv_sv_pT10_vtx_ez[rv_sv_njets][ivtx] = sqrt(rave_vtx->getCov()[2][2]);

        rv_sv_pT10_vtx_ntrk[rv_sv_njets][ivtx] = (int)rave_vtx->getNTracks();

        float vtx_mass, vtx_px, vtx_py, vtx_pz;
        int ntrk_good_pv = 0;
        rv_sv_pT10_vtx_ntrk_good[rv_sv_njets][ivtx] = GetSVMass_mom(rave_vtx,vtx_mass,vtx_px,vtx_py,vtx_pz,ntrk_good_pv);
        rv_sv_pT10_vtx_ntrk_good_pv[rv_sv_njets][ivtx] = ntrk_good_pv;

        TVector3 vec1(vtx_px, vtx_py, vtx_pz);
        TVector3 vec2(rv_sv_pT10_vtx_x[rv_sv_njets][ivtx]-rv_prim_vtx[0], rv_sv_pT10_vtx_y[rv_sv_njets][ivtx]-rv_prim_vtx[1], rv_sv_pT10_vtx_z[rv_sv_njets][ivtx]-rv_prim_vtx[2]);
        float theta = vec1.Angle(vec2);
        float A = vec1.Mag()*sin(theta);
        float vtx_mass_corr = sqrt(vtx_mass*vtx_mass + A*A) + A; 

        rv_sv_pT10_vtx_mass[rv_sv_njets][ivtx] = vtx_mass;
        rv_sv_pT10_vtx_mass_corr[rv_sv_njets][ivtx] = vtx_mass_corr;
        rv_sv_pT10_vtx_pT[rv_sv_njets][ivtx] = sqrt(vtx_px*vtx_px + vtx_py*vtx_py);

        float theta_jet = vec_jet.Angle(vec2);
        rv_sv_pT10_vtx_jet_theta[rv_sv_njets][ivtx] = theta_jet;

        rv_sv_pT10_vtx_chi2[rv_sv_njets][ivtx] = rave_vtx->getChi2();
        rv_sv_pT10_vtx_ndf[rv_sv_njets][ivtx] = rave_vtx->getNdf();
      }
    }

    for (genfit::GFRaveVertex* vertex: rave_vertices_jet){
      delete vertex;
    }
    rave_vertices_jet.clear();

    //! 
    if (rf_gf_tracks_jet_pT15.size()>1){
      try{
        _vertex_finder->findVertices(&rave_vertices_jet, rf_gf_tracks_jet_pT15);
      }catch (...){
        std::cout << PHWHERE << "GFRaveVertexFactory::findVertices failed!";
      }
    }

    if (_do_eval){
      rv_sv_pT15_nvtx[rv_sv_njets] = rave_vertices_jet.size();
      for (unsigned int ivtx=0; ivtx<rave_vertices_jet.size(); ++ivtx){
        genfit::GFRaveVertex* rave_vtx = rave_vertices_jet[ivtx];
        rv_sv_pT15_vtx_x[rv_sv_njets][ivtx] = rave_vtx->getPos().X();
        rv_sv_pT15_vtx_y[rv_sv_njets][ivtx] = rave_vtx->getPos().Y();
        rv_sv_pT15_vtx_z[rv_sv_njets][ivtx] = rave_vtx->getPos().Z();

        rv_sv_pT15_vtx_ex[rv_sv_njets][ivtx] = sqrt(rave_vtx->getCov()[0][0]);
        rv_sv_pT15_vtx_ey[rv_sv_njets][ivtx] = sqrt(rave_vtx->getCov()[1][1]);
        rv_sv_pT15_vtx_ez[rv_sv_njets][ivtx] = sqrt(rave_vtx->getCov()[2][2]);

        rv_sv_pT15_vtx_ntrk[rv_sv_njets][ivtx] = (int)rave_vtx->getNTracks();

        float vtx_mass, vtx_px, vtx_py, vtx_pz;
        int ntrk_good_pv = 0;
        rv_sv_pT15_vtx_ntrk_good[rv_sv_njets][ivtx] = GetSVMass_mom(rave_vtx,vtx_mass,vtx_px,vtx_py,vtx_pz,ntrk_good_pv);
        rv_sv_pT15_vtx_ntrk_good_pv[rv_sv_njets][ivtx] = ntrk_good_pv;

        TVector3 vec1(vtx_px, vtx_py, vtx_pz);
        TVector3 vec2(rv_sv_pT15_vtx_x[rv_sv_njets][ivtx]-rv_prim_vtx[0], rv_sv_pT15_vtx_y[rv_sv_njets][ivtx]-rv_prim_vtx[1], rv_sv_pT15_vtx_z[rv_sv_njets][ivtx]-rv_prim_vtx[2]);
        float theta = vec1.Angle(vec2);
        float A = vec1.Mag()*sin(theta);
        float vtx_mass_corr = sqrt(vtx_mass*vtx_mass + A*A) + A; 

        rv_sv_pT15_vtx_mass[rv_sv_njets][ivtx] = vtx_mass;
        rv_sv_pT15_vtx_mass_corr[rv_sv_njets][ivtx] = vtx_mass_corr;
        rv_sv_pT15_vtx_pT[rv_sv_njets][ivtx] = sqrt(vtx_px*vtx_px + vtx_py*vtx_py);

        float theta_jet = vec_jet.Angle(vec2);
        rv_sv_pT15_vtx_jet_theta[rv_sv_njets][ivtx] = theta_jet;

        rv_sv_pT15_vtx_chi2[rv_sv_njets][ivtx] = rave_vtx->getChi2();
        rv_sv_pT15_vtx_ndf[rv_sv_njets][ivtx] = rave_vtx->getNdf();
      }
    }

    for (genfit::GFRaveVertex* vertex: rave_vertices_jet){
      delete vertex;
    }
    rave_vertices_jet.clear();

    //! jet information
    rv_sv_jet_id[rv_sv_njets] = jet->get_id();
    rv_sv_jet_pT[rv_sv_njets] = jet_pT;
    rv_sv_jet_px[rv_sv_njets] = jet->get_px();
    rv_sv_jet_py[rv_sv_njets] = jet->get_py();
    rv_sv_jet_pz[rv_sv_njets] = jet->get_pz();
    /*
       if (jet->has_property(static_cast<Jet::PROPERTY>(prop_JetPartonFlavor))){
       rv_sv_jet_prop[rv_sv_njets][0] = int(jet->get_property(static_cast<Jet::PROPERTY>(prop_JetPartonFlavor)));
       rv_sv_jet_prop[rv_sv_njets][1] = int(jet->get_property(static_cast<Jet::PROPERTY>(prop_JetHadronFlavor)));
       }    
       */

    rv_sv_njets++;

  }//jetmap


  for (genfit::GFRaveVertex* vertex: rave_vertices_jet){
    delete vertex;
  }
  rave_vertices_jet.clear();

  //! Fill evaluation tree
  if (_do_eval){
    _eval_tree->Fill();
  }

  //! Clean up
  for (PHGenFit::Track* track: rf_phgf_tracks){
    delete track;
  }
  rf_phgf_tracks.clear();

  for (genfit::GFRaveVertex* vertex: rave_vertices){
    delete vertex;
  }
  rave_vertices.clear();

  return Fun4AllReturnCodes::EVENT_OK;
}

int SVReco::End(PHCompositeNode *topNode){

  if (_do_eval){
    if(Verbosity() >= 1)
      cout << PHWHERE << " Writing to file: " << _eval_outname << endl;
    PHTFileServer::get().cd(_eval_outname);
    _eval_tree->Write();
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

SVReco::~SVReco(){
  delete _fitter;
  delete _vertex_finder;
}

void SVReco::init_eval_tree(){

  _eval_tree = new TTree("T","SVReco evaluation");
  _eval_tree->Branch("gf_prim_vtx",gf_prim_vtx,"gf_prim_vtx[3]/F");
  _eval_tree->Branch("gf_prim_vtx_err",gf_prim_vtx_err,"gf_prim_vtx_err[3]/F");
  _eval_tree->Branch("gf_prim_vtx_ntrk",&gf_prim_vtx_ntrk,"gf_prim_vtx_ntrk/I");
  _eval_tree->Branch("rv_prim_vtx",rv_prim_vtx,"rv_prim_vtx[3]/F");
  _eval_tree->Branch("rv_prim_vtx_err",rv_prim_vtx_err,"rv_prim_vtx_err[3]/F");
  _eval_tree->Branch("rv_prim_vtx_ntrk",&rv_prim_vtx_ntrk,"rv_prim_vtx_ntrk/I");

  _eval_tree->Branch("rv_sv_njets",&rv_sv_njets,"rv_sv_njets/I");
  _eval_tree->Branch("rv_sv_jet_id",rv_sv_jet_id,"rv_sv_jet_id[rv_sv_njets]/I");
  _eval_tree->Branch("rv_sv_jet_prop",rv_sv_jet_prop,"rv_sv_jet_prop[rv_sv_njets][2]/I");
  _eval_tree->Branch("rv_sv_jet_pT",rv_sv_jet_pT,"rv_sv_jet_pT[rv_sv_njets]/F");
  _eval_tree->Branch("rv_sv_jet_px",rv_sv_jet_px,"rv_sv_jet_px[rv_sv_njets]/F");
  _eval_tree->Branch("rv_sv_jet_py",rv_sv_jet_py,"rv_sv_jet_py[rv_sv_njets]/F");
  _eval_tree->Branch("rv_sv_jet_pz",rv_sv_jet_pz,"rv_sv_jet_pz[rv_sv_njets]/F");

  _eval_tree->Branch("rv_sv_pT00_nvtx",rv_sv_pT00_nvtx,"rv_sv_pT00_nvtx[rv_sv_njets]/I");
  _eval_tree->Branch("rv_sv_pT00_vtx_x",rv_sv_pT00_vtx_x,"rv_sv_pT00_vtx_x[rv_sv_njets][30]/F");
  _eval_tree->Branch("rv_sv_pT00_vtx_y",rv_sv_pT00_vtx_y,"rv_sv_pT00_vtx_y[rv_sv_njets][30]/F");
  _eval_tree->Branch("rv_sv_pT00_vtx_z",rv_sv_pT00_vtx_z,"rv_sv_pT00_vtx_z[rv_sv_njets][30]/F");
  _eval_tree->Branch("rv_sv_pT00_vtx_ex",rv_sv_pT00_vtx_ex,"rv_sv_pT00_vtx_ex[rv_sv_njets][30]/F");
  _eval_tree->Branch("rv_sv_pT00_vtx_ey",rv_sv_pT00_vtx_ey,"rv_sv_pT00_vtx_ey[rv_sv_njets][30]/F");
  _eval_tree->Branch("rv_sv_pT00_vtx_ez",rv_sv_pT00_vtx_ez,"rv_sv_pT00_vtx_ez[rv_sv_njets][30]/F");
  _eval_tree->Branch("rv_sv_pT00_vtx_ntrk",rv_sv_pT00_vtx_ntrk,"rv_sv_pT00_vtx_ntrk[rv_sv_njets][30]/I");
  _eval_tree->Branch("rv_sv_pT00_vtx_ntrk_good",rv_sv_pT00_vtx_ntrk_good,"rv_sv_pT00_vtx_ntrk_good[rv_sv_njets][30]/I");
  _eval_tree->Branch("rv_sv_pT00_vtx_ntrk_good_pv",rv_sv_pT00_vtx_ntrk_good_pv,"rv_sv_pT00_vtx_ntrk_good_pv[rv_sv_njets][30]/I");
  _eval_tree->Branch("rv_sv_pT00_vtx_mass",rv_sv_pT00_vtx_mass,"rv_sv_pT00_vtx_mass[rv_sv_njets][30]/F");
  _eval_tree->Branch("rv_sv_pT00_vtx_mass_corr",rv_sv_pT00_vtx_mass_corr,"rv_sv_pT00_vtx_mass_corr[rv_sv_njets][30]/F");
  _eval_tree->Branch("rv_sv_pT00_vtx_pT",rv_sv_pT00_vtx_pT,"rv_sv_pT00_vtx_pT[rv_sv_njets][30]/F");
  _eval_tree->Branch("rv_sv_pT00_vtx_jet_theta",rv_sv_pT00_vtx_jet_theta,"rv_sv_pT00_vtx_jet_theta[rv_sv_njets][30]/F");
  _eval_tree->Branch("rv_sv_pT00_vtx_chi2",rv_sv_pT00_vtx_chi2,"rv_sv_pT00_vtx_chi2[rv_sv_njets][30]/F");
  _eval_tree->Branch("rv_sv_pT00_vtx_ndf",rv_sv_pT00_vtx_ndf,"rv_sv_pT00_vtx_ndf[rv_sv_njets][30]/F");

  _eval_tree->Branch("rv_sv_pT05_nvtx",rv_sv_pT05_nvtx,"rv_sv_pT05_nvtx[rv_sv_njets]/I");
  _eval_tree->Branch("rv_sv_pT05_vtx_x",rv_sv_pT05_vtx_x,"rv_sv_pT05_vtx_x[rv_sv_njets][30]/F");
  _eval_tree->Branch("rv_sv_pT05_vtx_y",rv_sv_pT05_vtx_y,"rv_sv_pT05_vtx_y[rv_sv_njets][30]/F");
  _eval_tree->Branch("rv_sv_pT05_vtx_z",rv_sv_pT05_vtx_z,"rv_sv_pT05_vtx_z[rv_sv_njets][30]/F");
  _eval_tree->Branch("rv_sv_pT05_vtx_ex",rv_sv_pT05_vtx_ex,"rv_sv_pT05_vtx_ex[rv_sv_njets][30]/F");
  _eval_tree->Branch("rv_sv_pT05_vtx_ey",rv_sv_pT05_vtx_ey,"rv_sv_pT05_vtx_ey[rv_sv_njets][30]/F");
  _eval_tree->Branch("rv_sv_pT05_vtx_ez",rv_sv_pT05_vtx_ez,"rv_sv_pT05_vtx_ez[rv_sv_njets][30]/F");
  _eval_tree->Branch("rv_sv_pT05_vtx_ntrk",rv_sv_pT05_vtx_ntrk,"rv_sv_pT05_vtx_ntrk[rv_sv_njets][30]/I");
  _eval_tree->Branch("rv_sv_pT05_vtx_ntrk_good",rv_sv_pT05_vtx_ntrk_good,"rv_sv_pT05_vtx_ntrk_good[rv_sv_njets][30]/I");
  _eval_tree->Branch("rv_sv_pT05_vtx_ntrk_good_pv",rv_sv_pT05_vtx_ntrk_good_pv,"rv_sv_pT05_vtx_ntrk_good_pv[rv_sv_njets][30]/I");
  _eval_tree->Branch("rv_sv_pT05_vtx_mass",rv_sv_pT05_vtx_mass,"rv_sv_pT05_vtx_mass[rv_sv_njets][30]/F");
  _eval_tree->Branch("rv_sv_pT05_vtx_mass_corr",rv_sv_pT05_vtx_mass_corr,"rv_sv_pT05_vtx_mass_corr[rv_sv_njets][30]/F");
  _eval_tree->Branch("rv_sv_pT05_vtx_pT",rv_sv_pT05_vtx_pT,"rv_sv_pT05_vtx_pT[rv_sv_njets][30]/F");
  _eval_tree->Branch("rv_sv_pT05_vtx_jet_theta",rv_sv_pT05_vtx_jet_theta,"rv_sv_pT05_vtx_jet_theta[rv_sv_njets][30]/F");
  _eval_tree->Branch("rv_sv_pT05_vtx_chi2",rv_sv_pT05_vtx_chi2,"rv_sv_pT05_vtx_chi2[rv_sv_njets][30]/F");
  _eval_tree->Branch("rv_sv_pT05_vtx_ndf",rv_sv_pT05_vtx_ndf,"rv_sv_pT05_vtx_ndf[rv_sv_njets][30]/F");

  _eval_tree->Branch("rv_sv_pT10_nvtx",rv_sv_pT10_nvtx,"rv_sv_pT10_nvtx[rv_sv_njets]/I");
  _eval_tree->Branch("rv_sv_pT10_vtx_x",rv_sv_pT10_vtx_x,"rv_sv_pT10_vtx_x[rv_sv_njets][30]/F");
  _eval_tree->Branch("rv_sv_pT10_vtx_y",rv_sv_pT10_vtx_y,"rv_sv_pT10_vtx_y[rv_sv_njets][30]/F");
  _eval_tree->Branch("rv_sv_pT10_vtx_z",rv_sv_pT10_vtx_z,"rv_sv_pT10_vtx_z[rv_sv_njets][30]/F");
  _eval_tree->Branch("rv_sv_pT10_vtx_ex",rv_sv_pT10_vtx_ex,"rv_sv_pT10_vtx_ex[rv_sv_njets][30]/F");
  _eval_tree->Branch("rv_sv_pT10_vtx_ey",rv_sv_pT10_vtx_ey,"rv_sv_pT10_vtx_ey[rv_sv_njets][30]/F");
  _eval_tree->Branch("rv_sv_pT10_vtx_ez",rv_sv_pT10_vtx_ez,"rv_sv_pT10_vtx_ez[rv_sv_njets][30]/F");
  _eval_tree->Branch("rv_sv_pT10_vtx_ntrk",rv_sv_pT10_vtx_ntrk,"rv_sv_pT10_vtx_ntrk[rv_sv_njets][30]/I");
  _eval_tree->Branch("rv_sv_pT10_vtx_ntrk_good",rv_sv_pT10_vtx_ntrk_good,"rv_sv_pT10_vtx_ntrk_good[rv_sv_njets][30]/I");
  _eval_tree->Branch("rv_sv_pT10_vtx_ntrk_good_pv",rv_sv_pT10_vtx_ntrk_good_pv,"rv_sv_pT10_vtx_ntrk_good_pv[rv_sv_njets][30]/I");
  _eval_tree->Branch("rv_sv_pT10_vtx_mass",rv_sv_pT10_vtx_mass,"rv_sv_pT10_vtx_mass[rv_sv_njets][30]/F");
  _eval_tree->Branch("rv_sv_pT10_vtx_mass_corr",rv_sv_pT10_vtx_mass_corr,"rv_sv_pT10_vtx_mass_corr[rv_sv_njets][30]/F");
  _eval_tree->Branch("rv_sv_pT10_vtx_pT",rv_sv_pT10_vtx_pT,"rv_sv_pT10_vtx_pT[rv_sv_njets][30]/F");
  _eval_tree->Branch("rv_sv_pT10_vtx_jet_theta",rv_sv_pT10_vtx_jet_theta,"rv_sv_pT10_vtx_jet_theta[rv_sv_njets][30]/F");
  _eval_tree->Branch("rv_sv_pT10_vtx_chi2",rv_sv_pT10_vtx_chi2,"rv_sv_pT10_vtx_chi2[rv_sv_njets][30]/F");
  _eval_tree->Branch("rv_sv_pT10_vtx_ndf",rv_sv_pT10_vtx_ndf,"rv_sv_pT10_vtx_ndf[rv_sv_njets][30]/F");

  _eval_tree->Branch("rv_sv_pT15_nvtx",rv_sv_pT15_nvtx,"rv_sv_pT15_nvtx[rv_sv_njets]/I");
  _eval_tree->Branch("rv_sv_pT15_vtx_x",rv_sv_pT15_vtx_x,"rv_sv_pT15_vtx_x[rv_sv_njets][30]/F");
  _eval_tree->Branch("rv_sv_pT15_vtx_y",rv_sv_pT15_vtx_y,"rv_sv_pT15_vtx_y[rv_sv_njets][30]/F");
  _eval_tree->Branch("rv_sv_pT15_vtx_z",rv_sv_pT15_vtx_z,"rv_sv_pT15_vtx_z[rv_sv_njets][30]/F");
  _eval_tree->Branch("rv_sv_pT15_vtx_ex",rv_sv_pT15_vtx_ex,"rv_sv_pT15_vtx_ex[rv_sv_njets][30]/F");
  _eval_tree->Branch("rv_sv_pT15_vtx_ey",rv_sv_pT15_vtx_ey,"rv_sv_pT15_vtx_ey[rv_sv_njets][30]/F");
  _eval_tree->Branch("rv_sv_pT15_vtx_ez",rv_sv_pT15_vtx_ez,"rv_sv_pT15_vtx_ez[rv_sv_njets][30]/F");
  _eval_tree->Branch("rv_sv_pT15_vtx_ntrk",rv_sv_pT15_vtx_ntrk,"rv_sv_pT15_vtx_ntrk[rv_sv_njets][30]/I");
  _eval_tree->Branch("rv_sv_pT15_vtx_ntrk_good",rv_sv_pT15_vtx_ntrk_good,"rv_sv_pT15_vtx_ntrk_good[rv_sv_njets][30]/I");
  _eval_tree->Branch("rv_sv_pT15_vtx_ntrk_good_pv",rv_sv_pT15_vtx_ntrk_good_pv,"rv_sv_pT15_vtx_ntrk_good_pv[rv_sv_njets][30]/I");
  _eval_tree->Branch("rv_sv_pT15_vtx_mass",rv_sv_pT15_vtx_mass,"rv_sv_pT15_vtx_mass[rv_sv_njets][30]/F");
  _eval_tree->Branch("rv_sv_pT15_vtx_mass_corr",rv_sv_pT15_vtx_mass_corr,"rv_sv_pT15_vtx_mass_corr[rv_sv_njets][30]/F");
  _eval_tree->Branch("rv_sv_pT15_vtx_pT",rv_sv_pT15_vtx_pT,"rv_sv_pT15_vtx_pT[rv_sv_njets][30]/F");
  _eval_tree->Branch("rv_sv_pT15_vtx_jet_theta",rv_sv_pT15_vtx_jet_theta,"rv_sv_pT15_vtx_jet_theta[rv_sv_njets][30]/F");
  _eval_tree->Branch("rv_sv_pT15_vtx_chi2",rv_sv_pT15_vtx_chi2,"rv_sv_pT15_vtx_chi2[rv_sv_njets][30]/F");
  _eval_tree->Branch("rv_sv_pT15_vtx_ndf",rv_sv_pT15_vtx_ndf,"rv_sv_pT15_vtx_ndf[rv_sv_njets][30]/F");


  return;
}

void SVReco::reset_eval_variables(){

  gf_prim_vtx_ntrk = rv_prim_vtx_ntrk = 0;
  for (int i=0; i<3; i++){
    gf_prim_vtx[i] = gf_prim_vtx_err[i] = -999;
    rv_prim_vtx[i] = rv_prim_vtx_err[i] = -999;
  }//i

  rv_sv_njets = 0;

  for (int ijet=0; ijet<10; ijet++){
    rv_sv_jet_id[ijet] = -999;
    rv_sv_jet_prop[ijet][0] = rv_sv_jet_prop[ijet][1] = -999;
    rv_sv_jet_pT[ijet] = -999;
    rv_sv_jet_px[ijet] = rv_sv_jet_py[ijet] = rv_sv_jet_pz[ijet] = -999;

    rv_sv_pT00_nvtx[ijet] = rv_sv_pT05_nvtx[ijet] = rv_sv_pT10_nvtx[ijet] = rv_sv_pT15_nvtx[ijet] = 0;

    for (int ivtx=0; ivtx<30; ivtx++){
      rv_sv_pT00_vtx_ntrk[ijet][ivtx] = rv_sv_pT05_vtx_ntrk[ijet][ivtx] = rv_sv_pT10_vtx_ntrk[ijet][ivtx] = rv_sv_pT15_vtx_ntrk[ijet][ivtx] = 0;
      rv_sv_pT00_vtx_ntrk_good[ijet][ivtx] = rv_sv_pT05_vtx_ntrk_good[ijet][ivtx] = rv_sv_pT10_vtx_ntrk_good[ijet][ivtx] = rv_sv_pT15_vtx_ntrk_good[ijet][ivtx] = 0;
      rv_sv_pT00_vtx_ntrk_good_pv[ijet][ivtx] = rv_sv_pT05_vtx_ntrk_good_pv[ijet][ivtx] = rv_sv_pT10_vtx_ntrk_good_pv[ijet][ivtx] = rv_sv_pT15_vtx_ntrk_good_pv[ijet][ivtx] = 0;

      rv_sv_pT00_vtx_mass[ijet][ivtx] = rv_sv_pT00_vtx_mass_corr[ijet][ivtx] = rv_sv_pT00_vtx_pT[ijet][ivtx] = -999;
      rv_sv_pT05_vtx_mass[ijet][ivtx] = rv_sv_pT05_vtx_mass_corr[ijet][ivtx] = rv_sv_pT05_vtx_pT[ijet][ivtx] = -999;
      rv_sv_pT10_vtx_mass[ijet][ivtx] = rv_sv_pT10_vtx_mass_corr[ijet][ivtx] = rv_sv_pT10_vtx_pT[ijet][ivtx] = -999;
      rv_sv_pT15_vtx_mass[ijet][ivtx] = rv_sv_pT15_vtx_mass_corr[ijet][ivtx] = rv_sv_pT15_vtx_pT[ijet][ivtx] = -999;

      rv_sv_pT00_vtx_x[ijet][ivtx] = rv_sv_pT00_vtx_y[ijet][ivtx] = rv_sv_pT00_vtx_z[ijet][ivtx] = -999;
      rv_sv_pT00_vtx_ex[ijet][ivtx] = rv_sv_pT00_vtx_ey[ijet][ivtx] = rv_sv_pT00_vtx_ez[ijet][ivtx] = -999;
      rv_sv_pT05_vtx_x[ijet][ivtx] = rv_sv_pT05_vtx_y[ijet][ivtx] = rv_sv_pT05_vtx_z[ijet][ivtx] = -999;
      rv_sv_pT05_vtx_ex[ijet][ivtx] = rv_sv_pT05_vtx_ey[ijet][ivtx] = rv_sv_pT05_vtx_ez[ijet][ivtx] = -999;
      rv_sv_pT10_vtx_x[ijet][ivtx] = rv_sv_pT10_vtx_y[ijet][ivtx] = rv_sv_pT10_vtx_z[ijet][ivtx] = -999;
      rv_sv_pT10_vtx_ex[ijet][ivtx] = rv_sv_pT10_vtx_ey[ijet][ivtx] = rv_sv_pT10_vtx_ez[ijet][ivtx] = -999;
      rv_sv_pT15_vtx_x[ijet][ivtx] = rv_sv_pT15_vtx_y[ijet][ivtx] = rv_sv_pT15_vtx_z[ijet][ivtx] = -999;
      rv_sv_pT15_vtx_ex[ijet][ivtx] = rv_sv_pT15_vtx_ey[ijet][ivtx] = rv_sv_pT15_vtx_ez[ijet][ivtx] = -999;

      rv_sv_pT00_vtx_jet_theta[ijet][ivtx] = rv_sv_pT00_vtx_pT[ijet][ivtx] = -999;
      rv_sv_pT05_vtx_jet_theta[ijet][ivtx] = rv_sv_pT05_vtx_pT[ijet][ivtx] = -999;
      rv_sv_pT10_vtx_jet_theta[ijet][ivtx] = rv_sv_pT10_vtx_pT[ijet][ivtx] = -999;
      rv_sv_pT15_vtx_jet_theta[ijet][ivtx] = rv_sv_pT15_vtx_pT[ijet][ivtx] = -999;

      rv_sv_pT00_vtx_chi2[ijet][ivtx] = rv_sv_pT00_vtx_ndf[ijet][ivtx] = -999;
      rv_sv_pT05_vtx_chi2[ijet][ivtx] = rv_sv_pT05_vtx_ndf[ijet][ivtx] = -999;
      rv_sv_pT10_vtx_chi2[ijet][ivtx] = rv_sv_pT10_vtx_ndf[ijet][ivtx] = -999;
      rv_sv_pT15_vtx_chi2[ijet][ivtx] = rv_sv_pT15_vtx_ndf[ijet][ivtx] = -999;

    }//ivtx
  }//ijet

  return;
}

int SVReco::CreateNodes(PHCompositeNode *topNode){

  return Fun4AllReturnCodes::EVENT_OK;
}

/*
 * GetNodes():
 *  Get all the all the required nodes off the node tree
 */
int SVReco::GetNodes(PHCompositeNode * topNode){
  //DST objects

  /* Input Svtx Clusters
     SvtxClusters have been moved to TrkrClusterContainer see:
https://github.com/sPHENIX-Collaboration/coresoftware/commit/fd7228dba1ad6ee26152006aeddd5a34563599b9#diff-fc82b294b9b151bbb2aa9ffd0c7bd77e
_clustermap = findNode::getClass<SvtxClusterMap>(topNode, "SvtxClusterMap");
if (!_clustermap && _event < 2){
cout << PHWHERE << " SvtxClusterMap node not found on node tree"
<< endl;
return Fun4AllReturnCodes::ABORTEVENT;
}*/

_clustermap = findNode::getClass<TrkrClusterContainer>(topNode, "TRKR_CLUSTER");
if (!_clustermap && _event < 2){
  cout << PHWHERE << " TRKR_CLUSTERS node not found on node tree"
    << endl;
  return Fun4AllReturnCodes::ABORTEVENT;
}

// Input Svtx Tracks
_trackmap = findNode::getClass<SvtxTrackMap>(topNode, "SvtxTrackMap");
if (!_trackmap && _event < 2){
  cout << PHWHERE << " SvtxTrackMap node not found on node tree"
    << endl;
  return Fun4AllReturnCodes::ABORTEVENT;
}

// Input Svtx Vertices
_vertexmap = findNode::getClass<SvtxVertexMap>(topNode, "SvtxVertexMap");
if (!_vertexmap && _event < 2){
  cout << PHWHERE << " SvtxVertexrMap node not found on node tree"
    << endl;
  return Fun4AllReturnCodes::ABORTEVENT;
}

// Input Jet Map 
_jetmap = findNode::getClass<JetMap>(topNode, _jetmap_name.data());
if (!_jetmap && _event < 2 && _cut_jet){
  cout << PHWHERE << " JetMap node not found on node tree"
    << endl;
  //return Fun4AllReturnCodes::ABORTEVENT;
}

return Fun4AllReturnCodes::EVENT_OK;
}


PHGenFit::Track* SVReco::MakeGenFitTrack(PHCompositeNode *topNode, const SvtxTrack* intrack, const SvtxVertex* vertex){
  if (!intrack){
    cerr << PHWHERE << " Input SvtxTrack is NULL!" << endl;
    return NULL;
  }

  /*	SvtxHitMap* hitsmap = NULL;
      hitsmap = findNode::getClass<SvtxHitMap>(topNode, "SvtxHitMap");
      if (!hitsmap) {
      cout << PHWHERE << "ERROR: Can't find node SvtxHitMap" << endl;
      return NULL;
      }

      PHG4CellContainer* cells_svtx = findNode::getClass<PHG4CellContainer>(topNode,"G4CELL_TPC");
      PHG4CellContainer* cells_intt = findNode::getClass<PHG4CellContainer>(topNode,"G4CELL_INTT");
      PHG4CellContainer* cells_maps = findNode::getClass<PHG4CellContainer>(topNode,"G4CELL_MVTX");

      if (_use_ladder_geom and !cells_svtx and !cells_intt and !cells_maps) {
      cout << PHWHERE << "No PHG4CellContainer found!" << endl;
      return NULL;
      }
      */
  PHG4CylinderGeomContainer* geom_container_intt = findNode::getClass<PHG4CylinderGeomContainer>(topNode, "CYLINDERGEOM_INTT");
  PHG4CylinderGeomContainer* geom_container_maps = findNode::getClass<PHG4CylinderGeomContainer>(topNode, "CYLINDERGEOM_MVTX");

  if (_use_ladder_geom and !geom_container_intt and !geom_container_maps) {
    cout << PHWHERE << "No PHG4CylinderGeomContainer found!" << endl;
    return NULL;
  }

  // prepare seed
  /*
     TVector3 seed_mom(100, 0, 0);
     TVector3 seed_pos(0, 0, 0);
     TMatrixDSym seed_cov(6);
     for (int i = 0; i < 6; i++) {
     for (int j = 0; j < 6; j++) {
     seed_cov[i][j] = 100.;
     }
     }
     */

  TVector3 seed_pos(intrack->get_x(), intrack->get_y(), intrack->get_z());
  TVector3 seed_mom(intrack->get_px(), intrack->get_py(), intrack->get_pz());
  TMatrixDSym seed_cov(6);
  for (int i=0; i<6; i++){
    for (int j=0; j<6; j++){
      seed_cov[i][j] = intrack->get_error(i,j);
      //seed_cov[i][j] = 100.;
    }
  }

  // Create measurements
  std::vector<PHGenFit::Measurement*> measurements;

  std::map<float, unsigned int> m_r_cluster_id;

  for (auto iter = intrack->begin_clusters(); iter != intrack->end_clusters(); ++iter){
    unsigned int cluster_id = *iter;
    TrkrCluster* cluster = _clustermap->findCluster(cluster_id);
    float x = cluster->getPosition(0);
    float y = cluster->getPosition(1);
    float r = sqrt(x*x+y*y);
    m_r_cluster_id.insert(std::pair<float, unsigned int>(r, cluster_id));
  }

  //what is the point of this loop
  for (auto iter = m_r_cluster_id.begin(); iter != m_r_cluster_id.end(); ++iter){
    //for (SvtxTrack::ConstClusterIter iter = intrack->begin_clusters(); iter != intrack->end_clusters(); ++iter){
    unsigned int cluster_id = iter->second;
    //unsigned int cluster_id = *iter;
    TrkrCluster* cluster = _clustermap->findCluster(cluster_id);
    if (!cluster) {
      LogError("No cluster Found!");
      continue;
    }

    TVector3 pos(cluster->getPosition(0), cluster->getPosition(1), cluster->getPosition(2));
    float radius = sqrt(cluster->getPosition(0)*cluster->getPosition(0)  + cluster->getPosition(1)*cluster->getPosition(1));

    seed_mom.SetPhi(pos.Phi());
    seed_mom.SetTheta(pos.Theta());

    //TODO use u, v explicitly?
    TVector3 n(cluster->getPosition(0), cluster->getPosition(1), 0);

    /*unsigned int begin_hit_id = *(cluster->begin_hits());
      SvtxHit* svtxhit = hitsmap->find(begin_hit_id)->second;

      PHG4Cell* cell_svtx = nullptr;
      PHG4Cell* cell_intt = nullptr;
      PHG4Cell* cell_maps = nullptr;

      if(_use_ladder_geom and cells_svtx) cell_svtx = cells_svtx->findCell(svtxhit->get_cellid());
      if(_use_ladder_geom and cells_intt) cell_intt = cells_intt->findCell(svtxhit->get_cellid());
      if(_use_ladder_geom and cells_maps) cell_maps = cells_maps->findCell(svtxhit->get_cellid());
      if(_use_ladder_geom and !(cell_svtx or cell_intt or cell_maps)){
      if(Verbosity()>=0)
      LogError("!(cell_svtx or cell_intt or cell_maps)");
      continue;
      }*/ 

    //float phi_tilt[7] = {0.304, 0.304, 0.304, 0.244, 0.244, 0.209, 0.201};
    unsigned int trkrid = TrkrDefs::getTrkrId(cluster_id);
    unsigned int layer = TrkrDefs::getLayer(cluster_id);

    //NEW
    if (_use_ladder_geom){ //I don't understand this bool
      if (trkrid == TrkrDefs::mvtxId) {
        int stave_index = MvtxDefs::getStaveId(cluster_id);
        int chip_index = MvtxDefs::getChipId(cluster_id);

        double ladder_location[3] = { 0.0, 0.0, 0.0 };
        //not exactly sure where the cylinder geoms are currently objectified. check this 
        CylinderGeom_Mvtx *geom = (CylinderGeom_Mvtx*) geom_container_maps->GetLayerGeom(layer);
        // returns the center of the sensor in world coordinates - used to get the ladder phi location
        geom->find_sensor_center(stave_index, 0, 0, chip_index, ladder_location);//the mvtx module and half stave are 0
        n.SetXYZ(ladder_location[0], ladder_location[1], 0);
        n.RotateZ(geom->get_stave_phi_tilt());
      } 
      else if (trkrid == TrkrDefs::inttId) {
        //this may bug but it looks ok for now
        CylinderGeomIntt* geom = (CylinderGeomIntt*) geom_container_intt->GetLayerGeom(layer);
        double hit_location[3] = { 0.0, 0.0, 0.0 };
        geom->find_segment_center(InttDefs::getLadderZId(cluster_id),InttDefs::getLadderPhiId(cluster_id), hit_location);

        n.SetXYZ(hit_location[0], hit_location[1], 0);
        n.RotateZ(geom->get_strip_phi_tilt());
      }
    }


    PHGenFit::Measurement* meas = new PHGenFit::PlanarMeasurement(pos, n,
        radius*cluster->getPhiError(), cluster->getZError());

    measurements.push_back(meas);
  }

  //TODO Add multiple TrackRep choices.
  genfit::AbsTrackRep* rep = new genfit::RKTrackRep(_primary_pid_guess);
  PHGenFit::Track* track(new PHGenFit::Track(rep, seed_pos, seed_mom, seed_cov));
  track->addMeasurements(measurements);

  if (_fitter->processTrack(track, false) != 0) {
    if (Verbosity() >= 1)
      LogWarning("Track fitting failed");
    return NULL;
  }

  //cout << "CHECK1 " << endl;

  /*
     TVector3 vertex_position(0, 0, 0);
     if (vertex){
     vertex_position.SetXYZ(vertex->get_x(), vertex->get_y(), vertex->get_z());
     }

     std::shared_ptr<genfit::MeasuredStateOnPlane> gf_state_vertex_ca = NULL;
     try {
     gf_state_vertex_ca = std::shared_ptr < genfit::MeasuredStateOnPlane> (track->extrapolateToPoint(vertex_position));
     } catch (...) {
     if (verbosity >= 2)
     LogWarning("extrapolateToPoint failed!");
     return NULL;
     }
     */

  track->getGenFitTrack()->setMcTrackId(intrack->get_id());

  //TVector3 mom = gf_state_vertex_ca->getMom();
  //TMatrixDSym cov = gf_state_vertex_ca->get6DCov();

  //cout << "OUT Ex: " << sqrt(cov[0][0]) << ", Ey: " << sqrt(cov[1][1]) << ", Ez: " << sqrt(cov[2][2]) << endl;

  //cout << "IN Px: " << intrack->get_px() << ", Py: " << intrack->get_py() << ", Pz: " << intrack->get_pz() << endl; 
  //cout << "OUT Px: " << mom.X() << ", Py: " << mom.Y() << ", Pz: " << mom.Z() << endl; 

  return track;
  }


  /*
   * Fill SvtxVertexMap from GFRaveVertexes and Tracks
   */
  void SVReco::FillVertexMap(
      const std::vector<genfit::GFRaveVertex*>& rave_vertices,
      const std::vector<genfit::Track*>& gf_tracks){

    for (unsigned int ivtx=0; ivtx<rave_vertices.size(); ++ivtx){
      genfit::GFRaveVertex* rave_vtx = rave_vertices[ivtx];

      //cout << "V0 x: " << rave_vtx->getPos().X() << ", y: " << rave_vtx->getPos().Y() << ", z: " << rave_vtx->getPos().Z() << endl;
      rv_prim_vtx[0] = rave_vtx->getPos().X();
      rv_prim_vtx[1] = rave_vtx->getPos().Y();
      rv_prim_vtx[2] = rave_vtx->getPos().Z();

      rv_prim_vtx_err[0] = sqrt(rave_vtx->getCov()[0][0]);
      rv_prim_vtx_err[1] = sqrt(rave_vtx->getCov()[1][1]);
      rv_prim_vtx_err[2] = sqrt(rave_vtx->getCov()[2][2]);

      rv_prim_vtx_ntrk = rave_vtx->getNTracks();

      TVector3 vertex_position(rv_prim_vtx[0], rv_prim_vtx[1], rv_prim_vtx[2]);

      //cout << "N TRK gf: " << gf_tracks.size() << ", rv: " << rv_prim_vtx_ntrk << endl;

      for (unsigned int itrk=0; itrk<(unsigned int)rv_prim_vtx_ntrk; itrk++){

        TVector3 rvtrk_mom = rave_vtx->getParameters(itrk)->getMom();
        float rvtrk_w = rave_vtx->getParameters(itrk)->getWeight();

        unsigned int rvtrk_mc_id = rave_vtx->getParameters(itrk)->getTrack()->getMcTrackId();
        svtxtrk_wt_map[rvtrk_mc_id] = rvtrk_w;

        //cout << "w: " << rvtrk_w << ", mc id: " << rvtrk_mc_id << endl;
        /*
           SvtxTrack* svtx_track = _trackmap->get(rvtrk_mc_id);

           cout << "rave trk, px: " << rvtrk_mom.Px() << ", py: " << rvtrk_mom.Py() << ", pz: " << rvtrk_mom.Pz() << endl;
           cout << "svtx trk, px: " << svtx_track->get_px() << ", py: " << svtx_track->get_py() << ", pz: " << svtx_track->get_pz() << endl;
           */

        /*
           for (SvtxTrackMap::ConstIter iter3=_trackmap->begin(); iter3!=_trackmap->end(); iter3++)
           {
           SvtxTrack* svtx_track = iter3->second;

           if ( fabs((svtx_track->get_px()-rvtrk_mom.Px())/svtx_track->get_px())<0.10
           && fabs((svtx_track->get_py()-rvtrk_mom.Py())/svtx_track->get_py())<0.10
           && fabs((svtx_track->get_pz()-rvtrk_mom.Pz())/svtx_track->get_pz())<0.10 ){
           cout << "rave trk, px: " << rvtrk_mom.Px() << ", py: " << rvtrk_mom.Py() << ", pz: " << rvtrk_mom.Pz() << endl;
        //cout << "ggff trk, px: " << gftrk_mom.Px() << ", py: " << gftrk_mom.Py() << ", pz: " << gftrk_mom.Pz() << endl;
        cout << "svtx trk, px: " << svtx_track->get_px() << ", py: " << svtx_track->get_py() << ", pz: " << svtx_track->get_pz() << endl;
        }
        }//iter3
        */

        //unsigned int trk_id = svtxtrk_id[itrk];

        /*
           cout << "W: " << w_trk 
           << ", id: " << rave_vtx->getParameters(itrk)->GetUniqueID()
           << ", px: " << rave_vtx->getParameters(itrk)->getMom().Px() 
           << ", py: " << rave_vtx->getParameters(itrk)->getMom().Py() 
           << ", pz: " << rave_vtx->getParameters(itrk)->getMom().Pz() 
           << endl;
           */

        //if (svtxtrk_gftrk_map.find(svtx_track->get_id())!=svtxtrk_gftrk_map.end()){
        //}

        //TVector3 mom_trk = rave_vtx->getParameters(itrk)->getMom();
      }
    }

    for (SvtxVertexMap::Iter iter = _vertexmap->begin(); iter != _vertexmap->end(); ++iter){
      SvtxVertex *svtx_vertex = iter->second;

      //cout << "V1 x: " << svtx_vertex->get_x() << ", y: " << svtx_vertex->get_y() << ", z: " << svtx_vertex->get_z() << endl;
      gf_prim_vtx[0] = svtx_vertex->get_x();
      gf_prim_vtx[1] = svtx_vertex->get_y();
      gf_prim_vtx[2] = svtx_vertex->get_z();

      gf_prim_vtx_err[0] = sqrt(svtx_vertex->get_error(0,0));
      gf_prim_vtx_err[1] = sqrt(svtx_vertex->get_error(1,1));
      gf_prim_vtx_err[2] = sqrt(svtx_vertex->get_error(2,2));

      gf_prim_vtx_ntrk = int(svtx_vertex->size_tracks());
    }

    return;
  }

  int SVReco::GetSVMass_mom(
      const genfit::GFRaveVertex* rave_vtx,
      float & vtx_mass,
      float & vtx_px,
      float & vtx_py,
      float & vtx_pz,
      int & ntrk_good_pv
      ){

    float sum_E = 0, sum_px = 0, sum_py = 0, sum_pz = 0;

    int N_good = 0, N_good_pv = 0;

    for (unsigned int itrk=0; itrk<rave_vtx->getNTracks(); itrk++){
      TVector3 mom_trk = rave_vtx->getParameters(itrk)->getMom(); 

      double w_trk = rave_vtx->getParameters(itrk)->getWeight();

      sum_px += mom_trk.X();
      sum_py += mom_trk.Y();
      sum_pz += mom_trk.Z();
      sum_E += sqrt(mom_trk.Mag2() + 0.140*0.140);

      //cout << "W: " << w_trk << endl;
      if ( w_trk>0.7 ){
        N_good++;

        unsigned int rvtrk_mc_id = rave_vtx->getParameters(itrk)->getTrack()->getMcTrackId();
        //cout << "mc_id: " << rvtrk_mc_id << ", wt: " << svtxtrk_wt_map[rvtrk_mc_id] << endl;
        if ( svtxtrk_wt_map[rvtrk_mc_id]>0.7 ){
          N_good_pv++;
        }
      }//

    }//itrk

    vtx_mass =  sqrt(sum_E*sum_E - sum_px*sum_px - sum_py*sum_py - sum_pz*sum_pz);
    vtx_px = sum_px;
    vtx_py = sum_py;
    vtx_pz = sum_pz;

    ntrk_good_pv = N_good_pv;

    //cout << "Mass: " << vtx_mass << ", pT: " << vtx_pT << endl;
    return N_good;
  }


  void SVReco::FillSVMap(
      const std::vector<genfit::GFRaveVertex*>& rave_vertices,
      const std::vector<genfit::Track*>& gf_tracks){

    return;

  }



