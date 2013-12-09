#ifndef LOADINGMENU_HPP
#define LOADINGMENU_HPP

#include <string>
#include <GL/glew.h>

class MyWindow;
class GraphicHandler;
class myGLWidget;

class LoadingMenu
{
public:
    LoadingMenu(int nbObject, MyWindow*);
    virtual ~LoadingMenu();
    void    newLoadedObject(GraphicHandler *);
    void    createTextTexture(const char*, int i);

private:
    MyWindow    *m_win;
    myGLWidget  *m_widget1;
    myGLWidget  *m_widget2;
    myGLWidget  *m_widget3;
    int         pourcent;
    int         obj;
    int         nbObject;
    GLuint      m_texture[101];
    std::string m_text;
};

#endif // LOADINGMENU_HPP
