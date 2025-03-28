
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
  // photonContainer = PiZero::buildPhotonContainer(evt);
  std::vector<const HepMC3::GenParticle*> photonContainer; 
  for (auto part : evt.particles()) 
  {
    if (part->abs_pid() == 22 && ParticleHelper::isFinal(part.get()) )
    {
      // MSG_DEBUG("Found 2 photons.")
      if ( std::abs(part->momentum().eta()) < m_etaMax && part->momentum().pt() > (m_minPt/1.e3) ){
        photonContainer.push_back( part.get() );
      }
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

  if (photonContainer.size() >= 2){
    for (auto photon :photonContainer){
      MSG_DEBUG("[Photon Container] Found 2 photons.")
      auto mother = photon->parents().at(0);
      if( mother->pid() == 111)
      {
        MSG_DEBUG("Has mother PiZero.")
        float eta = photon->momentum().eta();
        float pt = photon->momentum().pt();
        if ( std::abs(eta) < m_etaMax && pt > (m_minPt/1.e3) ){
          realPiZero.push_back( photon );
          MSG_DEBUG("Found a photon.")
        }
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
  if ( realPiZero.empty() ){
    MSG_DEBUG( "[realPiZero] There is no PiZero event inside of this event.");
    throw NotInterestingEvent();
  }
  else{
    MSG_DEBUG( "[realPiZero] Found a PiZero that decays in two photons.");
  }

  if (m_forceForwardPhoton){
    int fwdPhotonCounter = 0;
    for ( auto& e : PiZero ){
      float eta = e->momentum().eta();
      if (std::abs(eta) > 2.5 && std::abs(eta) < 3.2){
        fwdPhotonCounter++;
      }
    }
    if (fwdPhotonCounter == 0 || fwdPhotonCounter > 1){
      MSG_INFO("No forward photons or two forward photons");
      throw NotInterestingEvent();
    }
  }

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


