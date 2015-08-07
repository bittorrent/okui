#pragma once

#include "onair/okui/config.h"

#include "onair/okui/opengl/Shader.h"

namespace onair {
namespace okui {
namespace opengl {

class ShaderProgram {
public:
	ShaderProgram();
	~ShaderProgram();

	void attachShader(const Shader& shader);

	inline void attachShaders() {}

	template <typename T, typename... Rem>
	void attachShaders(T&& shader, Rem&&... rem) {
		if (!_error.empty()) { return; }
		attachShader(std::forward<T>(shader));
		attachShaders(std::forward<Rem>(rem)...);
	}
	
	void bindAttribute(GLuint id, const char* name);
	
	void link();
	
	const std::string& error() const { return _error; } 
	
	GLuint id() const { return _program; }
	void use() const { glUseProgram(_program); }

	struct Uniform {
		Uniform() {}
		explicit Uniform(GLint location) : location(location) {}
		void operator=(GLint n) { glUniform1i(location, n); }
		void operator=(GLfloat f) { glUniform1f(location, f); }
		void setMatrix4(const GLfloat* m, size_t count = 1) { glUniformMatrix4fv(location, static_cast<GLsizei>(count), GL_FALSE, m); }

		GLint location = GL_INVALID_VALUE;
	};
	
	Uniform uniform(const char* name) { return Uniform(glGetUniformLocation(_program, name)); }

private:
	std::string _error;
	GLuint _program = 0;
};

}}}
