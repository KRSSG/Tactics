#include <list>
#include "tSteal.hpp"
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

    TSteal::TSteal(int botID) :
      Tactic(botID)
    {

    } // TSteal
    

    TSteal::~TSteal()
    { } // ~TSteal
    bool TSteal::isCompleted(const BeliefState &bs) const {
      return false;
    }
    bool TSteal::isActiveTactic(void) const
    {
      return false;
    }
//CHOOSEbEST bOT AND the giving of parameters for going to the required point needs to be entered
//Choose best bot also needs to get the params that the tactic has in order to choose the best bot....

    int TSteal::chooseBestBot(const BeliefState &state, std::list<int>& freeBots, const Param& tParam, int prevID) const
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

    gr_Robot_Command TSteal::execute(const BeliefState &state, const Param& tParam)
    {
      /*if(state->pr_looseBall == true)
        {
//          printf("LOOSE BALL\n");
          Strategy::SkillSet::SkillID sID = SkillSet::GoToBall;
          Strategy::SkillSet *ptr = SkillSet::instance();
          return ptr->executeSkill(sID, sParam, state, botID);
          tState = RUNNING;
          if(Vector2D<int>::dist(state->ballPos,state->homePos[botID])<DRIBBLER_BALL_THRESH)   //
          {
//            printf("BALL CAPTURED\n");
            Strategy::SkillSet::SkillID sID = SkillSet::Dribble;
            Strategy::SkillSet *ptr = SkillSet::instance();
            return ptr->executeSkill(sID, sParam, state, botID);
            tState = COMPLETED;
          }
          //Util::Logger::toStdOut("Going to  : %f,%f\n",sParam.GoToPointP.x,sParam.GoToPointP.y);
        }
        if(state->pr_oppBall == true)
        {
//          printf("OPP BALL\n");
          Strategy::SkillSet::SkillID sID = SkillSet::GoToBall;
          Strategy::SkillSet *ptr = SkillSet::instance();
          return ptr->executeSkill(sID, sParam, state, botID);
          tState = RUNNING;
          if(Vector2D<int>::dist(state->ballPos,state->homePos[botID])<DRIBBLER_BALL_THRESH)
          {
//            printf("BALL CAPTURED , NOW TURNING WITH BALL\n");
            sID=SkillSet::TurnToAngle;
            float finalincline=state->homeAngle[botID]+PI/2;
            while((finalincline-state->homeAngle[botID])>0.5)     // to turn the bot once taken away from oppenent
            {
              sParam.TurnToAngleP.finalslope=finalincline;
              Strategy::SkillSet *ptr = SkillSet::instance();
              return ptr->executeSkill(sID, sParam, state, botID);
            }
            tState = COMPLETED;
          }
        }
        if(state->pr_ourBall == true)  //our team takes possession
        {
//          printf("OUR BALL\n");
          tState = COMPLETED;
          Strategy::SkillSet::SkillID sID = SkillSet::Stop;
          Strategy::SkillSet *ptr = SkillSet::instance();
          return ptr->executeSkill(sID, sParam, state, botID);
        }*/
      
    }
    Tactic::Param TSteal::paramFromJSON(string json) {
      using namespace rapidjson;
      Tactic::Param tParam;
      Document d;
      d.Parse(json.c_str());
      return tParam;
    }

    string TSteal::paramToJSON(Tactic::Param tParam) {
      using namespace rapidjson;
      StringBuffer buffer;
      Writer<StringBuffer> w(buffer);
      w.StartObject();
      w.EndObject();
      return buffer.GetString();
    }
} // namespace Strategy
