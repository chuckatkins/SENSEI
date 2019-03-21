#include "InTransitDataAdaptor.h"
#include "MeshMetadata.h"
#include "Partitioner.h"
#include "ConfigurablePartitioner.h"
#include "BlockPartitioner.h"
#include "Error.h"

#include <pugixml.hpp>

#include <vtkDataObject.h>
#include <vtkObjectFactory.h>

#include <map>
#include <vector>
#include <string>
#include <utility>

namespace sensei
{

struct InTransitDataAdaptor::InternalsType
{
  InternalsType() : Part(new BlockPartitioner) {}
  ~InternalsType() {}

  std::unique_ptr<sensei::Partitioner> Part;
  std::map<unsigned int, MeshMetadataPtr> ReceiverMetadata;
};

//----------------------------------------------------------------------------
InTransitDataAdaptor::InTransitDataAdaptor()
{
  this->Internals = new InternalsType;
}

//----------------------------------------------------------------------------
int InTransitDataAdaptor::Initialize(pugi::xml_node &node)
{
  // look for the presense of an optional partitioner spec
  pugi::xml_node partNode = node.child("partitioner");
  if (partNode)
    {
    // create and initialize the partitioner
    PartitionerPtr tmp = PartitionerPtr(new ConfigurablePartitioner);
    if (tmp->Initialize(partNode))
      {
      SENSEI_ERROR("Failed to initialize the partitioner from XML")
      return -1;
      }
    this->Internals->Part = std::move(tmp);
    }

  return 0;
}

//----------------------------------------------------------------------------
InTransitDataAdaptor::~InTransitDataAdaptor()
{
  delete this->Internals;
}

//----------------------------------------------------------------------------
void InTransitDataAdaptor::SetPartitioner(sensei::PartitionerPtr &partitioner)
{
  this->Internals->Part = std::move(partitioner);
}

//----------------------------------------------------------------------------
sensei::Partitioner *InTransitDataAdaptor::GetPartitioner()
{
  return this->Internals->Part.get();
}

//----------------------------------------------------------------------------
int InTransitDataAdaptor::GetReceiverMeshMetadata(unsigned int id,
  MeshMetadataPtr &metadata)
{
  std::map<unsigned int, MeshMetadataPtr>::iterator it =
    this->Internals->ReceiverMetadata.find(id);

  // don't report the error here, as caller may handle it
  if (it == this->Internals->ReceiverMetadata.end())
    return -1;

  metadata = it->second;
  return 0;
}

//----------------------------------------------------------------------------
int InTransitDataAdaptor::SetReceiverMeshMetadata(unsigned int id,
  MeshMetadataPtr &metadata)
{
  this->Internals->ReceiverMetadata[id] = metadata;
  return 0;
}
/*
//----------------------------------------------------------------------------
void InTransitDataAdaptor::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
*/
}
