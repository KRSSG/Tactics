#include <list>
#include "tReceiveBall.hpp"
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

    TReceiveBall::TReceiveBall(int botID) :
      Tactic(botID)
    {

    } // TReceiveBall
    

    TReceiveBall::~TReceiveBall()
    { } // ~TReceiveBall
    bool TReceiveBall::isCompleted(const BeliefState &bs) const {
      return false;
    }
    bool TReceiveBall::isActiveTactic(void) const
    {
      return false;
    }
//CHOOSEbEST bOT AND the giving of parameters for going to the required point needs to be entered
//Choose best bot also needs to get the params that the tactic has in order to choose the best bot....

    int TReceiveBall::chooseBestBot(const BeliefState &state, std::list<int>& freeBots, const Param& tParam, int prevID) const
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

    gr_Robot_Command TReceiveBall::execute(const BeliefState &state, const Param& tParam)
    {

      Vector2D<float> homePosfloat;
      homePosfloat.x = state.homePos[botID].x;
      homePosfloat.y = state.homePos[botID].y;
      float dist = Vector2D<int>::dist(Vector2D<int>(state.ballPos.x,state.ballPos.y), Vector2D<int>(state.homePos[botID].x,state.homePos[botID].y));
      float angle = normalizeAngle(Vector2D<float>::angle(Vector2D<int>(state.ballVel.x,state.ballVel.y), homePosfloat));   //angle between ball's predicted path and bot position
      Vector2D<float> target = state.ballVel / ((state.ballVel).abs());
      target = target * (dist * cos(angle));

      if(state.pr_looseBall)
      {
        Strategy::SkillSet::SkillID sID = SkillSet::GoToPoint;
        SkillSet::SParam sParam;
        sParam.GoToPointP.x = target.x;
        sParam.GoToPointP.y = target.y;
        sParam.GoToPointP.align = true;
        Strategy::SkillSet *ptr = SkillSet::instance();
        return ptr->executeSkill(sID, sParam, state, botID);
      }
      // if(state->pr_goalscored || state->pr_ourBall || state->pr_oppBall)
      //   tState = COMPLETED;
      // else
      //   tState = RUNNING;
      
    }
    Tactic::Param TReceiveBall::paramFromJSON(string json) {
      using namespace rapidjson;
      Tactic::Param tParam;
      Document d;
      d.Parse(json.c_str());
      tParam.PositionP.x = d["x"].GetDouble();
      tParam.PositionP.y = d["y"].GetDouble();
      tParam.PositionP.align = d["align"].GetInt();
      tParam.PositionP.finalSlope = d["finalSlope"].GetDouble();
      tParam.PositionP.finalVelocity = d["finalVelocity"].GetDouble();
      return tParam;
    }

    string TReceiveBall::paramToJSON(Tactic::Param tParam) {
      using namespace rapidjson;
      StringBuffer buffer;
      Writer<StringBuffer> w(buffer);
      w.StartObject();
      w.String("x");
      w.Double(tParam.PositionP.x);
      w.String("y");
      w.Double(tParam.PositionP.y);
      w.String("align");
      w.Int(tParam.PositionP.align);
      w.String("finalSlope");
      w.Double(tParam.PositionP.finalSlope);
      w.String("finalVelocity");
      w.Double(tParam.PositionP.finalVelocity);
      w.EndObject();
      return buffer.GetString();
    }
} // namespace Strategy
