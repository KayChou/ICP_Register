#include "ICP.h"
#include "utils.h"
#include "Opengl_Render.h"

using namespace gs;


int main()
{
    //create a static box point cloud used as a reference.

	Opengl_Render *module_opgl_render = new Opengl_Render();
    module_opgl_render->init();

    module_opgl_render->loop();

    module_opgl_render->destroy();
	delete module_opgl_render;
}