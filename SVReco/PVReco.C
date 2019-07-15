#include "PVReco.h"
#include <trackbase_historic/SvtxCluster.h>
#include <trackbase_historic/SvtxClusterMap.h>
#include <trackbase_historic/SvtxHitMap.h>
#include <trackbase_historic/SvtxTrack.h>
#include <trackbase_historic/SvtxVertex.h>
#include <trackbase_historic/SvtxTrackMap.h>
#include <trackbase_historic/SvtxVertexMap.h>

#include <g4jets/JetMap.h>

#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/PHTFileServer.h>

#include <g4detectors/PHG4CellContainer.h>
#include <g4detectors/PHG4CylinderGeomContainer.h>
#include <g4detectors/PHG4Cell.h>
#include <g4mvtx/PHG4CylinderGeom_MVTX.h>
#include <g4intt/PHG4CylinderGeomINTT.h>

#include <g4main/PHG4Hit.h>
#include <g4main/PHG4HitContainer.h>
#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4Particle.h>

#include <g4eval/SvtxEvalStack.h>
#include <g4eval/SvtxTrackEval.h>
#include <g4eval/SvtxClusterEval.h>
#include <g4eval/SvtxTruthEval.h>
#include <g4eval/SvtxVertexEval.h>
#include <g4eval/SvtxHitEval.h>

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
#include <GenFit/KalmanFitterInfo.h>

#include <phhepmc/PHHepMCGenEventMap.h>
#include <phhepmc/PHHepMCGenEvent.h>

//#include <HFJetTruthGeneration/HFJetDefs.h>

#include <TClonesArray.h>
#include <TMatrixDSym.h>
#include <TTree.h>
#include <TVector3.h>
#include <TRandom3.h>
#include <TRotation.h>

#include <iostream>
#include <map>
#include <utility>
#include <vector>
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
PVReco::PVReco(const string &name) :
	SubsysReco(name),
	_fitter(NULL),
	_track_fitting_alg_name("DafRef"),
	_primary_pid_guess(211),
	_cut_jet(true),
	_cut_Ncluster(false),
	_cut_min_pT(0.1),
	_cut_chi2_ndf(5),
	_cut_jet_pT(20.0),
	_cut_jet_eta(0.6),
	_cut_jet_R(0.4),
	_use_ladder_geom(true),
	_vertex_finder(NULL),
	_vertexing_method("avf-smoothing:1"),
	_clustermap(NULL),
	_trackmap(NULL),
	_vertexmap(NULL),
	_svtxevalstack(NULL),
	_do_eval(false),
	_eval_outname("PVReco_eval_tree.root"),
	_jetmap_name("AntiKt_Truth_r04"),
	_eval_tree(NULL),
	_do_evt_display(false){

		_event = 0;

}

/*
 * Init
 */
int PVReco::Init(PHCompositeNode *topNode) {


	return Fun4AllReturnCodes::EVENT_OK;
}

/*
 * Init run
 */
int PVReco::InitRun(PHCompositeNode *topNode) {

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

	gRandom = new TRandom3(0);

	return Fun4AllReturnCodes::EVENT_OK;
}
/*
 * process_event():
 *  Call user instructions for every event.
 *  This function contains the analysis structure.
 *
 */
int PVReco::process_event(PHCompositeNode *topNode) {
	_event++;
	//if (_event % 1000 == 0)
	if (1)
		cout << PHWHERE << "Events processed: " << _event << endl;

	GetNodes(topNode);

	if (_do_eval)
		reset_eval_variables();


	if ( _hepmc_event_map ){

		for (PHHepMCGenEventMap::ConstIter iter=_hepmc_event_map->begin(); iter!=_hepmc_event_map->end(); ++iter){
			const PHHepMCGenEvent *hepmc_event = iter->second;

			float xx = hepmc_event->get_collision_vertex().x();
			float yy = hepmc_event->get_collision_vertex().y();
			float zz = hepmc_event->get_collision_vertex().z();

			cout << "Read, X: " << xx << ", Y: " << yy << ", Z: " << zz << endl;

			gen_vtx[0] = xx;
			gen_vtx[1] = yy;
			gen_vtx[2] = zz;

		}    
	}//hepmc_event_map

	//smear for seed vertex
	TVector3 seed_vtx(0, 0, gen_vtx[2]+gRandom->Gaus(0,0.0150));


	//! stands for Refit_GenFit_Tracks
	vector<genfit::Track*> rf_gf_tracks_opt0; //inclusive
	rf_gf_tracks_opt0.clear();
	vector<genfit::Track*> rf_gf_tracks_opt1; //require 2 or more MVTX hit
	rf_gf_tracks_opt1.clear();
	vector<genfit::Track*> rf_gf_tracks_opt2; //DCA cut
	rf_gf_tracks_opt2.clear();
	vector<genfit::Track*> rf_gf_tracks_opt3; //inclusive, PHRaveVertexing conversion
	rf_gf_tracks_opt3.clear();
	vector<genfit::Track*> rf_gf_tracks_opt4; //inclusive, PHRaveVertexing conversion
	rf_gf_tracks_opt4.clear();

	vector<PHGenFit::Track*> rf_phgf_tracks;
	rf_phgf_tracks.clear();

	map<unsigned int, unsigned int> svtxtrk_gftrk_map;
	map<unsigned int, unsigned int> svtxtrk_nmvtx_map;
	map<unsigned int, unsigned int> svtxtrk_nintt_map;

	SvtxVertex *vertex = _vertexmap->get(0);

	int n_GOOD = 0;

	if ( _trackmap ){
		for (SvtxTrackMap::Iter iter = _trackmap->begin(); iter != _trackmap->end();
				++iter) {
			SvtxTrack* svtx_track = iter->second;
			if (!svtx_track)
				continue;
			if (!(svtx_track->get_pt()>_cut_min_pT))
				continue;
			//if ((svtx_track->get_chisq()/svtx_track->get_ndf())>_cut_chi2_ndf)
			//	continue;

			n_GOOD++;

			int n_MVTX = 0, n_INTT = 0;
			for (SvtxTrack::ConstClusterIter iter2 = svtx_track->begin_clusters(); iter2!=svtx_track->end_clusters(); iter2++) {

				SvtxCluster* cluster = _clustermap->get(*iter2);
				unsigned int layer = cluster->get_layer();

				if (layer<3) n_MVTX++;
				else if (layer<7) n_INTT++;
			}//cluster

			PHGenFit::Track* rf_phgf_track = MakeGenFitTrack(topNode, svtx_track, vertex);

			if (rf_phgf_track) {
				svtxtrk_gftrk_map[svtx_track->get_id()] = rf_phgf_tracks.size();
				svtxtrk_nmvtx_map[svtx_track->get_id()] = n_MVTX;
				svtxtrk_nintt_map[svtx_track->get_id()] = n_INTT;
				rf_phgf_tracks.push_back(rf_phgf_track);

				rf_gf_tracks_opt0.push_back(rf_phgf_track->getGenFitTrack());

				if ( n_MVTX>=2 ){
					rf_gf_tracks_opt1.push_back(rf_phgf_track->getGenFitTrack());
					if ( svtx_track->get_pt()>1.0 ){
						rf_gf_tracks_opt2.push_back(rf_phgf_track->getGenFitTrack());
					}
				}
			}//rf_phgf_track

			auto genfit_track = TranslateSvtxToGenFitTrack(svtx_track, vertex);
			if ( genfit_track ){

				rf_gf_tracks_opt3.push_back(const_cast<genfit::Track*> (genfit_track));

				if ( n_MVTX>=2 ){
					rf_gf_tracks_opt4.push_back(const_cast<genfit::Track*> (genfit_track));
				}
			}

		}//_trackmap
	}//if ( _trackmap )

	//! find vertex using tracks
	std::vector<genfit::GFRaveVertex*> rave_vertices_opt0;
	rave_vertices_opt0.clear();
	std::vector<genfit::GFRaveVertex*> rave_vertices_opt1;
	rave_vertices_opt1.clear();
	std::vector<genfit::GFRaveVertex*> rave_vertices_opt2;
	rave_vertices_opt2.clear();
	std::vector<genfit::GFRaveVertex*> rave_vertices_opt3;
	rave_vertices_opt3.clear();
	std::vector<genfit::GFRaveVertex*> rave_vertices_opt4;
	rave_vertices_opt4.clear();

	//!
	_vertex_finder->setMethod(_vertexing_method.data());
	if (rf_gf_tracks_opt0.size()>=2){
		try {
			_vertex_finder->findVertices(&rave_vertices_opt0, rf_gf_tracks_opt0);
		}catch (...){
			std::cout << PHWHERE << "GFRaveVertexFactory::findVertices failed!";
		}
	}

	if (rf_gf_tracks_opt1.size()>=2){
		try {
			_vertex_finder->findVertices(&rave_vertices_opt1, rf_gf_tracks_opt1);
		}catch (...){
			std::cout << PHWHERE << "GFRaveVertexFactory::findVertices failed!";
		}
	}

	if (rf_gf_tracks_opt2.size()>=2){
		try {
			_vertex_finder->findVertices(&rave_vertices_opt2, rf_gf_tracks_opt2);
		}catch (...){
			std::cout << PHWHERE << "GFRaveVertexFactory::findVertices failed!";
		}
	}

	if (rf_gf_tracks_opt3.size()>=2){
		try {
			_vertex_finder->findVertices(&rave_vertices_opt3, rf_gf_tracks_opt3);
		}catch (...){
			std::cout << PHWHERE << "GFRaveVertexFactory::findVertices failed!";
		}
	}

	if (rf_gf_tracks_opt4.size()>=2){
		try {
			_vertex_finder->findVertices(&rave_vertices_opt4, rf_gf_tracks_opt4);
		}catch (...){
			std::cout << PHWHERE << "GFRaveVertexFactory::findVertices failed!";
		}
	}
	
	FillVertexMap(
			rave_vertices_opt0, rf_gf_tracks_opt0,
			rave_vertices_opt1, rf_gf_tracks_opt1,
			rave_vertices_opt2, rf_gf_tracks_opt2,
			rave_vertices_opt3, rf_gf_tracks_opt3,
			rave_vertices_opt4, rf_gf_tracks_opt4
			);


  if (!_svtxevalstack) {
    _svtxevalstack = new SvtxEvalStack(topNode);
    _svtxevalstack->set_strict(false);
  } else {
    _svtxevalstack->next_event(topNode);
  }

	SvtxTrackEval *trackeval = _svtxevalstack->get_track_eval();
	SvtxTruthEval *trutheval = _svtxevalstack->get_truth_eval();

	//add track information
	//if ( n_GOOD<=5 ){
	if ( 1 ){
		if ( _trackmap ){
			for (SvtxTrackMap::Iter iter = _trackmap->begin(); iter != _trackmap->end();
					++iter) {
				SvtxTrack* svtx_track = iter->second;
				if (!svtx_track)
					continue;
				if (!(svtx_track->get_pt()>_cut_min_pT))
					continue;

				int n_MVTX = 0, n_INTT = 0;
				for (SvtxTrack::ConstClusterIter iter2 = svtx_track->begin_clusters(); iter2!=svtx_track->end_clusters(); iter2++) {

					SvtxCluster* cluster = _clustermap->get(*iter2);
					unsigned int layer = cluster->get_layer();

					if (layer<3) n_MVTX++;
					else if (layer<7) n_INTT++;
				}//cluster

				track_pt[ntrack] = svtx_track->get_pt();
				track_eta[ntrack] = svtx_track->get_eta();
				track_chiq[ntrack] = svtx_track->get_chisq(); 
				track_ndf[ntrack] = short(svtx_track->get_ndf()); 
				track_nmvtx[ntrack] = n_MVTX;
				track_nintt[ntrack] = n_INTT;

				PHG4Particle* g4particle = trackeval->max_truth_particle_by_nclusters(svtx_track);

				if ( g4particle ){
					track_gpt[ntrack] = sqrt(g4particle->get_px()*g4particle->get_px() + g4particle->get_py()*g4particle->get_py());
					track_gprimary[ntrack] = trutheval->is_primary(g4particle);
				}

				PHGenFit::Track* phgf_track = MakeGenFitTrack(topNode, svtx_track, vertex);
				std::shared_ptr<genfit::MeasuredStateOnPlane> gf_state_beam_line_ca = NULL;
				float dca_2d = -999;
				try {
					gf_state_beam_line_ca = std::shared_ptr<genfit::MeasuredStateOnPlane>(phgf_track->extrapolateToLine(seed_vtx,TVector3(0., 0., 1.)));
					dca_2d = gf_state_beam_line_ca->getState()[3];
				} catch (...) {
				}

				track_dca2dseedvtx[ntrack] = dca_2d;

				ntrack++;

				if ( ntrack>=100 ) break;

			}//
		}//_trackmap
	}//n_GOOD

	//! Fill evaluation tree
	if (_do_eval){
		_eval_tree->Fill();
	}

	//! Clean up
	for (PHGenFit::Track* track: rf_phgf_tracks){
		delete track;
	}
	rf_phgf_tracks.clear();

	for (genfit::GFRaveVertex* vertex: rave_vertices_opt0){
		delete vertex;
	}
	rave_vertices_opt0.clear();

	for (genfit::GFRaveVertex* vertex: rave_vertices_opt1){
		delete vertex;
	}
	rave_vertices_opt1.clear();

	for (genfit::GFRaveVertex* vertex: rave_vertices_opt2){
		delete vertex;
	}
	rave_vertices_opt2.clear();

	for (genfit::GFRaveVertex* vertex: rave_vertices_opt3){
		delete vertex;
	}
	rave_vertices_opt3.clear();

	for (genfit::GFRaveVertex* vertex: rave_vertices_opt4){
		delete vertex;
	}
	rave_vertices_opt4.clear();

	return Fun4AllReturnCodes::EVENT_OK;
}

int PVReco::End(PHCompositeNode *topNode){

	if (_do_eval){
		if(Verbosity() >= 1)
			cout << PHWHERE << " Writing to file: " << _eval_outname << endl;
		PHTFileServer::get().cd(_eval_outname);
		_eval_tree->Write();
	}

	return Fun4AllReturnCodes::EVENT_OK;
}

PVReco::~PVReco(){
	delete _fitter;
	delete _vertex_finder;
}

void PVReco::init_eval_tree(){

	_eval_tree = new TTree("T","PVReco evaluation");
	_eval_tree->Branch("gen_vtx",gen_vtx,"gen_vtx[3]/F");
	_eval_tree->Branch("gf_prim_vtx",gf_prim_vtx,"gf_prim_vtx[3]/F");
	_eval_tree->Branch("gf_prim_vtx_err",gf_prim_vtx_err,"gf_prim_vtx_err[3]/F");
	_eval_tree->Branch("gf_prim_vtx_ntrk",&gf_prim_vtx_ntrk,"gf_prim_vtx_ntrk/I");

	_eval_tree->Branch("rv_prim_vtx_opt0",rv_prim_vtx_opt0,"rv_prim_vtx_opt0[3]/F");
	_eval_tree->Branch("rv_prim_vtx_opt0_err",rv_prim_vtx_opt0_err,"rv_prim_vtx_opt0_err[3]/F");
	_eval_tree->Branch("rv_prim_vtx_opt0_ntrk",&rv_prim_vtx_opt0_ntrk,"rv_prim_vtx_opt0_ntrk/I");
	_eval_tree->Branch("rv_prim_vtx_opt0_ndf",&rv_prim_vtx_opt0_ndf,"rv_prim_vtx_opt0_ndf/S");
	_eval_tree->Branch("rv_prim_vtx_opt0_chi2",&rv_prim_vtx_opt0_chi2,"rv_prim_vtx_opt0_chi2/F");

	_eval_tree->Branch("rv_prim_vtx_opt1",rv_prim_vtx_opt1,"rv_prim_vtx_opt1[3]/F");
	_eval_tree->Branch("rv_prim_vtx_opt1_err",rv_prim_vtx_opt1_err,"rv_prim_vtx_opt1_err[3]/F");
	_eval_tree->Branch("rv_prim_vtx_opt1_ntrk",&rv_prim_vtx_opt1_ntrk,"rv_prim_vtx_opt1_ntrk/I");
	_eval_tree->Branch("rv_prim_vtx_opt1_ndf",&rv_prim_vtx_opt1_ndf,"rv_prim_vtx_opt1_ndf/S");
	_eval_tree->Branch("rv_prim_vtx_opt1_chi2",&rv_prim_vtx_opt1_chi2,"rv_prim_vtx_opt1_chi2/F");

	_eval_tree->Branch("rv_prim_vtx_opt2",rv_prim_vtx_opt2,"rv_prim_vtx_opt2[3]/F");
	_eval_tree->Branch("rv_prim_vtx_opt2_err",rv_prim_vtx_opt2_err,"rv_prim_vtx_opt2_err[3]/F");
	_eval_tree->Branch("rv_prim_vtx_opt2_ntrk",&rv_prim_vtx_opt2_ntrk,"rv_prim_vtx_opt2_ntrk/I");

	_eval_tree->Branch("rv_prim_vtx_opt3",rv_prim_vtx_opt3,"rv_prim_vtx_opt3[3]/F");
	_eval_tree->Branch("rv_prim_vtx_opt3_err",rv_prim_vtx_opt3_err,"rv_prim_vtx_opt3_err[3]/F");
	_eval_tree->Branch("rv_prim_vtx_opt3_ntrk",&rv_prim_vtx_opt3_ntrk,"rv_prim_vtx_opt3_ntrk/I");
	_eval_tree->Branch("rv_prim_vtx_opt4",rv_prim_vtx_opt4,"rv_prim_vtx_opt4[3]/F");
	_eval_tree->Branch("rv_prim_vtx_opt4_err",rv_prim_vtx_opt4_err,"rv_prim_vtx_opt4_err[3]/F");
	_eval_tree->Branch("rv_prim_vtx_opt4_ntrk",&rv_prim_vtx_opt4_ntrk,"rv_prim_vtx_opt4_ntrk/I");

	_eval_tree->Branch("ntrack",&ntrack,"ntrack/I");
	_eval_tree->Branch("track_pt",track_pt,"track_pt[ntrack]/F");
	_eval_tree->Branch("track_eta",track_eta,"track_eta[ntrack]/F");
	_eval_tree->Branch("track_chiq",track_chiq,"track_chiq[ntrack]/F");
	_eval_tree->Branch("track_ndf",track_ndf,"track_ndf[ntrack]/S");
	_eval_tree->Branch("track_nmvtx",track_nmvtx,"track_nmvtx[ntrack]/S");
	_eval_tree->Branch("track_nintt",track_nintt,"track_nintt[ntrack]/S");
	_eval_tree->Branch("track_gpt",track_gpt,"track_gpt[ntrack]/F");
	_eval_tree->Branch("track_dca2dseedvtx",track_dca2dseedvtx,"track_dca2dseedvtx[ntrack]/F");
	_eval_tree->Branch("track_gprimary",track_gprimary,"track_gprimary[ntrack]/S");

	_eval_tree->Branch("track_wt_opt0",track_wt_opt0,"track_wt_opt0[ntrack]/F");
	_eval_tree->Branch("track_wt_opt1",track_wt_opt1,"track_wt_opt1[ntrack]/F");

	return;
}

void PVReco::reset_eval_variables(){

	gf_prim_vtx_ntrk = rv_prim_vtx_opt0_ntrk = rv_prim_vtx_opt1_ntrk = rv_prim_vtx_opt2_ntrk = rv_prim_vtx_opt3_ntrk = rv_prim_vtx_opt4_ntrk = 0;
	for (int i=0; i<3; i++){
		gen_vtx[i] = -999;
		gf_prim_vtx[i] = gf_prim_vtx_err[i] = -999;
		rv_prim_vtx_opt0[i] = rv_prim_vtx_opt0_err[i] = -999;
		rv_prim_vtx_opt1[i] = rv_prim_vtx_opt1_err[i] = -999;
		rv_prim_vtx_opt2[i] = rv_prim_vtx_opt2_err[i] = -999;
		rv_prim_vtx_opt3[i] = rv_prim_vtx_opt3_err[i] = -999;
		rv_prim_vtx_opt4[i] = rv_prim_vtx_opt4_err[i] = -999;
	}//i

	ntrack = 0;
	for (int i=0; i<100; i++){
		track_pt[i] = track_eta[i] = track_chiq[i] = -999;
		track_ndf[i] = track_nmvtx[i] = track_nintt[i] = -999;
		track_gpt[i] = -999;
		track_dca2dseedvtx[i] = -999;
		track_gprimary[i] = -999;
		track_wt_opt0[i] = track_wt_opt1[i] = -1;
	}

	return;
}

int PVReco::CreateNodes(PHCompositeNode *topNode){

	return Fun4AllReturnCodes::EVENT_OK;
}

/*
 * GetNodes():
 *  Get all the all the required nodes off the node tree
 */
int PVReco::GetNodes(PHCompositeNode * topNode){
	//DST objects

	// Input Svtx Clusters
	_clustermap = findNode::getClass<SvtxClusterMap>(topNode, "SvtxClusterMap");
	if (!_clustermap && _event < 2){
		cout << PHWHERE << " SvtxClusterMap node not found on node tree"
				<< endl;
		return Fun4AllReturnCodes::ABORTEVENT;
	}

	// Input Svtx Tracks
	_trackmap = findNode::getClass<SvtxTrackMap>(topNode, "SvtxTrackMap");
	if (!_trackmap && _event < 2){
		cout << PHWHERE << " SvtxClusterMap node not found on node tree"
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

	_hepmc_event_map = findNode::getClass<PHHepMCGenEventMap>(topNode,"PHHepMCGenEventMap");
	if (!_hepmc_event_map && _event<2)
	{
		cout << PHWHERE << " PHHepMCGenEventMap node not found on node tree" << endl;
	}

	return Fun4AllReturnCodes::EVENT_OK;
}


PHGenFit::Track* PVReco::MakeGenFitTrack(PHCompositeNode *topNode, const SvtxTrack* intrack, const SvtxVertex* vertex){


	if (!intrack){
		cerr << PHWHERE << " Input SvtxTrack is NULL!" << endl;
		return NULL;
	}

	SvtxHitMap* hitsmap = NULL;
	hitsmap = findNode::getClass<SvtxHitMap>(topNode, "SvtxHitMap");
	if (!hitsmap) {
		cout << PHWHERE << "ERROR: Can't find node SvtxHitMap" << endl;
		return NULL;
	}

	PHG4CellContainer* cells_svtx = findNode::getClass<PHG4CellContainer>(topNode,"G4CELL_TPC");
	PHG4CellContainer* cells_intt = findNode::getClass<PHG4CellContainer>(topNode,"G4CELL_INTT");
	PHG4CellContainer* cells_maps = findNode::getClass<PHG4CellContainer>(topNode,"G4CELL_MVTX");

	if (_use_ladder_geom and !cells_svtx and !cells_intt and !cells_maps) {
	//if (_use_ladder_geom and !cells_intt and !cells_maps) {
		cout << PHWHERE << "No PHG4CellContainer found!" << endl;
		return NULL;
	}

	PHG4CylinderGeomContainer* geom_container_intt = findNode::getClass<PHG4CylinderGeomContainer>(topNode, "CYLINDERGEOM_INTT");
	PHG4CylinderGeomContainer* geom_container_maps = findNode::getClass<PHG4CylinderGeomContainer>(topNode, "CYLINDERGEOM_MVTX");

	if (_use_ladder_geom and !geom_container_intt and !geom_container_maps) {
		cout << PHWHERE << "No PHG4CylinderGeomContainer found!" << endl;
		return NULL;
	}

	// prepare seed
	TVector3 seed_mom(100, 0, 0);
	TVector3 seed_pos(0, 0, 0);
	TMatrixDSym seed_cov(6);
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 6; j++) {
			seed_cov[i][j] = 100.;
		}
	}

	/*
	TVector3 seed_pos(intrack->get_x(), intrack->get_y(), intrack->get_z());
	TVector3 seed_mom(intrack->get_px(), intrack->get_py(), intrack->get_pz());
	TMatrixDSym seed_cov(6);
	for (int i=0; i<6; i++){
		for (int j=0; j<6; j++){
			seed_cov[i][j] = intrack->get_error(i,j);
		}
	}
	*/

	// Create measurements
	std::vector<PHGenFit::Measurement*> measurements;


	std::map<float, unsigned int> m_r_cluster_id;

	for (auto iter = intrack->begin_clusters(); iter != intrack->end_clusters(); ++iter){
		unsigned int cluster_id = *iter;
		SvtxCluster* cluster = _clustermap->get(cluster_id);
		float x = cluster->get_x();
		float y = cluster->get_y();
		float r = sqrt(x*x+y*y);
		m_r_cluster_id.insert(std::pair<float, unsigned int>(r, cluster_id));
	}

	for (auto iter = m_r_cluster_id.begin(); iter != m_r_cluster_id.end(); ++iter){
	//for (SvtxTrack::ConstClusterIter iter = intrack->begin_clusters(); iter != intrack->end_clusters(); ++iter){
		unsigned int cluster_id = iter->second;
		//unsigned int cluster_id = *iter;
		SvtxCluster* cluster = _clustermap->get(cluster_id);
		if (!cluster) {
			LogError("No cluster Found!");
			continue;
		}

		TVector3 pos(cluster->get_x(), cluster->get_y(), cluster->get_z());

		seed_mom.SetPhi(pos.Phi());
		seed_mom.SetTheta(pos.Theta());

		//TODO use u, v explicitly?
		TVector3 n(cluster->get_x(), cluster->get_y(), 0);

		unsigned int begin_hit_id = *(cluster->begin_hits());
		SvtxHit* svtxhit = hitsmap->find(begin_hit_id)->second;

		unsigned int layer = cluster->get_layer();

		PHG4Cell* cell_svtx = nullptr;
		PHG4Cell* cell_intt = nullptr;
		PHG4Cell* cell_maps = nullptr;

		if(cells_svtx) cell_svtx = cells_svtx->findCell(svtxhit->get_cellid());
		if(cells_intt) cell_intt = cells_intt->findCell(svtxhit->get_cellid());
		if(cells_maps) cell_maps = cells_maps->findCell(svtxhit->get_cellid());
		if(!(cell_svtx or cell_intt or cell_maps)){
			if(Verbosity()>=0)
				LogError("!(cell_svtx or cell_intt or cell_maps)");
			continue;
		}   

		//float phi_tilt[7] = {0.304, 0.304, 0.304, 0.244, 0.244, 0.209, 0.201};

		//NEW
		if (cell_maps) {
			PHG4Cell* cell = cell_maps;

			int stave_index = cell->get_stave_index();
			int half_stave_index = cell->get_half_stave_index();
			int module_index = cell->get_module_index();
			int chip_index = cell->get_chip_index();

			double ladder_location[3] = { 0.0, 0.0, 0.0 };
			PHG4CylinderGeom_MVTX *geom = (PHG4CylinderGeom_MVTX*) geom_container_maps->GetLayerGeom(layer);
			// returns the center of the sensor in world coordinates - used to get the ladder phi location
			geom->find_sensor_center(stave_index, half_stave_index, module_index, chip_index, ladder_location);
			n.SetXYZ(ladder_location[0], ladder_location[1], 0);
			n.RotateZ(geom->get_stave_phi_tilt());
		} else if (cell_intt) {
			PHG4Cell* cell = cell_intt;
			PHG4CylinderGeomINTT* geom = (PHG4CylinderGeomINTT*) geom_container_intt->GetLayerGeom(layer);
			double hit_location[3] = { 0.0, 0.0, 0.0 };
			geom->find_segment_center(cell->get_ladder_z_index(), cell->get_ladder_phi_index(), hit_location);

			n.SetXYZ(hit_location[0], hit_location[1], 0);
			n.RotateZ(geom->get_strip_phi_tilt());
		}

		PHGenFit::Measurement* meas = new PHGenFit::PlanarMeasurement(pos, n,
				cluster->get_rphi_error(), cluster->get_z_error());

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

	TVector3 vertex_position(0, 0, 0);
	if (vertex){
		vertex_position.SetXYZ(vertex->get_x(), vertex->get_y(), vertex->get_z());
	}

	std::shared_ptr<genfit::MeasuredStateOnPlane> gf_state_vertex_ca = NULL;
	try {
		gf_state_vertex_ca = std::shared_ptr < genfit::MeasuredStateOnPlane> (track->extrapolateToPoint(vertex_position));
	} catch (...) {
		if (Verbosity() >= 2)
			LogWarning("extrapolateToPoint failed!");
		return NULL;
	}

	TVector3 mom = gf_state_vertex_ca->getMom();
	TMatrixDSym cov = gf_state_vertex_ca->get6DCov();

	//cout << "OUT Ex: " << sqrt(cov[0][0]) << ", Ey: " << sqrt(cov[1][1]) << ", Ez: " << sqrt(cov[2][2]) << endl;
	//cout << "IN Px: " << intrack->get_px() << ", Py: " << intrack->get_py() << ", Pz: " << intrack->get_pz() << endl; 
	//cout << "OUT Px: " << mom.X() << ", Py: " << mom.Y() << ", Pz: " << mom.Z() << endl; 

	return track;
}


/*
 * Fill SvtxVertexMap from GFRaveVertexes and Tracks
 */
void PVReco::FillVertexMap(
		const std::vector<genfit::GFRaveVertex*>& rave_vertices,
		const std::vector<genfit::Track*>& gf_tracks){

	for (unsigned int ivtx=0; ivtx<rave_vertices.size(); ++ivtx){
		genfit::GFRaveVertex* rave_vtx = rave_vertices[ivtx];

		//cout << "V0 x: " << rave_vtx->getPos().X() << ", y: " << rave_vtx->getPos().Y() << ", z: " << rave_vtx->getPos().Z() << endl;
		rv_prim_vtx_opt0[0] = rave_vtx->getPos().X();
		rv_prim_vtx_opt0[1] = rave_vtx->getPos().Y();
		rv_prim_vtx_opt0[2] = rave_vtx->getPos().Z();

		rv_prim_vtx_opt0_err[0] = sqrt(rave_vtx->getCov()[0][0]);
		rv_prim_vtx_opt0_err[1] = sqrt(rave_vtx->getCov()[1][1]);
		rv_prim_vtx_opt0_err[2] = sqrt(rave_vtx->getCov()[2][2]);

		rv_prim_vtx_opt0_ntrk = rave_vtx->getNTracks();
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

/*
 * Fill SvtxVertexMap from GFRaveVertexes and Tracks
 */
void PVReco::FillVertexMap(
		const std::vector<genfit::GFRaveVertex*>& rave_vertices_opt0,
		const std::vector<genfit::Track*>& gf_tracks_opt0,
		const std::vector<genfit::GFRaveVertex*>& rave_vertices_opt1,
		const std::vector<genfit::Track*>& gf_tracks_opt1,
		const std::vector<genfit::GFRaveVertex*>& rave_vertices_opt2,
		const std::vector<genfit::Track*>& gf_tracks_opt2,
		const std::vector<genfit::GFRaveVertex*>& rave_vertices_opt3,
		const std::vector<genfit::Track*>& gf_tracks_opt3,
		const std::vector<genfit::GFRaveVertex*>& rave_vertices_opt4,
		const std::vector<genfit::Track*>& gf_tracks_opt4
		){

	for (unsigned int ivtx=0; ivtx<rave_vertices_opt0.size(); ++ivtx){
		genfit::GFRaveVertex* rave_vtx = rave_vertices_opt0[ivtx];

		//cout << "V0 x: " << rave_vtx->getPos().X() << ", y: " << rave_vtx->getPos().Y() << ", z: " << rave_vtx->getPos().Z() << endl;
		rv_prim_vtx_opt0[0] = rave_vtx->getPos().X();
		rv_prim_vtx_opt0[1] = rave_vtx->getPos().Y();
		rv_prim_vtx_opt0[2] = rave_vtx->getPos().Z();

		rv_prim_vtx_opt0_err[0] = sqrt(rave_vtx->getCov()[0][0]);
		rv_prim_vtx_opt0_err[1] = sqrt(rave_vtx->getCov()[1][1]);
		rv_prim_vtx_opt0_err[2] = sqrt(rave_vtx->getCov()[2][2]);

		rv_prim_vtx_opt0_ntrk = rave_vtx->getNTracks();
		rv_prim_vtx_opt0_ndf = rave_vtx->getNdf();
		rv_prim_vtx_opt0_chi2 = rave_vtx->getChi2();

		short nmatched = 0;
		//tracks associated to vertex
		for (int itrk=0; itrk<rv_prim_vtx_opt0_ntrk; itrk++){
			genfit::GFRaveTrackParameters *gftrk_par = rave_vtx->getParameters(itrk);
			const genfit::Track *gftrk = gftrk_par->getTrack(); 
			for (int jtrk=0; jtrk<rv_prim_vtx_opt0_ntrk; jtrk++){
				if ( gftrk==gf_tracks_opt0[jtrk] ){
					nmatched++;
					track_wt_opt0[jtrk] = gftrk_par->getWeight();
					break;
				}
			}//jtrk
		}//itrk

		cout << "ntotal: " << rv_prim_vtx_opt0_ntrk << ", nused: " << rv_prim_vtx_opt0_ntrk << ", nmatched: " << nmatched << endl; 
	}

	for (unsigned int ivtx=0; ivtx<rave_vertices_opt1.size(); ++ivtx){
		genfit::GFRaveVertex* rave_vtx = rave_vertices_opt1[ivtx];

		//cout << "V0 x: " << rave_vtx->getPos().X() << ", y: " << rave_vtx->getPos().Y() << ", z: " << rave_vtx->getPos().Z() << endl;
		rv_prim_vtx_opt1[0] = rave_vtx->getPos().X();
		rv_prim_vtx_opt1[1] = rave_vtx->getPos().Y();
		rv_prim_vtx_opt1[2] = rave_vtx->getPos().Z();

		rv_prim_vtx_opt1_err[0] = sqrt(rave_vtx->getCov()[0][0]);
		rv_prim_vtx_opt1_err[1] = sqrt(rave_vtx->getCov()[1][1]);
		rv_prim_vtx_opt1_err[2] = sqrt(rave_vtx->getCov()[2][2]);

		rv_prim_vtx_opt1_ntrk = rave_vtx->getNTracks();
		rv_prim_vtx_opt1_ndf = rave_vtx->getNdf();
		rv_prim_vtx_opt1_chi2 = rave_vtx->getChi2();

		short nmatched = 0;
		for (int itrk=0; itrk<rv_prim_vtx_opt1_ntrk; itrk++){
			genfit::GFRaveTrackParameters *gftrk_par = rave_vtx->getParameters(itrk);
			const genfit::Track *gftrk = gftrk_par->getTrack(); 
			for (int jtrk=0; jtrk<rv_prim_vtx_opt0_ntrk; jtrk++){
				if ( gftrk==gf_tracks_opt0[jtrk] ){
					nmatched++;
					track_wt_opt1[jtrk] = gftrk_par->getWeight();
					break;
				}
			}//jtrk
		}//itrk

		cout << "ntotal: " << rv_prim_vtx_opt0_ntrk << ", nused: " << rv_prim_vtx_opt1_ntrk << ", nmatched: " << nmatched << endl; 

	}

	for (unsigned int ivtx=0; ivtx<rave_vertices_opt2.size(); ++ivtx){
		genfit::GFRaveVertex* rave_vtx = rave_vertices_opt2[ivtx];

		//cout << "V0 x: " << rave_vtx->getPos().X() << ", y: " << rave_vtx->getPos().Y() << ", z: " << rave_vtx->getPos().Z() << endl;
		rv_prim_vtx_opt2[0] = rave_vtx->getPos().X();
		rv_prim_vtx_opt2[1] = rave_vtx->getPos().Y();
		rv_prim_vtx_opt2[2] = rave_vtx->getPos().Z();

		rv_prim_vtx_opt2_err[0] = sqrt(rave_vtx->getCov()[0][0]);
		rv_prim_vtx_opt2_err[1] = sqrt(rave_vtx->getCov()[1][1]);
		rv_prim_vtx_opt2_err[2] = sqrt(rave_vtx->getCov()[2][2]);

		rv_prim_vtx_opt2_ntrk = rave_vtx->getNTracks();
	}

	for (unsigned int ivtx=0; ivtx<rave_vertices_opt3.size(); ++ivtx){
		genfit::GFRaveVertex* rave_vtx = rave_vertices_opt3[ivtx];

		//cout << "V0 x: " << rave_vtx->getPos().X() << ", y: " << rave_vtx->getPos().Y() << ", z: " << rave_vtx->getPos().Z() << endl;
		rv_prim_vtx_opt3[0] = rave_vtx->getPos().X();
		rv_prim_vtx_opt3[1] = rave_vtx->getPos().Y();
		rv_prim_vtx_opt3[2] = rave_vtx->getPos().Z();

		rv_prim_vtx_opt3_err[0] = sqrt(rave_vtx->getCov()[0][0]);
		rv_prim_vtx_opt3_err[1] = sqrt(rave_vtx->getCov()[1][1]);
		rv_prim_vtx_opt3_err[2] = sqrt(rave_vtx->getCov()[2][2]);

		rv_prim_vtx_opt3_ntrk = rave_vtx->getNTracks();
	}

	for (unsigned int ivtx=0; ivtx<rave_vertices_opt4.size(); ++ivtx){
		genfit::GFRaveVertex* rave_vtx = rave_vertices_opt4[ivtx];

		//cout << "V0 x: " << rave_vtx->getPos().X() << ", y: " << rave_vtx->getPos().Y() << ", z: " << rave_vtx->getPos().Z() << endl;
		rv_prim_vtx_opt4[0] = rave_vtx->getPos().X();
		rv_prim_vtx_opt4[1] = rave_vtx->getPos().Y();
		rv_prim_vtx_opt4[2] = rave_vtx->getPos().Z();

		rv_prim_vtx_opt4_err[0] = sqrt(rave_vtx->getCov()[0][0]);
		rv_prim_vtx_opt4_err[1] = sqrt(rave_vtx->getCov()[1][1]);
		rv_prim_vtx_opt4_err[2] = sqrt(rave_vtx->getCov()[2][2]);

		rv_prim_vtx_opt4_ntrk = rave_vtx->getNTracks();
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

void PVReco::FillSVMap(
		const std::vector<genfit::GFRaveVertex*>& rave_vertices,
		const std::vector<genfit::Track*>& gf_tracks){

	return;

}

genfit::Track* PVReco::TranslateSvtxToGenFitTrack(SvtxTrack* svtx_track, const SvtxVertex* vertex) {

	try {
		// The first state is extracted to PCA, second one is the one with measurement
		SvtxTrackState* svtx_state = (++(svtx_track->begin_states()))->second;
		//SvtxTrackState* svtx_state = (svtx_track->begin_states())->second;

		TVector3 pos(svtx_state->get_x(), svtx_state->get_y(), svtx_state->get_z());
		TVector3 mom(svtx_state->get_px(), svtx_state->get_py(), svtx_state->get_pz());
		TMatrixDSym cov(6);
		for(int i=0;i<6;++i) {
			for(int j=0;j<6;++j) {
				cov[i][j] = svtx_state->get_error(i, j);
			}
		}

		genfit::AbsTrackRep * rep = new genfit::RKTrackRep(_primary_pid_guess);
		genfit::Track* genfit_track = new genfit::Track(rep, TVector3(0,0,0), TVector3(0,0,0));

		genfit::FitStatus * fs = new genfit::FitStatus();
		fs->setCharge(svtx_track->get_charge());
		fs->setChi2(svtx_track->get_chisq());
		fs->setNdf(svtx_track->get_ndf());
		fs->setIsFitted(true);
		fs->setIsFitConvergedFully(true);

		genfit_track->setFitStatus(fs, rep);

		genfit::TrackPoint *tp = new genfit::TrackPoint(genfit_track);

		genfit::KalmanFitterInfo* fi = new genfit::KalmanFitterInfo(tp, rep);
		tp->setFitterInfo(fi);

		genfit::MeasuredStateOnPlane * ms = new genfit::MeasuredStateOnPlane(rep);
		ms->setPosMomCov(pos, mom, cov);
		genfit::KalmanFittedStateOnPlane * kfs = new genfit::KalmanFittedStateOnPlane(*ms, 1., 1.);

		//< Acording to the special order of using the stored states
		fi->setForwardUpdate(kfs);

		genfit_track->insertPoint(tp);

		return genfit_track;
	} catch (...) {
		LogDebug("TranslateSvtxToGenFitTrack failed!");
	}

	return nullptr;
}


genfit::Track* PVReco::TranslateSvtxToGenFitTrackPV(SvtxTrack* svtx_track, const SvtxVertex* vertex) {

	try {
		// The first state is extracted to PCA, second one is the one with measurement
		//SvtxTrackState* svtx_state = (++(svtx_track->begin_states()))->second;
		SvtxTrackState* svtx_state = (svtx_track->begin_states())->second;

		TVector3 pos(svtx_state->get_x(), svtx_state->get_y(), svtx_state->get_z());
		TVector3 mom(svtx_state->get_px(), svtx_state->get_py(), svtx_state->get_pz());
		TMatrixDSym cov(6);
		for(int i=0;i<6;++i) {
			for(int j=0;j<6;++j) {
				cov[i][j] = svtx_state->get_error(i, j);
			}
		}

		genfit::AbsTrackRep * rep = new genfit::RKTrackRep(_primary_pid_guess);
		genfit::Track* genfit_track = new genfit::Track(rep, TVector3(0,0,0), TVector3(0,0,0));

		genfit::FitStatus * fs = new genfit::FitStatus();
		fs->setCharge(svtx_track->get_charge());
		fs->setChi2(svtx_track->get_chisq());
		fs->setNdf(svtx_track->get_ndf());
		fs->setIsFitted(true);
		fs->setIsFitConvergedFully(true);

		genfit_track->setFitStatus(fs, rep);

		genfit::TrackPoint *tp = new genfit::TrackPoint(genfit_track);

		genfit::KalmanFitterInfo* fi = new genfit::KalmanFitterInfo(tp, rep);
		tp->setFitterInfo(fi);

		genfit::MeasuredStateOnPlane * ms = new genfit::MeasuredStateOnPlane(rep);
		ms->setPosMomCov(pos, mom, cov);
		genfit::KalmanFittedStateOnPlane * kfs = new genfit::KalmanFittedStateOnPlane(*ms, 1., 1.);

		//< Acording to the special order of using the stored states
		fi->setForwardUpdate(kfs);

		genfit_track->insertPoint(tp);

		return genfit_track;
	} catch (...) {
		LogDebug("TranslateSvtxToGenFitTrack failed!");
	}

	return nullptr;
}

