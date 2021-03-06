#ifndef TEXT_HPP
#define TEXT_HPP

#include <string>
#include "../myGLWidget.hpp"

namespace Object
{
  class Text : public myGLWidget
  {
  public:
    Text(std::string text, Color = Color(), float x = 0.f, float y = 0.f, float font_size = 0.2f, OBJECT_ALIGN align = LEFT_ALIGN);
    Text(const char *text, Color = Color(), float x = 0.f, float y = 0.f, float font_size = 0.2f, OBJECT_ALIGN align = LEFT_ALIGN);
    Text(std::string text, Color = Color(), Vector3D = Vector3D(), Rotation = Rotation(), float font_size = 0.2f);
    Text(const char *text, Color = Color(), Vector3D = Vector3D(), Rotation = Rotation(), float font_size = 0.2f);
    Text(Texture const &s, std::string text, Vector3D v, Rotation = Rotation(), float font_size = 0.2f, OBJECT_ALIGN align = LEFT_ALIGN);
    Text(Texture const &s, float length, int lines, Vector3D v, Rotation = Rotation(), float font_size = 0.2f, OBJECT_ALIGN align = LEFT_ALIGN);
    Text(Texture const &s, std::string text, float x = 0.f, float y = 0.f, float font_size = 0.2f, OBJECT_ALIGN align = LEFT_ALIGN);
    Text(Texture const &s, float length, int lines, float x = 0.f, float y = 0.f, float font_size = 0.2f, OBJECT_ALIGN align = LEFT_ALIGN);
    virtual ~Text();
    virtual void    initializeGL();
    virtual void    paintGL(const glm::mat4& view_matrix, const glm::mat4& proj_matrix);
    virtual void    setTexture(const Texture &);
    virtual void    updateColors();
    void            setText(std::string);
    void            setText(const char*);
    std::string const &getText() const;
    GLfloat const   &getSize() const;

  protected:
    void        fill2DVertices(bool recalc = false);
    std::string m_text;
    float       m_font_size;
    int         m_lines;
    float       m_size;
  };
}

#endif // TEXT_HPP
