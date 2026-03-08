#ifndef CHARACTER_H
#define CHARACTER_H

#include <string>

class Character {
    
protected:
    int atk;
    int hp;
    int maxHp;
    int def;
    std::string nom;
    
public:
    Character(const std::string& nom, int hp, int atk, int def);

    bool estEnVie() const;
    void subirDegats(int amount);
    void soigner(int amount);

    std::string getNom() const;
    int getHp() const;
    int getMaxHp() const;
    int getAtk() const;
    int getDef() const;
};

#endif

