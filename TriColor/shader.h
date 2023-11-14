#pragma once

#ifndef SHADER_H
#include <glad/glad.h>

#include <iostream>
#include <fstream>
#include <sstream>

typedef std::string string;

class Shader
{
public:
	unsigned int ID;

	Shader(string vertexPath, string fragmentPath);

	void use();
};

#endif // !SHADER_H