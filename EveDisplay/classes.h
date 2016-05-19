/**
 * @file classes.h
 * @brief Dictionary selection for RecoBase
 * 
 * Original author Rob Kutschke, modified by klg
 * 
 * @note The system is not able to deal with
 * `art::Wrapper<std::vector<std::string>>`.
 * The problem is somewhere inside ROOT's REFLEX mechanism
 * and Philippe Canal says that it is (as of March 2010) a known problem.
 * He also says that they do not have any plans to fix it soon.
 * We can always work around it by putting the string inside another object.
 */

#include "art/Persistency/Common/PtrVector.h" 
#include "art/Persistency/Common/Wrapper.h"
#include "art/Persistency/Common/Assns.h"

#include "TEveTrack.h"
#include "TEveScene.h"

// make dummy variables of the PtrVector types so that we are
// sure to generate the dictionaries for them
namespace {
  art::PtrVector<TEveTrack>       teev;
  art::PtrVector<TEveScene>       teev;
}

//
// Only include objects that we would like to be able to put into the event.
// Do not include the objects they contain internally.
//

template class std::vector<TEveTrack>;
template class std::vector<TEveScene>;

template class art::Ptr<TEveTrack>;
template class art::Ptr<TEveScene>;

template class std::pair< art::Ptr<TEveTrack>, art::Ptr<recob::Track>      >; 
template class std::pair< art::Ptr<TEveScene>, art::Ptr<recob::Track>      >; 

template class art::Assns<TEveTrack,       recob::Track,      void>;
template class art::Assns<TEveScene,       TEveTrack,      void>;

template class art::Wrapper< std::vector< TEveTrack                        > >;
template class art::Wrapper< std::vector< TEveScene                        > >;
template class art::Wrapper< std::vector< art::PtrVector< TEveTrack        > >;
template class art::Wrapper< std::vector< art::PtrVector< TEveScene        > >;

template class art::Wrapper< art::Assns<TEveTrack,       recob::Track,      void> >;
template class art::Wrapper< art::Assns<TEveScene,       TEveTrack,      void> >;
