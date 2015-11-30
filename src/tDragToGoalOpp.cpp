#include <list>
#include "tDragToGoalOpp.hpp"
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

    TDragToGoalOpp::TDragToGoalOpp(int botID) :
      Tactic(botID)
    {

    } // TDragToGoalOpp
    

    TDragToGoalOpp::~TDragToGoalOpp()
    { } // ~TDragToGoalOpp
    bool TDragToGoalOpp::isCompleted(const BeliefState &bs) const {
      return false;
    }
    bool TDragToGoalOpp::isActiveTactic(void) const
    {
      return false;
    }
//CHOOSEbEST bOT AND the giving of parameters for going to the required point needs to be entered
//Choose best bot also needs to get the params that the tactic has in order to choose the best bot....

    int TDragToGoalOpp::chooseBestBot(const BeliefState &state, std::list<int>& freeBots, const Param& tParam, int prevID) const
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

    gr_Robot_Command TDragToGoalOpp::execute(const BeliefState &state, const Param& tParam)
    {

      // Select the skill to be executed next
      #undef HALF_FIELD_MAXX
      #define HALF_FIELD_MAXX -SELECT(3025, 225)
      SkillSet::SParam sParam;

      Vector2D<int> fastBallPos  = Vector2D<int>((int)state.ballPos.x + 0.3*state.ballVel.x, (int)state.ballPos.y + 0.3*state.ballVel.y);
      Vector2D<int> goalPoint(HALF_FIELD_MAXX, 0);
      float theta               = normalizeAngle(Vector2D<int>::angle(goalPoint, fastBallPos));
      float angleWithBall       = normalizeAngle(Vector2D<int>::angle(fastBallPos, Vector2D<int>(state.homePos[botID].x,state.homePos[botID].y)));
      Vector2D<int> finalPoint  = Vector2D<int>(2*BOT_BALL_THRESH*cos(theta) + state.homePos[botID].x, 2*BOT_BALL_THRESH*sin(theta) + state.homePos[botID].y);
      
      Vector2D<int> targetPoint = fastBallPos - Vector2D<int>(2*BOT_BALL_THRESH*cos(theta), 2*BOT_BALL_THRESH*sin(theta));
      float dist = Vector2D<int>::dist(targetPoint, Vector2D<int>(state.homePos[botID].x,state.homePos[botID].y));
      printf("Drag to Goal: %f, %d, %f, \n", dist, 2*BOT_BALL_THRESH, fabs(firaNormalizeAngle(theta)));
      float sign = (state.homePos[botID].x - fastBallPos.x)*(state.homePos[botID].x - HALF_FIELD_MAXX);
      static int stt = 1;
      printf("State: %d\n", stt);
      if(stt == 0 && dist < 2*BOT_BALL_THRESH && fabs(firaNormalizeAngle(theta - angleWithBall)) < 0.05 && sign > 0) {
        stt = 1;
      }
      if(stt == 1 && dist > 3*BOT_BALL_THRESH || sign < 0) {
        stt = 0;
      }
      if(stt == 1)
      {
        //Util::Logger::toStdOut("Dribbling to goal %d %d", goalPoint.x, goalPoint.y);
        Strategy::SkillSet::SkillID sID = SkillSet::GoToPoint;
        sParam.GoToPointP.finalslope = normalizeAngle(Vector2D<int>::angle( goalPoint, fastBallPos ));
        sParam.GoToPointP.x = goalPoint.x;
        sParam.GoToPointP.y = goalPoint.y;
        
        sParam.GoToPointP.finalVelocity = 0.1;
        sParam.GoToPointP.align = true;
        Strategy::SkillSet *ptr = SkillSet::instance();
        return ptr->executeSkill(sID, sParam, state, botID);
      }
      else{
        //Util::Logger::toStdOut("Going to ball");
        Strategy::SkillSet::SkillID sID = SkillSet::GoToPoint;
        sParam.GoToPointP.align = true;
        sParam.GoToPointP.x = targetPoint.x;
        sParam.GoToPointP.y = targetPoint.y;
        sParam.GoToPointP.finalslope = Vector2D<int>::angle(goalPoint, Vector2D<int>(state.homePos[botID].x,state.homePos[botID].y));
        Strategy::SkillSet *ptr = SkillSet::instance();
        return ptr->executeSkill(sID, sParam, state, botID);
      }
      /*if (state.pr_oppBall || state.pr_looseBall||state->pr_goalscored)
      {
//      tState = COMPLETED;
      }*/
      #undef HALF_FIELD_MAXX
      #define HALF_FIELD_MAXX SELECT(3025, 225)
      // Select the skill to the executed next
//      printf("botpos x:%d\ty:%d\n", state->homePos[botID].x, state->homePos[botID].y);

    

      
    }
    Tactic::Param TDragToGoalOpp::paramFromJSON(string json) {
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

    string TDragToGoalOpp::paramToJSON(Tactic::Param tParam) {
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
