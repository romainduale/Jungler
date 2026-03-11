// Game.h
#ifndef GAME_H
#define GAME_H

#include "Player.h"
#include "Bestiary.h"
#include "ActCatalog.h"
#include "Monster.h"
#include <vector>
#include <string>

class Game {
private:
    Player player;
    Bestiary bestiary;
    ActCatalog actCatalog;
    std::vector<Monster*> monsterPool;

    // Chargement des données
    void loadItems();
    void loadMonsters();
    void loadActs();

    // Menu et affichage
    void showMainMenu();
    void showBestiary() const;
    void showStats() const;
    void showItems();
    void showEnding() const;

    // Combat
    void startRandomBattle();
    void battle(Monster* monster);
    void displayBattleStatus(Monster* monster) const;

public:
    Game();
    ~Game();  // Nettoyage mémoire des Monster*

    void start();
};

#endif
