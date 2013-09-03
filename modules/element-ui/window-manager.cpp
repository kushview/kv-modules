
#include "guiapp.hpp"
#include "window-manager.hpp"

namespace element {
    namespace gui {
        WindowManager::WindowManager (GuiApp& gui_)
            : gui (gui_)
        {
            //gui.commander().registerAllCommandsForTarget (this);
        }
    }
}
