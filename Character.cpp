
#include "Character.h"

Character::Character(const std::string& nom, int hp, int atk, int def){
    
    this->atk = atk;
    this->hp = hp;
    this->maxHp = hp;
    this->def = def;
    this->nom = nom;
    
    
}

bool Character::estEnVie() const{
    return hp > 0;
}

void Character::subirDegats(int amount){
    
    hp -= amount;
    
    if (hp<0){
        
        hp = 0;
    }
    
}

void Character::soigner(int amount){
    hp += amount;
    
    if (hp>maxHp){
        
        hp = maxHp;
    }
    
}

std::string Character::getNom() const{
    return nom;
}

int Character::getHp() const{
    return hp;
}

int Character::getMaxHp() const{
    return maxHp;
}

int Character::getAtk() const{
    return atk;
}

int Character::getDef() const{
    return def;
}
