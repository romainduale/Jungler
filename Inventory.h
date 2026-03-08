#ifndef Inventory_H
#define Inventory_H

#include "ItemStack.h"
#include "Item.h"
#include <vector>

class Player;

class Inventory {
    
private:
    std::vector<ItemStack> items;
    
public:
    Inventory();
    
    void ajouter_item(const Item& item, int quantite);
    void utiliser_item(int index, Player& player);
    void afficher_item() const;
    
    std::vector<ItemStack> getItems() const;
    bool estVide() const;
    
};

#endif
