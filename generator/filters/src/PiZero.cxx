
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

StatusCode PiZero::displayParticleInfo(const HepMC3::GenParticle* particle){
  // Retrieve momentum properties of the mother particle
  int pid = particle->pid();
  float pt = particle->momentum().pt() / 1e3;  // Convert pt to GeV
  float eta = particle->momentum().eta();
  float phi = particle->momentum().phi();
  float px = particle->momentum().px();
  float py = particle->momentum().py();
  float pz = particle->momentum().pz();

  MSG_DEBUG(Form("This particle has PID = %i with pt = %.3f GeV, eta = %.3f and phi = %.3f and pos x = %.3f y = %.3f z = %.3f ", pid, pt, eta, phi, px, py, pz))

  return StatusCode::SUCCESS;
}

StatusCode PiZero::displayMotherInfo(const HepMC3::GenParticle* particle){
  auto mother = particle->parents().at(0); // Get the first parent
  // Retrieve momentum properties of the mother particle
  int pid = mother->pid();
  float pt = mother->momentum().pt() / 1e3;  // Convert pt to GeV
  float eta = mother->momentum().eta();
  float phi = mother->momentum().phi();
  float px = mother->momentum().px();
  float py = mother->momentum().py();
  float pz = mother->momentum().pz();

  MSG_DEBUG(Form("This mother particle has PID = %i with pt = %.3f GeV, eta = %.3f and phi = %.3f and pos x = %.3f y = %.3f z = %.3f ", pid, pt, eta, phi, px, py, pz))

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
  std::vector<const HepMC3::GenParticle*> photonContainer;
  bool display_particle_information = true;
  for (auto part : evt.particles()) 
  {
    if (part->pid() == 111 )
    {
      // MSG_DEBUG("Adding one Pi0 to PiZeroContainer.")
      piZeroContainer.push_back( part.get() );
    }
    if (part->pid() == 22 && ParticleHelper::isFinal(part.get()) )
    {
      // MSG_DEBUG("Adding one Pi0 to PiZeroContainer.")
      photonContainer.push_back( part.get() );
    }
  }

  // Sort the photonContainer based on pt() using a lambda function as comparator
  std::sort(photonContainer.begin(), photonContainer.end(), 
  [](const HepMC3::GenParticle* a, const HepMC3::GenParticle* b) {
      return a->momentum().pt() > b->momentum().pt();
  });
    
  const auto main_event_t = sample_t();
  const auto main_event_z = sample_z();

  std::vector<const HepMC3::GenParticle*> PiZero;
  std::vector<const HepMC3::GenParticle*> realPiZero;

  // build realPizero
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
        displayMotherInfo(photon); // mother info
        float eta = photon->momentum().eta();
        float pt = photon->momentum().pt();
        if (std::abs(eta) < m_etaMax && pt > (m_minPt / 1.e3)) {
              realPiZero.push_back(photon);
              displayParticleInfo(photon); // photon info
              MSG_INFO("Add particle with PDGID " << photon->pid() << " and pos x="<< photon->momentum().px() <<", y=" <<photon->momentum().py() <<", z=" << photon->momentum().pz() <<" into the context."); 
              MSG_DEBUG("Found a photon with Pi0 mother.")
        }
      }// From Pi0?
    }
  }

  // build regular piZero
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

  // if ( PiZero.empty() ){
  //   MSG_DEBUG( "There is no PiZero event inside of this event.");
  //   // throw NotInterestingEvent();
  // }

  // checks if there's a real PiZero that has two photons decaying from same particle pdgid
  if ( realPiZero.size() < 2 ){
    MSG_DEBUG( "[realPiZero] There is 1 or 0 photons in PiZero event inside of this event.");
    throw NotInterestingEvent();
  }

  MSG_INFO("Filling realPiZero events into the context...");

  MSG_DEBUG("Print all photons in  photonContainer...");
  for (auto photon :photonContainer){
    displayParticleInfo(photon);
  }
  MSG_DEBUG("[END] Print all photons in  photonContainer...");
  MSG_DEBUG("Print all piZero in  piZeroContainer...");
  for (auto piZero :piZeroContainer){
    displayParticleInfo(piZero);
  }
  MSG_DEBUG("[END] Print all piZero in  piZeroContainer...");

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
    // const HepMC3::GenParticle* mother = p->parents().at(0);
    displayMotherInfo(p); // mother info
    displayParticleInfo(p); // photon info
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


