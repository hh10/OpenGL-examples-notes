#pragma once

#include <string>

struct ShaderSources {
  const std::string vertexSource;
  const std::string fragmentSource;
};

class Shader {
private:
  std::string filepath;
  unsigned int rendererId;
  unsigned int CompileShader(const std::string &source, unsigned int type);
  ShaderSources ParseShader(const std::string &filepath);
  unsigned int CreateShader(const std::string &vertexShader,
                            const std::string &fragmentShader);

public:
  Shader(const std::string &filepath);
  ~Shader();

  void Bind() const;
  void Unbind() const;

  void SetUniform4f(const std::string &uname, const float *vec);
  int GetUniformLocation(const std::string &uName);
};