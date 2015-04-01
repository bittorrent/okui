#include "bittorrent/ui/opengl/Shader.h"

namespace bittorrent {
namespace ui {
namespace opengl {

Shader::Shader(const char* source, Shader::Type type) {
	auto shader = glCreateShader(type == kVertexShader ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER);
	const GLchar* sourcePointer = source;
	glShaderSource(shader, 1, &sourcePointer, nullptr);
	glCompileShader(shader);

	GLint compileStatus = GL_FALSE;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);

	if (!compileStatus) {
		char buf[200];
		glGetShaderInfoLog(shader, sizeof(buf), nullptr, buf);
		_error = buf;
		glDeleteShader(shader);
	} else {
		_shader = shader;
	}
}

Shader::~Shader() {
	glDeleteShader(_shader);
}

}}}