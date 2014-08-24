
#include "GraphicObjManager.h"



class GraphicObjManager::Representation
{
public:
    static std::shared_ptr<GraphicObjManager> instance;

    using ObjectPtr = std::shared_ptr<GraphicObject>;
    using ObjectList = std::set<ObjectPtr>;
    ObjectList objects;
};
std::shared_ptr<GraphicObjManager> GraphicObjManager::Representation::instance;


GraphicObjManager::~GraphicObjManager() {
    delete rep;
}

std::shared_ptr<GraphicObjManager> GraphicObjManager::Instance()
{
    if( Representation::instance )
        return Representation::instance;

    std::shared_ptr<GraphicObjManager> manager( new GraphicObjManager );
    manager->rep = new Representation;
    Representation::instance = manager;

    return manager;
}

void GraphicObjManager::RegisterObject(std::shared_ptr<GraphicObject> object) {
    rep->objects.insert(object);
}

void GraphicObjManager::ReleaseGraphics() {
    for( auto obj : rep->objects )
        obj->Release();
}
