#include "shader.h"

Shader::Shader(string vertexPath, string fragmentPath)
{
	// Retrieve the vertex/fragment source code from filepath.
	string vertexCode, fragmentCode;
	std::ifstream vShaderFile, fShaderFile;

	// Ensure ifstream objects can throw exceptions.
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	// Debug
	//printf("Vertex path: %s\n", vertexPath.c_str());
	//printf("Fragment path: %s\n", fragmentPath.c_str());

	try
	{
		// Open files.
		vShaderFile.open(vertexPath.c_str());
		fShaderFile.open(fragmentPath.c_str());

		std::stringstream vShaderStream, fShaderStream;

		// Read file's contents into streams.
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();

		// Close file handlers.
		vShaderFile.close();
		fShaderFile.close();

		// Convert stream into string.
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (const std::ifstream::failure e)
	{
		printf("Error. Faield to read shader files.\n");
	}

	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	//printf("%s\n", vShaderCode);

	// Compile shader.
	unsigned int vertex, fragment;
	int success;
	char infoLog[512];

	// Vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);

	// Print compile errors if any
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		printf("Error. Vertex shader compilation failed.\n%s\n", infoLog);
	}

	// Fragment shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);

	// Print compile errors if any
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		printf("Error. %s\n", infoLog);
	}

	// Shader program
	this->ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);

	// Print linking errors if any
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		printf("Error. %s\n", infoLog);
	}

	// Delete the shaders as they're linked into our program now and no longer necessary
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void Shader::use()
{
	glUseProgram(this->ID);
}
