/*
    This file is part of the element modules for the JUCE Library
    Copyright (C) 2014  Kushview, LLC.  All rights reserved.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

const Colour LookAndFeel_E1::elementBlue            = Colour (0xff4765a0);

const Colour LookAndFeel_E1::backgroundColor        = Colour (LookAndFeel_E1::defaultBackgroundColor);
const Colour LookAndFeel_E1::contentBackgroundColor = Colour (0x212125);
const Colour LookAndFeel_E1::widgetBackgroundColor  = Colour (0x3b3b3b);

const Colour LookAndFeel_E1::textColor              = Colour (LookAndFeel_E1::defaultTextColor);
const Colour LookAndFeel_E1::textActiveColor        = Colour (LookAndFeel_E1::defaultTextActiveColor);
const Colour LookAndFeel_E1::textBoldColor          = Colour (LookAndFeel_E1::defaultTextBoldColor);
const Colour LookAndFeel_E1::highlightBackgroundColor = LookAndFeel_E1::textColor.darker(0.6000006).withAlpha(0.6f);

LookAndFeel_E1::LookAndFeel_E1()
{
    // Text Buttons
    setColour (TextButton::buttonColourId, Colour (0xff525252));
    setColour (TextButton::buttonOnColourId, Colour (0xff525252));
    setColour (TextButton::textColourOffId, Colours::white);
    setColour (TextButton::textColourOnId, Colours::white);
    
    // PopupMenu Styling
    setColour (PopupMenu::backgroundColourId, Colour (0xfff0f0f0));
    setColour (PopupMenu::textColourId, Colour (0xff1d1d1e));
    setColour (PopupMenu::headerTextColourId, Colour (0xff1d1d1e));
    setColour (PopupMenu::highlightedBackgroundColourId, elementBlue);
    setColour (PopupMenu::highlightedTextColourId, Colour (0xfff0f0f0));

    // ComboBox Styling
    setColour (ComboBox::backgroundColourId, Colours::black);
    setColour (ComboBox::outlineColourId, Colours::black.brighter(0.2f));
    setColour (ComboBox::buttonColourId, Colours::black.brighter(0.2f));
    setColour (ComboBox::textColourId, Colour((uint32)defaultTextActiveColor));
    setColour (ComboBox::arrowColourId, Colour((uint32)defaultTextColor));

    // Meter Styling
    typedef Element::DigitalMeter Meter;
    setColour (Meter::levelOverColourId, Colours::red);
    setColour (Meter::level0dBColourId, Colours::red);
    setColour (Meter::level3dBColourId, Colours::yellow);
    setColour (Meter::level6dBColourId, Colours::yellow);
    setColour (Meter::level10dBColourId, Colours::green);
    setColour (Meter::backgroundColourId, Colours::black);

    // ListBox Styling
    setColour (ListBox::backgroundColourId, Colour (0x00000000));
    setColour (ListBox::textColourId, LookAndFeel_E1::textColor);

    setColour (TextEditor::backgroundColourId, LookAndFeel_E1::backgroundColor);
    setColour (TextEditor::highlightColourId, LookAndFeel_E1::backgroundColor.brighter());
    setColour (TextEditor::highlightColourId, highlightBackgroundColor);
    setColour (TextEditor::highlightedTextColourId, LookAndFeel_E1::textColor.contrasting());
    setColour (TextEditor::textColourId, LookAndFeel_E1::textColor);

    // Toolbar Styling
    setColour (Toolbar::backgroundColourId, LookAndFeel_E1::backgroundColor.brighter (0.05f));
    setColour (Toolbar::buttonMouseDownBackgroundColourId, LookAndFeel_E1::backgroundColor.brighter (0.1f));
    setColour (Toolbar::buttonMouseOverBackgroundColourId, LookAndFeel_E1::backgroundColor.darker (0.046f));

    // alert window
    setColour (AlertWindow::backgroundColourId,  LookAndFeel_E1::backgroundColor);
    setColour (AlertWindow::textColourId, LookAndFeel_E1::textColor);

    // Label
    setColour(Label::textColourId, LookAndFeel_E1::textColor);

    // search path component
    setColour (FileSearchPathListComponent::backgroundColourId, LookAndFeel_E1::backgroundColor);

    // Tree View
    setColour (TreeView::backgroundColourId, Colour (0x00000000));
    setColour (TreeView::linesColourId, LookAndFeel_E1::textColor);
    setColour (TreeView::dragAndDropIndicatorColourId, Colours::orange.darker());
    setColour (TreeView::selectedItemBackgroundColourId, highlightBackgroundColor);

    // Digital meter styling
    setColour (DigitalMeter::levelOverColourId, Colours::yellow.darker());
    setColour (DigitalMeter::level0dBColourId, Colours::yellowgreen);
    setColour (DigitalMeter::level3dBColourId, Colours::lightgreen);
    setColour (DigitalMeter::level6dBColourId, Colours::green);
    setColour (DigitalMeter::level10dBColourId, Colours::darkgreen.darker());
    setColour (DigitalMeter::backgroundColourId, Colours::transparentBlack);
    setColour (DigitalMeter::foregroundColourId, Colours::transparentWhite);


    setColour (mainBackgroundColourId, Colour (0xff333333));
    setColour (treeviewHighlightColourId, Colour (0xffeeeeee));

#if 0
    setColour (ListBox::backgroundColourId, Colour (0xff222222));

    setColour (TreeView::selectedItemBackgroundColourId, Colour (0x301111ee));
    setColour (TreeView::backgroundColourId, Colour (0xff222222));

    const Colour textButtonColour (0xffeeeeff);

    setColour (TextButton::buttonColourId, textButtonColour);
    setColour (ComboBox::buttonColourId, textButtonColour);
    setColour (ScrollBar::thumbColourId, Colour::greyLevel (0.8f).contrasting().withAlpha (0.13f));
#endif
}

LookAndFeel_E1::~LookAndFeel_E1() { }

bool LookAndFeel_E1::areScrollbarButtonsVisible() { return false; }

void LookAndFeel_E1::drawStretchableLayoutResizerBar (Graphics& g, int /*w*/, int /*h*/, bool /*isVerticalBar*/,
                                             bool isMouseOver, bool isMouseDragging)
{
    if (isMouseOver || isMouseDragging)
        g.fillAll (elementBlue.withAlpha (0.4f));
}

void LookAndFeel_E1::drawScrollbar (Graphics& g, ScrollBar& scrollbar, int x, int y, int width, int height,
                                    bool isScrollbarVertical, int thumbStartPosition, int thumbSize, bool isMouseOver, bool isMouseDown)
{
    Path thumbPath;

    if (thumbSize > 0)
    {
        const float thumbIndent = (isScrollbarVertical ? width : height) * 0.25f;
        const float thumbIndentx2 = thumbIndent * 2.0f;

        if (isScrollbarVertical)
            thumbPath.addRoundedRectangle (x + thumbIndent, thumbStartPosition + thumbIndent,
                                           width - thumbIndentx2, thumbSize - thumbIndentx2, (width - thumbIndentx2) * 0.5f);
        else
            thumbPath.addRoundedRectangle (thumbStartPosition + thumbIndent, y + thumbIndent,
                                           thumbSize - thumbIndentx2, height - thumbIndentx2, (height - thumbIndentx2) * 0.5f);
    }

    Colour thumbCol (scrollbar.findColour (ScrollBar::thumbColourId, true));

    if (isMouseOver || isMouseDown)
        thumbCol = thumbCol.withMultipliedAlpha (2.0f);

    g.setColour (thumbCol);
    g.fillPath (thumbPath);

    g.setColour (thumbCol.contrasting ((isMouseOver  || isMouseDown) ? 0.2f : 0.1f));
    g.strokePath (thumbPath, PathStrokeType (1.0f));
}

void LookAndFeel_E1::drawConcertinaPanelHeader (Graphics& g, const Rectangle<int>& area,
                                                bool isMouseOver, bool /*isMouseDown*/,
                                                ConcertinaPanel&, Component& panel)
{
    const Colour bkg (Colours::grey);

    g.setGradientFill (ColourGradient (Colours::white.withAlpha (isMouseOver ? 0.4f : 0.2f), 0, (float) area.getY(),
                                       Colours::darkgrey.withAlpha (0.2f), 0, (float) area.getBottom(), false));
    g.fillAll();

    g.setColour (bkg.contrasting().withAlpha (0.04f));
    g.fillRect (area.withHeight (1));
    g.fillRect (area.withTop (area.getBottom() - 1));

    g.setColour (bkg.contrasting());
    g.setFont (Font (area.getHeight() * 0.6f).boldened());
    g.drawFittedText (panel.getName(), 4, 0, area.getWidth() - 6, area.getHeight(), Justification::centredLeft, 1);
}

static void drawButtonShape (Graphics& g, const Path& outline, Colour baseColour, float height)
{
    const float mainBrightness = baseColour.getBrightness();
    const float mainAlpha = baseColour.getFloatAlpha();

    g.setGradientFill (ColourGradient (baseColour.brighter (0.2f), 0.0f, 0.0f,
                                       baseColour.darker (0.25f), 0.0f, height, false));
    g.fillPath (outline);

    g.setColour (Colours::white.withAlpha (0.4f * mainAlpha * mainBrightness * mainBrightness));
    g.strokePath (outline, PathStrokeType (1.0f), AffineTransform::translation (0.0f, 1.0f)
                                                        .scaled (1.0f, (height - 1.6f) / height));

    g.setColour (Colours::black.withAlpha (0.4f * mainAlpha));
    g.strokePath (outline, PathStrokeType (1.0f));
}

void LookAndFeel_E1::drawButtonBackground (Graphics& g, Button& button, const Colour& backgroundColour,
                                           bool isMouseOverButton, bool isButtonDown)
{
    Colour baseColour (backgroundColour.withMultipliedSaturation(button.hasKeyboardFocus (true) ? 1.3f : 0.9f)
                                       .withMultipliedAlpha(button.isEnabled() ? 0.9f : 0.5f));

    if (isButtonDown || isMouseOverButton)
        baseColour = baseColour.contrasting (isButtonDown ? 0.2f : 0.1f);

    const bool flatOnLeft   = button.isConnectedOnLeft();
    const bool flatOnRight  = button.isConnectedOnRight();
    const bool flatOnTop    = button.isConnectedOnTop();
    const bool flatOnBottom = button.isConnectedOnBottom();

    const float width  = button.getWidth() - 1.0f;
    const float height = button.getHeight() - 1.0f;
    const float cornerSize = 2.0f;

    Path outline;
    outline.addRoundedRectangle (0.5f, 0.5f, width, height, cornerSize, cornerSize,
                                 ! (flatOnLeft  || flatOnTop),
                                 ! (flatOnRight || flatOnTop),
                                 ! (flatOnLeft  || flatOnBottom),
                                 ! (flatOnRight || flatOnBottom));

    drawButtonShape (g, outline, baseColour, height);
}

void LookAndFeel_E1::drawTableHeaderBackground (Graphics& g, TableHeaderComponent& header)
{
    Rectangle<int> r (header.getLocalBounds());

    g.setColour (Colours::black.withAlpha (0.5f));
    g.fillRect (r.removeFromBottom (1));

    g.setColour (Colours::white.withAlpha (0.6f));
    g.fillRect (r);

    g.setColour (Colours::black.withAlpha (0.5f));

    for (int i = header.getNumColumns (true); --i >= 0;)
        g.fillRect (header.getColumnPosition (i).removeFromRight (1));
}

int LookAndFeel_E1::getTabButtonOverlap (int /*tabDepth*/)            { return -1; }
int LookAndFeel_E1::getTabButtonSpaceAroundImage()                    { return 1; }

void LookAndFeel_E1::createTabTextLayout (const TabBarButton& button, float length, float depth,
                                          Colour colour, TextLayout& textLayout)
{
    Font font (depth * 0.5f);
    font.setUnderline (button.hasKeyboardFocus (false));

    AttributedString s;
    s.setJustification (Justification::centred);
    s.append (button.getButtonText().trim(), font, colour);

    textLayout.createLayout (s, length);
}

void LookAndFeel_E1::drawTabButton (TabBarButton& button, Graphics& g, bool isMouseOver, bool isMouseDown)
{
    const Rectangle<int> activeArea (button.getActiveArea());

    const TabbedButtonBar::Orientation o = button.getTabbedButtonBar().getOrientation();

    const Colour bkg (button.getTabBackgroundColour());

    if (button.getToggleState())
    {
        g.setColour (bkg);
        g.fillRect (activeArea);
    }
    else
    {
        Point<int> p1, p2;

        switch (o)
        {
            case TabbedButtonBar::TabsAtBottom:   p1 = activeArea.getBottomLeft(); p2 = activeArea.getTopLeft();    break;
            case TabbedButtonBar::TabsAtTop:      p1 = activeArea.getTopLeft();    p2 = activeArea.getBottomLeft(); break;
            case TabbedButtonBar::TabsAtRight:    p1 = activeArea.getTopRight();   p2 = activeArea.getTopLeft();    break;
            case TabbedButtonBar::TabsAtLeft:     p1 = activeArea.getTopLeft();    p2 = activeArea.getTopRight();   break;
            default:                              jassertfalse; break;
        }

        g.setGradientFill (ColourGradient (bkg.brighter (0.2f), (float) p1.x, (float) p1.y,
                                           bkg.darker (0.1f),   (float) p2.x, (float) p2.y, false));
        g.fillRect (activeArea);
    }

    g.setColour (bkg.contrasting (0.3f));
    Rectangle<int> r (activeArea);

    if (o != TabbedButtonBar::TabsAtBottom)   g.fillRect (r.removeFromTop (1));
    if (o != TabbedButtonBar::TabsAtTop)      g.fillRect (r.removeFromBottom (1));
    if (o != TabbedButtonBar::TabsAtRight)    g.fillRect (r.removeFromLeft (1));
    if (o != TabbedButtonBar::TabsAtLeft)     g.fillRect (r.removeFromRight (1));

    const float alpha = button.isEnabled() ? ((isMouseOver || isMouseDown) ? 1.0f : 0.8f) : 0.3f;
    const Colour col (bkg.contrasting().withMultipliedAlpha (alpha));

    const Rectangle<float> area (button.getTextArea().toFloat());

    float length = area.getWidth();
    float depth  = area.getHeight();

    if (button.getTabbedButtonBar().isVertical())
        std::swap (length, depth);

    TextLayout textLayout;
    createTabTextLayout (button, length, depth, col, textLayout);

    AffineTransform t;

    switch (o)
    {
        case TabbedButtonBar::TabsAtLeft:   t = t.rotated (float_Pi * -0.5f).translated (area.getX(), area.getBottom()); break;
        case TabbedButtonBar::TabsAtRight:  t = t.rotated (float_Pi *  0.5f).translated (area.getRight(), area.getY()); break;
        case TabbedButtonBar::TabsAtTop:
        case TabbedButtonBar::TabsAtBottom: t = t.translated (area.getX(), area.getY()); break;
        default:                            jassertfalse; break;
    }

    g.addTransform (t);
    textLayout.draw (g, Rectangle<float> (length, depth));
}

void LookAndFeel_E1::drawTreeviewPlusMinusBox (Graphics& g, const Rectangle<float>& area,
                                               Colour backgroundColour, bool isOpen, bool isMouseOver)
{
    Path p;
    p.addTriangle (0.0f, 0.0f, 1.0f, isOpen ? 0.0f : 0.5f, isOpen ? 0.5f : 0.0f, 1.0f);

    g.setColour (backgroundColour.contrasting().withAlpha (isMouseOver ? 0.5f : 0.3f));
    g.fillPath (p, p.getTransformToScaleToFit (area.reduced (2, area.getHeight() / 4), true));
}

bool LookAndFeel_E1::areLinesDrawnForTreeView (TreeView&)
{
    return false;
}

int LookAndFeel_E1::getTreeViewIndentSize (TreeView&)
{
    return 20;
}

void LookAndFeel_E1::drawComboBox (Graphics& g, int width, int height, const bool isButtonDown,
                                   int buttonX, int buttonY, int buttonW, int buttonH, ComboBox& box)
{
    g.fillAll (box.findColour (ComboBox::backgroundColourId));

    const Colour buttonColour (box.findColour (ComboBox::buttonColourId));

    if (box.isEnabled() && box.hasKeyboardFocus (false))
    {
        g.setColour (buttonColour);
        g.drawRect (0, 0, width, height, 2);
    }
    else
    {
        g.setColour (box.findColour (ComboBox::outlineColourId));
        g.drawRect (0, 0, width, height);
    }

    const float outlineThickness = box.isEnabled() ? (isButtonDown ? 1.2f : 0.5f) : 0.3f;

    Path buttonShape;
    buttonShape.addRectangle (buttonX + outlineThickness,
                              buttonY + outlineThickness,
                              buttonW - outlineThickness * 2.0f,
                              buttonH - outlineThickness * 2.0f);

    drawButtonShape (g, buttonShape,
                     buttonColour.withMultipliedSaturation (box.hasKeyboardFocus (true) ? 1.3f : 0.9f)
                                 .withMultipliedAlpha (box.isEnabled() ? 0.9f : 0.5f),
                     (float) height);

    if (box.isEnabled())
    {
        const float arrowX = 0.3f;
        const float arrowH = 0.2f;

        Path p;
        p.addTriangle (buttonX + buttonW * 0.5f,            buttonY + buttonH * (0.45f - arrowH),
                       buttonX + buttonW * (1.0f - arrowX), buttonY + buttonH * 0.45f,
                       buttonX + buttonW * arrowX,          buttonY + buttonH * 0.45f);

        p.addTriangle (buttonX + buttonW * 0.5f,            buttonY + buttonH * (0.55f + arrowH),
                       buttonX + buttonW * (1.0f - arrowX), buttonY + buttonH * 0.55f,
                       buttonX + buttonW * arrowX,          buttonY + buttonH * 0.55f);

        g.setColour (box.findColour (ComboBox::arrowColourId));
        g.fillPath (p);
    }
}


// MARK: Popup Menu

void LookAndFeel_E1::drawPopupMenuBackground (Graphics& g, int width, int height)
{
    const Rectangle<float> r (0.f, 0.f, (float)width, (float)height);
    g.setColour (findColour (PopupMenu::backgroundColourId));
    g.fillRoundedRectangle (r, 0.0f);
}

void LookAndFeel_E1::getIdealPopupMenuItemSize (const String &text, bool isSeparator,
                                                int standardMenuItemHeight, int &idealWidth, int &idealHeight)
{
    LookAndFeel_V3::getIdealPopupMenuItemSize (text, isSeparator, standardMenuItemHeight, idealWidth, idealHeight);
    if (isSeparator) {
        return;
    }
    
    idealHeight = 20;
}

// MARK: MenuBar

void LookAndFeel_E1::drawMenuBarBackground (Graphics& g, int width, int height,
                                            bool isMouseOverBar, MenuBarComponent& mbc)
{
    LookAndFeel_V3::drawMenuBarBackground (g, width, height, isMouseOverBar, mbc);
}

void LookAndFeel_E1::drawMenuBarItem (Graphics& g, int width, int height, int itemIndex,
                                      const String& itemText, bool isMouseOverItem,
                                      bool isMenuOpen, bool isMouseOverBar, MenuBarComponent& bar)
{
    if (isMouseOverItem || isMenuOpen)
    {
        /* const Rectangle<float> r1(0.f, 3.0f, (float)width, (float)height - 6.0f);
         g.setColour(getToggleColour());
         g.setOpacity(0.20f);
         g.fillRect(r1); */
    }

    g.setOpacity (0.90);
    g.setColour (LookAndFeel_E1::textBoldColor);
    g.drawText (itemText, 0, 0, width, height, Justification::centred);
}



void LookAndFeel_E1::drawKeymapChangeButton (Graphics& g, int width, int height,
                               Button& button, const String& keyDescription)
{
    const Colour textColour (button.findColour (0x100ad01 /*KeyMappingEditorComponent::textColourId*/, true));

    if (keyDescription.isNotEmpty())
    {
        if (button.isEnabled())
        {
            g.setColour (textColour.withAlpha (button.isDown() ? 0.4f : (button.isOver() ? 0.2f : 0.1f)));
            g.fillRoundedRectangle (button.getLocalBounds().toFloat(), 4.0f);
            g.drawRoundedRectangle (button.getLocalBounds().toFloat(), 4.0f, 1.0f);
        }

        g.setColour (textColour);
        g.setFont (height * 0.6f);
        g.drawFittedText (keyDescription, 4, 0, width - 8, height, Justification::centred, 1);
    }
    else
    {
        const float thickness = 7.0f;
        const float indent = 22.0f;

        Path p;
        p.addEllipse (0.0f, 0.0f, 100.0f, 100.0f);
        p.addRectangle (indent, 50.0f - thickness, 100.0f - indent * 2.0f, thickness * 2.0f);
        p.addRectangle (50.0f - thickness, indent, thickness * 2.0f, 50.0f - indent - thickness);
        p.addRectangle (50.0f - thickness, 50.0f + thickness, thickness * 2.0f, 50.0f - indent - thickness);
        p.setUsingNonZeroWinding (false);

        g.setColour (textColour.darker(0.1f).withAlpha (button.isDown() ? 0.7f : (button.isOver() ? 0.5f : 0.3f)));
        g.fillPath (p, p.getTransformToScaleToFit (2.0f, 2.0f, width - 4.0f, height - 4.0f, true));
    }

    if (button.hasKeyboardFocus (false))
    {
        g.setColour (textColour.withAlpha (0.4f));
        g.drawRect (0, 0, width, height);
    }
}
