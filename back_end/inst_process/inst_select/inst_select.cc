#include"inst_select.h"

void RiscV64Unit::SelectInstructionAndBuildCFG()
{
    //TODO
    //原架构中dest->xx 直接替换成xx 比如dest->functions直接替换成functions
    LowerFrame();//最后调用LowerFrame
}
void RiscV64Unit::LowerFrame()
{
    //TODO
    //原架构中需要通过unit获取functions等信息，此处直接获取即可
}
void RiscV64Unit::LowerStack()
{
    //TODO
    //原架构中需要通过unit获取functions等信息，此处直接获取即可
}


