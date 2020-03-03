#include "rmsfactory.h"

/*RMS工厂指针*/
static RMS * rmsFactory = nullptr;

/*设置工厂*/
void SetRMSFactory(RMS * ptrRMS)
{
    rmsFactory = ptrRMS;
}

/*获得工厂*/
RMS * RMSFactory()
{
    return rmsFactory;
}
