#ifndef CLIPBIN_HPP
#define CLIPBIN_HPP

#include <memory>

#include "element/list.hpp"

#include "clip.hpp"

namespace element {

    class ClipFactory;

    class ClipBin
    {
    public:

        typedef List<Clip>::size_type size_type;

        ClipBin (ClipFactory&);
        ~ClipBin() { recycle(); }

        Clip::iterator       begin()       { return m_clips.begin(); }
        Clip::iterator       end()         { return m_clips.end(); }
        Clip::const_iterator begin() const { return m_clips.begin(); }
        Clip::const_iterator end()   const { return m_clips.end(); }

        inline void
        clear()
        {
            recycle();
        }

        inline bool
        contains (ClipRef clip) const
        {
            int index = m_clips.indexOf (*clip);
            return index >= 0;
        }


        inline void
        insertSorted (ClipRef clip)
        {

            if (m_clips.empty())
            {
                push_back (clip);
                return;
            }

            Clip::const_iterator c = m_clips.begin();
            Clip::const_iterator e = m_clips.end();

            while (c != e)
            {
                const Clip& prev (*c);
                if (prev.frame_start() >= clip->frame_start())
                {
                    break;
                }
                ++c;
            }

            if (! clip->is_linked())
            {
               clip->me = m_clips.insert (c, *clip);
            }
            else
            {
               m_clips.splice (c, clip->container(),
                                  clip->position());
               clip->me = m_clips.iterator_to (*clip);
            }

            clip->list = &m_clips;
            assert (clip->is_linked());
        }

        inline void
        push_back (ClipRef clip)
        {
            if (! clip->is_linked())
            {
                clip->me = m_clips.insert (m_clips.end(), *clip);
            }
            else
            {
                m_clips.splice (m_clips.end(), clip->container(), clip->me);
                clip->me = m_clips.iterator_to (*clip);
            }

            clip->list = &m_clips;
            assert (clip->is_linked());
        }

        inline size_type
        size() const
        {
            return m_clips.size();
        }

        inline void
        transfer (ClipBin& other)
        {
            // TODO: consolodate this into a single loop
            m_clips.splice (m_clips.begin(), other.m_clips);
            for (auto& clip : m_clips)
                clip.list = &m_clips;
        }

#if 0
        iterator find (const Raul::Path& path)
        {
            iterator e = end();

            for (const auto& c : *this)
                if (false)
                    return begin();

            return e;
        }
#endif

    protected:

    private:
        friend class ClipFactory;
        ClipFactory& m_factory;
        List<Clip>   m_clips;
        void recycle();
    };

}


#endif // CLIPBIN_HPP
