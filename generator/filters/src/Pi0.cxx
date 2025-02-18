
#include <cmath>
#include "Pi0.h"
#include "helper.h"

using namespace generator;

Pi0::Pi0(const std::string name, IGenerator *gen):
  IAlgorithm(gen),
  IMsgService(name)
{
  declareProperty( "EtaMax"               , m_etaMax=1.4                );
  declareProperty( "MinPt"                , m_minPt=0.0                 );
  declareProperty( "ZeroVertexParticles"  , m_zeroVertexParticles=false );
  declareProperty( "ForceForwardPhoton" , m_forceForwardPhoton=false );
}


Pi0::~Pi0()
{;}


StatusCode Pi0::initialize()
{
  setMsgLevel(m_outputLevel);
  if(generator()->initialize().isFailure()){
    MSG_FATAL("Its not possible to initialize the generator. Abort!");
  }

  return StatusCode::SUCCESS;
}



StatusCode Pi0::execute( generator::Event &ctx )
{

  HepMC3::GenEvent evt( HepMC3::Units::GEV, HepMC3::Units::MM);


  // Generate main event. Quit if too many failures.
  if (generator()->execute(evt).isFailure()){
    return StatusCode::FAILURE;
  }
  
  
  const auto main_event_t = sample_t();
  const auto main_event_z = sample_z();


  std::vector<const HepMC3::GenParticle*> Pi0; 

  for (auto part : evt.particles()) 
  {
    // Is electron and final state?
    if (part->abs_pid() == 22 && ParticleHelper::isFinal(part.get()) )
    {
      if(part->parents().empty()){
        continue;
      }

      auto mother = part->parents().at(0);
      // The mother is Z?
      if( mother->pid() == 211)
      {
        float eta = part->momentum().eta();
        float pt = part->momentum().pt();
        if ( std::abs(eta) < m_etaMax && pt > (m_minPt/1.e3) ){
          Pi0.push_back( part.get() );
        }
      }// From Z?
    }// Is electron?
  }

  if ( Pi0.empty() ){
    MSG_DEBUG( "There is not Pi0 event inside of this event");
    throw NotInterestingEvent();
  }

  if (m_forceForwardPhoton){
    int fwdPhotonCounter = 0;
    for ( auto& e : Pi0 ){
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

  MSG_INFO("Filling Pi0 events into the context...");

  // Each electron is a seed
  for ( auto& e : Pi0 ){

    auto seed = generator::Seed( e->momentum().eta(), e->momentum().phi() );

    const auto vtx    = e->production_vertex();    
    float zVertexPos  = vtx->position().pz() + main_event_z;

    // Calibration use only.
    if (m_zeroVertexParticles){
      zVertexPos  = vtx->position().pz();
    }

    seed.emplace_back( 1, 0,
                       e->pid(), 
                       e->momentum().px(), 
                       e->momentum().py(), 
                       e->momentum().pz(), 
                       e->momentum().eta(), 
                       e->momentum().phi(), 
                       vtx->position().px(), 
                       vtx->position().py(), 
                       //  vtx->position().pz() + main_event_z,
                       zVertexPos,
                       vtx->position().t() + main_event_t, 
                       e->momentum().e(), 
                       e->momentum().pt() ); 
    // MSG_INFO("Add particle with PDGID " << e->pid() << " into the context.");
    MSG_INFO("Add particle with PDGID " << e->pid() << " and vertex x="<< vtx->position().px() <<", y=" <<vtx->position().py() <<", z=" << zVertexPos <<", t="<< vtx->position().t()+main_event_t <<" into the context."); 
    ctx.push_back( seed );
  }
  

  ctx.setEventNumber(evt.event_number());
  return StatusCode::SUCCESS;
}



StatusCode Pi0::finalize()
{
  MSG_INFO( "Finalize the Pi0 Event." );
  if(generator()->finalize().isFailure()){
    MSG_FATAL("Its not possible to finalize the generator. Abort!");
  }
  return StatusCode::SUCCESS;
}


