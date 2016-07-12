#pragma once

#include "onair/okui/config.h"

#include "onair/okui/opengl/OpenGL.h"

#include <string>

namespace onair {
namespace okui {
namespace opengl {

class Shader {
public:
	enum Type {
		kVertexShader,
		kFragmentShader,
	};

	Shader(const char* source, Type type);
	~Shader();
	
	const std::string& error() const { return _error; }

	GLuint id() const { return _shader; }

private:
	std::string _error;
	GLuint _shader = 0;
};

}}}
