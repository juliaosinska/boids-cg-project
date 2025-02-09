#include "ShaderClass.h"
#include<glm.hpp>
#include <gtc/type_ptr.hpp>

// reads a text file and outputs a string with everything in the text file
std::string get_file_contents(const char* filename)
{
    std::ifstream in(filename, std::ios::binary);
    if (in)
    {
        std::string contents;
        in.seekg(0, std::ios::end);
        contents.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&contents[0], contents.size());
        in.close();
        return(contents);
    }
    throw(errno);
}

// constructor that builds the Shader Program from 2 different shaders
Shader::Shader(const char* vertexFile, const char* fragmentFile)
{
    // read vertexFile and fragmentFile and store the strings
    std::string vertexCode = get_file_contents(vertexFile);
    std::string fragmentCode = get_file_contents(fragmentFile);

    // convert the shader source strings into character arrays
    const char* vertexSource = vertexCode.c_str();
    const char* fragmentSource = fragmentCode.c_str();

    // create Vertex Shader Object and get its reference
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    // attach Vertex Shader source to the Vertex Shader Object
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    // compile the Vertex Shader into machine code
    glCompileShader(vertexShader);
    compileErrors(vertexShader, "VERTEX");

    // create Fragment Shader Object and get its reference
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    // attach Fragment Shader source to the Fragment Shader Object
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    // compile the Fragment Shader into machine code
    glCompileShader(fragmentShader);
    compileErrors(fragmentShader, "FRAGMENT");

    // create Shader Program Object and get its reference
    ID = glCreateProgram();
    // attach the Vertex and Fragment Shaders to the Shader Program
    glAttachShader(ID, vertexShader);
    glAttachShader(ID, fragmentShader);
    // wrap-up/link all the shaders together into the Shader Program
    glLinkProgram(ID);
    compileErrors(ID, "PROGRAM");

    // delete the now useless Vertex and Fragment Shader objects
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

// activates the Shader Program
void Shader::Activate()
{
    glUseProgram(ID);
}

// deletes the Shader Program
void Shader::Delete()
{
    glDeleteProgram(ID);
}

// checks if the different Shaders have compiled properly
void Shader::compileErrors(unsigned int shader, const char* type)
{
    // stores status of compilation
    GLint hasCompiled;
    // character array to store error message in
    char infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
        if (hasCompiled == GL_FALSE)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "SHADER_COMPILATION_ERROR for:" << type << "\n" << infoLog << std::endl;
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &hasCompiled);
        if (hasCompiled == GL_FALSE)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "SHADER_LINKING_ERROR for:" << type << "\n" << infoLog << std::endl;
        }
    }
}

// utility function to set a mat4 uniform
void Shader::SetMat4(const std::string& name, const glm::mat4& mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

// utility function to set a vec3 uniform
void Shader::SetVec3(const std::string& name, const glm::vec3& value) const
{
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
}

// utility function to set an int uniform
void Shader::SetInt(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
