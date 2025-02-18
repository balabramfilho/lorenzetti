#ifndef Evtgen_Pi0_h
#define Evtgen_Pi0_h

#include "GenKernel/IAlgorithm.h"
#include "GenKernel/IGenerator.h"

namespace generator{

  class Pi0 : public IAlgorithm
  {
    public:
    
      Pi0(const std::string , IGenerator *);
      ~Pi0();
  
      virtual StatusCode initialize() override;
      virtual StatusCode execute( Event& ) override;
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
