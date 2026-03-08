#include "BestiaryEntry.h"

using namespace std;

BestiaryEntry::BestiaryEntry(const string& nom, Category category, int maxHp, int atk, int def, const string& resultat){
    
    this->nom = nom;
    this->category = category;
    this->maxHp = maxHp;
    this->atk = atk;
    this->def = def;
    this->resultat = resultat;
}

string BestiaryEntry::getNom() const{
    return nom;
}

Category BestiaryEntry::getCategory() const{
    return category;
}

int BestiaryEntry::getMaxHp() const{
    return maxHp;
}

int BestiaryEntry::getAtk() const{
    return atk;
}

int BestiaryEntry::getDef() const{
    return def;
}

string BestiaryEntry::getResultat() const{
    return resultat;
}
