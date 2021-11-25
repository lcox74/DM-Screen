#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "shader.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <stdlib.h>
#include <stdio.h>
#include <string>

struct PlayerProperties {
    const char *name;
    float x, y, range;
    glm::vec3 colour;

    PlayerProperties(const char *name, float range, glm::vec3 colour) {
        this->name = name;
        this->range = range;
        this->colour = colour;
        this->x = 0.0f;
        this->y = 0.0f;
    }

    void renderProperties(Shader shader) {
        std::string range_tag = this->name;         range_tag.append("_range");
        std::string position_tag = this->name;      position_tag.append("_pos");
        std::string col_tag = this->name;      position_tag.append("_col");
        
        shader.setFloat(range_tag, this->range);
        shader.setVec2(position_tag, this->x, this->y);
        shader.setVec3(col_tag, colour.x, colour.y, colour.z);
    }
};

void init_map_image(unsigned int *, unsigned int *, unsigned int *, unsigned int *);

void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    //glfwMakeContextCurrent(window);
    //glViewport(0, 0, width, height);
}

void initialise_glfw() {
    glfwSetErrorCallback(error_callback);

    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialise GLFW");
        exit(1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

void create_dm_view(GLFWwindow** window) {
    (*window) = glfwCreateWindow(640, 480, "DM View", NULL, NULL);
    if (!(*window)) {
        glfwTerminate();
        fprintf(stderr, "Failed to create DM Window");
        exit(1);
    }
    glfwMakeContextCurrent(*window);
    glfwSetFramebufferSizeCallback(*window, framebuffer_size_callback);
}

void create_game_view(GLFWwindow** window) {
    (*window) = glfwCreateWindow(640, 480, "Game View", NULL, NULL);
    if (!(*window)) {
        glfwTerminate();
        fprintf(stderr, "Failed to create Game Window");
        exit(1);
    }
    glfwMakeContextCurrent(*window);
    glfwSetFramebufferSizeCallback(*window, framebuffer_size_callback);
}

int main(int argc, char* argv[]) {
    GLFWwindow *dm_window, *game_window;
    const char *glsl_version = "#version 330";

    initialise_glfw();
    
    create_dm_view(&dm_window);
    create_game_view(&game_window);

    glfwMakeContextCurrent(dm_window);
    gladLoadGL(glfwGetProcAddress);
    glfwSwapInterval(1);

    // Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(dm_window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

    glfwMakeContextCurrent(game_window);
    gladLoadGL(glfwGetProcAddress);
    glfwSwapInterval(1);

    Shader ourShader("shader.vs", "shader.fs");
    unsigned int texture;
    unsigned int VBO, VAO, EBO;
    init_map_image(&texture, &VBO, &VAO, &EBO);
    

    ourShader.use(); 
    ourShader.setInt("tex", 0);

    PlayerProperties player1("player1", 10, glm::vec3(1.0f, 0.5f, 0.0f));
    player1.x = 50;
    player1.y = 50;

    static bool hide_map = true;

    while (!glfwWindowShouldClose(dm_window)) {

        // Render to DM window
        glfwMakeContextCurrent(dm_window);
        glfwPollEvents();
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
		glClear(GL_COLOR_BUFFER_BIT);
        

        ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

        ImGui::Begin("Map Properties");
		
		ImGui::Checkbox("Hide Map?", &hide_map);
        ImGui::End();

        ImGui::Begin("Player Properties");
		ImGui::SliderFloat("Player 1 Range", &player1.range, 0, 500.00f);
        ImGui::End();

        ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(dm_window);

        // Render To Game Window

        glfwMakeContextCurrent(game_window);

        glClearColor(1.0f, 1.0f, 1.0f, 1.00f);
		glClear(GL_COLOR_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        ourShader.use();
        ourShader.setBool("hide_map", hide_map);
        player1.renderProperties(ourShader);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(game_window);
        
    }

    ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwDestroyWindow(dm_window);
    glfwDestroyWindow(game_window);
    glfwTerminate();
    return 0;
}


void init_map_image(unsigned int *texture, unsigned int *VBO, unsigned int *VAO, unsigned int *EBO) {
    float vertices[] = {
        // positions          // colors           // texture coords
         1.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
         1.0f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
        -1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
        -1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
    };
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
    
    glGenVertexArrays(1, VAO);
    glGenBuffers(1, VBO);
    glGenBuffers(1, EBO);

    glBindVertexArray(*VAO);

    glBindBuffer(GL_ARRAY_BUFFER, *VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // load and create a texture 
    // -------------------------
    
    // texture 1
    // ---------
    glGenTextures(1, texture);
    glBindTexture(GL_TEXTURE_2D, *texture); 
     // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    unsigned char *data = stbi_load("img.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
}