#ifndef MODEL_HPP
#define MODEL_HPP

#include "../myGLWidget.hpp"

namespace Object
{
  class Model : public myGLWidget
  {
  public:
    Model(Vector3D, Rotation, std::string model, std::string mtlFileName, float height = 2.f);
    Model(Vector3D, Rotation, const char *model, const char *mtlFileName, float height = 2.f);
    virtual   ~Model();
    void      initializeGL();
    void      paintGL(const glm::mat4 &view_matrix, const glm::mat4 &proj_matrix);
    virtual std::string getClassName() const;

  protected:
    bool  loadMaterial();

    std::string modelName;
    std::string mtlName;
    float       m_height;
  };
}

#endif // MODEL_HPP
