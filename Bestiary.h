#ifndef BESTIARY_H
#define BESTIARY_H

#include <vector>
#include "BestiaryEntry.h"

class Bestiary {
    
private:
    std::vector<BestiaryEntry> bestiaire;
    
public:
    Bestiary();
    
    void ajouter_monstre(const BestiaryEntry& entry);
    void afficher_bestiaire() const;
    
    std::vector<BestiaryEntry> getBestiaire() const;
    bool estVide() const;
    
};

#endif
