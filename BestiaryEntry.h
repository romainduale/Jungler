#ifndef BESTIARYENTRY_H
#define BESTIARYENTRY_H

#include "Monster.h"
#include <string>

class BestiaryEntry {
    
private:
    std::string nom;
    Category category;
    int maxHp;
    int atk;
    int def;
    std::string resultat;
    
public:
    BestiaryEntry(const std::string& nom, Category category, int maxHp, int atk, int def, const std::string& resultat);
    
    std::string getNom() const;
    Category getCategory() const;
    int getMaxHp() const;
    int getAtk() const;
    int getDef() const;
    std::string getResultat() const;
    
    
};

#endif
