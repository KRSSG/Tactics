#include <list>
#include "tVelocity.hpp"
#include "skills/skillSet.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <iostream>
#include <stdio.h>
#include <ssl_common/geometry.hpp>
#include <skills/skillSet.h>
namespace Strategy
{

    TVelocity::TVelocity(int botID) :
      Tactic(botID)
    {

    } // TVelocity

    TVelocity::~TVelocity()
    { } //~Tvelocity

    bool TVelocity::isCompleted(const BeliefState &bs) const {
      return false;
    }
    
    bool TVelocity::isActiveTactic(void) const
    {
      return false;
    }
//CHOOSEbEST bOT AND the giving of parameters for going to the required point needs to be entered
//Choose best bot also needs to get the params that the tactic has in order to choose the best bot....

    int TVelocity::chooseBestBot(const BeliefState &state, std::list<int>& freeBots, const Param& tParam, int prevID) const
    {
      for (std::list<int>::iterator it=freeBots.begin(); it!=freeBots.end(); ++it)
      {
        // TODO make the bot choosing process more sophisticated, the logic below returns the 1st available bot
        return *it;
      }
      
    } // chooseBestBot

    gr_Robot_Command TVelocity::execute(const BeliefState &state, const Param& tParam)
    {
      // Select the skill to the executed next
      Strategy::SkillSet::SkillID sID = SkillSet::GoToPoint;
      SkillSet::SParam sParam;
      sParam.VelocityP.v_x=tParam.VelocityP.v_x;
      sParam.VelocityP.v_y=tParam.VelocityP.v_y;
      sParam.VelocityP.v_t=tParam.VelocityP.v_t;

      // Execute the selected skill
      Strategy::SkillSet *ptr = SkillSet::instance();
      return ptr->executeSkill(sID, sParam, state, botID);

    }
    Tactic::Param TVelocity::paramFromJSON(string json) {
      using namespace rapidjson;
      Tactic::Param tParam;
      Document d;
      d.Parse(json.c_str());
      tParam.VelocityP.v_x = d["v_x"].GetDouble();
      tParam.VelocityP.v_y = d["v_y"].GetDouble();
      tParam.VelocityP.v_t = d["v_t"].GetDouble();
      return tParam;
    }

    string TVelocity::paramToJSON(Tactic::Param tParam) {
      using namespace rapidjson;
      StringBuffer buffer;
      Writer<StringBuffer> w(buffer);
      w.StartObject();
      w.String("v_x");
      w.Double(tParam.VelocityP.v_x);
      w.String("v_y");
      w.Double(tParam.VelocityP.v_y);
      w.String("v_t");
      w.Double(tParam.VelocityP.v_t);
      w.EndObject();
      return buffer.GetString();
    }
} // namespace Strategy
