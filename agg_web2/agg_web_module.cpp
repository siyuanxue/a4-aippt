#include <agg_web2/agg_web_module_factory.h>

A4_USE_NAMESPACE(module);
A4_BEGIN_NAMESPACE(agg_web);

ModuleFactory* CreateFactory()
{
    return new AGG_NS(AGG2)::AggWeb2ModuleFactory;
}

void DestroyFactory(ModuleFactory *factory)
{
    factory->Destroy();
}

A4_END_NAMESPACE(agg_web);

