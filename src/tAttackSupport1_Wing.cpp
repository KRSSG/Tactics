#include <list>
#include "tactics/tAttackSupport1_Wing.hpp"
#include "skills/skillSet.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <ssl_common/config.h>
#include <iostream>
#include <stdio.h>
#include <ssl_common/geometry.hpp>

#include <fstream>

#define MARGIN_PASS_AHEAD 0
#define SIDE_THRESH 2*DBOX_WIDTH	

namespace Strategy{

	TAttackSupport1_Wing::TAttackSupport1_Wing(int botID):
		Tactic(botID){

		}
	
	TAttackSupport1_Wing::~TAttackSupport1_Wing(){

	}

	bool TAttackSupport1_Wing::isCompleted(const BeliefState &bs) const{
		//add logic
		return false;
	}
	bool TAttackSupport1_Wing::isActiveTactic(void)const{
		return false;
	}

	bool TAttackSupport1_Wing::isApplicable(const BeliefState &bs, const Param& tParam){
		bool Applicable = false;

		float x = bs.ballPos.x;
		float y = bs.ballPos.y;

		if( x < (- HALF_FIELD_MAXX / 3)  && x > (HALF_FIELD_MAXX) ){
			if( fabs(y) < HALF_FIELD_MAXY ){
				Applicable = true;
			}
		}

		else if( x <  HALF_FIELD_MAXX/2  ){
			if( fabs(y) < HALF_FIELD_MAXY/1.5 )
				Applicable = true;
		}

		return Applicable;
	}

	int TAttackSupport1_Wing::chooseBestBot(const BeliefState &state, std::list<int>& freeBots, const Param& tParam, int prevID) const{

		int attacker_ID = tParam.AttackSupportP.id;

		Vector2D<int> attacker(state.homePos[attacker_ID].x , state.homePos[attacker_ID].y);
		Vector2D<int> ballPos(state.ballPos.x , state.ballPos.y);
		int top_half=0,defended_side;

		for(int i=0; i<6; ++i){

			if(state.awayPos[i].x > 0){
				if(state.awayPos[i].y > 0)
					top_half++;
				else if(state.awayPos[i].y < 0)
					top_half--;
			}

		}

		if(top_half > 0)
			defended_side = 1;
		else if(top_half < 0)
			defended_side = -1;
		else
			defended_side = 0;

		int side = (-1) * defended_side;
		int on_side_bots = 0, free_bots = 0;

		float target_x , target_y;

		if(state.ballPos.x < 0){
			target_x = 0.25 * HALF_FIELD_MAXX - MARGIN_PASS_AHEAD;

			target_y = 0.75 * HALF_FIELD_MAXY * side;
		}

		else if(state.ballPos.x < (HALF_FIELD_MAXX / 3)){
			target_x = (state.ballPos.x + HALF_FIELD_MAXX)/2 - MARGIN_PASS_AHEAD ;

			target_y = 0.6 * HALF_FIELD_MAXY * side;
		}

		else{
			target_x = (state.ballPos.x + HALF_FIELD_MAXX)/2 - MARGIN_PASS_AHEAD ;

			target_y = (state.ballPos.x / HALF_FIELD_MAXX) * HALF_FIELD_MAXY * side;

			if( fabs(target_y) < DBOX_HEIGHT)
				target_y = DBOX_HEIGHT * side;
			else if( fabs(target_y) > 0.6* HALF_FIELD_MAXY)
				target_y = target_y = 0.6 * HALF_FIELD_MAXY * side;
		}

		Vector2D<int> target(target_x , target_y);

		int test_bot_gen = *(freeBots.begin());
		int test_bot_on_side = *(freeBots.begin());
		float min_distance_from_target = 2 * HALF_FIELD_MAXX;
		float min_distance_from_target_on_side = 2 * HALF_FIELD_MAXX;


		for (std::list<int>::iterator it = freeBots.begin(); it != freeBots.end(); ++it){
			Vector2D<int> testPos(state.homePos[*it].x , state.homePos[*it].y);

			if( sqrt((ballPos - testPos).absSq()) > DRIBBLER_BALL_THRESH){

				float distance_from_target =  sqrt((testPos - target).absSq());

				if(distance_from_target < min_distance_from_target){
					test_bot_gen = *it;
					min_distance_from_target = distance_from_target;
				}

				if((state.homePos[*it].y * side) > 0){
					++on_side_bots;

					if(distance_from_target < min_distance_from_target_on_side){
						test_bot_on_side = *it;
						min_distance_from_target_on_side = distance_from_target;
					}

				}
			}
			++free_bots;
		}

		if( free_bots - on_side_bots >= 2 || (min_distance_from_target_on_side - min_distance_from_target) <= SIDE_THRESH ){
			return test_bot_gen;
		}

		return test_bot_on_side;
	}



	


	gr_Robot_Command TAttackSupport1_Wing::execute(const BeliefState &state, const Tactic::Param& tParam){

		Strategy::SkillSet::SkillID sID;
		SkillSet::SParam sParam;

		int attacker_ID = tParam.AttackSupportP.id;

		Vector2D<int> attacker(state.homePos[attacker_ID].x , state.homePos[attacker_ID].y);
		Vector2D<int> botPos(state.homePos[botID].x , state.homePos[botID].y);
		Vector2D<int> ballPos(state.ballPos.x , state.ballPos.y);
		int top_half=0,defended_side;

		for(int i=0; i<6; ++i){

			if(state.awayPos[i].x > 0){
				if(state.awayPos[i].y > 0)
					top_half++;
				else if(state.awayPos[i].y < 0)
					top_half--;
			}

		}

		if(top_half > 0)
			defended_side = 1;
		else if(top_half < 0)
			defended_side = -1;
		else
			defended_side = 0;

		int side = (-1) * defended_side;

		float target_x , target_y;

		if(state.ballPos.x < 0){
			target_x = 0.25 * HALF_FIELD_MAXX - MARGIN_PASS_AHEAD;

			target_y = 0.75 * HALF_FIELD_MAXY * side;
		}

		else if(state.ballPos.x < (HALF_FIELD_MAXX / 3)){
			target_x = (state.ballPos.x + HALF_FIELD_MAXX)/2 - MARGIN_PASS_AHEAD ;

			target_y = 0.6 * HALF_FIELD_MAXY * side;
		}

		else{
			target_x = (state.ballPos.x + HALF_FIELD_MAXX)/2  - MARGIN_PASS_AHEAD;

			target_y = (1- (state.ballPos.x / HALF_FIELD_MAXX)) * HALF_FIELD_MAXY * side;

			if( fabs(target_y) < DBOX_HEIGHT/1.3)
				target_y = DBOX_HEIGHT/1.3 * side;
			else if( fabs(target_y) > 0.6* HALF_FIELD_MAXY)
				target_y = target_y = 0.6 * HALF_FIELD_MAXY * side;
		}

		Vector2D<int> target( target_x , target_y);
		float theta = normalizeAngle(Vector2D<int>::angle(ballPos , target)) , our_distance_from_line = 999999, opp_distance_from_line = 999999;
		
		for( int id=0; id<6; id++){
			Vector2D <int> opp_test (state.awayPos[id].x , state.awayPos[id].y);
			Vector2D <int> home_test (state.homePos[id].x , state.homePos[id].y);

			opp_distance_from_line = fabs(((target_y - state.ballPos.y) * (state.awayPos[id].x)) - ((target_x - state.ballPos.x) * (state.awayPos[id].y)) + (target_x* state.ballPos.y) - (target_y * state.ballPos.x)) / sqrt((botPos - ballPos).absSq());
			if( (ballPos - opp_test).absSq() > (ballPos - target).absSq() || (botPos - opp_test).absSq() > (ballPos - target).absSq() )
				opp_distance_from_line = 999999;
			our_distance_from_line = fabs(((target_y - state.ballPos.y) * (state.homePos[id].x)) - ((target_x - state.ballPos.x) * (state.homePos[id].y)) + (target_x* state.ballPos.y) - (target_y * state.ballPos.x)) / sqrt((botPos - ballPos).absSq());
			if( (ballPos - home_test).absSq() > (ballPos - target).absSq() || (botPos - home_test).absSq() > (ballPos - target).absSq() || id == attacker_ID || id == botID)
				our_distance_from_line = 999999;
			if ( opp_distance_from_line < 100 || our_distance_from_line < 60){

				target_x += 4 * BOT_RADIUS;
				target_y -= 4 * BOT_RADIUS * side;

				if(target_x >= HALF_FIELD_MAXX)
					target_x -= 8 * BOT_RADIUS;
			

				sID = SkillSet::GoToPoint;
				sParam.GoToPointP.x = target_x;
				sParam.GoToPointP.y = target_y;
				sParam.GoToPointP.finalVelocity = 0;

				Vector2D<int> final_target( target_x , target_y);
				float angleToTurn = normalizeAngle(Vector2D<int>::angle(ballPos , final_target));

				sParam.GoToPointP.finalslope = angleToTurn ;

				return SkillSet::instance()->executeSkill(sID, sParam, state, botID);
			}
		}

		sID = SkillSet::GoToPoint;

		if(target_x > (HALF_FIELD_MAXX - DBOX_WIDTH/3))
			target_x = (HALF_FIELD_MAXX - DBOX_WIDTH/3);

		sParam.GoToPointP.x = target_x;

		sParam.GoToPointP.y = target_y;

		sParam.GoToPointP.finalVelocity = 0;

		Vector2D<int> final_target( target_x , target_y);
		theta = normalizeAngle(Vector2D<int>::angle(ballPos , final_target));
		sParam.GoToPointP.finalslope = theta ;

		return SkillSet::instance()->executeSkill(sID, sParam, state, botID);

		

	}

 	 Tactic::Param TAttackSupport1_Wing::paramFromJSON(string json) {
	      using namespace rapidjson;
	      Tactic::Param tParam;
	      Document d;
	      d.Parse(json.c_str());
	      tParam.AttackSupportP.id = d["id"].GetInt();
	      return tParam;
	}

    	string TAttackSupport1_Wing::paramToJSON(Tactic::Param tParam) {
	      using namespace rapidjson;
	      StringBuffer buffer;
	      Writer <StringBuffer> w(buffer);
	      w.StartObject();
	      w.String("id");
	      w.Int(tParam.AttackSupportP.id);
	      w.EndObject();
	      return buffer.GetString();
	}
	    
}
