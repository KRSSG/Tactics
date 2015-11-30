#include <list>
#include "tAttack.hpp"
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

  enum InternalState
    {
      APPROACHING,
      SPINNING_CCW=-1,
      SPINNING_CW=1,      
      ATTACKING
    }iState;

    TAttack::TAttack(int botID):
      Tactic(botID)
    { 
      iState = APPROACHING;
    }//TAttack


  

    TAttack::~TAttack()
    { } // ~TAttack

    bool TAttack::isCompleted(const BeliefState &bs) const {
      return false;
    }
    bool TAttack::isActiveTactic(void) const
    {
      return false;
    }
//CHOOSEbEST bOT AND the giving of parameters for going to the required point needs to be entered
//Choose best bot also needs to get the params that the tactic has in order to choose the best bot....

    int TAttack::chooseBestBot(const BeliefState &state, std::list<int>& freeBots, const Param& tParam, int prevID) const
    {
      int minv = *(freeBots.begin());
      int mindis = 10000000;//(state.homePos[*(freeBots.begin())] - state.ballPos.absSq();
      for (std::list<int>::iterator it = freeBots.begin(); it != freeBots.end(); ++it)
      {
        // TODO make the bot choosing process more sophisticated, the logic below returns the 1st available bot
        float dis_from_ball = 10000;//(state.homePos[*it] - state.ballPos).absSq();
        if(*it == botID)
          dis_from_ball -=HYSTERESIS;
        if(dis_from_ball < mindis)
        {
          dis_from_ball = mindis;
          minv = *it;
        }
      }
      return minv;
    } // chooseBestBot

    bool pointxInField(const Vector2D<int> final)
    {
      if((final.x < HALF_FIELD_MAXX + (BALL_AT_CORNER_THRESH)/*||final.x<-HALF_FIELD_MAXX-(BALL_AT_CORNER_THRESH))*/))
      {
        if((final.y < OPP_GOAL_MINY||final.y > OPP_GOAL_MAXY))
        {
          return false;
        }
        else return true;
      }
      else  return true;
    }
    

    gr_Robot_Command TAttack::execute(const BeliefState &state, const Param& tParam)
    {
      
//       SkillSet::SParam sParam;
//       Strategy::SkillSet *ptr = SkillSet::instance();

//       float dist = Vector2D<int>::dist(state.ballPos,state.homePos[botID]);
//       if(!pointxInField(state.ballPos))
//       {
// //        printf("---------------------------My Code\n");
//         if(dist < BOT_BALL_THRESH/2)
//         {
//           Strategy::SkillSet::SkillID sID = SkillSet::Spin;
//           sParam.SpinP.radPerSec=MAX_BOT_OMEGA;
//           return ptr->executeSkill(sID, sParam, state, botID);
// //          printf("SPIN\n");
//         }
//         else
//         {
//           if(state.ballPos.y>0)
//           {
//             Strategy::SkillSet::SkillID sID = SkillSet::GoToPoint;
//             sParam.GoToPointP.align = true;
//             sParam.GoToPointP.x = state.ballPos.x;
//             sParam.GoToPointP.y = state.ballPos.y;
//             return ptr->executeSkill(sID, sParam, state, botID);
//           }
//           else
//           {
//             Strategy::SkillSet::SkillID sID = SkillSet::GoToPoint;
//             sParam.GoToPointP.align = true;
//             sParam.GoToPointP.x = state.ballPos.x;
//             sParam.GoToPointP.y = state.ballPos.y;
//             return ptr->executeSkill(sID, sParam, state, botID);
//           }
//         }
//       }
//       else
//       {    
//         if(iState == SPINNING_CW || iState == SPINNING_CCW)
//         {
//           if(dist > BOT_BALL_THRESH*1.2)
//           {
//             iState = APPROACHING;
//           }
//           else
//           {
//           Strategy::SkillSet::SkillID sID = SkillSet::Spin;
//           if(iState == SPINNING_CW)
//             sParam.SpinP.radPerSec = MAX_BOT_OMEGA;
//           else
//             sParam.SpinP.radPerSec = -MAX_BOT_OMEGA;
//           return ptr->executeSkill(sID, sParam, state, botID);
//           return;
//           }
//         }
//         if(dist < BOT_BALL_THRESH)
//         {
//           /*Ball with bot, but 90 deg angle. So spin*/
//           if(fabs(normalizeAngle(state.homeAngle[botID] - atan2(state.homePos[botID].y - state.ballPos.y, state.homePos[botID].x - state.ballPos.x))) < PI/2 + PI/10 && fabs(normalizeAngle(state.homeAngle[botID] - atan2(state.homePos[botID].y - state.ballPos.y, state.homePos[botID].x - state.ballPos.x)))  > PI/2 - PI/10)
//           {
//             if(state.ballPos.y>0)
//               iState = SPINNING_CW;
//             else
//               iState = SPINNING_CCW;
//           }
//           else
//           {
//           /* Ball is with bot. So go to goal */
//           //Util::Logger::toStdOut("******* Go to GOAL ***********\n");
//           Strategy::SkillSet::SkillID sID = SkillSet::GoToPoint;
//           sParam.GoToPointP.align = true;
//           sParam.GoToPointP.x = HALF_FIELD_MAXX;
//           sParam.GoToPointP.y = 0;
//           sParam.GoToPointP.finalslope = Vector2D<int>::angle(Vector2D<int>(HALF_FIELD_MAXX,0),state.ballPos);
//           return ptr->executeSkill(sID, sParam, state, botID);
//           tState = RUNNING;
//           iState = ATTACKING;
//           }
//         }
//         else
//         {
//           if(state.pr_ourBall)
//           {
//             //this.tState = Tactic::COMPLETED;
//             //Util::Logger::toStdOut("Attack Tactic completed\n");
//           }//else
//              //Util::Logger::toStdOut("Attack Tactic NOT completed\n");
//           /* Ball is not with bot so go to ball first */
//           //Util::Logger::toStdOut("******* Go to BALL ***********\n");
//           Strategy::SkillSet::SkillID sID = SkillSet::GoToPoint;
//           sParam.GoToPointP.align = true;
//           sParam.GoToPointP.x = state.ballPos.x;
//           sParam.GoToPointP.y = state.ballPos.y;
//           sParam.GoToPointP.finalslope = Vector2D<int>::angle(Vector2D<int>(HALF_FIELD_MAXX,0),state.ballPos);
//           return ptr->executeSkill(sID, sParam, state, botID);
//         }
//       }
    }



    Tactic::Param TAttack::paramFromJSON(string json) {
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

    string TAttack::paramToJSON(Tactic::Param tParam) {
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
