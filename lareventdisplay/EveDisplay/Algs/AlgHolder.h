// author: Andrew Olivier aoliv23@lsu.edu
// brief:  Holds a heterogeneous collection of algorithm objects with properties detailed below
// date:   5/17/2016

//Template Parameter
//ALG: An algorithm that constructs TEveScene objects
//An ALG must have the following methods: 
//*ALG::ALG()
//*void ALG::reconfigure(const fhicl::ParameterSet& p)
//*TEveScene* ALG::makeScene(const art::Event& evt)
//*void ALG::initialize()

//c++ include
#include <memory>

#ifndef ALGS_ALGHOLDER_H
#define ALGS_ALGHOLDER_H

namespace eved
{
  template <typename ALG>
  struct SingleAlgHolder
  {
    SingleAlgHolder() { this->fAlg.reset(new ALG()); }
    ALG* get() { return this->fAlg.get(); }
  
    private:
      std::unique_ptr<ALG> fAlg;
  };
}

#endif
