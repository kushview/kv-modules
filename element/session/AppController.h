#ifndef ELEMENT_APP_CONTROLLER_HPP
#define ELEMENT_APP_CONTROLLER_HPP

#include "element/Controller.h"

namespace Element {

    class Session;
    class World;

    class AppController :   public Controller
    {
    public:

        AppController (World& world);
        virtual ~AppController();
        World& world();

    private:

        World& elementWorld;

    };

}

#endif  /* ELEMENT_APP_CONTROLLER_HPP */
