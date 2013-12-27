#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "Vector3D.hpp"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#define BUFSIZE 1024
#define NEED_MOUSE_MOVE 15

class MyWindow;
class HandleMutex;

namespace Object
{
  class SkyBox;
}

class Camera
{
public:
  Camera();
  virtual ~Camera();
  void                resize(int w, int h);
  virtual void        mouseMoveEvent(int x, int y);
  virtual void        keyPressEvent(int);
  virtual void        keyReleaseEvent(int);
  virtual void        setSpeed(float s);
  virtual const float &getSpeed() const;
  virtual void        setsensivity(float s);
  virtual void        look();
  virtual void        setPosition(const Vector3D &p);
  const Vector3D      &getPosition() const;
  void                lookAt();
  void                setView(MyWindow *v);
  int                 mouseX() const;
  int                 mouseY() const;

  static glm::mat4 const &getViewMatrix();
  static glm::mat4 const &getProjectionMatrix();
  static glm::mat4 const &get2DProjectionMatrix();
  static glm::vec3       &getVecPosition();
  static float const     &getDistanceView();
  static float const     &getRatio();

private:
  void  vectorsFromAngles();

  float       m_speed;
  float       m_sensivity;
  float       m_phi;
  float       m_theta;
  Vector3D    m_position;
  Vector3D    m_up;
  Vector3D    m_target;
  MyWindow    *m_win;
  int         m_oldX;
  int         m_oldY;
  float       m_y;
  float       tmpSpeed;
  Object::SkyBox      *m_skybox;
  float       r_temp;
  Vector3D    m_forward;
  Vector3D    m_left;
  float       tmp1;
  float       tmp2;
  int         m_curX;
  int         m_curY;
  HandleMutex *m_mutex;

  static float       distanceView;
  static float       m_ratio;
  static glm::vec3   m_vecPos;
  static glm::mat4   m_view;
  static glm::mat4   m_proj;
  static glm::mat4   m_2Dproj;
};

#endif // CAMERA_HPP
