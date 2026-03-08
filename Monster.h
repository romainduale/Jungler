#ifndef MONSTER_H
#define MONSTER_H

#include "Character.h"

#include <vector>
#include <string>

enum Category {
    NORMAL,
    MINIBOSS,
    BOSS
};

class Monster : public Character {
    
private:
    
    Category category;
    int mercy;
    int mercy_goal;
    std::vector<std::string> actIds;
    
public:
    Monster(const std::string& nom, int hp, int atk, int def, Category category, int mercyGoal, const std::vector<std::string>& actIds);
    
    void modifier_mercy(int n);
    bool est_epargne() const;
    
    std::vector<std::string> getAvailableActs() const;
    Category getCategory() const;
    int getMercy() const;
    int getMercyGoal() const;

    
};

#endif
