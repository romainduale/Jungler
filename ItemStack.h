#ifndef ItemStack_H
#define ItemStack_H

#include "Item.h"

class ItemStack {
    
private:
    Item item;
    int quantite;
    
public:
    ItemStack(const Item& item, int quantite);
    
    void ajouter_quantite(int n);
    void enlever_quantite(int n);
    
    int getQuantite() const;
    Item getItem() const;

};
#endif
