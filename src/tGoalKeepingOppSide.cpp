#include <list>
#include "tGoalKeepingOppSide.hpp"
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

    TGoalKeepingOppSide::TGoalKeepingOppSide(int botID) :
      Tactic(botID)
    {
      state1=0;

    } // TPosition
    

    TGoalKeepingOppSide::~TGoalKeepingOppSide()
    { } // ~TPosition
    bool TGoalKeepingOppSide::isCompleted(const BeliefState &bs) const {
      return false;
    }
    bool TGoalKeepingOppSide::isActiveTactic(void) const
    {
      return false;
    }
//CHOOSEbEST bOT AND the giving of parameters for going to the required point needs to be entered
//Choose best bot also needs to get the params that the tactic has in order to choose the best bot....

    int TGoalKeepingOppSide::chooseBestBot(const BeliefState &state, std::list<int>& freeBots, const Param& tParam, int prevID) const
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

    bool TGoalKeepingOppSide::isGoalKeeperInPosition(const BeliefState &state)
    {
      //Util::Logger::toStdOut("Half_field_max : %d, Dbox_width : %d\n",HALF_FIELD_MAXX,DBOX_WIDTH);
      if ((state.homePos[botID].x < HALF_FIELD_MAXX) &&
          (state.homePos[botID].x > HALF_FIELD_MAXX - DBOX_WIDTH) &&
          (state.homePos[botID].y >= OUR_GOAL_MINY) &&
          (state.homePos[botID].y <= OUR_GOAL_MAXY))
        return true;
      else
        return false;
      
    }

    int TGoalKeepingOppSide::getBotDestPointY(const BeliefState &state)
    {
        Vector2D<int> ballFinalpos, botDestination, point;
        ballFinalpos.x = state.ballPos.x+state.ballVel.x;
        ballFinalpos.y = state.ballPos.y+state.ballVel.y;
        botDestination.x = HALF_FIELD_MAXX - DBOX_WIDTH/2;
        
        /* Workaround for ball velocity 0*/
        if(fabs(ballFinalpos.y - state.ballPos.y)<5 || fabs(ballFinalpos.x - state.ballPos.x)<5) 
        {
          //Util::Logger::toStdOut("******************Ball velocity is zero.\n");
          point.x = (HALF_FIELD_MAXX) - DBOX_WIDTH/2;
          point.y = state.ballPos.y;
        }
        else{
          botDestination.y = (ballFinalpos.y - state.ballPos.y)/(ballFinalpos.x - state.ballPos.x)*(botDestination.x - ballFinalpos.x) + ballFinalpos.y;
          point.x = (HALF_FIELD_MAXX) - DBOX_WIDTH/2;;
          point.y = botDestination.y;
          point.y = state.ballPos.y;
        }
          /* Set Limits on y to not exceed DBOX Y Limits*/
          if(point.y<OUR_GOAL_MINY)
            point.y = OUR_GOAL_MINY + BOT_RADIUS;
          else if(point.y>OUR_GOAL_MAXY)
            point.y = OUR_GOAL_MAXY - BOT_RADIUS;
            
          return point.y;
    }

    gr_Robot_Command TGoalKeepingOppSide::execute(const BeliefState &state, const Param& tParam)
    {

      /*    
      if (state->homePos[botID].x < -HALF_FIELD_MAXX || state->homePos[botID].x > -HALF_FIELD_MAXX + 5 * BOT_RADIUS)
      {
        sID = SkillSet::GoToPoint;
        sParam.GoToPointP.finalslope  = PI/2;
        sParam.GoToPointP.x           = -(HALF_FIELD_MAXX)  + 2.5 * BOT_RADIUS;
        sParam.GoToPointP.y           = (OUR_GOAL_MAXY      + OUR_GOAL_MINY)/2;
*/
      // printf("Ball Pos: %d %d\n", state.ballPos.x, state.ballPos.y);
//      if (!isGoalKeeperInPosition() && state1 == 0)
//      {
//        //Bot elsewhere in field. Bring the bot to the Goalkeeper position.
//        //Util::Logger::toStdOut("Bot going to goalkeeper positon.");
//        sID = SkillSet::GoToPoint;
////        sParam.GoToPointP.align = true;
//        sParam.GoToPointP.finalslope = PI/2;
//        sParam.GoToPointP.x=(HALF_FIELD_MAXX)-DBOX_WIDTH/6;
//        sParam.GoToPointP.y=(OUR_GOAL_MAXY+OUR_GOAL_MINY)/2;
//        skillSet->executeSkill(sID, sParam);
//      }
//      

      {
        //Bot in Goalkeeper position. Show goalkeeping skills.
        state1 =1 ;
        //printf("Ball x:%d y:%d v_x:%f v_y:%f\n", state->ballPos.x, state->ballPos.y, state->ballVel.x, state->ballVel.y);
        Strategy::SkillSet::SkillID sID = SkillSet::GoToPoint;
        SkillSet::SParam sParam;
        sParam.GoToPointP.x = (HALF_FIELD_MAXX)-DBOX_WIDTH;
        sParam.GoToPointP.y = getBotDestPointY(state);
//        sParam.GoToPointP.align = true;
        sParam.GoToPointP.finalslope = PI/2;
        Strategy::SkillSet *ptr = SkillSet::instance();
        return ptr->executeSkill(sID, sParam, state, botID);
        //Util::Logger::toStdOut("Bot showing goalkeeping skills (%f,%f).\n",sParam.GoToPointP.x,sParam.GoToPointP.y);
        
      }
        //tState = COMPLETED;
      // Select the skill to the executed next
//      printf("botpos x:%d\ty:%d\n", state->homePos[botID].x, state->homePos[botID].y);


      // Execute the selected skill
      

      // if((state->homePos[botID] - Vector2D<int>(tParam.PositionP.x, tParam.PositionP.y)).absSq() < BOT_POINT_THRESH * BOT_POINT_THRESH)
      // {
      //   tState = COMPLETED;
      // }
    }
    Tactic::Param TGoalKeepingOppSide::paramFromJSON(string json) {
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

    string TGoalKeepingOppSide::paramToJSON(Tactic::Param tParam) {
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
