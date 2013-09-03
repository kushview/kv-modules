
#include "clip-bin.hpp"
#include "clip-factory.hpp"

namespace element {

    ClipBin::ClipBin (ClipFactory& factory)
        : m_factory (factory)
    {
    }

    void
    ClipBin::recycle()
    {
        m_factory.m_free.transfer (*this);
        assert (this->m_clips.empty());
    }
}
