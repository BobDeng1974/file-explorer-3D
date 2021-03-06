#ifndef MYGLWIDGET_H
#define MYGLWIDGET_H

#include "GL/glew.h"
#include "Rotation.hpp"
#include "Color.hpp"
#include "Vector3D.hpp"
#include "Texture.hpp"

#include "MyWindow.hpp"

#include <iostream>
#include <string>
#include <vector>
#include "shaders/Shader.hpp"
#include "../glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

using namespace std;

#define BUFFER_OFFSET(offset) ((char*)NULL + (offset))
#ifndef M_PI
#define M_PI 3.1415926535897932385
#endif

enum {
  VERTEX_COORD,
  COLOR_COORD,
  TEXTURE_COORD,
  NORMAL_COORD
};

namespace Object {
  typedef enum {
    LEFT_ALIGN,
    CENTER_ALIGN,
    RIGHT_ALIGN
  } OBJECT_ALIGN;
}

class myGLWidget
{
public:
  explicit myGLWidget(Vector3D, Rotation);
  explicit myGLWidget(Vector3D, Rotation, Color);
  explicit myGLWidget(Vector3D, Rotation, const std::string texture);
  explicit myGLWidget(Vector3D, Rotation, Texture const &tex);
  virtual ~myGLWidget();
  virtual void    initializeGL() = 0;
  virtual void    paintGL(const glm::mat4& view_matrix, const glm::mat4& proj_matrix) = 0;

  //deprecated
  virtual void    initializeGLNoList();

  virtual bool    loadTexture();
  virtual void    update(float const&);
  bool            hasTexture() const;
  virtual void    setTexture(const std::string);
  virtual void    setTexture(GLuint);
  virtual void    setTexture(Texture const &);
  GLuint          getTextureId() const;
  void            setColor(Color);
  Color const     &getColor() const;
  Color const     &getPikingColor() const;
  void            setPikingColor(Color);
  virtual Vector3D        &getPosition();
  virtual void            setPosition(Vector3D);
  virtual void    resize(int width, int height);
  virtual void    setSelected(bool);
  bool            isSelected() const;
  virtual void    pick(const glm::mat4& view_matrix, const glm::mat4& proj_matrix);
  void            setPickingAllowed(bool);
  bool            isPickingAllowed();
  virtual bool    hasBeenPicked(int);
  virtual int     objectSize();
  virtual void    setMainWindow(MyWindow*);
  MyWindow        *getMainWindow() const;
  Rotation        &rotation();
  void            setRotation(Rotation);
  Texture         &getTexture();
  void            setRender2D(bool);
  bool            is2D() const;
  std::vector<GLfloat>        &getColors();
  virtual void                updateColors(std::vector<GLfloat> const&);
  virtual void                updateColors();
  std::vector<GLfloat>        &getVertices();
  virtual void                updateVertices(std::vector<GLfloat> const&);
  virtual void                updateVertices();
  std::vector<GLfloat>        &getTextures();
  virtual void                updateTextures(std::vector<GLfloat> const&);
  virtual void                updateTextures();
  std::string const &getTextureName() const;
  virtual void    initVertexBufferObject(GLenum = GL_STATIC_DRAW);
  virtual void    bindVertexBufferObject();
  virtual void    updateVertexBufferObject(void *data, int byteSize, int shift);
  virtual void    initVertexArrayObject();
  std::string const &getClassName() const;
  void            setDrawMode(GLenum);
  GLenum          drawMode() const;
  void            setShader(Shader*);
  Shader          *getShader();

protected:
  void        convertTRIANGLE_STRIP_To_TRIANGLES(std::vector<Vector3D> const &tmp_vertices, std::vector<Vector3D> const &tmp_textures);
  void        convertTRIANGLE_STRIP_To_TRIANGLES(std::vector<Vector3D> const &tmp_vertices);
  bool                    m_hasTexture;
  Vector3D                m_pos;
  Rotation                m_rot;
  Color                   m_color;
  bool                    m_selected;
  bool                    m_pickAllow;
  MyWindow                *mainWindow;
  Shader                  *m_shader;
  GLuint                  m_vboID;
  GLuint                  m_vaoID;
  GLuint                  m_normalsSize;
  GLuint                  m_verticesSize;
  GLuint                  m_colorsSize;
  GLuint                  m_texturesSize;
  GLuint                  m_pointsNumber;
  bool                    m_render2D;
  Texture                 m_texture;
  GLenum                  m_drawMode;
  GLint                   m_uniLoc_modelView;
  GLint                   m_uniLoc_projection;
  GLint                   m_uniloc_rot;
  GLint                   m_uniloc_pos;
  Object::OBJECT_ALIGN    m_alignment;
  Color                   m_pickingColor;

  std::string           m_className;
  std::vector<GLfloat>  m_vertices;
  std::vector<GLfloat>  m_colors;
  std::vector<GLfloat>  m_textures;
  std::vector<GLfloat>  m_normals;

  //static Color  pick_color;
  //static Color  &getStaticPickColor();
};

#endif // MYGLWIDGET_H
