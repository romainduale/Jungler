#include "Inventory.h"
#include "Player.h"
#include <iostream>

using namespace std;

Inventory::Inventory()
{
}


void Inventory::ajouter_item(const Item& item, int quantite)
{
    for (int i = 0; i < items.size(); i++)
    {
        ItemStack stack = items[i];
        Item item_stack = stack.getItem();

        if (item_stack.getNom() == item.getNom())
        {
            items[i].ajouter_quantite(quantite);
            return;
        }
    }

    
    ItemStack nouveau(item, quantite);
    items.push_back(nouveau);
}

void Inventory::utiliser_item(int index, Player& player)
{
    if (index < 0 || index >= items.size())
    {
        cout << "Choix invalide." << endl;
        return;
    }

    ItemStack stack = items[index];
    Item item = stack.getItem();

    if (item.getType() == HEAL)
    {
        player.soigner(item.getValeur());
        cout << "Vous utilisez " << item.getNom() << endl;
    }

    items[index].enlever_quantite(1);

    if (items[index].getQuantite() == 0)
    {
        items.erase(items.begin() + index);
    }
}

void Inventory::afficher_item() const
{
    if (items.size() == 0)
    {
        cout << "Inventaire vide." << endl;
        return;
    }

    for (int i = 0; i < items.size(); i++)
    {
        ItemStack stack = items[i];

        cout << i << " : "
             << stack.getItem().getNom()
             << " x"
             << stack.getQuantite()
             << endl;
    }
}


vector<ItemStack> Inventory::getItems() const
{
    return items;
}

bool Inventory::estVide() const
{
    if (items.size() == 0)
        return true;
    else
        return false;
}
