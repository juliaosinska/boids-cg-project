#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cerrno>
#include <glm.hpp>
#include <gtc/type_ptr.hpp>

std::string get_file_contents(const char* filename);

class Shader
{
public:
    GLuint ID;
    Shader(const char* vertexFile, const char* fragmentFile);

    void Activate();
    void Delete();
 

    // Utility functions for setting uniforms
    void SetMat4(const std::string& name, const glm::mat4& mat) const;
    void SetVec3(const std::string& name, const glm::vec3& value) const;
    void SetInt(const std::string& name, int value) const;

private:
    void compileErrors(unsigned int shader, const char* type);
};

#endif // !SHADER_CLASS_H