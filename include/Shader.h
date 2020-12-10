#define GLEW_STATIC

#include <GL/glew.h>
#include <GL/gl.h>
#include <string>


class Shader {
private:
    std::string ParseShader(const std::string& filepath);
    GLuint CompileShader(GLenum type,const std::string& source);
    GLuint ID;

public:
    Shader(GLenum type,const std::string& filepath);
    ~Shader() = default;
    GLuint  getID();
};


