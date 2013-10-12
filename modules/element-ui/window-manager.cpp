
#include "guiapp.hpp"
#include "window-manager.hpp"

namespace Element {
    namespace gui {
        WindowManager::WindowManager (GuiApp& gui_)
            : gui (gui_)
        {
            //gui.commander().registerAllCommandsForTarget (this);
        }
    }
}
