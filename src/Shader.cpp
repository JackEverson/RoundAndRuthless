#include <ostream>
#include "Shader.hpp"
#include "Renderer.hpp"

unsigned int m_RendererID;
Shader::Shader(const std::string& filepath)
    : m_RendererID(0), m_FilePath(filepath) 

{
    ShaderSource shaders = ParseShader(m_FilePath);
    m_RendererID = CompileShaders(shaders.VertexSource, shaders.FragmentSource);
} 

Shader::~Shader()
{
    GLCall(glDeleteProgram(m_RendererID));
}

ShaderSource Shader::ParseShader(std::string& filepath){
    std::ifstream stream(filepath);

    enum class ShaderType{
        NONE = -1,
        VERTEX = 0,
        FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;

    while(getline(stream, line)){

        if (line.find("#shader") != std::string::npos){
            if (line.find("vertex") != std::string::npos){
                type = ShaderType::VERTEX;
            }
            else if (line.find("fragment") != std::string::npos){
                type = ShaderType::FRAGMENT;
            }
        }
        else {
            ss[(int)type] << line << '\n';
        }
    }

    // print the shader source
     std::cout << ss[0].str() << std::endl;
     std::cout << ss[1].str() << std::endl;

    return {ss[0].str(), ss[1].str()};
}

unsigned int Shader::CompileShaders(std::string vertexshaderstring, std::string fragmentshaderstring){
    const char* VertexShaderSource = vertexshaderstring.c_str();
    const char* FragmentShaderSource = fragmentshaderstring.c_str();

    int success;
    char infoLog[512];

    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    GLCall(glShaderSource(vertexShader, 1, &VertexShaderSource, NULL));
    GLCall(glCompileShader(vertexShader));
    GLCall(glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success));
    if (!success){
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        throw std::runtime_error("Vertex shader failed to compile");
    }

    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    GLCall(glShaderSource(fragmentShader, 1, &FragmentShaderSource, NULL));
    GLCall(glCompileShader(fragmentShader));
    GLCall(glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success));
    if (!success){
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        throw std::runtime_error("Fragment shader failed to compile");
    }
    
    unsigned int shaderProgram;
    GLCall(shaderProgram = glCreateProgram());
    GLCall(glAttachShader(shaderProgram, vertexShader));
    GLCall(glAttachShader(shaderProgram, fragmentShader));
    GLCall(glLinkProgram(shaderProgram));
    glad_glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success){
        glad_glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        throw std::runtime_error("Failed to link Shader program");
    }

    GLCall(glUseProgram(shaderProgram));
    GLCall(glDeleteShader(vertexShader));
    GLCall(glDeleteShader(fragmentShader));

    return shaderProgram;
}

void Shader::Bind() const{
    GLCall(glUseProgram(m_RendererID));
}

void Shader::Unbind() const{
    GLCall(glUseProgram(0));
}

void Shader::SetUniform1i(const std::string& name, int i){
    GLCall(glUniform1i(GetUniformLocation(name), i));
}

void Shader::SetUniform1f(const std::string& name, float f){
    GLCall(glUniform1f(GetUniformLocation(name), f));
}

void Shader::SetUniform3f(const std::string& name, float v0, float v1, float v2){
    GLCall(glUniform3f(GetUniformLocation(name), v0, v1, v2));
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3){
    GLCall(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3));
}

unsigned int Shader::GetUniformLocation(const std::string& name){
    if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end()){
        return m_UniformLocationCache[name];
    }

    GLCall(int location = glGetUniformLocation(m_RendererID, name.c_str()));
    if (location == -1){
        std::cout << "Warning: uniform " << name << " does not exist" << std::endl;
    }
    m_UniformLocationCache[name] = location;
    return location;
}

