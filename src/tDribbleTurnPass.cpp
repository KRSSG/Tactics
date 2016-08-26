#include <list>
#include "tDribbleTurnPass.h"
#include "skills/skillSet.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <ssl_common/geometry.hpp>
#include <skills/skillSet.h>

#define THRES (0.8f)

namespace Strategy
{
  TDribbleTurnPass::TDribbleTurnPass(int botID) : Tactic( botID) {
     //point = Vector2D<int>(HALF_FIELD_MAXX / 2.0f , HALF_FIELD_MAXt / 2.0f);
  }

  TDribbleTurnPass::~TDribbleTurnPass() { } 

  bool TDribbleTurnPass::isCompleted(const BeliefState &bs,const Tactic::Param& tParam) const {
    Vector2D<int> botPos(bs.homePos[botID].x, bs.homePos[botID].y);
    Vector2D<int> point(tParam.DribbleTurnPassP.x, tParam.DribbleTurnPassP.y);
    Vector2D<int> ballPos(bs.ballPos.x, bs.ballPos.y);
    Vector2D<int> ballVel(bs.ballVel.x, bs.ballVel.y);
    float ballBotAngle = Vector2D<int>::angle(botPos,ballPos);
    float pointBotAngle = Vector2D<int>::angle(botPos, point);

    float ballDist = Vector2D<int>::dist(botPos, ballPos);
   
    //if the ball is within a radius of the bot and travelling away from the bot then it is assumed to have been kicked
  
     if(ballDist>1.6*DRIBBLER_BALL_THRESH && ballVel.x/fabs(ballVel.x)==(ballPos.x-botPos.x)/fabs(ballPos.x-botPos.x) && \
        ballVel.x/fabs(ballVel.x)==(tParam.PassToPointP.x-ballPos.x)/fabs((tParam.PassToPointP.x-ballPos.x))) 
    return true;
    return false;
  }
  
  inline bool TDribbleTurnPass::isActiveTactic(void) const {
    return iState != FINISHED;
  }

  int TDribbleTurnPass::chooseBestBot(const BeliefState &state, std::list<int>& freeBots, const Param& tParam, int prevID) const {
  
    int minv = *(freeBots.begin());
    float mindis = -1;
    for (std::list<int>::const_iterator it = freeBots.begin(); it != freeBots.end(); ++it)
    {
      Vector2D<int> homePos(state.homePos[*it].x, state.homePos[*it].y);
      Vector2D<int> ballPos(state.ballPos.x, state.ballPos.y);

      float dis_from_ball = (homePos - ballPos).absSq();
      if (mindis < 0) {
        mindis = dis_from_ball;
        minv = *it; 
      }
      else if(dis_from_ball < mindis) {
        mindis = dis_from_ball;
        minv = *it;
      }
    }
    return minv;
  }

  gr_Robot_Command TDribbleTurnPass::execute(const BeliefState &state, const Tactic::Param& tParam) {
    
    Vector2D<int> point(tParam.DribbleTurnPassP.x, tParam.DribbleTurnPassP.y);
    Vector2D<int> ballPos(state.ballPos.x, state.ballPos.y);
    Vector2D<int> botPos(state.homePos[botID].x, state.homePos[botID].y);
    
    float dist = Vector2D<int>::dist(ballPos, botPos);
    float angleToTurn = normalizeAngle(state.homePos[botID].theta - Vector2D<int>::angle(point, ballPos));       

    float pointDis = Vector2D<int>::dist(botPos, point);
    float goalBotAngle = Vector2D<int>::angle(point, botPos);
    float ballBotAngle = Vector2D<int>::angle(ballPos, botPos);
    float angle = Vector2D<int>::angle(point, botPos);
    float angleUp = angle + 4*asin(BOT_RADIUS / (10.0 * pointDis));
    float angleDown = angle - 4*asin(BOT_RADIUS / (10.0 * pointDis));

    Strategy::SkillSet::SkillID sID;
    SkillSet::SParam sParam;

    if(iState==FINISHED)
    {
      iState=FINISHED;
    }
    else if (dist >= DRIBBLER_BALL_THRESH) {
      iState=GOTOBALL;     
    }
    
    else if (state.homePos[botID].theta > angleUp || state.homePos[botID].theta < angleDown){
      iState=DRIBBLETURN;
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
      case DRIBBLETURN:
      {
        sID = SkillSet::DribbleTurn;
        sParam.DribbleTurnP.x = point.x;
        sParam.DribbleTurnP.y = point.y;
        sParam.DribbleTurnP.max_omega = MAX_BOT_OMEGA/4;
        sParam.DribbleTurnP.turn_radius = 3*BOT_RADIUS;
        return SkillSet::instance()->executeSkill(sID, sParam, state, botID);
        break;
      }  
      case PASSING:
      {
        float power = pointDis / (HALF_FIELD_MAXX / 8.0);
        power = power > 6.0 ? 6.0 : power;
        power = power < 3.0 ? 3.0 : power;
        sID = SkillSet::Kick;
        sParam.KickP.power = power; 
        iState = FINISHED;
        return SkillSet::instance()->executeSkill(sID, sParam, state, botID);
        break;
      }  
      default : iState=FINISHED;
                break;
    }
  }  

  Tactic::Param TDribbleTurnPass::paramFromJSON(string json) {
    using namespace rapidjson;
      Tactic::Param tParam;
      Document d;
      d.Parse(json.c_str());
      tParam.DribbleTurnPassP.x = d["x"].GetDouble();
      tParam.DribbleTurnPassP.y = d["y"].GetDouble();
      return tParam;
  }

  string TDribbleTurnPass::paramToJSON(Tactic::Param tParam) {
    using namespace rapidjson;
      StringBuffer buffer;
      Writer<StringBuffer> w(buffer);
      w.StartObject();
      w.String("x");
      w.Double(tParam.DribbleTurnPassP.x);
      w.String("y");
      w.Double(tParam.DribbleTurnPassP.y);
      w.EndObject();
      return buffer.GetString();
  }
    
} 
