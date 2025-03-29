
#include <cmath>
#include "PiZero.h"
#include "helper.h"

using namespace generator;

PiZero::PiZero(const std::string name, IGenerator *gen):
  IAlgorithm(gen),
  IMsgService(name)
{
  declareProperty( "EtaMax"               , m_etaMax=1.4                );
  declareProperty( "MinPt"                , m_minPt=0.0                 );
  declareProperty( "ZeroVertexParticles"  , m_zeroVertexParticles=false );
  declareProperty( "ForceForwardPhoton" , m_forceForwardPhoton=false );
}


PiZero::~PiZero()
{;}


StatusCode PiZero::initialize()
{
  setMsgLevel(m_outputLevel);
  if(generator()->initialize().isFailure()){
    MSG_FATAL("Its not possible to initialize the generator. Abort!");
  }

  return StatusCode::SUCCESS;
}

// StatusCode PiZero::buildPhotonContainer(HepMC3::GenEvent evt)
// {
//   std::vector<const HepMC3::GenParticle*> photonContainer; 
//   for (auto part : evt.particles()) 
//   {
//     if (part->abs_pid() == 22 && ParticleHelper::isFinal(part.get()) )
//     {
//       if ( std::abs(eta) < m_etaMax && pt > (m_minPt/1.e3) ){
//         photonContainer.push_back( part.get() );
//       }
//     }
//   }

//   // Sort the photonContainer based on pt() using a lambda function as comparator
//   std::sort(photonContainer.begin(), photonContainer.end(), 
//   [](const HepMC3::GenParticle* a, const HepMC3::GenParticle* b) {
//       return a->momentum().pt() > b->momentum().pt();
//   });

//   return photonContainer;
// }



StatusCode PiZero::execute( generator::Event &ctx )
{

  HepMC3::GenEvent evt( HepMC3::Units::GEV, HepMC3::Units::MM);

  // Generate main event. Quit if too many failures.
  if (generator()->execute(evt).isFailure()){
    return StatusCode::FAILURE;
  }

  std::vector<const HepMC3::GenParticle*> piZeroContainer; 
  for (auto part : evt.particles()) 
  {
    if (part->pid() == 111 && ParticleHelper::isFinal(part.get()) )
    {
      // MSG_DEBUG("Found 2 photons.")
      // if ( std::abs(part->momentum().eta()) < m_etaMax && part->momentum().pt() > (m_minPt/1.e3) ){
        piZeroContainer.push_back( part.get() );
        float piZeroPt = part->momentum().pt() / 1e3;  // Convert pt to GeV if needed
        float piZeroEta = part->momentum().eta();
        float piZeroPhi = part->momentum().phi();
        float piZeroPx = part->momentum().px();
        float piZeroPy = part->momentum().py();
        float piZeroPz = part->momentum().pz();
        MSG_DEBUG(Form("This is a PiZero (%i) with pt = %.3f GeV, eta = %.3f and phi = %.3f and vertex x = %.3f y = %.3f z = %.3f ", part->pid(), piZeroPt, piZeroEta, piZeroPhi, piZeroPx, piZeroPy, piZeroPz))
      // }
    }

    // // Get and print the children (decay products) of the PiZero
    // for (int iChild = 0; iChild < part->children().size(); ++iChild) {
    //   const HepMC3::GenParticle* child = part->children().at(iChild);

    //   if (child != nullptr) {
    //       // Extract child particle properties
    //       int childPid = child->pid();
    //       float childPt = child->momentum().pt() / 1e3;  // Convert pt to GeV
    //       float childEta = child->momentum().eta();
    //       float childPhi = child->momentum().phi();
    //       float childPx = child->momentum().px();
    //       float childPy = child->momentum().py();
    //       float childPz = child->momentum().pz();

    //       // Print child particle information
    //       MSG_DEBUG(Form("PiZero child: pid = %i, pt = %.3f GeV, eta = %.3f, phi = %.3f, px = %.3f, py = %.3f, pz = %.3f", 
    //                      childPid, childPt, childEta, childPhi, childPx, childPy, childPz));
    //     }
    // }
  }


  // photonContainer = PiZero::buildPhotonContainer(evt);
  std::vector<const HepMC3::GenParticle*> photonContainer; 
  for (auto part : evt.particles()) 
  {
    if (part->pid() == 22 && ParticleHelper::isFinal(part.get()) )
    {
      // MSG_DEBUG("Found 2 photons.")
      // if ( std::abs(part->momentum().eta()) < m_etaMax && part->momentum().pt() > (m_minPt/1.e3) ){
      photonContainer.push_back( part.get() );
      // }
    }
  }

  // // Sort the photonContainer based on pt() using a lambda function as comparator
  // std::sort(photonContainer.begin(), photonContainer.end(), 
  // [](const HepMC3::GenParticle* a, const HepMC3::GenParticle* b) {
  //     return a->momentum().pt() > b->momentum().pt();
  // });
    
  const auto main_event_t = sample_t();
  const auto main_event_z = sample_z();

  std::vector<const HepMC3::GenParticle*> PiZero;
  std::vector<const HepMC3::GenParticle*> realPiZero;

  // // Get the mother of the current photon
  // auto mother = photon->parents().at(0);

  // // Check if this photon has a Pi0 mother
  // if (mother->pid() == 111) {
  //     MSG_DEBUG("Has mother PiZero.")

  //     // Store the first photon’s Pi0 mother
  //     if (!pi0Mother) {
  //         pi0Mother = mother;
  //     }

  //     // Check if the photon has the same Pi0 mother
  //     if (mother == pi0Mother) {
  //         // If the photon has the same Pi0 mother, check the kinematic cuts
  //         float eta = photon->momentum().eta();
  //         float pt = photon->momentum().pt();
  //         if (std::abs(eta) < m_etaMax && pt > (m_minPt / 1.e3)) {
  //             realPiZero.push_back(photon);
  //             MSG_DEBUG("Found a photon with the same Pi0 mother.")
  //         }
  //     } else {
  //         MSG_DEBUG("Photon has a different mother.");
  //     }

  if (photonContainer.size() >= 2){
    // const xAOD::TruthParticle* pi0Mother = nullptr;
    for (auto photon :photonContainer){
      MSG_DEBUG("[Photon Container] Found 2 photons.")
      auto mother = photon->parents().at(0);
      if( mother->pid() == 111)
      {
        float motherPt = mother->momentum().pt() / 1e3;  // Convert pt to GeV if needed
        float motherPhi = mother->momentum().phi();
        float motherEta = mother->momentum().eta();
        float motherPx = mother->momentum().px();
        float motherPy = mother->momentum().py();
        float motherPz = mother->momentum().pz();
        MSG_DEBUG(Form("Has mother PiZero (%i) with pt = %.3f GeV, eta = %.3f and phi = %.3f and vertex x = %.3f y = %.3f z = %.3f ", mother->pid(), motherPt, motherEta, motherPhi, motherPx, motherPy, motherPz))
        float eta = photon->momentum().eta();
        float pt = photon->momentum().pt();
        if (std::abs(eta) < m_etaMax && pt > (m_minPt / 1.e3)) {
              realPiZero.push_back(photon);
              MSG_DEBUG("Found a photon with Pi0 mother.")
        }
        
        // float motherPt = mother->momentum().pt() / 1e3;  // Convert pt to GeV if needed
        // float motherEta = mother->momentum().eta();
        // MSG_DEBUG(Form("Has mother PiZero with pt = %.3f GeV and eta = %.3f", motherPt, motherEta))
        // float eta = photon->momentum().eta();
        // float pt = photon->momentum().pt();
        // if ( std::abs(eta) < m_etaMax && pt > (m_minPt/1.e3) ){
        //   realPiZero.push_back( photon );
        //   MSG_DEBUG("Found a photon.")
        // }
      }// From Pi0?
    }
  }

  for (auto part : evt.particles()) 
  {
    // MSG_DEBUG("Particle info:\n" <<
    //   "PID: " << part->abs_pid() << "\n" <<
    //   "Pt: " << part->momentum().pt() << "\n" <<
    //   "Eta: " << part->momentum().eta() << "\n" <<
    //   "Phi: " << part->momentum().phi() << "\n" <<
    //   "Final State: " << ParticleHelper::isFinal(part.get()));
    // Is photon and final state?
    if (part->abs_pid() == 22 && ParticleHelper::isFinal(part.get()) )
    {
      if(part->parents().empty()){
        continue;
      }

      auto mother = part->parents().at(0);
      // The mother is Pi0?
      if( mother->pid() == 111)
      {
        float eta = part->momentum().eta();
        float pt = part->momentum().pt();
        if ( std::abs(eta) < m_etaMax && pt > (m_minPt/1.e3) ){
          PiZero.push_back( part.get() );
        }
      }// From Pi0?
    }// Is photon?
  }

  if ( PiZero.empty() ){
    MSG_DEBUG( "There is no PiZero event inside of this event.");
    // throw NotInterestingEvent();
  }

  // checks if there's a real PiZero that has two photons decaying from same particle pdgid
  if ( realPiZero.size() < 2 ){
    MSG_DEBUG( "[realPiZero] There is 1 or 0 photons in PiZero event inside of this event.");
    throw NotInterestingEvent();
  }

  // if (m_forceForwardPhoton){
  //   int fwdPhotonCounter = 0;
  //   for ( auto& e : PiZero ){
  //     float eta = e->momentum().eta();
  //     if (std::abs(eta) > 2.5 && std::abs(eta) < 3.2){
  //       fwdPhotonCounter++;
  //     }
  //   }
  //   if (fwdPhotonCounter == 0 || fwdPhotonCounter > 1){
  //     MSG_INFO("No forward photons or two forward photons");
  //     throw NotInterestingEvent();
  //   }
  // }

  MSG_INFO("Filling PiZero events into the context...");

  // Each photon is a seed
  for ( auto& p : realPiZero ){
  // for ( auto& p : PiZero ){

    auto seed = generator::Seed( p->momentum().eta(), p->momentum().phi() );

    const auto vtx    = p->production_vertex();    
    float zVertexPos  = vtx->position().pz() + main_event_z;

    // Calibration use only.
    if (m_zeroVertexParticles){
      zVertexPos  = vtx->position().pz();
    }

    seed.emplace_back( 1, 0,
                       p->pid(), 
                       p->momentum().px(), 
                       p->momentum().py(), 
                       p->momentum().pz(), 
                       p->momentum().eta(), 
                       p->momentum().phi(), 
                       vtx->position().px(), 
                       vtx->position().py(), 
                       //  vtx->position().pz() + main_event_z,
                       zVertexPos,
                       vtx->position().t() + main_event_t, 
                       p->momentum().e(), 
                       p->momentum().pt() ); 
    // MSG_INFO("Add particle with PDGID " << e->pid() << " into the context.");
    MSG_INFO("Add particle with PDGID " << p->pid() << " and vertex x="<< vtx->position().px() <<", y=" <<vtx->position().py() <<", z=" << zVertexPos <<", t="<< vtx->position().t()+main_event_t <<" into the context."); 
    ctx.push_back( seed );
  }
  

  ctx.setEventNumber(evt.event_number());
  return StatusCode::SUCCESS;
}



StatusCode PiZero::finalize()
{
  MSG_INFO( "Finalize the PiZero Event." );
  if(generator()->finalize().isFailure()){
    MSG_FATAL("Its not possible to finalize the generator. Abort!");
  }
  return StatusCode::SUCCESS;
}


