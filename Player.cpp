#include "Player.h"
#include <iostream>

using namespace std;

Player::Player(const std::string& nom, int hp, int atk, int def): Character(nom, hp, atk, def){
    
    this->n_victoire = 0;
    this->n_kill = 0;
    this->n_epargne = 0;
    
    
}

void Player::add_kill(){
    n_kill++;
}

void Player::add_epargne(){
    n_epargne++;
}

void Player::add_victoire(){
    n_victoire++;
}

void Player::useItem(int choix){
    inventory.utiliser_item(choix, *this);
}

void Player::stats() const{
    
    cout << "Nom : " << getNom() << endl;
    cout << "HP : " << getHp() << endl;
    cout << "Attaque : " << getAtk() << endl;
    cout << "Defense : " << getDef() << endl;
    cout << "Victoires : " << getN_victoire() << endl;
    cout << "Kill : " << getN_kill() << endl;
    cout << "Epargne : " << getN_epargne() << endl;
    
    
}

int Player::getN_victoire() const{
    return n_victoire;
}

int Player::getN_kill() const{
    return n_kill;
}

int Player::getN_epargne() const{
    return n_epargne;
}

Inventory& Player::getInventory(){
    return inventory;
}
