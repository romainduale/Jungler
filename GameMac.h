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
    std::string saveFilename;

   
    void loadItems();
    void loadMonsters();
    void loadActs();


    void showMainMenu();
    void showBestiary() const;
    void showStats() const;
    void showItems();
    void showEnding() const;


    void startRandomBattle();
    void battle(Monster* monster);
    void displayBattleStatus(Monster* monster) const;

  
    bool saveGame(const std::string& filename = "save.txt") const;
    bool loadGame(const std::string& filename = "save.txt");

public:
    Game();
    ~Game();  // Nettoyage mémoire des Monster*

    void start();
};

#endif
