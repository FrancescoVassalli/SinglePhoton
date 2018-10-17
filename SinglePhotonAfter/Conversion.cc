#include "Conversion.h"

#include <g4eval/SvtxTrackEval.h>
int Conversion::setRecoTracks(SvtxTrackEval* trackeval){	
  if (e1)
  {
  	reco1=trackeval->best_track_from(e1); // have not checked that these are in range 
  }
  if (e2)
  {
  	reco2=trackeval->best_track_from(e1);
  }
  int r=0;
  if (reco1)
  {
  	r++;
  }
  if (reco2)
  {
  	r++;
  }
  return r;
}
