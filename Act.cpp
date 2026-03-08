#include "Act.h"

using namespace std;

Act::Act(const string& id, const string& texte, int mercy_impact){
    
    this->id = id;
    this->texte = texte;
    this->mercy_impact = mercy_impact;
    
}


string Act::getId() const{
    return id;
}

string Act::getTexte() const{
    return texte;
}

int Act::getMercy_impact() const{
    return mercy_impact;
}
