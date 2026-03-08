#include "Bestiary.h"
#include <iostream>


using namespace std;

Bestiary::Bestiary(){
    
}

void Bestiary::ajouter_monstre(const BestiaryEntry& entry){
    bestiaire.push_back(entry);
}

void Bestiary::afficher_bestiaire() const{
    
    for (int i=0; i < bestiaire.size(); i++){
        cout << i << " : " << bestiaire[i].getNom() << endl;
    }
    
}

vector<BestiaryEntry> Bestiary::getBestiaire() const{
    return bestiaire;
}

bool Bestiary::estVide() const{
    return bestiaire.size() == 0;
}

