#ifndef ACTCATALOG_H
#define ACTCATALOG_H

#include <map>
#include <string>
#include "Act.h"

class ActCatalog {
    
private:
    std::map<std::string, Act> acts;
    
public:
    ActCatalog();
    
    void addAct(const Act& act);

    const std::map<std::string, Act>& getActs() const;

    const Act* getAct(const std::string& id) const;
    
};

#endif

