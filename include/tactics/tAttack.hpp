#ifndef TTATTACK_HPP
#define TTATTACK_HPP
#include "tactic.h"
#include "skills/skillSet.h"
#include "krssg_ssl_msgs/BeliefState.h"
#include "ssl_common/config.h"
#include <sys/time.h>
#include <unistd.h>
#include "tactic_factory.h"

namespace Strategy
{
  class TAttack : public Tactic
  {
  public:
    TAttack(int botID);

    ~TAttack();

    

    virtual bool isCompleted(const BeliefState &bs) const ;

    inline bool isActiveTactic(void) const;

    virtual int chooseBestBot(const BeliefState &bs, std::list<int>& freeBots, const Param& tParam, int prevID = -1) const;
   

    virtual gr_Robot_Command execute(const BeliefState &state, const Param& tParam);

    virtual Tactic::Param paramFromJSON(string json);
    virtual string paramToJSON(Tactic::Param p);

    bool pointxInField(const Vector2D<int> final) ;
    
  }; // class TAttack
  // registering tactic with factory:
  REGISTER_TACTIC(TAttack)
} // namespace Strategy

#endif
