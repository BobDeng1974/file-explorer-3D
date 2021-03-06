#ifndef HANDLE_2048_HPP
#define HANDLE_2048_HPP

#include "myGLWidget.hpp"
#include <vector>
#include "Text.hpp"
#include "Plane.hpp"
#include "String_utils.hpp"

#define TRANS_TIME 0.1f
#define MAX_UNZOOM 1.5f

class Handle_2048 : public myGLWidget
{
  enum {
    NO_MOVE,
    R_MOVE,
    L_MOVE,
    U_MOVE,
    D_MOVE
  };
  typedef struct Tile {
    float         to_x;
    float         to_y;
    float         pos_x;
    float         pos_y;
    float         from_x;
    float         from_y;
    float         m_zoom;
    float         m_unzoom;
    int           value;
    int           x;
    int           y;
    Object::Text  *m_text;
    Object::Plane *m_back;

    Tile(const float &s, const float &x, const float &y) : to_x(0.f), to_y(0.f), pos_x(0.f), pos_y(0.f), from_x(0.f), from_y(0.f),
      m_zoom(1.f), m_unzoom(.99f), value(0), x(x), y(y), m_text(0), m_back(0) {
      m_text = new Object::Text("", BLACK, x, y, s - s / 20, Object::CENTER_ALIGN);
      m_back = new Object::Plane(Vector3D(x, y),
                                 Rotation(), RED, s, s, false);
      m_back->setRender2D(true);
    }
    Tile(int y, int x) : to_x(0.f), to_y(0.f), pos_x(0.f), pos_y(0.f), from_x(0.f), from_y(0.f),
      m_zoom(1.f), m_unzoom(.99f), value(0), x(x), y(y), m_text(0), m_back(0) {}
    ~Tile() {
      if (m_text)
        delete m_text;
      if (m_back)
        delete m_back;
    }
    void initializeGL() {
      if (m_text)
        m_text->initializeGL();
      if (m_back) {
          m_back->initializeGL();
          to_x = pos_x = from_x = m_back->getPosition().x();
          to_y = pos_y = from_y = m_back->getPosition().y();
        }
    }
    void paintGL(const glm::mat4 &view_matrix, const glm::mat4 &proj_matrix) {
      if (!value)
        return;
      if (m_back)
        m_back->paintGL(view_matrix, proj_matrix);
      if (m_text)
        m_text->paintGL(view_matrix, proj_matrix);
    }

    bool  _zoom(float add_value) {
      if (m_zoom < 1.f && add_value > 0.f) {
          std::vector<GLfloat>  &t2 = m_back->getVertices();
          if (m_zoom > 0.f)
            for (unsigned int i = 0; i < t2.size(); i += 2) {
                t2[i] /= m_zoom;
                t2[i + 1] /= m_zoom;
              }
          m_zoom += add_value;
          if (m_zoom > 1.f)
            m_zoom = 1.f;

          for (unsigned int i = 0; i < t2.size(); i += 2) {
              t2[i] *= m_zoom;
              t2[i + 1] *= m_zoom;
            }
          m_back->updateVertices();
          return true;
        }
      return false;
    }
    bool  _unzoom(float sub_value) {
      if (m_unzoom > 1.f && sub_value > 0.f) {
          std::vector<GLfloat>  &t2 = m_back->getVertices();
          if (m_unzoom < MAX_UNZOOM)
            for (unsigned int i = 0; i < t2.size(); i += 2) {
                t2[i] /= m_unzoom;
                t2[i + 1] /= m_unzoom;
              }
          m_unzoom -= sub_value;
          if (m_unzoom < 1.f)
            m_unzoom = 1.f;

          for (unsigned int i = 0; i < t2.size(); i += 2) {
              t2[i] *= m_unzoom;
              t2[i + 1] *= m_unzoom;
            }
          m_back->updateVertices();
          return true;
        }
      return false;
    }
    void  reset_zoom() {
      if (!m_back)
        return;
      float w(m_back->getWidth()), h(m_back->getHeight());
      GLfloat verticesTmp[] = {
        w / -2.f, h / 2.f,
        w / 2.f, h / 2.f,
        w / -2.f, h / -2.f,
        w / 2.f, h / -2.f
      };
      std::vector<GLfloat>  &t = m_back->getVertices();
      for (unsigned int i = 0; i < t.size(); i += 2) {
          t[i] = verticesTmp[i];
          t[i + 1] = verticesTmp[i + 1];
        }
      m_back->updateVertices();
      m_zoom = m_unzoom = 1.f;
    }

    void  update(const float &t, const float &move) {
      if (!value)
        return;
      if (move > 0.f && (to_x != from_x || to_y != from_y)) {
          float tt = move - t;
          if (tt < 0.f)
            tt = t + tt;
          else
            tt = t;
          float tmp = tt / TRANS_TIME;
          float add_x = tmp * (to_x - from_x);
          float add_y = tmp * (to_y - from_y);

          pos_x += add_x;
          pos_y += add_y;

          m_text->setPosition(Vector3D(pos_x, pos_y));
          m_back->setPosition(Vector3D(pos_x, pos_y));

          std::vector<GLfloat>  &t = m_text->getVertices();
          for (unsigned int i = 0; i < t.size(); i += 2) {
              t[i] += add_x;
              t[i + 1] += add_y;
            }
          m_text->updateVertices();
        }

      if (!_zoom(1.f * TRANS_TIME))
        _unzoom(1.f * TRANS_TIME);
      if (m_text)
        m_text->update(t);
      if (m_back)
        m_back->update(t);
    }
    void operator=(Tile &t) {
      pos_x = t.pos_x;
      pos_y = t.pos_y;
      from_x = t.from_x;
      from_y = t.from_y;
      m_zoom = t.m_zoom;
      m_unzoom = t.m_unzoom;

      this->setValue(t.value);

      std::vector<GLfloat>  &v = m_text->getVertices();
      std::vector<GLfloat>  &v2 = t.m_text->getVertices();

      for (unsigned int i = v2.size(); i > 0; --i)
        v[i - 1] = v2[i - 1];

      std::vector<GLfloat>  &vv = m_back->getVertices();
      std::vector<GLfloat>  &vv2 = t.m_back->getVertices();

      for (unsigned int i = vv2.size(); i > 0; --i)
        vv[i - 1] = vv2[i - 1];
      m_back->updateVertices();
      m_text->updateVertices();
    }
    void  setValue(int x) {
      if (x <= 0) {
          value = 0;
          m_zoom = 0.f;
          return;
        }
      if (x == value || x & 1)
        return;
      value = x;
      m_text->setText(Utility::toString<int>(value));
      switch (x) {
        case 2:
          m_back->setColor(Color(0.93f, 0.89f, 0.85f));
          m_text->setColor(GREY);
          break;
        case 4:
          m_back->setColor(Color(0.93f, 0.88f, 0.78f));
          m_text->setColor(GREY);
          break;
        case 8:
          m_back->setColor(Color(0.95f, 0.69f, 0.47f));
          m_text->setColor(WHITE);
          break;
        case 16:
          m_back->setColor(Color(0.96f, 0.58f, 0.39f));
          m_text->setColor(WHITE);
          break;
        case 32:
          m_back->setColor(Color(0.96f, 0.48f, 0.37f));
          m_text->setColor(WHITE);
          break;
        case 64:
          m_back->setColor(Color(0.96f, 0.36f, 0.23f));
          m_text->setColor(WHITE);
          break;
        case 128:
          m_back->setColor(Color(0.93f, 0.81f, 0.45f));
          m_text->setColor(WHITE);
          break;
        case 256:
          m_back->setColor(Color(0.93f, 0.8f, 0.38f));
          m_text->setColor(WHITE);
          break;
        case 512:
          m_back->setColor(Color(0.93f, 0.78f, 0.36f));
          m_text->setColor(WHITE);
          break;
        case 1024:
          m_back->setColor(Color(0.93f, 0.76f, 0.32f));
          m_text->setColor(WHITE);
          break;
        case 2048:
          m_back->setColor(Color(0.93f, 0.75f, 0.29f));
          m_text->setColor(WHITE);
          break;
        default:
          m_back->setColor(Color(0.01f, 0.01f, 0.01f));
          m_text->setColor(WHITE);
          break;
        }
      m_back->updateColors();
      m_text->updateColors();
    }
  } Tile;

public:
  Handle_2048();
  virtual ~Handle_2048();
  void  initializeGL();
  void  keyReleaseEvent(int key);
  void  update(const float &);
  void  paintGL(const glm::mat4 &view_matrix, const glm::mat4 &proj_matrix);

private:
  void  create_back_tile(Vector3D, float size, Color);
  bool  create_new_number();
  void  switchTile(Tile *tile, Tile *tmp, int &z);
  bool  canMove(int y, int x, int value);
  bool  checkTile(Tile*);
  void  restart();
  const float                       tile_s, border_s;
  Object::Text                      *m_score;
  Object::Text                      *m_msg;
  Object::Text                      *m_title;
  bool                              m_end;
  bool                              m_win;
  std::vector<std::vector<Tile*> >  m_tiles;
  std::vector<Tile*>                m_move_tiles;
  float                             m_move;
  char                              m_direction;
};

#endif // HANDLE_2048_HPP
