#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "include/Shader.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

glm::vec3 upDir = glm::vec3(0.0f,1.0f,0.0f);
glm::vec3 camPos =   glm::vec3(0.0f);
glm::mat4 rotMatrix =   glm::mat4(1.0f);

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "BazeleGraficii", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glew: load all OpenGL function pointers
    // ---------------------------------------
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }


    // build and compile our shader program
    // ------------------------------------
    Shader vertex_shader(GL_VERTEX_SHADER,"shaders/vertex_shader.vert");
    Shader fragment_shader(GL_FRAGMENT_SHADER,"shaders/fragment_shader.frag");

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram,vertex_shader.getID());
    glAttachShader(shaderProgram,fragment_shader.getID());
    glLinkProgram(shaderProgram);

    GLint success;
    GLchar* infoLog = NULL;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertex_shader.getID());
    glDeleteShader(fragment_shader.getID());

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------

    float vertices[] = {
            -0.5f,  0.5f, -2.0f,
              1.0f,0.0f,0.0f,
            0.5f,  -0.5f, -2.0f,
              1.0f,1.0f,0.0f,
            -0.5f, -0.5f, -2.0f,
                0.0f,1.0f,1.0f,
            0.5f, 0.5f, -2.0f,
                0.0f,0.0f,1.0f,
         -0.5f,  0.5f, -3.0f,
                1.0f,0.0f,1.0f,
         0.5f,  -0.5f, -3.0f,
                1.0f,1.0f,1.0f,
        -0.5f, -0.5f, -3.0f,
                0.0f,0.0f,0.0f,
        0.5f, 0.5f, -3.0f,
            1.0f,0.0f,0.0f,
    };
    unsigned int indices[] = {
        0, 1, 2, // front face
        0,3,1,

        3,5,1, // right face
        3,7,5,

        4,6,5, // back face
        5,7,4,

        6,2,1, // bottom face
        1,5,2,

        4,3,0, // top face
        4,7,3,

        0,2,6, // right face
        6,4,0


    };

//    float vertices[] = {
//         -0.5f,  0.5f, -2.0f,
//         0.0f,0.0f,1.0f,
//
//         0.5f,  -0.5f, -2.0f,
//         0.0f,1.0f,0.0f,
//
//        -0.5f, -0.5f, -2.0f,
//        1.0f,1.0f,0.0f,
//
//        0.5f, 0.5f, -2.0f,
//        1.0f,0.5f,0.0f,
//    };
//    unsigned int indices[] = {
//        0, 1, 2,
//        0,3,1
//    };



    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    GLint posIndex = glGetAttribLocation(shaderProgram,"position");
    glVertexAttribPointer(posIndex, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(posIndex);

    GLint colorIndex = glGetAttribLocation(shaderProgram,"color");
    glVertexAttribPointer(colorIndex, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(sizeof(float)*3));
    glEnableVertexAttribArray(colorIndex);

    // Uniforms

    glm::mat4 perpectiveMat = glm::perspective(glm::radians(45.0f),(float)SCR_WIDTH/(float)SCR_HEIGHT,0.1f,100.0f);

    GLuint perspProjID = glGetUniformLocation(shaderProgram,"perspProj");
    glProgramUniformMatrix4fv(shaderProgram,perspProjID,1,GL_FALSE,glm::value_ptr(perpectiveMat));

    double time = glfwGetTime();
    GLuint timeID = glGetUniformLocation(shaderProgram,"time");

    GLuint rotMatrixID = glGetUniformLocation(shaderProgram,"rotMatrix");

    GLuint camPosID = glGetUniformLocation(shaderProgram,"camPos");


    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);


    // uncomment this call to draw in wireframe polygons.
//    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_DEPTH_TEST);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // time update
        glProgramUniform1f(shaderProgram,timeID,time);

        // input
        // -----
        processInput(window);

        glProgramUniform3fv(shaderProgram,camPosID,1,glm::value_ptr(camPos));
        glProgramUniformMatrix4fv(shaderProgram,rotMatrixID,1,GL_FALSE,glm::value_ptr(rotMatrix));

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw our first triangle
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        //glDrawArrays(GL_TRIANGLES, 0, 6);
        glDrawElements(GL_TRIANGLES, 12*3, GL_UNSIGNED_INT, 0);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
float camSpeed = 0.1f;
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camPos.z -= camSpeed;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camPos.z += camSpeed;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camPos.x += camSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camPos.x -= camSpeed;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        rotMatrix = glm::rotate(rotMatrix,-0.01f,upDir);
     if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        rotMatrix = glm::rotate(rotMatrix,0.01f,upDir);

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
