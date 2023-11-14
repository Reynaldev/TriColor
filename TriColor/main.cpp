#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "shader.h"

#define GLSL_VERSION "#version 330 core"
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define APP_TITLE "TriColor"

typedef std::string string;

void frameBufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

string getFilePath(string filename)
{
	string path(__FILE__);
	size_t index = path.rfind("\\") + 1;
	string filePath = path.substr(0, index) + filename;

	return filePath;
}

int main(int argc, char** argv)
{
	// Init
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GLFW_VERSION_MAJOR);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GLFW_VERSION_MINOR);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, APP_TITLE, NULL, NULL);
	if (window == NULL)
	{
		printf("ERROR. Failed to create window.\n");
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		printf("ERROR. Failed to initialize GLAD.\n");
		return -1;
	}

	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(GLSL_VERSION);

	// Triangle
	float vertices[] = {
		// Position				// Color
		 0.0f,  0.5f, 0.0f,		1.0f, 0.0f, 0.0f,		// Head
		-0.5f, -0.5f, 0.0f,		0.0f, 1.0f, 0.0f,		// Left foot
		 0.5f, -0.5f, 0.0f,		0.0f, 0.0f, 1.0f		// Right foot
	};

	// Shaders
	Shader tShader("TriangleVertex.glsl", "TriangleFragment.glsl");

	// Vertex buffers
	GLuint VBO, VAO;
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (GLvoid*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	// Triangle init
	int tColor = glGetUniformLocation(tShader.ID, "uColor");

	// Window config
	bool showConfigWindow = true;

	while (!glfwWindowShouldClose(window))
	{
		static ImVec4 colors = ImVec4(0.2f, 0.3f, 0.1f, 1.0f);

		glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Viewport render
		tShader.use();
		glBindVertexArray(VAO);
		glUniform3f(tColor, colors.x, colors.y, colors.z);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		if (ImGui::BeginMainMenuBar())
		{
			ImGui::EndMainMenuBar();
		}
		
		// UI render
		if (ImGui::Begin("Playground", &showConfigWindow))
		{
			ImGui::SeparatorText("Color Picker");
			ImGui::ColorEdit4("Triangle Color", &colors.x, ImGuiColorEditFlags_NoInputs);

			ImGui::End();
		}

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

end:
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}