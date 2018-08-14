#ifndef CALORIMETERPROJECTOR_H__
#define CALORIMETERPROJECTOR_H__

// --- need to check all these includes...
#include <fun4all/SubsysReco.h>

class PHCompositeNode;

class CalorimeterProjector: public SubsysReco
{
 public:
  CalorimeterProjector();
  int Init(PHCompositeNode*);
  int process_event(PHCompositeNode*);
  int End(PHCompositeNode*);

 private:
  inline float deltaR( float eta1, float eta2, float phi1, float phi2 ) {
    float deta = eta1 - eta2;
    float dphi = phi1 - phi2;
    if ( dphi > 3.14159 ) dphi -= 2 * 3.14159;
    if ( dphi < -3.14159 ) dphi += 2 * 3.14159;
    return sqrt( pow( deta, 2 ) + pow( dphi, 2 ) );
  }
};

#endif // __CALORIMETERPROJECTOR_H__
