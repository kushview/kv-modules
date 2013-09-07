/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2013 - Raw Material Software Ltd.

   Permission is granted to use this software under the terms of either:
   a) the GPL v2 (or any later version)
   b) the Affero GPL v3

   Details of these licenses can be found at: www.gnu.org/licenses

   JUCE is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
   A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

   ------------------------------------------------------------------------------

   To release a closed-source product which uses JUCE, commercial licenses are
   available: visit www.juce.com for more information.

  ==============================================================================
*/

#include "element/gui/treeview-base.hpp"
#include "element/gui/style.hpp"

namespace element {

//==============================================================================
void TreePanelBase::setRoot (TreeItemBase* root)
{
    rootItem = root;
    tree.setRootItem (root);
    tree.getRootItem()->setOpen (true);
#if 0
    if (project != nullptr)
    {
        const ScopedPointer<XmlElement> treeOpenness (project->getStoredProperties()
                                                          .getXmlValue (opennessStateKey));
        if (treeOpenness != nullptr)
        {
            tree.restoreOpennessState (*treeOpenness, true);

            for (int i = tree.getNumSelectedItems(); --i >= 0;)
                if (JucerTreeViewBase* item = dynamic_cast<JucerTreeViewBase*> (tree.getSelectedItem (i)))
                    item->cancelDelayedSelectionTimer();
        }
    }
#endif
}

void TreePanelBase::saveOpenness()
{
#if 0
    if (project != nullptr)
    {
        const ScopedPointer<XmlElement> opennessState (tree.getOpennessState (true));

        if (opennessState != nullptr)
            project->getStoredProperties().setValue (opennessStateKey, opennessState);
        else
            project->getStoredProperties().removeValue (opennessStateKey);
    }
#endif
}

//==============================================================================
TreeItemBase::TreeItemBase()  : textX (0)
{
    setLinesDrawnForSubItems (false);
}

TreeItemBase::~TreeItemBase()
{
    masterReference.clear();
}

void TreeItemBase::refreshSubItems()
{
    WholeTreeOpennessRestorer wtor (*this);
    clearSubItems();
    addSubItems();
}

Font TreeItemBase::getFont() const
{
    return Font (getItemHeight() * 0.6f);
}

void TreeItemBase::paintItem (Graphics& g, int /*width*/, int /*height*/)
{
    if (isSelected())
        g.fillAll (getOwnerView()->findColour (gui::treeviewHighlightColourId));
}

float TreeItemBase::getIconSize() const
{
    return jmin (getItemHeight() - 4.0f, 18.0f);
}

void TreeItemBase::paintOpenCloseButton (Graphics& g, int width, int height, bool /*isMouseOver*/)
{
    Path p;

    if (isOpen())
        p.addTriangle (width * 0.2f,  height * 0.25f, width * 0.8f, height * 0.25f, width * 0.5f, height * 0.75f);
    else
        p.addTriangle (width * 0.25f, height * 0.25f, width * 0.8f, height * 0.5f,  width * 0.25f, height * 0.75f);

    g.setColour (getOwnerView()->findColour (gui::mainBackgroundColourId).contrasting (0.3f));
    g.fillPath (p);
}

Colour TreeItemBase::getBackgroundColour() const
{
    Colour background (getOwnerView()->findColour (gui::mainBackgroundColourId));

    if (isSelected())
        background = background.overlaidWith (getOwnerView()->findColour (gui::treeviewHighlightColourId));

    return background;
}

Colour TreeItemBase::getContrastingColour (float contrast) const
{
    return getBackgroundColour().contrasting (contrast);
}

Colour TreeItemBase::getContrastingColour (const Colour& target, float minContrast) const
{
    return getBackgroundColour().contrasting (target, minContrast);
}

void TreeItemBase::paintContent (Graphics& g, const Rectangle<int>& area)
{
    g.setFont (getFont());
    g.setColour (isMissing() ? getContrastingColour (Colours::red, 0.8f)
                             : getContrastingColour (0.8f));

    g.drawFittedText (getDisplayName(), area, Justification::centredLeft, 1, 0.8f);
}

Component* TreeItemBase::createItemComponent()
{
    return new TreeItemComponent (*this);
}

//==============================================================================
class RenameTreeItemCallback  : public ModalComponentManager::Callback,
                                public TextEditorListener
{
public:
    RenameTreeItemCallback (TreeItemBase& ti, Component& parent, const Rectangle<int>& bounds)
        : item (ti)
    {
        ed.setMultiLine (false, false);
        ed.setPopupMenuEnabled (false);
        ed.setSelectAllWhenFocused (true);
        ed.setFont (item.getFont());
        ed.addListener (this);
        ed.setText (item.getRenamingName());
        ed.setBounds (bounds);

        parent.addAndMakeVisible (&ed);
        ed.enterModalState (true, this);
    }

    void modalStateFinished (int resultCode) override
    {
        if (resultCode != 0)
            item.setName (ed.getText());
    }

    void textEditorTextChanged (TextEditor&) override               {}
    void textEditorReturnKeyPressed (TextEditor& editor) override    { editor.exitModalState (1); }
    void textEditorEscapeKeyPressed (TextEditor& editor) override    { editor.exitModalState (0); }
    void textEditorFocusLost (TextEditor& editor) override           { editor.exitModalState (0); }

private:
    struct RenameEditor   : public TextEditor
    {
        void inputAttemptWhenModal() override   { exitModalState (0); }
    };

    RenameEditor ed;
    TreeItemBase& item;

    JUCE_DECLARE_NON_COPYABLE (RenameTreeItemCallback)
};

void TreeItemBase::showRenameBox()
{
    Rectangle<int> r (getItemPosition (true));
    r.setLeft (r.getX() + textX);
    r.setHeight (getItemHeight());

    new RenameTreeItemCallback (*this, *getOwnerView(), r);
}

void TreeItemBase::itemClicked (const MouseEvent& e)
{
    if (e.mods.isPopupMenu())
    {
        if (getOwnerView()->getNumSelectedItems() > 1)
            showMultiSelectionPopupMenu();
        else
            showPopupMenu();
    }
}

void TreeItemBase::deleteItem()    {}
void TreeItemBase::deleteAllSelectedItems() {}
void TreeItemBase::showDocument()  {}
void TreeItemBase::showPopupMenu() {}
void TreeItemBase::showMultiSelectionPopupMenu() {}

static void treeViewMenuItemChosen (int resultCode, WeakReference<TreeItemBase> item)
{
    if (item != nullptr)
        item->handlePopupMenuResult (resultCode);
}

void TreeItemBase::launchPopupMenu (PopupMenu& m)
{
    m.showMenuAsync (PopupMenu::Options(),
                     ModalCallbackFunction::create (treeViewMenuItemChosen, WeakReference<TreeItemBase> (this)));
}

void TreeItemBase::handlePopupMenuResult (int)
{
}

//==============================================================================
class TreeItemBase::ItemSelectionTimer  : public Timer
{
public:
    ItemSelectionTimer (TreeItemBase& tvb)  : owner (tvb) {}

    void timerCallback() override    { owner.invokeShowDocument(); }

private:
    TreeItemBase& owner;
    JUCE_DECLARE_NON_COPYABLE (ItemSelectionTimer)
};

void TreeItemBase::itemSelectionChanged (bool isNowSelected)
{
    if (isNowSelected)
    {
        delayedSelectionTimer = new ItemSelectionTimer (*this);
        delayedSelectionTimer->startTimer (getMillisecsAllowedForDragGesture());
    }
    else
    {
        cancelDelayedSelectionTimer();
    }
}

void TreeItemBase::invokeShowDocument()
{
    cancelDelayedSelectionTimer();
    showDocument();
}

void TreeItemBase::itemDoubleClicked (const MouseEvent&)
{
    invokeShowDocument();
}

void TreeItemBase::cancelDelayedSelectionTimer()
{
    delayedSelectionTimer = nullptr;
}

}
