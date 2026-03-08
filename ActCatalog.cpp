#include "ActCatalog.h"

using namespace std;

ActCatalog::ActCatalog(){
    
}

void ActCatalog::addAct(const Act& act)
{
    acts[act.getId()] = act;
}

const map<string, Act>& ActCatalog::getActs() const
{
    return acts;
}

const Act* ActCatalog::getAct(const string& id) const
{
    map<string, Act>::const_iterator it = acts.find(id);

    if (it != acts.end())
    {
        return &(it->second);
    }

    return nullptr;
}
