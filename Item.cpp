#include "Item.h"

using namespace std;

Item::Item(const string& nom, ItemType type, int valeur){
    
    this->nom = nom;
    this->type = type;
    this->valeur = valeur;
    
}

int Item::getValeur() const{
    return valeur;
}

ItemType Item::getType() const{
    return type;
}

std::string Item::getNom() const{
    return nom;
}

