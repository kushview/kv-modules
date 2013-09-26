
#include "element/gui/style.hpp"

namespace element {
namespace gui {


    Style::Style()
    {

        setColour (mainBackgroundColourId, Colours::darkgrey);
        setColour (treeviewHighlightColourId, Colours::transparentBlack);

#if 0
        setColour (MidiGrid::blackKeyColourId, Colours::darkgrey);
        setColour (MidiGrid::blackKeyBrightColourId, Colours::darkgrey);
        setColour (MidiGrid::whiteKeyColourId, Colours::grey);
        setColour (MidiGrid::whiteKeyBrightColourId, Colours::grey);
        setColour (MidiGrid::backGridColourId, Colours::lightgrey);
        setColour (MidiGrid::headerColourId , Colours::darkgrey);
        setColour (MidiGrid::indicatorColourId, Colours::red);
        setColour (MidiGrid::noteFillColourId, Colours::red);
        setColour (MidiGrid::noteBorderColourId, Colours::lightgrey);
#endif

        // set up the standard set of colours..
        const uint32 textButtonColour      = 0xff555555;
        const uint32 textHighlightColour   = 0x30000000;
        const uint32 standardOutlineColour = 0x55000000;

        const uint32 standardBackgroundColour       = 0xff595959;
        const uint32 standardTextColour             = 0xfffafafa;
        const uint32 standardTextHighlightColour    = 0xffffffff;

        static const uint32 standardColours[] =
        {
            TextButton::buttonColourId,                 textButtonColour,
            TextButton::buttonOnColourId,               0xff4444ff,
            TextButton::textColourOnId,                 0xffeeeeee,
            TextButton::textColourOffId,                0xffdddddd,

            ToggleButton::textColourId,                 0xff000000,

            TextEditor::backgroundColourId,             standardBackgroundColour,
            TextEditor::textColourId,                   standardTextColour,
            TextEditor::highlightColourId,              0xffff0000,
            TextEditor::highlightedTextColourId,        0xff000000,
            TextEditor::outlineColourId,                0x00000000,
            TextEditor::focusedOutlineColourId,         textButtonColour,
            TextEditor::shadowColourId,                 0x38000000,

            CaretComponent::caretColourId,              0xff000000,

            Label::backgroundColourId,                  0x00000000,
            Label::textColourId,                        standardTextColour,
            Label::outlineColourId,                     0x00000000,

            ScrollBar::backgroundColourId,              0x00000000,
            ScrollBar::thumbColourId,                   0xff515151,

            TreeView::linesColourId,                    0x4c000000,
            TreeView::backgroundColourId,               0x00000000,
            TreeView::dragAndDropIndicatorColourId,     0x80ff0000,

            PopupMenu::backgroundColourId,              0xff999999,
            PopupMenu::textColourId,                    0xff333333,
            PopupMenu::headerTextColourId,              0xff000000,
            PopupMenu::highlightedTextColourId,         0xffffffff,
            PopupMenu::highlightedBackgroundColourId,   0xff000000,

            ComboBox::buttonColourId,                   0xff212121,
            ComboBox::outlineColourId,                  standardOutlineColour,
            ComboBox::textColourId,                     standardTextColour,
            ComboBox::backgroundColourId,               standardBackgroundColour,
            ComboBox::arrowColourId,                    standardTextColour,

            TextPropertyComponent::backgroundColourId,  standardBackgroundColour,
            TextPropertyComponent::textColourId,        standardTextColour,
            TextPropertyComponent::outlineColourId,     standardOutlineColour,

            ListBox::backgroundColourId,                standardBackgroundColour,
            ListBox::outlineColourId,                   standardOutlineColour,
            ListBox::textColourId,                      standardTextColour,

            Slider::backgroundColourId,                 0x00000000,
            Slider::thumbColourId,                      textButtonColour,
            Slider::trackColourId,                      0x7fffffff,
            Slider::rotarySliderFillColourId,           0x7f0000ff,
            Slider::rotarySliderOutlineColourId,        0x66000000,
            Slider::textBoxTextColourId,                0xff000000,
            Slider::textBoxBackgroundColourId,          0xffffffff,
            Slider::textBoxHighlightColourId,           textHighlightColour,
            Slider::textBoxOutlineColourId,             standardOutlineColour,

            ResizableWindow::backgroundColourId,        0xff777777,
            //DocumentWindow::textColourId,               0xff000000, // (this is deliberately not set)

            AlertWindow::backgroundColourId,            0xffededed,
            AlertWindow::textColourId,                  0xff000000,
            AlertWindow::outlineColourId,               0xff666666,

            ProgressBar::backgroundColourId,            0xffeeeeee,
            ProgressBar::foregroundColourId,            0xffaaaaee,

            TooltipWindow::backgroundColourId,          0xffeeeebb,
            TooltipWindow::textColourId,                0xff000000,
            TooltipWindow::outlineColourId,             0x4c000000,

            TabbedComponent::backgroundColourId,        0x00000000,
            TabbedComponent::outlineColourId,           0xff777777,
            TabbedButtonBar::tabOutlineColourId,        0x80000000,
            TabbedButtonBar::frontOutlineColourId,      0x90000000,

            Toolbar::backgroundColourId,                0xfff6f8f9,
            Toolbar::separatorColourId,                 0x4c000000,
            Toolbar::buttonMouseOverBackgroundColourId, 0x4c0000ff,
            Toolbar::buttonMouseDownBackgroundColourId, 0x800000ff,
            Toolbar::labelTextColourId,                 0xff000000,
            Toolbar::editingModeOutlineColourId,        0xffff0000,

            DrawableButton::backgroundColourId,         0x00000000,
            DrawableButton::backgroundOnColourId,       0xaabbbbff,

            HyperlinkButton::textColourId,              0xcc1111ee,

            GroupComponent::outlineColourId,            0x66000000,
            GroupComponent::textColourId,               0xff000000,

            BubbleComponent::backgroundColourId,        0xeeeeeebb,
            BubbleComponent::outlineColourId,           0x77000000,

            DirectoryContentsDisplayComponent::highlightColourId,   standardTextHighlightColour,
            DirectoryContentsDisplayComponent::textColourId,        standardTextColour,
            FileListComponent::highlightColourId, 0x33000000,

            0x1000440, /*LassoComponent::lassoFillColourId*/        0x66dddddd,
            0x1000441, /*LassoComponent::lassoOutlineColourId*/     0x99111111,

            0x1005000, /*MidiKeyboardComponent::whiteNoteColourId*/               0xffffffff,
            0x1005001, /*MidiKeyboardComponent::blackNoteColourId*/               0xff000000,
            0x1005002, /*MidiKeyboardComponent::keySeparatorLineColourId*/        0x66000000,
            0x1005003, /*MidiKeyboardComponent::mouseOverKeyOverlayColourId*/     0x80ffff00,
            0x1005004, /*MidiKeyboardComponent::keyDownOverlayColourId*/          0xffb6b600,
            0x1005005, /*MidiKeyboardComponent::textLabelColourId*/               0xff000000,
            0x1005006, /*MidiKeyboardComponent::upDownButtonBackgroundColourId*/  0xffd3d3d3,
            0x1005007, /*MidiKeyboardComponent::upDownButtonArrowColourId*/       0xff000000,

            0x1004500, /*CodeEditorComponent::backgroundColourId*/                0xffffffff,
            0x1004502, /*CodeEditorComponent::highlightColourId*/                 textHighlightColour,
            0x1004503, /*CodeEditorComponent::defaultTextColourId*/               0xff000000,
            0x1004504, /*CodeEditorComponent::lineNumberBackgroundId*/            0x44999999,
            0x1004505, /*CodeEditorComponent::lineNumberTextId*/                  0x44000000,

            0x1007000, /*ColourSelector::backgroundColourId*/                     0xffe5e5e5,
            0x1007001, /*ColourSelector::labelTextColourId*/                      0xff000000,

            0x100ad00, /*KeyMappingEditorComponent::backgroundColourId*/          0x00000000,
            0x100ad01, /*KeyMappingEditorComponent::textColourId*/                0xff000000,

            FileSearchPathListComponent::backgroundColourId,        0xff515151,

            FileChooserDialogBox::titleTextColourId,                0xffffffff,
            FileChooserDialogBox::backgroundColourId,               0xff333333,
            DrawableButton::textColourId,                           0xff000000,
        };

        for (int i = 0; i < numElementsInArray (standardColours); i += 2)
            setColour ((int) standardColours [i], Colour ((uint32) standardColours [i + 1]));
    }

    Style::~Style() { }

    void Style::drawComboBox (Graphics& g, int width, int height,
                                    const bool isButtonDown,
                                    int buttonX, int buttonY,
                                    int buttonW, int buttonH,
                                    ComboBox& box)
    {
        g.fillAll (box.findColour (ComboBox::backgroundColourId));

        if (box.isEnabled() && box.hasKeyboardFocus (false))
        {
            g.setColour (box.findColour (ComboBox::buttonColourId));
            g.drawRect (0, 0, width, height, 2);
        }
        else
        {
            g.setColour (box.findColour (ComboBox::outlineColourId));
            g.drawRect (0, 0, width, height);
        }

        // const float outlineThickness = box.isEnabled() ? (isButtonDown ? 1.2f : 0.5f) : 0.3f;

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

    void Style::drawShinyButtonShape (Graphics& g,
                                            float x, float y, float w, float h,
                                            float maxCornerSize,
                                            const Colour& baseColour,
                                            const float strokeWidth,
                                            const bool flatOnLeft,
                                            const bool flatOnRight,
                                            const bool flatOnTop,
                                            const bool flatOnBottom) noexcept
    {
        if (w <= strokeWidth * 1.1f || h <= strokeWidth * 1.1f)
            return;

        const float cs = jmin (maxCornerSize, w * 0.5f, h * 0.5f);

        Path outline;
        outline.addRoundedRectangle (x, y, w, h, cs, cs,
                                     ! (flatOnLeft  || flatOnTop),
                                     ! (flatOnRight || flatOnTop),
                                     ! (flatOnLeft  || flatOnBottom),
                                     ! (flatOnRight || flatOnBottom));

        ColourGradient cg (baseColour, 0.0f, y,
                           baseColour.overlaidWith (Colour (0x070000ff)), 0.0f, y + h,
                           false);

        cg.addColour (0.5,  baseColour.overlaidWith (Colour (0x33ffffff)));
        cg.addColour (0.51, baseColour.overlaidWith (Colour (0x110000ff)));

        g.setGradientFill (cg);
        g.fillPath (outline);

        g.setColour (Colour (0x80000000));
        g.strokePath (outline, PathStrokeType (strokeWidth));
    }

    void Style::drawPopupMenuBackground (Graphics& g, int width, int height)
    {
        const Colour background (findColour (PopupMenu::backgroundColourId));
        g.setColour (background);
        g.fillRect (0, 0, width, height);

        g.setColour (Colours::black);
        g.drawRect (Rectangle<float> (0, 0, width, height), 0.80f);
    }


    Colour Style::getScrollbarColourForBackground (Colour background)
    {
        return background.contrasting().withAlpha (0.13f);
    }

    Rectangle<int> Style::getPropertyComponentContentPosition (PropertyComponent& component)
    {
        //if (component.findParentComponentOfClass<AppearanceEditor::EditorPanel>() != nullptr)
          //  return component.getLocalBounds().reduced (1).removeFromRight (component.getWidth() / 2);

        return LookAndFeel::getPropertyComponentContentPosition (component);
    }

    int Style::getTabButtonOverlap (int /*tabDepth*/)                      { return -1; }
    int Style::getTabButtonSpaceAroundImage()                              { return 1; }
    int Style::getTabButtonBestWidth (TabBarButton&, int /*tabDepth*/)     { return 120; }

    static void createTabTextLayout (const TabBarButton& button, const Rectangle<int>& textArea,
                                     const Colour colour, TextLayout& textLayout)
    {
        Font font (textArea.getHeight() * 0.5f);
        font.setUnderline (button.hasKeyboardFocus (false));

        AttributedString s;
        s.setJustification (Justification::centred);
        s.append (button.getButtonText().trim(), font, colour);

        textLayout.createLayout (s, (float) textArea.getWidth());
    }

    Colour Style::getTabBackgroundColour (TabBarButton& button)
    {
        const Colour bkg (button.findColour (mainBackgroundColourId).contrasting (0.15f));

        if (button.isFrontTab())
            return bkg.overlaidWith (Colours::orange.withAlpha (0.5f));

        return bkg;
    }

    void Style::drawTabButton (TabBarButton& button, Graphics& g, bool isMouseOver, bool isMouseDown)
    {
        const Rectangle<int> activeArea (button.getActiveArea());

        const Colour bkg (getTabBackgroundColour (button));

        g.setGradientFill (ColourGradient (bkg.brighter (0.1f), 0, (float) activeArea.getY(),
                                           bkg.darker (0.1f), 0, (float) activeArea.getBottom(), false));
        g.fillRect (activeArea);

        g.setColour (button.findColour (mainBackgroundColourId).darker (0.3f));
        g.drawRect (activeArea);

        const float alpha = button.isEnabled() ? ((isMouseOver || isMouseDown) ? 1.0f : 0.8f) : 0.3f;
        const Colour col (bkg.contrasting().withMultipliedAlpha (alpha));

        TextLayout textLayout;
        createTabTextLayout (button, button.getTextArea(), col, textLayout);

        textLayout.draw (g, button.getTextArea().toFloat());
    }

    Rectangle<int> Style::getTabButtonExtraComponentBounds (const TabBarButton& button, Rectangle<int>& textArea, Component& comp)
    {
        TextLayout textLayout;
        createTabTextLayout (button, textArea, Colours::black, textLayout);
        const int textWidth = (int) textLayout.getWidth();
        const int extraSpace = jmax (0, textArea.getWidth() - (textWidth + comp.getWidth())) / 2;

        textArea.removeFromRight (extraSpace);
        textArea.removeFromLeft (extraSpace);
        return textArea.removeFromRight (comp.getWidth());
    }

    void Style::drawStretchableLayoutResizerBar (Graphics& g, int /*w*/, int /*h*/, bool /*isVerticalBar*/, bool isMouseOver, bool isMouseDragging)
    {
        if (isMouseOver || isMouseDragging)
            g.fillAll (Colours::yellow.withAlpha (0.4f));
    }

    void Style::drawScrollbar (Graphics& g, ScrollBar& scrollbar, int x, int y, int width, int height,
                                               bool isScrollbarVertical, int thumbStartPosition, int thumbSize,
                                               bool isMouseOver, bool isMouseDown)
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

    static Range<float> getBrightnessRange (const Image& im)
    {
        float minB = 1.0f, maxB = 0;
        const int w = im.getWidth();
        const int h = im.getHeight();

        for (int y = 0; y < h; ++y)
        {
            for (int x = 0; x < w; ++x)
            {
                const float b = im.getPixelAt (x, y).getBrightness();
                minB = jmin (minB, b);
                maxB = jmax (maxB, b);
            }
        }

        return Range<float> (minB, maxB);
    }


    void Style::drawConcertinaPanelHeader (Graphics& g, const Rectangle<int>& area,
                                                           bool isMouseOver, bool /*isMouseDown*/,
                                                           ConcertinaPanel&, Component& panel)
    {
        const Colour bkg (findColour (mainBackgroundColourId));

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

    void Style::drawButtonBackground (Graphics& g,
                                                      Button& button,
                                                      const Colour& backgroundColour,
                                                      bool isMouseOverButton,
                                                      bool isButtonDown)
    {
        const bool flatOnLeft   = button.isConnectedOnLeft();
        const bool flatOnRight  = button.isConnectedOnRight();
        const bool flatOnTop    = button.isConnectedOnTop();
        const bool flatOnBottom = button.isConnectedOnBottom();

        const float width  = (float) button.getWidth();
        const float height = (float) button.getHeight();

        const float x = 0.5f;
        const float y = 0.5f;
        const float w = width  - 1.0f;
        const float h = height - 1.0f;
        const float cornerSize = 4.0f;

        Colour baseColour (backgroundColour.withMultipliedSaturation (button.hasKeyboardFocus (true)
                                                                          ? 1.3f : 0.9f)
                                           .withMultipliedAlpha (button.isEnabled() ? 0.9f : 0.5f));

        if (isButtonDown)           baseColour = baseColour.contrasting (0.2f);
        else if (isMouseOverButton) baseColour = baseColour.contrasting (0.1f);

        const float mainBrightness = baseColour.getBrightness();
        const float mainAlpha = baseColour.getFloatAlpha();

        Path outline;
        outline.addRoundedRectangle (x, y, w, h, cornerSize, cornerSize,
                                     ! (flatOnLeft  || flatOnTop),
                                     ! (flatOnRight || flatOnTop),
                                     ! (flatOnLeft  || flatOnBottom),
                                     ! (flatOnRight || flatOnBottom));

        g.setGradientFill (ColourGradient (baseColour.brighter (0.2f), 0.0f, 0.0f,
                                           baseColour.darker (0.25f), 0.0f, height, false));
        g.fillPath (outline);

        g.setColour (Colours::white.withAlpha (0.4f * mainAlpha * mainBrightness * mainBrightness));
        g.strokePath (outline, PathStrokeType (1.0f), AffineTransform::translation (0.0f, 1.0f)
                                                            .scaled (1.0f, (h - 1.6f) / h));

        g.setColour (Colours::black.withAlpha (0.4f * mainAlpha));
        g.strokePath (outline, PathStrokeType (1.0f));
    }
}}
