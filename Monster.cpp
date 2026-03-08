#include "Monster.h"

using namespace std;

Monster::Monster(const string& nom, int hp, int atk, int def, Category category, int mercyGoal, const vector<string>& actIds):Character(nom, hp, atk, def){
    
    this->mercy = 0;
    this->category = category;
    this->actIds = actIds;
    this->mercy_goal = mercyGoal;
    
}

void Monster::modifier_mercy(int n){
    mercy += n;
    if (mercy < 0){
            mercy = 0;
        }

    if (mercy > mercy_goal){
            mercy = mercy_goal;
        }
}

bool Monster::est_epargne() const{
    return mercy >= mercy_goal;
}

vector<string> Monster::getAvailableActs() const{
    return actIds;
}

Category Monster::getCategory() const{
    return category;
}

int Monster::getMercy() const{
    return mercy;
}

int Monster::getMercyGoal() const{
    return mercy_goal;
}
