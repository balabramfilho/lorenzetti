#ifndef Evtgen_PiZero_h
#define Evtgen_PiZero_h

#include "GenKernel/IAlgorithm.h"
#include "GenKernel/IGenerator.h"

namespace generator{

  class PiZero : public IAlgorithm
  {
    public:
    
      PiZero(const std::string , IGenerator *);
      ~PiZero();
  
      virtual StatusCode initialize() override;
      virtual StatusCode execute( Event& ) override;
      // virtual std::vector<const HepMC3::GenParticle*> buildPhotonContainer ( Evt& );
      StatusCode displayParticleInfo(const HepMC3::GenParticle* particle);
      StatusCode displayMotherInfo(const HepMC3::GenParticle* particle);
      virtual StatusCode finalize() override;
  
    private:
      /**
       * @brief Minimum pT required for the electron/positron pair
       * 
       */
      float m_minPt;
      /**
       * @brief Maximum eta allowed for the electron/positron pair
       * 
       */
      float m_etaMax;
      /**
       * @brief If true, includes original pz position  (zero) without fluctuations
       * Useful for calibration purposes
       * 
       */
      bool  m_zeroVertexParticles;
      /**
       * @brief If true, forces one and only one photon/positron to be forward
       * 
       */
      bool  m_forceForwardPhoton;
     
  };

}
#endif
