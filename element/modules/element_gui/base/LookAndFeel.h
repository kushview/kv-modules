
#ifndef ELEMENT_STYLE_H
#define ELEMENT_STYLE_H

enum ColourIds
{
    mainBackgroundColourId          = 0x2340000,
    treeviewHighlightColourId       = 0x2340002
};

class JUCE_API  Style   : public LookAndFeel_V3
{
public:

    Style();
    virtual ~Style();

    virtual void drawButtonBackground (Graphics&, Button&, const Colour& backgroundColour,
                                       bool isMouseOverButton, bool isButtonDown) override;

    virtual void drawTableHeaderBackground (Graphics&, TableHeaderComponent&) override;

    virtual bool areLinesDrawnForTreeView (TreeView&) override;
    virtual void drawTreeviewPlusMinusBox (Graphics&, const Rectangle<float>& area, Colour backgroundColour, bool isOpen, bool isMouseOver) override;
    virtual int getTreeViewIndentSize (TreeView&) override;

    virtual void drawComboBox (Graphics& g, int width, int height, bool isButtonDown,
                               int buttonX, int buttonY, int buttonW, int buttonH, ComboBox& box) override;

    virtual void drawKeymapChangeButton (Graphics& g, int width, int height, Button& button, const String& keyDescription) override;

    virtual void drawPopupMenuBackground (Graphics& g, int width, int height) override;

    virtual int getTabButtonOverlap (int tabDepth) override;
    virtual int getTabButtonSpaceAroundImage() override;
    virtual void drawTabButton (TabBarButton&, Graphics&, bool isMouseOver, bool isMouseDown) override;


    virtual void drawStretchableLayoutResizerBar (Graphics&, int w, int h, bool isVerticalBar, bool isMouseOver, bool isMouseDragging) override;


    bool areScrollbarButtonsVisible() override;
    virtual void drawScrollbar (Graphics&, ScrollBar&, int x, int y, int width, int height, bool isScrollbarVertical,
                                int thumbStartPosition, int thumbSize, bool isMouseOver, bool isMouseDown) override;

    virtual void drawConcertinaPanelHeader (Graphics&, const Rectangle<int>& area, bool isMouseOver, bool isMouseDown,
                                            ConcertinaPanel&, Component&) override;

    static void createTabTextLayout (const TabBarButton& button, float length, float depth, Colour colour, TextLayout&);

private:

    Image backgroundTexture;
    Colour backgroundTextureBaseColour;

};



#endif /* ELEMENT_STYLE_H */
