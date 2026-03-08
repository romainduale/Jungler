#ifndef ACT_H
#define ACT_H

#include <string>

class Act {
    
private:
    std::string id;
    std::string texte;
    int mercy_impact;
    
public:
    Act(const std::string& id, const std::string& texte, int mercy_impact);
    
    std::string getId() const;
    std::string getTexte() const;
    int getMercy_impact() const;
    
    
    
    
};

#endif

