#include <list>
#include "tPositionForStart.hpp"
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

    TPositionForStart::TPositionForStart(int botID) :
      Tactic(botID)
    {

    } // TPositionForStart
    

    TPositionForStart::~TPositionForStart()
    { } // ~TPositionForStart
    bool TPositionForStart::isCompleted(const BeliefState &bs) const {
      return false;
    }
    bool TPositionForStart::isActiveTactic(void) const
    {
      return false;
    }
//CHOOSEbEST bOT AND the giving of parameters for going to the required point needs to be entered
//Choose best bot also needs to get the params that the tactic has in order to choose the best bot....

    int TPositionForStart::chooseBestBot(const BeliefState &state, std::list<int>& freeBots, const Param& tParam, int prevID) const
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

    gr_Robot_Command TPositionForStart::execute(const BeliefState &state, const Param& tParam)
    {
    
      // Select the skill to the executed next
//      printf("botpos x:%d\ty:%d\n", state->homePos[botID].x, state->homePos[botID].y);

      Strategy::SkillSet::SkillID sID = SkillSet::GoToPoint;
      SkillSet::SParam sParam;
      sParam.GoToPointP.x             = tParam.PositionForStartP.x ;
      sParam.GoToPointP.y             = tParam.PositionForStartP.y ;
      sParam.GoToPointP.align         = tParam.PositionForStartP.align;
      sParam.GoToPointP.finalslope    = tParam.PositionForStartP.finalSlope ;
      sParam.GoToPointP.finalVelocity = tParam.PositionForStartP.finalVelocity;

      // Execute the selected skill
      Strategy::SkillSet *ptr = SkillSet::instance();
      return ptr->executeSkill(sID, sParam, state, botID);

      // if((state->homePos[botID] - Vector2D<int>(tParam.PositionP.x, tParam.PositionP.y)).absSq() < BOT_POINT_THRESH * BOT_POINT_THRESH)
      // {
      //   tState = COMPLETED;
      // }
    }
    Tactic::Param TPositionForStart::paramFromJSON(string json) {
      using namespace rapidjson;
      Tactic::Param tParam;
      Document d;
      d.Parse(json.c_str());
      tParam.PositionForStartP.x = d["x"].GetDouble();
      tParam.PositionForStartP.y = d["y"].GetDouble();
      tParam.PositionForStartP.align = d["align"].GetInt();
      tParam.PositionForStartP.finalSlope = d["finalSlope"].GetDouble();
      tParam.PositionForStartP.finalVelocity = d["finalVelocity"].GetDouble();
      return tParam;
    }

    string TPositionForStart::paramToJSON(Tactic::Param tParam) {
      using namespace rapidjson;
      StringBuffer buffer;
      Writer<StringBuffer> w(buffer);
      w.StartObject();
      w.String("x");
      w.Double(tParam.PositionForStartP.x);
      w.String("y");
      w.Double(tParam.PositionForStartP.y);
      w.String("align");
      w.Int(tParam.PositionForStartP.align);
      w.String("finalSlope");
      w.Double(tParam.PositionForStartP.finalSlope);
      w.String("finalVelocity");
      w.Double(tParam.PositionForStartP.finalVelocity);
      w.EndObject();
      return buffer.GetString();
    }
} // namespace Strategy
