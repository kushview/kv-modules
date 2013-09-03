#ifndef ELEMENT_SESSION_HPP
#define ELEMENT_SESSION_HPP

#include <list>
#include <string>

#include "element/asset.hpp"
#include "element/atomic.hpp"

namespace element {

class Session
{
public:

    ~Session();

    inline static Session* create() { return new Session(); }

    /** Session Properties */
    struct Properties
    {
        Properties()
            : path ("/"),
              name ("Untitled-0"),
              description ("Session description ...")
        {  }

        std::string path;
        std::string name;
        std::string description;

        inline const char*
        full_path() const
        {
            return "";
        }

        inline Properties&
        operator=(const Properties& other)
        {
            path        = other.path;
            name        = other.name;
            description = other.description;
            return *this;
        }
    };

    inline const Properties& properties() const { return m_props; }
    inline void set_properties (const Properties& props) { m_props = props; }

    /** Realtime lock/unlock methods */
    bool acquire() { return m_lock.acquire(); }
    void release() { m_lock.release(); }

    /** Non-realtime lock/unlock */
    void lock()   { m_lock.lock(); }
    void unlock() { m_lock.unlock(); }

protected:

    Session();

private:

    int m_seq_counter;

    AtomicLock  m_lock;
    Properties  m_props;

    class Private;
    Scoped<Private>            priv;

};

}

#endif /* ELEMENT_SESSION_HPP */
