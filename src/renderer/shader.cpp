#include "shader.hh"

#include "renderer.hh"

#include <fstream>
#include <iostream>
#include <sstream>

Shader::Shader(const std::string &filepath)
    : filepath(filepath), rendererId(0) {
  ShaderSources shaderSources = ParseShader(filepath);
  std::cout << "\n\nSHADER loader with SOURCES:" << std::endl;
  std::cout << "VERTEX:\n" << shaderSources.vertexSource << std::endl;
  std::cout << "FRAGMENT:\n" << shaderSources.fragmentSource << std::endl;

  rendererId = CreateShader(shaderSources.vertexSource, shaderSources.fragmentSource);
}

Shader::~Shader() { glDeleteProgram(rendererId); }

ShaderSources Shader::ParseShader(const std::string &filePath) {
  std::ifstream stream(filePath);
  std::string line;

  enum ShaderType { NONE = -1, VERTEX = 0, FRAGMENT = 1 };
  ShaderType shaderType = ShaderType::NONE;

  std::stringstream ss[2];
  while (getline(stream, line)) {
    if (line.find("#shader") != std::string::npos) {
      if (line.find("vertex") != std::string::npos) {
        // its a vertex shader
        shaderType = ShaderType::VERTEX;
      }
      if (line.find("fragment") != std::string::npos) {
        // its a fragment shader
        shaderType = ShaderType::FRAGMENT;
      }
    } else {
      ss[shaderType] << line << "\n";
    }
  }
  return {ss[0].str(), ss[1].str()};
}

unsigned int Shader::CompileShader(const std::string &source,
                                   unsigned int type) {
  unsigned int id = glCreateShader(type);
  const char *src = source.c_str();
  glShaderSource(id, 1, &src, nullptr);
  glCompileShader(id);

  // debugging the compile status of the shader
  int result;
  glGetShaderiv(id, GL_COMPILE_STATUS, &result);
  if (result == GL_FALSE) {
    int length;
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
    char *message = (char *)alloca(length * sizeof(char));
    glGetShaderInfoLog(id, length, &length, message);
    std::cout << "failed to compile"
              << (type == GL_VERTEX_SHADER ? "vertex" : "fragement") << "shader"
              << std::endl;
    std::cout << message << std::endl;
    glDeleteShader(id);
    return 0;
  }
  return id;
}

// args are the source codes for the shaders, returns a unique id for the
// common shader generated
unsigned int Shader::CreateShader(const std::string &vertexShader,
                                  const std::string &fragmentShader) {
  unsigned int program = glCreateProgram();
  unsigned int vs = CompileShader(vertexShader, GL_VERTEX_SHADER);
  unsigned int fs = CompileShader(fragmentShader, GL_FRAGMENT_SHADER);

  glAttachShader(program, vs);
  glAttachShader(program, fs);
  glLinkProgram(program);
  glValidateProgram(program);
  // glDetachShader
  glDeleteShader(vs);
  glDeleteShader(fs);
  return program;
}

void Shader::Bind() const { glUseProgram(rendererId); }

void Shader::Unbind() const { glUseProgram(0); }

void Shader::SetUniform4f(const std::string &uname, const float *vec) {
  glUniform4f(GetUniformLocation(uname), vec[0], vec[1], vec[2], vec[3]);
}

int Shader::GetUniformLocation(const std::string &uName) {
  int ulocation = glGetUniformLocation(rendererId, uName.c_str());
  if (ulocation == -1) {
    std::cout << "Uniform not found..!" << std::endl;
  }
  return ulocation;
}
