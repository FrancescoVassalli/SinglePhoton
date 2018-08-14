#include "CalorimeterProjector.h"

#include <phool/getClass.h>
#include <fun4all/Fun4AllServer.h>

#include <phool/PHCompositeNode.h>

CalorimeterProjector::CalorimeterProjector() : SubsysReco("SinglePhoton")
{

}

int CalorimeterProjector::Init(PHCompositeNode *topNode)
{
  
  return 0;
}

int CalorimeterProjector::process_event(PHCompositeNode *topNode)
{

  return 0;
}

int CalorimeterProjector::End(PHCompositeNode *topNode)
{
  return 0;
}

