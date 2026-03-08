#ifndef PLAYER_H
#define PLAYER_H

#include "Character.h"
#include "Inventory.h"

class Player : public Character {
    
private:
    int n_victoire;
    int n_kill;
    int n_epargne;
    Inventory inventory;
    
public:
    Player(const std::string& nom, int hp, int atk, int def);
    
    void add_kill();
    void add_epargne();
    void add_victoire();
    
    void useItem(int choix);
    void stats() const;
    
    int getN_victoire() const;
    int getN_kill() const;
    int getN_epargne() const;
    Inventory& getInventory();
    
    
};

#endif


