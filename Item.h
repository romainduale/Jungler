#ifndef Item_H
#define Item_H

#include <string>

enum ItemType {
    HEAL
};

class Item {
    
private:
    std::string nom;
    ItemType type;
    int valeur;
    
public:
    Item(const std::string& nom, ItemType type, int valeur);
    
    
    int getValeur() const;
    ItemType getType() const;
    std::string getNom() const;
    
    
};

#endif
