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

	// Buffers
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

	// Triangle config
	// Triangle color 
	ImVec4 colors = ImVec4(1.0f, 0.3f, 0.1f, 1.0f);
	// Triangle position
	float position[] = { 0.0f, 0.0f, 0.0f };

	// Uniform location for "uColor"
	int tColor = glGetUniformLocation(tShader.ID, "uColor");
	// Uniform location for "uPos"
	int tPos = glGetUniformLocation(tShader.ID, "uPos");

	// ImGui config
	// Enable/Disable demo window, self-explanatory
	bool showDemoWindow = false;
	// Enable/Disable triangle configuration window
	bool showConfigWindow = true;
	// Enable/Disable triangle color animation cycle
	bool enableTriangleColorAnim = false;

	while (!glfwWindowShouldClose(window))
	{
		float timeVal = (float)glfwGetTime();

		// Input
		if (GLFW_PRESS == (glfwGetKey(window, GLFW_KEY_W) | glfwGetKey(window, GLFW_KEY_UP)))
		{
			position[1] += 0.01f;
		}
		if (GLFW_PRESS == (glfwGetKey(window, GLFW_KEY_A) | glfwGetKey(window, GLFW_KEY_LEFT)))
		{
			position[0] -= 0.01f;
		}
		if (GLFW_PRESS == (glfwGetKey(window, GLFW_KEY_S) | glfwGetKey(window, GLFW_KEY_DOWN)))
		{
			position[1] -= 0.01f;
		}
		if (GLFW_PRESS == (glfwGetKey(window, GLFW_KEY_D) | glfwGetKey(window, GLFW_KEY_RIGHT)))
		{
			position[0] += 0.01f;
		}

		glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// UI
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// Main menu bar
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Exit"))
					break;

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Tools"))
			{
				ImGui::MenuItem("Triangle Config", NULL, &showConfigWindow);
				ImGui::Separator();
				ImGui::MenuItem("Demo window", NULL, &showDemoWindow);

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}

		/* Show/Unshow window for triangle configuration */
		if (showConfigWindow)
		{
			ImGui::Begin("Triangle Config", &showConfigWindow);

			// Color picker
			if (ImGui::CollapsingHeader("Color"))
			{
				ImGuiColorEditFlags tColorPickerFlags;
				tColorPickerFlags |= ImGuiColorEditFlags_DisplayRGB;
				tColorPickerFlags |= ImGuiColorEditFlags_DisplayHSV;
				tColorPickerFlags |= ImGuiColorEditFlags_PickerHueWheel;
				tColorPickerFlags |= ImGuiColorEditFlags_NoSidePreview;

				ImGui::ColorPicker3("Triangle Color", (float*)&colors, tColorPickerFlags);

				ImGui::Checkbox("Animate", &enableTriangleColorAnim);
				ImGui::SetItemTooltip("Enable/Disable a wave color animation on the triangle");
				if (enableTriangleColorAnim)
				{
					ImGui::PlotHistogram("RGB Value", (float*)&colors, 3, 
						0, NULL, 0.0f, 1.0f, ImVec2(0, 75.0f));
				}
			}

			// Triangle position
			if (ImGui::CollapsingHeader("Position"))
			{
				ImGui::Text("You can move the triangle by pressing WASD/Arrow keys.");

				if (ImGui::ArrowButton("Left", ImGuiDir_Left)) 
				{
					position[0] -= 0.01f;
				}
				ImGui::SameLine();
				if (ImGui::ArrowButton("Up", ImGuiDir_Up)) 
				{
					position[1] += 0.01f;
				}
				ImGui::SameLine();
				if (ImGui::ArrowButton("Down", ImGuiDir_Down)) 
				{
					position[1] -= 0.01f;
				}
				ImGui::SameLine();
				if (ImGui::ArrowButton("Right", ImGuiDir_Right)) 
				{
					position[0] += 0.01f;
				}

				if (ImGui::Button("Reset", ImVec2(50, 25)))
				{
					position[0] = 0.0f;
					position[1] = 0.0f;
				}
			}
				
			ImGui::End();
		}

		// Demo window
		if (showDemoWindow)
			ImGui::ShowDemoWindow(&showDemoWindow);

		// If enabled, Calculate RGB color to make a rainbow wave color
		if (enableTriangleColorAnim)
		{
			colors.x = (cos(timeVal) / 2.0f) + 0.5f;
			colors.y = (sin(timeVal) / 2.0f) + 0.5f;
			colors.z = (sin(timeVal + 30.0f) / 2.0f) + 0.5f;
		}

		// Viewport
		tShader.use();
		glBindVertexArray(VAO);
		glUniform3f(tColor, colors.x, colors.y, colors.z);
		glUniform3f(tPos, position[0], position[1], 0.0f);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);

		// Render
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

end:
	printf("Exiting TriColor");

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}