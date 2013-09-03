#ifndef MAINMENU_HPP
#define MAINMENU_HPP

#include "main-window.hpp"

namespace element {
namespace gui {

    class MainWindow;

class MainMenu : public MenuBarModel,
                 public ApplicationCommandTarget
{

    ScopedPointer<PopupMenu> macMenu;
public:

    enum RootNames {
        File, Edit, View, Workspace, Help, NumMenus
    };

    MainMenu (MainWindow& parent)
        : owner (parent)
    {
#if JUCE_MAC
       {
           macMenu = new PopupMenu();
           macMenu->addCommandItem (&owner.app().commander(), Commands::showAbout, "About Element...");
           macMenu->addSeparator();
           macMenu->addCommandItem (&owner.app().commander(), Commands::showPreferences, "Preferences...");
           MainMenu::setMacMainMenu (this, macMenu, "Recent Items...");
       }
#else
       owner.setMenuBar (this);
#endif
    }

    ~MainMenu()
    {
#if JUCE_MAC
      MainMenu::setMacMainMenu (nullptr);
      macMenu = nullptr;
#else
      owner.setMenuBar (nullptr);
#endif
    }

    StringArray getMenuBarNames()
    {
        const char* const names[] = { "File", "Edit", "View", "Workspace", "Help", nullptr };
        return StringArray (names);
    }

    PopupMenu getMenuForIndex (int id, const String&)
    {
       PopupMenu menu;
       if (id == File)        buildFileMenu (menu);
       else if (id == Edit)   buildEditMenu (menu);
       else if (id == View)   buildViewMenu (menu);
       else if (id == Workspace) buildWindowMenu (menu);
       else if (id == Help)   buildHelpMenu (menu);
       else  { };
       return menu;
    }

   void buildFileMenu (PopupMenu& menu)
   {
#if ! JUCE_MAC
       menu.addCommandItem (&owner.app().commander(), Commands::showPreferences, "Preferences..");
       menu.addSeparator();
       menu.addCommandItem (&owner.app().commander(), StandardApplicationCommandIDs::quit);
#endif
   }

   void buildEditMenu (PopupMenu& menu)
   {
        ApplicationCommandManager* acm = &owner.app().commander();
        menu.addCommandItem (acm, StandardApplicationCommandIDs::undo, "Undo");
        menu.addCommandItem (acm, StandardApplicationCommandIDs::redo, "Redo");
        menu.addSeparator();
        menu.addCommandItem (acm, StandardApplicationCommandIDs::cut, "Cut");
        menu.addCommandItem (acm, StandardApplicationCommandIDs::copy, "Copy");
        menu.addCommandItem (acm, StandardApplicationCommandIDs::paste, "Paste");
        menu.addSeparator();
        menu.addCommandItem (acm, StandardApplicationCommandIDs::selectAll, "Select All");

   }

   void buildViewMenu (PopupMenu& menu)
   {

   }

   void buildWindowMenu (PopupMenu& menu)
   {
       //XXX this shows up 'greyed out' menu.addCommandItem (&owner.app().commander(), Commands::showPluginManager);
       menu.addItem (123, "Plugin Manager");
   }

   void buildHelpMenu (PopupMenu& menu)
   {
   }

   void menuItemSelected (int index, int menu)
   {
   }

   // Command Target

   ApplicationCommandTarget* getNextCommandTarget() { return nullptr; }

   void getAllCommands (Array <CommandID>& commands)
   {
       const CommandID ids[] = {
           Commands::showAbout,
           Commands::showPreferences,
           Commands::showPluginManager,
           StandardApplicationCommandIDs::undo,
           StandardApplicationCommandIDs::redo,
           StandardApplicationCommandIDs::cut,
           StandardApplicationCommandIDs::copy,
           StandardApplicationCommandIDs::paste,
           StandardApplicationCommandIDs::selectAll
       };

       commands.addArray (ids, numElementsInArray (ids));
   }

   void getCommandInfo (CommandID command, ApplicationCommandInfo& result)
   {
       switch (command)
       {
           case Commands::showPreferences:
               result.setInfo ("Preferences", "Element Preferences", "Application", 0);
               break;
           case Commands::showAbout:
               result.setInfo ("About", "About Element", "Application", 0);
               break;
           case Commands::showPluginManager:
               result.setInfo ("Plugin Manager", "Element Plugin Management", "Application", 0);
               break;
           case StandardApplicationCommandIDs::undo:
               result.setInfo ("Undo", "Element Plugin Management", "Application", 0);
               break;
            case StandardApplicationCommandIDs::redo:
               result.setInfo ("Redo", "Element Plugin Management", "Application", 0);
               break;
           case     StandardApplicationCommandIDs::cut:
               result.setInfo ("Cut", "Element Plugin Management", "Application", 0);
               break;
            case    StandardApplicationCommandIDs::copy:
               result.setInfo ("Copy", "Element Plugin Management", "Application", 0);
               break;
            case    StandardApplicationCommandIDs::paste:
               result.setInfo ("Paste", "Element Plugin Management", "Application", 0);
               break;
            case    StandardApplicationCommandIDs::selectAll:
               result.setInfo ("Select All", "Element Plugin Management", "Application", 0);
               break;
           default: break;
       }
   }

   bool perform (const InvocationInfo& info)
   {
       switch (info.commandID)
       {
           case Commands::showAbout:
           case Commands::showPluginManager:
               break;
           case Commands::showPreferences:
               owner.app().openDialog ("element://dialogs/preferences");
           return true;
           break;

       case 0: break;
       }

       return false;
   }


private:

    MainWindow& owner;
    //Workspace workspace;

};


}}

#endif // MAINMENU_HPP
