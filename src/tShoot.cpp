#include <list>
#include "tShoot.hpp"
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

    TShoot::TShoot(int botID) :
      Tactic(botID)
    {

    } // TShoot
    

    TShoot::~TShoot()
    { } // ~TShoot
    bool TShoot::isCompleted(const BeliefState &bs) const {
      return false;
    }
    bool TShoot::isActiveTactic(void) const
    {
      return false;
    }
//CHOOSEbEST bOT AND the giving of parameters for going to the required point needs to be entered
//Choose best bot also needs to get the params that the tactic has in order to choose the best bot....

    int TShoot::chooseBestBot(const BeliefState &state, std::list<int>& freeBots, const Param& tParam, int prevID) const
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

    Vector2D<int> TShoot::getOppGoalie(const BeliefState &state)
    {
      return Vector2D<int>(state.awayPos[0].x,state.awayPos[0].y);
    }

    gr_Robot_Command TShoot::execute(const BeliefState &state, const Param& tParam)
    {
      float tmp_angle1,tmp_angle2,tmp_angle3;
      float angle1,angle2,finalAngle;
      SkillSet::SParam sParam;
      
      tmp_angle1 = Vector2D<int>::angle(Vector2D<int>(HALF_FIELD_MAXX,OPP_GOAL_MAXY),Vector2D<int>(state.ballPos.x,state.ballPos.y));
      Vector2D<int> OppGoalie = getOppGoalie(state);
      tmp_angle2 = Vector2D<int>::angle(OppGoalie,Vector2D<int>(state.ballPos.x,state.ballPos.y));
      tmp_angle3 = Vector2D<int>::angle(Vector2D<int>(HALF_FIELD_MAXX,OPP_GOAL_MINY),Vector2D<int>(state.ballPos.x,state.ballPos.y));
      
      angle1 = fabs(tmp_angle2 - tmp_angle1);
      angle2 = fabs(tmp_angle3 - tmp_angle2);
      if(angle1 > angle2)
        finalAngle = (tmp_angle2 + tmp_angle1)/2;
      else
        finalAngle = (tmp_angle2 + tmp_angle3)/2;
      //Util::Logger::toStdOut("Final Angle : %f\n",finalAngle);
      float dist_from_ball = Vector2D<int>::dist(Vector2D<int>(state.homePos[botID].x,state.homePos[botID].y), Vector2D<int>(state.ballPos.x,state.ballPos.y));
      if(dist_from_ball < BOT_BALL_THRESH)
      {
        sParam.GoToPointP.x = HALF_FIELD_MAXX;
        /* The point of intersection with the goal Vertical line y = mx1 + (y2 - mx2)*/
        sParam.GoToPointP.y = finalAngle*HALF_FIELD_MAXX + (state.homePos[botID].y - finalAngle*state.homePos[botID].x);
      }
      else 
      {
        sParam.GoToPointP.x = state.ballPos.x;
        sParam.GoToPointP.y = state.ballPos.y; 
      }
      sParam.GoToPointP.align = true;
      sParam.GoToPointP.finalVelocity = 0;
      sParam.GoToPointP.finalslope = finalAngle;
      Strategy::SkillSet::SkillID sID = SkillSet::GoToPoint;
      Strategy::SkillSet *ptr = SkillSet::instance();
      return ptr->executeSkill(sID, sParam, state, botID);
      
    }
    Tactic::Param TShoot::paramFromJSON(string json) {
      using namespace rapidjson;
      Tactic::Param tParam;
      Document d;
      d.Parse(json.c_str());
      return tParam;
    }

    string TShoot::paramToJSON(Tactic::Param tParam) {
      using namespace rapidjson;
      StringBuffer buffer;
      Writer<StringBuffer> w(buffer);
      w.StartObject();
      w.EndObject();
      return buffer.GetString();
    }
} // namespace Strategy
