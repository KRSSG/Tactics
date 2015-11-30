#include <list>
#include "tDefendPoint.hpp"
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

    TDefendPoint::TDefendPoint(int botID) :
      Tactic(botID)
    {

    } // TPosition
    

    TDefendPoint::~TDefendPoint()
    { } // ~TPosition
    bool TDefendPoint::isCompleted(const BeliefState &bs) const {
      return false;
    }
    bool TDefendPoint::isActiveTactic(void) const
    {
      return false;
    }
//CHOOSEbEST bOT AND the giving of parameters for going to the required point needs to be entered
//Choose best bot also needs to get the params that the tactic has in order to choose the best bot....

    int TDefendPoint::chooseBestBot(const BeliefState &state, std::list<int>& freeBots, const Param& tParam, int prevID) const
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

    gr_Robot_Command TDefendPoint::execute(const BeliefState &state, const Param& tParam)
    {
      // Select the skill to the executed next

      Strategy::SkillSet::SkillID sID = SkillSet::DefendPoint;
      SkillSet::SParam sParam;
      sParam.DefendPointP.x             = tParam.DefendPointP.x ;
      sParam.DefendPointP.y             = tParam.DefendPointP.y ;
      sParam.DefendPointP.radius         = tParam.DefendPointP.radius;
      
      // Execute the selected skill
      Strategy::SkillSet *ptr = SkillSet::instance();
      return ptr->executeSkill(sID, sParam, state, botID);

      
    }
    Tactic::Param TDefendPoint::paramFromJSON(string json) {
      using namespace rapidjson;
      Tactic::Param tParam;
      Document d;
      d.Parse(json.c_str());
      tParam.DefendPointP.x = d["x"].GetDouble();
      tParam.DefendPointP.y = d["y"].GetDouble();
      tParam.DefendPointP.radius = d["radius"].GetInt();
      return tParam;
    }

    string TDefendPoint::paramToJSON(Tactic::Param tParam) {
      using namespace rapidjson;
      StringBuffer buffer;
      Writer<StringBuffer> w(buffer);
      w.StartObject();
      w.String("x");
      w.Double(tParam.DefendPointP.x);
      w.String("y");
      w.Double(tParam.DefendPointP.y);
      w.String("radius");
      w.Int(tParam.DefendPointP.radius);
      w.EndObject();
      return buffer.GetString();
    }
} // namespace Strategy
