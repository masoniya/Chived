#pragma once

#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>


class ShaderProgram
{
public:

	void compileShaders(const char* vertPath, const char* fragPath);
	void compileShaders(const char* vertPath, const char* geomPath, const char* fragPath);
	void useProgram();

	void setUniformInt(const char* name, int value);
	void setUniformInt(std::string name, int value);

	void setUniformFloat(const char* name, float value);
	void setUniformFloat(std::string name, float value);

	void setUniformVec3(const char* name, glm::vec3 value);
	void setUniformVec3(const char* name, const float *value);

	void setUniformMat4(const char* name, glm::mat4 value);
	void setUniformMat4(const char* name, const float *value);

private:

	uint32_t shaderProgram;

	void compileShader(const char* path, uint32_t shader);

};
