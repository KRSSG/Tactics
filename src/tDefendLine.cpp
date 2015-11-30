#include <list>
#include "tDefendLine.hpp"
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

    TDefendLine::TDefendLine(int botID) :
      Tactic(botID)
    {

    } // TPosition
    

    TDefendLine::~TDefendLine()
    { } // ~TPosition
    bool TDefendLine::isCompleted(const BeliefState &bs) const {
      return false;
    }
    bool TDefendLine::isActiveTactic(void) const
    {
      return false;
    }
//CHOOSEbEST bOT AND the giving of parameters for going to the required point needs to be entered
//Choose best bot also needs to get the params that the tactic has in order to choose the best bot....

    int TDefendLine::chooseBestBot(const BeliefState &state, std::list<int>& freeBots, const Param& tParam, int prevID) const
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
      return minv;
    } // chooseBestBot

    gr_Robot_Command TDefendLine::execute(const BeliefState &state, const Param& tParam)
    {
      float x1,x3,x2,x4,y1,y2,y3,y4;
      x1=tParam.DefendLineP.x1;
      x2=tParam.DefendLineP.x2;
      y1=tParam.DefendLineP.y1;
      y2=tParam.DefendLineP.y2;
      x3=state.ballPos.x;
      y3=state.ballPos.y;
      x4=state.ballPos.x + state.ballVel.x;
      y4=state.ballPos.y+ state.ballVel.y;
      if(state.ballVel.x==0)
      {
//        printf("here1\n");
        //return;
        x4=(x1+x2)/2;
        //y4=(y1+y2)/2;
      }
      
      Vector2D<int> targetPoint;
      // intersection formula copied from http://en.wikipedia.org/wiki/Line-line_intersection
      float denominator= (x1-x2)*(y3-y4) - (y1-y2)*(x3-x4);
      if(denominator==0)
      {
//        printf("here2\n");
        targetPoint.x=(x1+x2)/2;
        targetPoint.y =state.ballPos.y+ state.ballVel.y;
        
      }
      else
      {
      targetPoint.x = ((x1*y2-y1*x2)* (x3-x4) - (x1-x2) *(x3*y4-y3*x4))/denominator;
      targetPoint.y = ((x1*y2-y1*x2)* (y3-y4) - (y1-y2) *(x3*y4-y3*x4))/denominator;
      }
      if(fabs(targetPoint.x) > fabs(HALF_FIELD_MAXX))
      {
        targetPoint.x=(x1+x2)/2;
      }
      if(fabs(targetPoint.y) > fabs(HALF_FIELD_MAXY))
      {
        targetPoint.y=(y1+y2)/2;
      }
//      if(x3 < x1)
//      {
//        /**Shifts the defence line if the ball has already crossed it.*/
//        targetPoint.x = state->ballPos.x  + 4*BOT_BALL_THRESH;
//        targetPoint.y = state->ballPos.y; + state->ballVel.y*0.3;
//      }
//      else
//      {
//        /**Continue moving along the line if the ball has not crossed it.*/
//        targetPoint.x = x1;
//        targetPoint.y = state->ballPos.y; + state->ballVel.y*0.3;
//      }
//
//      /**Set DefendPoint coordinates on the defence line*/
//      int xfinal = targetPoint.x < HALF_FIELD_MAXX ? targetPoint.x : HALF_FIELD_MAXX;
//      int yfinal = targetPoint.y;
      
      /**Tactic completed if ball comes in our possession or if ball crosses the defence line.*/
      if(state.pr_ourBall && x3>x1)
      {
        //tState = COMPLETED;
      }
      // Select the skill to the executed next
//      printf("botpos x:%d\ty:%d\n", state->homePos[botID].x, state->homePos[botID].y);

      Strategy::SkillSet::SkillID sID = SkillSet::DefendPoint;
      SkillSet::SParam sParam;
      sParam.DefendPointP.x             = targetPoint.x ;
      sParam.DefendPointP.y             = targetPoint.y ;
      sParam.DefendPointP.radius = tParam.DefendLineP.radius;
      // Execute the selected skill
      Strategy::SkillSet *ptr = SkillSet::instance();
      return ptr->executeSkill(sID, sParam, state, botID);

      // if((state->homePos[botID] - Vector2D<int>(tParam.PositionP.x, tParam.PositionP.y)).absSq() < BOT_POINT_THRESH * BOT_POINT_THRESH)
      // {
      //   tState = COMPLETED;
      // }
    }
    Tactic::Param TDefendLine::paramFromJSON(string json) {
      using namespace rapidjson;
      Tactic::Param tParam;
      Document d;
      d.Parse(json.c_str());
      tParam.DefendLineP.x1 = d["x1"].GetDouble();
      tParam.DefendLineP.x2 = d["x2"].GetDouble();
      tParam.DefendLineP.y1 = d["y1"].GetDouble();
      tParam.DefendLineP.y2 = d["y2"].GetDouble();
      tParam.DefendLineP.radius = d["radius"].GetDouble();
      return tParam;
    }

    string TDefendLine::paramToJSON(Tactic::Param tParam) {
      using namespace rapidjson;
      StringBuffer buffer;
      Writer<StringBuffer> w(buffer);
      w.StartObject();
      w.String("x1");
      w.Double(tParam.DefendLineP.x1);
      w.String("x2");
      w.Double(tParam.DefendLineP.x2);
      w.String("y1");
      w.Double(tParam.DefendLineP.y1);
      w.String("y2");
      w.Double(tParam.DefendLineP.y2);
      w.String("radius");
      w.Int(tParam.DefendLineP.radius);
      w.EndObject();
      return buffer.GetString();
    }
} // namespace Strategy
