

#include "element/gui/timeline.hpp"

namespace element {
namespace gui {

      class Timeline::Indicator : public Component
      {
      public:

         Indicator (Timeline& parent)
         : timeline (parent)
         {
            color = Colours::white;
            setSize (3, timeline.getHeight());
            setMouseCursor (MouseCursor::LeftRightResizeCursor);
            pos = parent.getBegin() + 20;
         }

         virtual ~Indicator() { }

         void paint (Graphics& g)
         {
            g.setColour (Colours::black);
            g.setOpacity (0.30);
            g.drawVerticalLine (0, 0, getHeight());
            g.drawVerticalLine (2, 0, getHeight());

            g.setColour (color);
            g.setOpacity (0.98);
            g.drawVerticalLine (1, 0, getHeight());
         }


         void mouseDown (const MouseEvent& ev)
         {
            dragger.startDraggingComponent (this, ev);
         }

         void mouseDrag (const MouseEvent& ev)
         {
            dragger.dragComponent (this, ev, nullptr);
            setBounds (getX(), 0, 3, timeline.getHeight());
            pos = getUnits();
         }

         void parentSizeChanged()
         {
            setSize (3, timeline.getHeight());
         }

         double getPosition() const { return pos; }

      private:
         Timeline&        timeline;
         ComponentDragger dragger;
         Colour           color;

         double pos;

         double getUnits() const
         {
            return timeline.xToTime (getBoundsInParent().getX() + 1 - timeline.trackWidth);
         }
      };

      
      TimelineHeader::TimelineHeader (Timeline& parent)
      : owner (parent)
      {
         
      }
      
      TimelineHeader::~TimelineHeader()
      {
      }
      
      void TimelineHeader::paint (Graphics& g)
      {
         g.fillAll (Colour(0xff595959));
         paintDivider (g);
         paintTicks (g);
         
         g.setColour (Colour (0xff454545));
         g.drawHorizontalLine (getHeight() - 1, 0, getWidth());
      }
      
      void TimelineHeader::resized()
      {
         middle = getHeight() / 2;
      }
      
      void TimelineHeader::paintDivider (Graphics& g)
      {
         Colour dark  (0xff555555);
         Colour light (0xff626262);
         
         g.setColour (dark);
         g.drawHorizontalLine ((getHeight() / 2) - 2, 0, getWidth());
         g.setColour (light);
         g.drawHorizontalLine ((getHeight() / 2) - 1, 0, getWidth());
      }
      
      void TimelineHeader::paintTicks (Graphics& g)
      {
         g.setColour (Colours::whitesmoke);
         g.setOpacity (0.50);
         int div = 0;
         
         int divs  = 4;
         int ppd = 100 / divs;
         
         int size = 1;
         
         double iter = floor (owner.getOffset());
         int x = 0;
         int w = getWidth();
         while (x < w)
         {
            x = owner.timeToX (iter);
            
            g.setColour (Colours::whitesmoke);
            g.setOpacity(0.60);
            
            String s; s << iter ;
            g.drawSingleLineText (s, x, 20, Justification::left);
            
            // if (iter - floor (iter) == 0)
            g.drawVerticalLine (x, 0, getHeight());
            //   else
            //      g.drawVerticalLine (x, getHeight() - (getHeight() / 6), getHeight());
            
            iter += owner.getMajorTickSize();
         }
         
#if 0
         for (int x = 0; x < getWidth(); x += 100)
         {
            g.drawVerticalLine (x, 0, getHeight());
            
            for (div = 1; div < divs; ++div)
            {
               g.drawVerticalLine (x + (div * ppd), getHeight() - 10, getHeight());
            }
            
         }
#endif
         
      }
      
      
      TimelineBody::TimelineBody (Timeline& parent) : owner(parent) { }
      TimelineBody::~TimelineBody() { }
      void TimelineBody::paint (Graphics& g)
      {
         int div = 0;
         int x = 0;
         
         double iter = floor (owner.getOffset());
         
         while (x < getWidth())
         {
            x = floor (owner.timeToX (iter));
#if 1
            // lines
            if (div == 0)
            {
#if 1
               g.setColour (Colours::whitesmoke);
               g.setOpacity(0.60);
               String s; s << iter ;
               g.drawSingleLineText (s, x, 100,Justification::left);
#endif
               
               // major line
               g.setColour (Colour (0xff404040));
            }
            else
            {
               // minor line
               g.setColour (Colour (0xff515151));
            }
            
#endif
            
            g.drawVerticalLine (x, 0.0, getHeight());
            
            
            ++div;
            
            if (div == 4)
               div = 0;
            
            iter += owner.getMajorTickSize();
         }
      }
      
      Timeline::Timeline()
        : vscroll (true), hscroll (false),
      header (*this), body (*this), zoom("zoom", "Zoom")
      {
         ind = new Timeline::Indicator (*this);
         
         begin = 1.0;
         duration = 10 * 60;
         offset = 1.0;
         
         testItem = new Timeline::TestItem (*this);
         testItem->setSize (100, 60);
         addAndMakeVisible (testItem);
         
         addAndMakeVisible (&header);
         addAndMakeVisible (&body);
         
         addAndMakeVisible (&vscroll);
         addAndMakeVisible (&hscroll);
         this->setBufferedToImage (true);
         hscroll.addListener (this);
         
         addAndMakeVisible (ind);
         ind->setTopLeftPosition (200, 0);
         ind->toFront (false);
         
         trackWidth = 100;
         scrollBarWidth = 12;
         
         zoom.set (1600.0 / duration, 1600, 600);
         Slider * s = new Slider();
         s->setBounds (0, 100, 400, 20);
         s->setRange (zoom.min(), zoom.max(), 0);
         s->setValue (zoom.value());
         s->setSkewFactor (1);
         pixPerUnit = zoom.logarithmic();
         s->addListener (this);
         addAndMakeVisible (s);
      }
      
      Timeline::~Timeline()
      {
         delete ind;
      }
      
      void
      Timeline::paint (Graphics& g)
      {
          g.setColour (Colour (0xff606060));
          g.fillAll();
      }
      
      void
      Timeline::resized()
      {
         const int w = getWidth();
         const int h = getHeight();
         
         header.setBounds (trackWidth, 0, w - scrollBarWidth - trackWidth, 40);
         int headerHeight = header.getHeight();
         
         body.setBounds (trackWidth, headerHeight, w - scrollBarWidth - trackWidth, h - scrollBarWidth - headerHeight);
         vscroll.setBounds (w - scrollBarWidth, headerHeight, scrollBarWidth, h - scrollBarWidth - headerHeight);
         vscroll.setRangeLimits (0,1);
         vscroll.setCurrentRange (0, .999999);
         
         hscroll.setBounds (body.getBounds().getX(), h - scrollBarWidth, body.getWidth(), scrollBarWidth);
         hscroll.setRangeLimits (begin, begin + duration);
         
         //std::cout << "Range: " << hscroll.getCurrentRangeStart() << std::endl;
      }
      
      void Timeline::scrollBarMoved (ScrollBar* scrollBarThatHasMoved,
                                     double newRangeStart)
      {
         offset = newRangeStart; //(newRangeStart * duration) - begin;
         ind->setBounds (timeToX (ind->getPosition()) + trackWidth, 0, 3, getHeight());
         
         repaint();
      }
      
      void Timeline::sliderValueChanged (Slider* slider)
      {
         zoom.set (slider->getValue());
         pixPerUnit = zoom.logarithmic(); //zoom.logarithmic();
         
         double range = xToTime (body.getWidth()) - offset;
         offset = ind->getPosition() - (range / 2);
         
         if (offset < begin)
            offset = begin;
         else if  (xToTime(body.getWidth()) > begin + duration) {
            offset -=  (xToTime(body.getWidth()) - (begin + duration));
            if (offset < begin) offset = begin;
         }
         
         
         // Locate the indicator
         int x = timeToX (ind->getPosition()) + trackWidth;
         ind->setBounds (x, 0, 3, getHeight());
         
         testItem->parentSizeChanged();
         testItem->resized();
         
         repaint();
      }
      
      double Timeline::getMajorTickSize()
      {
         // this should be more efficient !
         
         double tickSize = 0;
         double ppt = 0;
         
         while (ppt <= 120)
         {
            if (tickSize < 0.125)
               tickSize += 0.125;
            else if (tickSize < 1.0)
               tickSize += 0.25;
            else if (tickSize < 2.0)
               tickSize += 0.50;
            else if (tickSize < 4.0)
               tickSize += 1.0;
            else if (tickSize < 8.0)
               tickSize += 2.0;
            else if (tickSize < 16.0)
               tickSize += 4;
            else if (tickSize < 32.0)
               tickSize += 8;
            else if (tickSize < 64.0)
               tickSize += 16;
            else if (tickSize < 128.0)
               tickSize += 32;
            else if (tickSize < 256.0)
               tickSize += 64;
            else if (tickSize < 512.0)
               tickSize += 128;
            else
               tickSize += 256;
            
            ppt = pixPerUnit * tickSize;
         }
         
         return tickSize;
      }
      
      double
      Timeline::xToTime (int x)
      {
         return ((double)x / (double) pixPerUnit) + offset;
      }
      
      int
      Timeline::timeToX (double time)
      {
         return ((time - offset) * pixPerUnit);
      }
      
      double
      Timeline::getTime()
      {
        return 0.0f;
      }
      
   }}
