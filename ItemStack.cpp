#include "ItemStack.h"


ItemStack::ItemStack(const Item& item, int quantite)
    : item(item), quantite(quantite){
    this->item = item;
    this->quantite = quantite;
}

void ItemStack::ajouter_quantite(int n){
    quantite += n;
}

void ItemStack::enlever_quantite(int n){
    quantite -= n;
    if (quantite < 0){
        quantite = 0;
    }
}

int ItemStack::getQuantite() const{
    return quantite;
}

Item ItemStack::getItem() const{
    return item;
}

