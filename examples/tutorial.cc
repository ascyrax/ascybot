#include <sc2api/sc2_api.h>

#include <iostream>
#include <vector>

#define endl std::endl
#define cout std::cout

using namespace sc2;

// global variables
std::vector<const Unit*>idleLarvas,idleDrones,idleOverlords,idleHatcheries,idleQueens,idleOverseers,idleZergs;
int gameLoop = 0;// Observation()->GetGameLoop();

int minerals = 0;// Observation()->GetMinerals();
int vespene = 0;// Observation()->GetVespene();

int foodCap = 0;// Observation()->GetFoodCap();
int foodUsed = 0;// Observation()->GetFoodUsed();
int foodArmy = 0;// Observation()->GetFoodArmy();
int foodWorkers = 0;// Observation()->GetFoodWorkers();

int idleWorkerCnt = 0;// Observation()->GetIdleWorkerCount();
int armyCnt = 0;// Observation()->GetArmyCount();
// bug. GetLarvaCnt always returns 0.
//int larvaCnt =0;// Observation()->GetLarvaCount();
int larvaCnt = 0;// idleLarvas.size();

bool flagOverlord14 = true;// build an overlord at 14 supply

int nActions = 0;

class Bot :public Agent {
public:


    virtual void OnGameStart() final {
        cout << "Hello ascyrax. Game started." << endl;
    }

    virtual void OnUnitIdle(const Unit* unit) {
        if (unit->unit_type == UNIT_TYPEID::ZERG_LARVA)
            idleLarvas.push_back(unit);
        else if (unit->unit_type == UNIT_TYPEID::ZERG_DRONE)
            idleDrones.push_back(unit);
        else if (unit->unit_type == UNIT_TYPEID::ZERG_OVERLORD)
            idleOverlords.push_back(unit);
        else if (unit->unit_type == UNIT_TYPEID::ZERG_HATCHERY)
            idleHatcheries.push_back(unit);
        else if (unit->unit_type == UNIT_TYPEID::ZERG_QUEEN)
            idleQueens.push_back(unit);
        else if (unit->unit_type == UNIT_TYPEID::ZERG_OVERSEER)
            idleOverseers.push_back(unit);
    }

    virtual void OnUnitCreated(const Unit* unit) {
        cout << "A " << UnitTypeToName(unit->unit_type) <<" was created."<< endl;
    }

    virtual void OnGameEnd() {
        cout << "Bye ascyrax. Game ended." << endl;
    }

    // coordinator.update() forwards the game by a certain amount of game steps
    // after a step is completed, an observation is received => client events are run
    // Our OnStep function is run after the client events.

    void trainDrone() {
        const Unit* larva = Observation()->GetUnit(idleLarvas[0]->tag);
        Actions()->UnitCommand(larva, ABILITY_ID::TRAIN_DRONE);
        idleLarvas.erase(idleLarvas.begin());
        larvaCnt = idleLarvas.size();
        nActions++;
        return;
    }
    void trainOverlord() {
        const Unit* larva = Observation()->GetUnit(idleLarvas[0]->tag);
        Actions()->UnitCommand(larva, ABILITY_ID::TRAIN_OVERLORD);
        idleLarvas.erase(idleLarvas.begin());
        nActions++;
        return;
    }

    virtual void OnStep() final {
        gameLoop = Observation()->GetGameLoop();

        minerals = Observation()->GetMinerals();
        vespene = Observation()->GetVespene();

        foodCap = Observation()->GetFoodCap();
        foodUsed = Observation()->GetFoodUsed();
        foodArmy = Observation()->GetFoodArmy();
        foodWorkers = Observation()->GetFoodWorkers();

        idleWorkerCnt = Observation()->GetIdleWorkerCount();
        armyCnt = Observation()->GetArmyCount();
        // bug. GetLarvaCnt always returns 0.
        // larvaCnt = Observation()->GetLarvaCount();
        larvaCnt = idleLarvas.size();

     /*   if(minerals%50==0){
            cout << gameLoop << " " << minerals << " " << vespene << " " << endl;
            cout << foodCap << " " << foodUsed << " " << foodArmy << " " << foodWorkers << endl;
            cout << idleWorkerCnt << " " << armyCnt << " " << larvaCnt << endl;
            cout << "idleLarvas" << endl;
            for (auto el : idleLarvas)cout << UnitTypeToName(el->unit_type)
                << " , unit tag = "<<el->tag<< endl;
        }*/
        

        if (foodCap == 14 && foodUsed == 12) {
            // get a larva
            if(larvaCnt && minerals>=50)
                trainDrone();
        }
        if (foodCap == 14 && foodUsed == 13) {
            // get a larva
            if (larvaCnt && flagOverlord14 && minerals>=100)
            {
                trainOverlord();
                flagOverlord14=false;
            }
            if (flagOverlord14==false && minerals >= 50)trainDrone();
        }
        if (foodCap == 22 && foodUsed == 14) {
            // get two larvas
            if (larvaCnt >= 2 && minerals>=100) {
                trainDrone();
                trainDrone();
            }
        }
        // get the natural
        if (minerals >= 200) {
            // 
        }
    }
};


int main(int argc, char* argv[]) {
    Coordinator coordinator;
    coordinator.SetStepSize(1);
    ProcessSettings prSet;
    prSet.step_size = 1;
    prSet.full_screen = true;
    coordinator.LoadSettings(argc, argv);

    Bot bot;
    coordinator.SetParticipants({
        CreateParticipant(Race::Zerg, &bot),
        CreateComputer(Race::Random,Difficulty::CheatInsane,AIBuild::Rush)
        });

    coordinator.LaunchStarcraft();
    coordinator.StartGame(sc2::kMapBelShirVestigeLE);

    int updateCnt = 0;
    cout << "0 updates done." << endl;
    while (coordinator.Update()) {
        ++updateCnt;
        //cout << updateCnt << " updates done." << endl;
        //for (auto el : idleLarvas) 
            //cout << el->tag << " "; cout << endl;
        //cout << "update loop finishes here." <<endl<<endl<< endl;
        //if (updateCnt == 3200)break;
        /*if (nActions >= 10)break;*/
    }
    return 0;
}
