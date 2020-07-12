//internal includes
#include "common.h"
#include "ShaderProgram.h"

//External dependencies
#define GLFW_DLL
#include <GLFW/glfw3.h>
#include <random>

#include <glm/vec3.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <unistd.h>

#include <SOIL/SOIL.h>

static const GLsizei WIDTH = 800, HEIGHT = 600; //размеры окна

GLuint texture;
GLuint g_vertexBufferObject;
GLuint g_vertexArrayObject;

int initGL()
{
    int res = 0;
    //грузим функции opengl через glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize OpenGL context" << std::endl;
        return -1;
    }

    std::cout << "Vendor: "   << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "Version: "  << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL: "     << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    return 0;
}

int main(int argc, char** argv)
{
    if(!glfwInit())
        return -1;

    //запрашиваем контекст opengl версии 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow*  window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL basic sample", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    if(initGL() != 0)
        return -1;

    //Reset any OpenGL errors which could be present for some reason
    GLenum gl_error = glGetError();
    while (gl_error != GL_NO_ERROR)
        gl_error = glGetError();

    //создание шейдерной программы из двух файлов с исходниками шейдеров
    //используется класс-обертка ShaderProgram
    std::unordered_map<GLenum, std::string> shaders;
    shaders[GL_VERTEX_SHADER]   = "vertex.glsl";
    shaders[GL_FRAGMENT_SHADER] = "fragment.glsl";
    ShaderProgram program(shaders); GL_CHECK_ERRORS;

    glfwSwapInterval(1); // force 60 frames per second

    //Создаем и загружаем геометрию поверхности

    //GLuint g_vertexBufferObject;
    //GLuint g_vertexArrayObject;

    glm::mat4 Projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 10.0f);
    glm::mat4 View       = glm::lookAt(
            glm::vec3(2.0f, 2.0f, 5.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f)
    );


    {

        float rectangle[] =
                {
                        0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.1f, 0.0f, 0.0f,
                        0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.1f, 0.0f, 10.0f,
                        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.1f, 10.0f,10.0f,
                        -0.5, 0.5, 0.0f, 0.0f, 0.0f, 0.1f, 10.0f, 0.0f
                };

        float triangle[] =
                {
                        -0.1f, -0.1f, 0.0f,
                        0.1f, -0.1f, 0.0f,
                        0.0f, +0.1f, 0.0f
                };


        glGenTextures(1, &texture); GL_CHECK_ERRORS;
        glBindTexture(GL_TEXTURE_2D, texture); GL_CHECK_ERRORS;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); GL_CHECK_ERRORS;// Set texture wrapping to GL_REPEAT
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); GL_CHECK_ERRORS;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); GL_CHECK_ERRORS;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); GL_CHECK_ERRORS;

        int width, height;
        unsigned char* image = SOIL_load_image("../pic.jpg", &width, &height, 0, SOIL_LOAD_RGB); GL_CHECK_ERRORS;

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image); GL_CHECK_ERRORS;
        glGenerateMipmap(GL_TEXTURE_2D); GL_CHECK_ERRORS;
        SOIL_free_image_data(image); GL_CHECK_ERRORS;
        glBindTexture(GL_TEXTURE_2D, 0); GL_CHECK_ERRORS;


        g_vertexBufferObject = 0;
        GLuint vertexLocation = 0; // simple layout, assume have only positions at location = 0

        glGenBuffers(1, &g_vertexBufferObject); GL_CHECK_ERRORS;

        glBindBuffer(GL_ARRAY_BUFFER, g_vertexBufferObject);
        glBufferData(GL_ARRAY_BUFFER, 4 * 8 * sizeof(GLuint), &rectangle[0], GL_STATIC_DRAW); GL_CHECK_ERRORS;



        glGenVertexArrays(1, &g_vertexArrayObject); GL_CHECK_ERRORS;
        glBindVertexArray(g_vertexArrayObject); GL_CHECK_ERRORS;

        glBufferData(GL_ARRAY_BUFFER, 4 * 8 * sizeof(GLuint), &rectangle[0], GL_STATIC_DRAW);


        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);




        //glBindVertexArray(0);
        //glBindVertexArray(1);
        //glBindVertexArray(2);

    }

    //цикл обработки сообщений и отрисовки сцены каждый кадр
    while (!glfwWindowShouldClose(window))
    {
        time_t begin = time(0);
        glfwPollEvents();

        //очищаем экран каждый кадр
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        GL_CHECK_ERRORS;
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        GL_CHECK_ERRORS;

        program.StartUseShader();
        GL_CHECK_ERRORS;

        // очистка и заполнение экрана цветом

        glViewport (0, 0, WIDTH, HEIGHT);
        /*int bufferWidth, bufferHeight;
        glfwGetFramebufferSize(window, &bufferWidth, &bufferHeight);
        glViewport(0, 0, bufferWidth, bufferHeight);*/

        glEnable(GL_DEPTH_TEST);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        glBindVertexArray(g_vertexArrayObject);

        //rectangle
        GLuint hasTextureLoc2 = glGetUniformLocation(program.GetProgram(), "hasTexture"); GL_CHECK_ERRORS;
        glUniform1i(hasTextureLoc2, 1); GL_CHECK_ERRORS;

        glm::mat4 trans(1.0f);
        GLuint transformLoc = glGetUniformLocation(program.GetProgram(), "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
        glBindTexture(GL_TEXTURE_2D, texture);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        glBindVertexArray(0);


        glBindVertexArray(g_vertexArrayObject);

        program.StopUseShader();

        glfwSwapBuffers(window);

        sleep(0.015 - difftime(time(0), begin));
    }

    //очищаем vboи vao перед закрытием программы


    glfwTerminate();
    return 0;
}