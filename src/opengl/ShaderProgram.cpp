#include "onair/okui/opengl/ShaderProgram.h"

namespace onair {
namespace okui {
namespace opengl {

ShaderProgram::ShaderProgram() {
	_program = glCreateProgram();
}

ShaderProgram::~ShaderProgram() {
	glDeleteProgram(_program);
}

void ShaderProgram::attachShader(const Shader& shader) {
	if (!_error.empty()) { return; }
	if (!(_error = shader.error()).empty()) { return; }

	glAttachShader(_program, shader.id());
}

void ShaderProgram::bindAttribute(GLuint id, const char* name) {
	glBindAttribLocation(_program, id, name);
}

GLint ShaderProgram::attribute(const char* name) {
    return glGetAttribLocation(_program, name);
}

void ShaderProgram::link() {
	if (!_error.empty()) { return; }
		
	glLinkProgram(_program);

	GLint linkStatus = GL_FALSE;
	glGetProgramiv(_program, GL_LINK_STATUS, &linkStatus);

	if (!linkStatus) {
		char buf[200];
		glGetProgramInfoLog(_program, sizeof(buf), nullptr, buf);
		_error = buf;
		glDeleteProgram(_program);
		_program = 0;
	}
}

}}}