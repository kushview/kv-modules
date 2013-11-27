#ifndef ELEMENT_APP_CONTROLLER_HPP
#define ELEMENT_APP_CONTROLLER_HPP

    class World;

    class AppController :   public Controller
    {
    public:

        AppController (World& world);
        virtual ~AppController();

    protected:

        World& world();

    private:

        World& elementWorld;

    };


#endif  /* ELEMENT_APP_CONTROLLER_HPP */
