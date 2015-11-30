#include <list>
#include "tPass.hpp"
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

    TPass::TPass(int botID) :
      Tactic(botID)
    {

    } // TPass
    

    TPass::~TPass()
    { } // ~TPass
    bool TPass::isCompleted(const BeliefState &bs) const {
      return false;
    }
    bool TPass::isActiveTactic(void) const
    {
      return false;
    }
//CHOOSEbEST bOT AND the giving of parameters for going to the required point needs to be entered
//Choose best bot also needs to get the params that the tactic has in order to choose the best bot....

    int TPass::chooseBestBot(const BeliefState &state, std::list<int>& freeBots, const Param& tParam, int prevID) const
    {
      int minv   = *(freeBots.begin());
      int mindis = 1000000000;
      Vector2D<int> tGoToPoint(tParam.PositionP.x, tParam.PositionP.y);
      for (std::list<int>::iterator it = freeBots.begin(); it != freeBots.end(); ++it)
      {
        // TODO make the bot choosing process more sophisticated, the logic below returns the 1st available bot
        Vector2D<int> homePos(state.homePos[*it].x, state.homePos[*it].y);
        float dis_from_point = (homePos - tGoToPoint).absSq();
        if(*it == prevID)
          dis_from_point -= HYSTERESIS;
        if(dis_from_point < mindis)
        {
          mindis = dis_from_point;
          minv = *it;
        }
      }
      printf("%d assigned Position\n", minv);
      return minv;
    } // chooseBestBot

    gr_Robot_Command TPass::execute(const BeliefState &state, const Param& tParam)
    {
      // Select the skill to the executed next
      Strategy::SkillSet::SkillID sID = SkillSet::KickToPoint;
      SkillSet::SParam sParam;
      sParam.KickToPointP.x = tParam.PassP.x;
      sParam.KickToPointP.y = tParam.PassP.y;
      sParam.KickToPointP.power = tParam.PassP.power;
      
      Strategy::SkillSet *ptr = SkillSet::instance();
      return ptr->executeSkill(sID, sParam, state, botID);              // Execute the selected skill
      
      /*if (Vstate.homePos[botID].absSq() < BOT_BALL_THRESH * BOT_BALL_THRESH)      //end conditions
      {
        //tState = COMPLETED;
      }*/     
        
      


      

      
    }
    Tactic::Param TPass::paramFromJSON(string json) {
      using namespace rapidjson;
      Tactic::Param tParam;
      Document d;
      d.Parse(json.c_str());
      tParam.PassP.x = d["x"].GetDouble();
      tParam.PassP.y = d["y"].GetDouble();
      tParam.PassP.power = d["power"].GetInt();
      return tParam;
    }

    string TPass::paramToJSON(Tactic::Param tParam) {
      using namespace rapidjson;
      StringBuffer buffer;
      Writer<StringBuffer> w(buffer);
      w.StartObject();
      w.String("x");
      w.Double(tParam.PassP.x);
      w.String("y");
      w.Double(tParam.PassP.y);
      w.String("power");
      w.Int(tParam.PassP.power);
      w.String("finalSlope");
      w.Double(tParam.PositionP.finalSlope);
      w.String("finalVelocity");
      w.Double(tParam.PositionP.finalVelocity);
      w.EndObject();
      return buffer.GetString();
    }
} // namespace Strategy
