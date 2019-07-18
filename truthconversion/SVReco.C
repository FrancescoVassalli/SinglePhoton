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

/*Rave
#include <rave/Version.h>
//#include <rave/Track.h>
#include <rave/VertexFactory.h>
#include <rave/ConstantMagneticField.h>
*/
//GenFit
//#include <GenFit/GFRaveConverters.h>


/*
 * Constructor
 */
SVReco::SVReco(const string &name) :
  _mag_field_file_name("/phenix/upgrades/decadal/fieldmaps/sPHENIX.2d.root"),
  _mag_field_re_scaling_factor(1.4 / 1.5), //what is this and why?
  _reverse_mag_field(false),
  _fitter(NULL),
  _track_fitting_alg_name("DafRef"),
  _n_maps_layer(3),
  _n_intt_layer(4),
  _primary_pid_guess(11), //for the tracks
  _cut_Ncluster(false),
  _cut_min_pT(0.1),
  _cut_dca(5.0), //probably in mm
  _cut_chi2_ndf(5),
  _use_ladder_geom(false),
  _vertex_finder(NULL),
  _vertexing_method("avf-smoothing:1"), /*need a list of these and their proper domains*/
  _clustermap(NULL),
  _trackmap(NULL),
  _vertexmap(NULL),
  _do_eval(false),
  _verbosity(10),
  _do_evt_display(false)
{

}


int SVReco::InitEvent(PHCompositeNode *topNode) {
  GetNodes(topNode);
  //cout<<"got vertexing nodes"<<endl;
  //! stands for Refit_GenFit_Tracks
  vector<genfit::Track*> rf_gf_tracks;
  for(auto p:rf_gf_tracks) delete p;
  rf_gf_tracks.clear();

  for(auto p : _main_rf_phgf_tracks) delete p;
  _main_rf_phgf_tracks.clear();

  //! find vertex using tracks
  std::vector<genfit::GFRaveVertex*> rave_vertices;
  for(auto p : rave_vertices) delete p;
  rave_vertices.clear();

  svtxtrk_gftrk_map.clear();
  svtxtrk_wt_map.clear();
  svtxtrk_id.clear();
  //is this the priamry vetex?
  SvtxVertex *vertex = _vertexmap->get(0);
  cout<<"starting track loop with vertex:\n";
  if (vertex)
  {
    vertex->identify();
  }
  else{
    cout<<"NULL"<<endl;
  }

  //iterate over all tracks to find priary vertex and make rave/genfit objects
  for (SvtxTrackMap::Iter iter = _trackmap->begin(); iter != _trackmap->end();
      ++iter) {
    SvtxTrack* svtx_track = iter->second;
    // do track cuts
    if (!svtx_track || svtx_track->get_ndf()<40 || svtx_track->get_pt()<_cut_min_pT ||
      svtx_track->get_chisq()/svtx_track->get_ndf())>_cut_chi2_ndf ||
      fabs(svtx_track->get_dca3d_xy())>_cut_dca || fabs(svtx_track->get_dca3d_z())>_cut_dca)
      continue;

    int n_MVTX = 0, n_INTT = 0, n_TPC = 0;
    //cout<<"Keys:";
    for (SvtxTrack::ConstClusterKeyIter iter2 = svtx_track->begin_cluster_keys(); iter2!=svtx_track->end_cluster_keys(); iter2++) {
      TrkrDefs::cluskey cluster_key = *iter2;
      //cout<<cluster_key<<',';
      //count where the hits are
      float layer = (float) TrkrDefs::getLayer(cluster_key);
      if (layer<_n_maps_layer) n_MVTX++;
      else if (layer<_n_maps_layer+_n_intt_layer) n_INTT++;
      else n_TPC++;
    }//cluster loop
    //cluster cuts
    //cout<<"\n cluster loop with n_MVTX="<<n_MVTX<<" n_INTT="<<n_INTT<<" and nTPC="<<n_TPC<<endl;
    if ( _cut_Ncluster && (n_MVTX<2 || n_INTT<2 || n_TPC<25) ){
      continue;
    }
    //cout << (svtx_track->get_chisq()/svtx_track->get_ndf()) << ", " << n_TPC << ", " << svtx_track->get_pt() << endl;
    //cout << svtx_track->get_ndf() << ", " << svtx_track->size_clusters() << endl;
    //cout<<"making genfit"<<endl;
    PHGenFit::Track* rf_phgf_track = MakeGenFitTrack(topNode, svtx_track, vertex); //convert SvtxTrack to GenFit Track
    //cout<<"made genfit"<<endl;

    if (rf_phgf_track) {
      //svtx_track->identify();
      //make a map to connect SvtxTracks to their respective GenFit Tracks
      svtxtrk_id.push_back(svtx_track->get_id());
      svtxtrk_gftrk_map[svtx_track->get_id()] = _main_rf_phgf_tracks.size();
      _main_rf_phgf_tracks.push_back(rf_phgf_track); //to be used by findSecondaryVerticies
      rf_gf_tracks.push_back(rf_phgf_track->getGenFitTrack());
    }
  }
  cout<<"exit track loop ntracks="<<rf_gf_tracks.size()<<endl;
  //
  _vertex_finder->setMethod(_vertexing_method.data());
  if (rf_gf_tracks.size()>=2){
    try {
      _vertex_finder->findVertices(&rave_vertices, rf_gf_tracks);
    }catch (...){
      std::cerr << PHWHERE << "GFRaveVertexFactory::findVertices failed!";
    }
    //cout<<"filling vtx map"<<endl;
    //FillVertexMap(rave_vertices, rf_gf_tracks);
  }

  //cout<<"Done event init"<<endl;
  return Fun4AllReturnCodes::EVENT_OK;
}


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

  _vertex_finder = new genfit::GFRaveVertexFactory(_verbosity);
  _vertex_finder->setMethod(_vertexing_method.data());

  if (!_vertex_finder) {
    std::cerr<< PHWHERE<<" bad run init no SVR"<<endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

std::vector<genfit::GFRaveVertex*> SVReco::findSecondaryVertices(std::vector<std::pair<SvtxTrack*, SvtxTrack*>> *conversion_pairs) {
  //_vertex_finder->setMethod("avr-smoothing:1");
  //_vertex_finder->setMethod("avr");
  _vertex_finder->setMethod("avf");
  vector<genfit::GFRaveVertex*> rave_vertices_conversion;
  rave_vertices_conversion.clear();

  //calculate secondary verticies
  for (std::vector<std::pair<SvtxTrack*, SvtxTrack*>>::iterator iter = conversion_pairs->begin(); iter!=conversion_pairs->end(); iter++)
  {
    SvtxTrack* track1 = iter->first;
    SvtxTrack* track2 = iter->second;

    vector<genfit::Track*> rf_gf_tracks_conversion;
    rf_gf_tracks_conversion.clear();

    if (svtxtrk_gftrk_map.find(track1->get_id())!=svtxtrk_gftrk_map.end()&&
        svtxtrk_gftrk_map.find(track2->get_id())!=svtxtrk_gftrk_map.end()){

      unsigned int trk_index = svtxtrk_gftrk_map[track1->get_id()];
      PHGenFit::Track* rf_phgf_track = _main_rf_phgf_tracks[trk_index];
      rf_gf_tracks_conversion.push_back(rf_phgf_track->getGenFitTrack());

      trk_index = svtxtrk_gftrk_map[track2->get_id()];
      rf_phgf_track = _main_rf_phgf_tracks[trk_index];
      rf_gf_tracks_conversion.push_back(rf_phgf_track->getGenFitTrack());
    }
    if (rf_gf_tracks_conversion.size()>1){
      try{
        _vertex_finder->findVertices(&rave_vertices_conversion, rf_gf_tracks_conversion);
      }catch (...){
        std::cout << PHWHERE << "GFRaveVertexFactory::findVertices failed!";
      }
    }
  }//conversion pairs
  return rave_vertices_conversion;
}

SVReco::~SVReco(){
  delete _fitter;
  delete _vertex_finder;
  for (std::vector<PHGenFit::Track*>::iterator i = _main_rf_phgf_tracks.begin(); i != _main_rf_phgf_tracks.end(); ++i)
  {
    delete *i;
  }
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
  _clustermap = findNode::getClass<TrkrClusterContainer>(topNode, "TRKR_CLUSTER");
  if (!_clustermap){
    cout << PHWHERE << " TRKR_CLUSTERS node not found on node tree"
      << endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }
  // Input Svtx Tracks
  _trackmap = findNode::getClass<SvtxTrackMap>(topNode, "SvtxTrackMap");
  if (!_trackmap){
    cout << PHWHERE << " SvtxTrackMap node not found on node tree"
      << endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }
  // Input Svtx Vertices
  _vertexmap = findNode::getClass<SvtxVertexMap>(topNode, "SvtxVertexMap");
  if (!_vertexmap){
    cout << PHWHERE << " SvtxVertexrMap node not found on node tree"
      << endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  if(!_vertex_finder){
    std::cerr<< PHWHERE<<" bad run init no SVR"<<endl;
    return Fun4AllReturnCodes::ABORTRUN;
  } 

  return Fun4AllReturnCodes::EVENT_OK;
}

//From @sh-lim
 PHGenFit::Track* SVReco::MakeGenFitTrack(PHCompositeNode *topNode, const SvtxTrack* intrack, const SvtxVertex* vertex){
  if (!intrack){
    cerr << PHWHERE << " Input SvtxTrack is NULL!" << endl;
    return NULL;
  }
  PHG4CylinderGeomContainer* geom_container_intt = findNode::getClass<PHG4CylinderGeomContainer>(topNode, "CYLINDERGEOM_INTT");
  PHG4CylinderGeomContainer* geom_container_maps = findNode::getClass<PHG4CylinderGeomContainer>(topNode, "CYLINDERGEOM_MVTX");

  if (_use_ladder_geom and !geom_container_intt and !geom_container_maps) {
    cout << PHWHERE << "No PHG4CylinderGeomContainer found!" << endl;
    return NULL;
  }

  TVector3 seed_pos(intrack->get_x(), intrack->get_y(), intrack->get_z());
  TVector3 seed_mom(intrack->get_px(), intrack->get_py(), intrack->get_pz());
  TMatrixDSym seed_cov(6);
  for (int i=0; i<6; i++){
    for (int j=0; j<6; j++){
      seed_cov[i][j] = intrack->get_error(i,j);
    }
  }

  // Create measurements
  std::vector<PHGenFit::Measurement*> measurements;

  for (auto iter = intrack->begin_cluster_keys(); iter != intrack->end_cluster_keys(); ++iter){
//    unsigned int cluster_id = *iter;
    TrkrCluster* cluster = _clustermap->findCluster(*iter);
    if (!cluster) {
      LogError("No cluster Found!");
      continue;
    }
    float x = cluster->getPosition(0);
    float y = cluster->getPosition(1);
    float radius = sqrt(x*x+y*y);
    TVector3 pos(cluster->getPosition(0), cluster->getPosition(1), cluster->getPosition(2));
    seed_mom.SetPhi(pos.Phi());
    seed_mom.SetTheta(pos.Theta());

    TVector3 n(cluster->getPosition(0), cluster->getPosition(1), 0);
    //cout<<"Cluster with {"<<cluster->getPosition(0)<<','<<cluster->getPosition(0)<<"}\n";

    if (_use_ladder_geom){ //I don't understand this bool
      unsigned int trkrid = TrkrDefs::getTrkrId(*iter);
      unsigned int layer = TrkrDefs::getLayer(*iter);
      if (trkrid == TrkrDefs::mvtxId) {
        int stave_index = MvtxDefs::getStaveId(*iter);
        int chip_index = MvtxDefs::getChipId(*iter);

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
        geom->find_segment_center(InttDefs::getLadderZId(*iter),InttDefs::getLadderPhiId(*iter), hit_location);

        n.SetXYZ(hit_location[0], hit_location[1], 0);
        n.RotateZ(geom->get_strip_phi_tilt());
      }
    }//if use_ladder_geom
    PHGenFit::Measurement* meas = new PHGenFit::PlanarMeasurement(pos, n,radius*cluster->getPhiError(), cluster->getZError());
    measurements.push_back(meas);
  }//cluster loop
  genfit::AbsTrackRep* rep = new genfit::RKTrackRep(_primary_pid_guess);
  PHGenFit::Track* track(new PHGenFit::Track(rep, seed_pos, seed_mom, seed_cov));
  track->addMeasurements(measurements);

  if (_fitter->processTrack(track, false) != 0) {
    if (_verbosity >= 1)
      LogWarning("Track fitting failed");
    return NULL;
  }

  track->getGenFitTrack()->setMcTrackId(intrack->get_id());

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
    if (_do_eval)
    {
      rv_prim_vtx[0] = rave_vtx->getPos().X();
      rv_prim_vtx[1] = rave_vtx->getPos().Y();
      rv_prim_vtx[2] = rave_vtx->getPos().Z();

      rv_prim_vtx_err[0] = sqrt(rave_vtx->getCov()[0][0]);
      rv_prim_vtx_err[1] = sqrt(rave_vtx->getCov()[1][1]);
      rv_prim_vtx_err[2] = sqrt(rave_vtx->getCov()[2][2]);
    }

    rv_prim_vtx_ntrk = rave_vtx->getNTracks();

    //TVector3 vertex_position(rv_prim_vtx[0], rv_prim_vtx[1], rv_prim_vtx[2]);

    //cout << "N TRK gf: " << gf_tracks.size() << ", rv: " << rv_prim_vtx_ntrk << endl;

    for (int itrk=0; itrk< rv_prim_vtx_ntrk; itrk++){

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
  if (_do_eval)
  {
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
