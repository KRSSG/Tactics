#include <list>
#include "tPassToPoint.hpp"
#include "skills/skillSet.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <ssl_common/geometry.hpp>
#include <skills/skillSet.h>
#include "ros/ros.h"

#define THRES (0.8f)

namespace Strategy
{
  TPassToPoint::TPassToPoint(int botID) : Tactic( botID) { 
  }

  
  TPassToPoint::~TPassToPoint() { } 

  bool TPassToPoint::isCompleted(const BeliefState &bs) const {
    return iState == FINISHED;
  }
  
  inline bool TPassToPoint::isActiveTactic(void) const {
    return !(iState == FINISHED);
  }

  int TPassToPoint::chooseBestBot(const BeliefState &state, std::list<int>& freeBots, const Param& tParam, int prevID) const
   {
       int minv   = *(freeBots.begin());
      int mindis = 1000000000;
      Vector2D<int> tGoToPoint(tParam.PassToPointP.x, tParam.PassToPointP.y);
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
      //printf("%d tPassPointn\n", minv);
      return minv;
  }

  gr_Robot_Command TPassToPoint::execute(const BeliefState &state, const Tactic::Param& tParam) 
  {
    Vector2D<int> point(tParam.PassToPointP.x,tParam.PassToPointP.y);

    Vector2D<int> ballPos(state.ballPos.x, state.ballPos.y);
    Vector2D<int> botPos(state.homePos[botID].x, state.homePos[botID].y);
    
    float dist = Vector2D<int>::dist(ballPos, botPos);
    float angleToTurn = normalizeAngle(state.homePos[botID].theta - Vector2D<int>::angle(point, ballPos));
       

    float pointDis = Vector2D<int>::dist(botPos, point);
    float goalBotAngle = Vector2D<int>::angle(point, botPos);
    float ballBotAngle = Vector2D<int>::angle(ballPos, botPos);
    float angle = Vector2D<int>::angle(point, botPos);
    float angleUp = angle + asin(BOT_RADIUS / pointDis);
    float angleDown = angle - asin(BOT_RADIUS / pointDis);

    Strategy::SkillSet::SkillID sID;
    SkillSet::SParam sParam;

    if (dist >= DRIBBLER_BALL_THRESH) {
      iState=GOTOBALL;     
    }
    
    else if (state.homePos[botID].theta > angleUp || state.homePos[botID].theta < angleDown){
      iState=TURNING;
    }
    else {
      iState=PASSING;
    }  
    
    
    switch(iState)
    {
      case GOTOBALL:
      {
        sID = SkillSet::GoToBall;
        return SkillSet::instance()->executeSkill(sID, sParam, state, botID);
        break;
      }
      case TURNING:
      {
        sID = SkillSet::TurnToPoint;
        sParam.TurnToPointP.x = point.x;
        sParam.TurnToPointP.y = point.y;
        sParam.TurnToPointP.max_omega = MAX_BOT_OMEGA;
        return SkillSet::instance()->executeSkill(sID, sParam, state, botID);
        break;
      }  
      case PASSING:
      {
        double power = pointDis * 5.0f / HALF_FIELD_MAXX;
        power  < 3.0 ? 3.0 : power;
        power > 10.0? 9.0 : power;
        sID = SkillSet::Kick;
        sParam.KickP.power = power; 
        iState = FINISHED;
        return SkillSet::instance()->executeSkill(sID, sParam, state, botID);
        break;
      }        

    }
  }  

  Tactic::Param TPassToPoint::paramFromJSON(string json) {
    using namespace rapidjson;
      Tactic::Param tParam;
      Document d;
      d.Parse(json.c_str());
      tParam.PassToPointP.x = d["x"].GetDouble();
      tParam.PassToPointP.y = d["y"].GetDouble();
    return tParam;
  }

  string TPassToPoint::paramToJSON(Tactic::Param tParam) {
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