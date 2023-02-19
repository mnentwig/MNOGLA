#include "../MNOGLA.h"
#include "../twoD/twoDView.h"
class guiContainer{
    public:
    void feedEvtPtr(int32_t n, int32_t* buf);
    void render(MNOGLA::twoDView& v);    
};