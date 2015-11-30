#ifndef TDRAGTOGOAL_HPP
#define TDRAGTOGOAL_HPP
#include "tactic.h"
#include "skills/skillSet.h"
#include "krssg_ssl_msgs/BeliefState.h"
#include "ssl_common/config.h"
#include <sys/time.h>
#include <unistd.h>
#include "tactic_factory.h"
namespace Strategy  
{
  class TDragToGoal: public Tactic
  {
  public:

    TDragToGoal(int botID);

    ~TDragToGoal();
    virtual bool isCompleted(const BeliefState &bs) const ;

    virtual bool isActiveTactic(void) const;
//CHOOSEbEST bOT AND the giving of parameters for going to the required point needs to be entered
//Choose best bot also needs to get the params that the tactic has in order to choose the best bot....

    virtual int chooseBestBot(const BeliefState &bs, std::list<int>& freeBots, const Param& tParam, int prevID = -1) const;
   

    virtual gr_Robot_Command execute(const BeliefState &state, const Param& tParam);

    virtual Tactic::Param paramFromJSON(string json);
    virtual string paramToJSON(Tactic::Param p);
    
  }; // class TDragToGoal
  // registering tactic with factory:
  REGISTER_TACTIC(TDragToGoal)
} // namespace Strategy
#endif