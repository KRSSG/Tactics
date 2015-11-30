#include <list>
#include "tKickToGoal.hpp"
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
    Vector2D<int> goal;
    Vector2D<int> dribblePoint;

    TKickToGoal::TKickToGoal(int botID) :
      Tactic(botID)
    {

    } // TKickToGoal
    

    TKickToGoal::~TKickToGoal()
    { } // ~TKickToGoal
    bool TKickToGoal::isCompleted(const BeliefState &bs) const {
      return false;
    }
    bool TKickToGoal::isActiveTactic(void) const
    {
      return false;
    }
//CHOOSEbEST bOT AND the giving of parameters for going to the required point needs to be entered
//Choose best bot also needs to get the params that the tactic has in order to choose the best bot....

    int TKickToGoal::chooseBestBot(const BeliefState &state, std::list<int>& freeBots, const Param& tParam, int prevID) const
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

    gr_Robot_Command TKickToGoal::execute(const BeliefState &state, const Param& tParam)
    {
      SkillSet::SParam sParam;
      Strategy::SkillSet *ptr = SkillSet::instance();
      float dist=0, finalSlope=0, turnAngleLeft=0, angleWithBall=0;
      dist = Vector2D<int>::dist(Vector2D<int>(state.ballPos.x,state.ballPos.y), Vector2D<int>(state.homePos[botID].x,state.homePos[botID].y));
      finalSlope = Vector2D<int>::angle(goal, Vector2D<int>(state.homePos[botID].x,state.homePos[botID].y));
      turnAngleLeft = normalizeAngle(finalSlope - state.homeAngle[botID]); // Angle left to turn
      angleWithBall = normalizeAngle(state.homeAngle[botID] - Vector2D<int>::angle(Vector2D<int>(state.ballPos.x,state.ballPos.y), Vector2D<int>(state.homePos[botID].x,state.homePos[botID].y));
//      switch(iState)
//      {
//        case GOTOBALL:
//            if(!(dist > BOT_BALL_THRESH || fabs(angleWithBall) > DRIBBLER_BALL_ANGLE_RANGE))
//            {
//              iState=TURNING;///////////////////////////
//              printf("state changed gotoball to turning\n");
//              break;
//            }  
//            sID = SkillSet::GoToBall;
//            printf("going to ball %d, %f, %f\n", BOT_BALL_THRESH, dist, angleWithBall);
//            skillSet->executeSkill(sID, sParam);
//            break;
//        case FINDPOINT:
////            int destRadius = (int)Vector2D<int>::dist(state->homePos[botID], goal)/2;
//            dribblePoint.x = (10*state->homePos[botID].x+goal.x)/11;
//            dribblePoint.y = (10*state->homePos[botID].y+goal.y)/11;
//            iState = DRIBBLING;
//            break;
//        case DRIBBLING:
//            if(dist > BOT_BALL_THRESH*2 /*|| fabs(angleWithBall) > DRIBBLER_BALL_ANGLE_RANGE*/)
//            {
//              iState = GOTOBALL;
//              break;
//            }
//            if(Vector2D<int>::dist(dribblePoint, state->homePos[botID])< BOT_POINT_THRESH)
//            {
//              iState = TURNING;
//              break;
//            }
//            printf("dribbling..\n");
//            sID = SkillSet::DribbleToPoint;
//            sParam.DribbleToPointP.x = dribblePoint.x;
//            sParam.DribbleToPointP.y = dribblePoint.y;
//            sParam.DribbleToPointP.finalslope = Vector2D<int>::angle(goal, dribblePoint);
//            skillSet->executeSkill(sID, sParam);
//            break;
//        case TURNING:
//            if(!(fabs(turnAngleLeft) > SATISFIABLE_THETA/2))
//            {
//              iState = KICKING;
//              printf("state changed turning to kicking\n");
//              break;
//            }
//            printf("turning to balll %f %f\n", turnAngleLeft, angleWithBall);
//            sID = SkillSet::TurnToPoint;
//            sParam.TurnToPointP.x = goal.x;
//            sParam.TurnToPointP.y = goal.y;
//#ifdef SSL_COMM
//        sParam.TurnToPointP.max_omega = MAX_BOT_OMEGA / 5;
//#else
//        sParam.TurnToPointP.max_omega = MAX_BOT_OMEGA*3;
//#endif
//            skillSet->executeSkill(sID, sParam);
//            break;
//        case KICKING:
//            printf("should kick now %f %f %f\n", turnAngleLeft, finalSlope, angleWithBall);
//            sID = SkillSet::Kick;
//            sParam.KickP.power = 7;
//            skillSet->executeSkill(sID, sParam);
//            if(dist > BOT_BALL_THRESH || fabs(angleWithBall) > DRIBBLER_BALL_ANGLE_RANGE)
//            {
//              iState = GOTOBALL;
//              printf("state changed kicking to gotoball\n");
//              break;
//            }
//            break;
//      } 
      if((dist > BOT_BALL_THRESH*1.5 || fabs(angleWithBall) > DRIBBLER_BALL_ANGLE_RANGE))
      {
        
        Strategy::SkillSet::SkillID sID = SkillSet::GoToBall;
//        printf("going to ball %d, %f, %f\n", BOT_BALL_THRESH, dist, angleWithBall);
        return ptr->executeSkill(sID, sParam, state, botID);
        
      }
      if((fabs(turnAngleLeft) > SATISFIABLE_THETA/2))
      {
//        printf("turning to balll %f %f\n", turnAngleLeft, angleWithBall);
        Strategy::SkillSet::SkillID sID = SkillSet::TurnToPoint;
        sParam.TurnToPointP.x = goal.x;
        sParam.TurnToPointP.y = goal.y;
  #ifdef SSL_COMM
    sParam.TurnToPointP.max_omega = MAX_BOT_OMEGA / 5;
  #else
    sParam.TurnToPointP.max_omega = MAX_BOT_OMEGA*5;
  #endif
        return ptr->executeSkill(sID, sParam, state, botID);
        
      }
      if((dist > BOT_BALL_THRESH || fabs(angleWithBall) > DRIBBLER_BALL_ANGLE_RANGE))
      {
        
        Strategy::SkillSet::SkillID sID = SkillSet::GoToBall;
//        printf("going to ball %d, %f, %f\n", BOT_BALL_THRESH, dist, angleWithBall);
        return ptr->executeSkill(sID, sParam, state, botID);
        
      }
//      printf("should kick now %f %f %f\n", turnAngleLeft, finalSlope, angleWithBall);
      Strategy::SkillSet::SkillID sID = SkillSet::Kick;
      sParam.KickP.power = 7;
      return ptr->executeSkill(sID, sParam, state, botID);
      
//      if(fabs(turnAngleLeft) > SATISFIABLE_THETA/2)
//      {

//        return;
//      }
      
      /*if (state.homePos[botID].absSq() < BOT_BALL_THRESH * BOT_BALL_THRESH)
      {
       // tState = COMPLETED;
      }*/
    }
  
      // Select the skill to the executed next
//      printf("botpos x:%d\ty:%d\n", state->homePos[botID].x, state->homePos[botID].y);


      // if((state->homePos[botID] - Vector2D<int>(tParam.PositionP.x, tParam.PositionP.y)).absSq() < BOT_POINT_THRESH * BOT_POINT_THRESH)
      // {
      //   tState = COMPLETED;
      // }
    
    Tactic::Param TKickToGoal::paramFromJSON(string json) {
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

    string TKickToGoal::paramToJSON(Tactic::Param tParam) {
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
