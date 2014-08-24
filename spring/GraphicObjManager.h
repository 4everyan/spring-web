
#ifndef __GRAPHICOBJMANAGER_H__
#define __GRAPHICOBJMANAGER_H__

#include <set>
#include <memory>


class GraphicObject;
class GraphicObjManager;


class GraphicObject
{
public:
    virtual ~GraphicObject() {}

private:
    friend class GraphicObjManager;
    virtual void Release() = 0;
};


class GraphicObjManager
{
private:
	GraphicObjManager() = default;
    GraphicObjManager(const GraphicObjManager&) = delete;
    void operator = (const GraphicObjManager&) = delete;

public:
    ~GraphicObjManager();
    static std::shared_ptr<GraphicObjManager> Instance();

    void RegisterObject(std::shared_ptr<GraphicObject> object);
    void ReleaseGraphics();

private:
    class Representation;
    Representation* rep;
};


#endif //__GRAPHICOBJMANAGER_H__
