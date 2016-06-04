#include <list>
#include "tReceive.hpp"
#include "skills/skillSet.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <iostream>
#include <stdio.h>
#include <ssl_common/geometry.hpp>
#include <skills/skillSet.h>
#include "ros/ros.h"

#define THRES (0.8f)

namespace Strategy
{
  TReceive::TReceive(int botID) : Tactic( botID) { 
  }

  
  TReceive::~TReceive() { } 

  bool TReceive::isCompleted(const BeliefState &bs) const {
    return iState == FINISHED;
  }
  
  inline bool TReceive::isActiveTactic(void) const {
    return !(iState == FINISHED);
  }

  int TReceive::chooseBestBot(const BeliefState &state, std::list<int>& freeBots, const Param& tParam, int prevID) const
   {
     int minv   = *(freeBots.begin());
      int mindis = 1000000000;
      Vector2D<int> tGoToPoint(tParam.ReceiveP.x, tParam.ReceiveP.y);
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
      //printf("%d tReceive\n", minv);
      return minv;
  }

  gr_Robot_Command TReceive::execute(const BeliefState &state, const Tactic::Param& tParam) 
  {
    Vector2D<int> point(tParam.PassToPointP.x,tParam.PassToPointP.y);
    Vector2D<int> ballPos(state.ballPos.x, state.ballPos.y);
    Vector2D<int> botPos(state.homePos[botID].x, state.homePos[botID].y);
    
    float dist = Vector2D<int>::dist(point, botPos);
    float balldist=Vector2D<int>::dist(ballPos,botPos);
    float finalslope=normalizeAngle(Vector2D<int>::angle(ballPos,botPos));

    Strategy::SkillSet::SkillID sID;
    SkillSet::SParam sParam;

    if (dist >= DRIBBLER_BALL_THRESH) {
      iState=GOTOPOS;     
    }
     else if(balldist<1.5*DRIBBLER_BALL_THRESH)
    {
      iState=FINISHED;
    }
    else 
    {
      iState=GOTOBALL; 
    }
   ROS_INFO("istate : %d",iState);
    
    switch(iState)
    {
      case GOTOPOS:
      {
        sID = SkillSet::GoToPoint;
        sParam.GoToPointP.x=point.x;
        sParam.GoToPointP.y=point.y;
        sParam.GoToPointP.finalslope=finalslope;
        sParam.GoToPointP.align=false;
        return SkillSet::instance()->executeSkill(sID, sParam, state, botID);
        break;
      }
      case GOTOBALL:
      {
        sID = SkillSet::GoToBall;
        sParam.GoToBallP.intercept=false;
        return SkillSet::instance()->executeSkill(sID, sParam, state, botID);
        break;
      }
      case FINISHED:
      {
        sID = SkillSet::Dribble;
        return SkillSet::instance()->executeSkill(sID, sParam, state, botID);
        break;
      }
    }
  }  

  Tactic::Param TReceive::paramFromJSON(string json) {
    using namespace rapidjson;
      Tactic::Param tParam;
      Document d;
      d.Parse(json.c_str());
      tParam.PassToPointP.x = d["x"].GetDouble();
      tParam.PassToPointP.y = d["y"].GetDouble();
    return tParam;
  }

  string TReceive::paramToJSON(Tactic::Param tParam) {
    using namespace rapidjson;
      StringBuffer buffer;
      Writer<StringBuffer> w(buffer);
      w.StartObject();
      w.String("x");
      w.Double(tParam.PassToPointP.x);
      w.String("y");
      w.Double(tParam.PassToPointP.y);
      w.EndObject();
      return buffer.GetString();
  }
    
} 