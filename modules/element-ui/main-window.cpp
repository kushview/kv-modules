
#include "element/juce.hpp"
#include "element/gui/timeline.hpp"

#include "commands.hpp"
#include "main-window.hpp"
#include "main-menu.hpp"
#include "workspace.hpp"

namespace element {
namespace gui {

   MainWindow::MainWindow (GuiApp& gui_)
       : DocumentWindow ("Element", Colours::darkgrey, DocumentWindow::allButtons, false),
         gui (gui_)
   {
       Component* content = new Component();
       setContentOwned (content, false);

       setUsingNativeTitleBar (true);
       setResizable (true, false);

       mainMenu = new MainMenu (*this);
       gui.commander().registerAllCommandsForTarget (mainMenu);
       gui.commander().setFirstCommandTarget (mainMenu);
   }

   MainWindow::~MainWindow()
   {
      mainMenu = nullptr;
   }

   void
   MainWindow::closeButtonPressed()
   {
       JUCEApplication* app (JUCEApplication::getInstance());
       app->systemRequestedQuit();
   }


}} /* namespace Drumbot::Gui */
