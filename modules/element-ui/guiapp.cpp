
#include "element/juce.hpp"

#include "guiapp.hpp"
#include "main-window.hpp"
#include "element/session/world.hpp"

namespace element {
namespace gui {


    GuiApp::GuiApp (World& world)
        : AppController (world),
          mainWindow (nullptr), windowManager (nullptr)
    {
        LookAndFeel::setDefaultLookAndFeel (&elementStyle);
        windowManager.reset (new WindowManager (*this));
    }

    GuiApp::~GuiApp()
    {
        std::clog << "~GuiApp()\n";

        windowManager.reset (nullptr);

        mainWindow->setVisible(false);
        mainWindow->removeFromDesktop();
        mainWindow.reset(nullptr);

        audioEngine.reset();
    }

    Shared<GuiApp>
    GuiApp::create (World& world)
    {
        Shared<GuiApp> element (new GuiApp (world));

        world.plugins().addDefaultFormats();
        element->commander().registerAllCommandsForTarget (JUCEApplication::getInstance());

        element->showSplash();
        return element;
    }

    void
    GuiApp::showSplash()
    {

    }

    void
    GuiApp::run()
    {
        audioEngine = world().engine();
        mainWindow.reset (new MainWindow (*this));
        mainWindow->centreWithSize (900, 600);
        mainWindow->setVisible (true);
        mainWindow->addToDesktop();
    }


    class GuiModule :  public Module
    {
    public:
        GuiModule()
        {

        }

        ~GuiModule() { guiApp.reset(); }

        void load (World *world)
        {
            guiApp = GuiApp::create (*world);
        }

        void run (World *world)
        {
            if (guiApp != nullptr)
                guiApp->run();
        }

    private:

        Shared<GuiApp> guiApp;

    };

}}

extern "C" {

    element::Module*
    element_module_load()
    {
        return new element::gui::GuiModule();
    }

}
