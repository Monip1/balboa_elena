#include "hw3.h"
#include "3rdparty/glad.h" // needs to be included before GLFW!
#include "3rdparty/glfw/include/GLFW/glfw3.h"
#include "hw3_scenes.h"
#include "3rdparty/glm/vec3.hpp" // glm::vec3
#include "3rdparty/glm/vec4.hpp" // glm::vec4
#include "3rdparty/glm/mat4x4.hpp" // glm::mat4
#include "3rdparty/glm/ext/matrix_transform.hpp" // glm::translate, glm::rotate, glm::scale
#include "3rdparty/glm/ext/matrix_clip_space.hpp" // glm::perspective
#include "3rdparty/glm/ext/scalar_constants.hpp"
#include "3rdparty/glm/glm.hpp"
#include "3rdparty/glm/gtc/type_ptr.hpp"


using namespace hw3;

const char* vertexShaderSource3_3 = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"layout (location = 2) in vec3 aNormal;\n"
"uniform mat4 projection;\n"
"uniform mat4 view;\n"
"uniform mat4 model;\n"
"out vec3 vertexColor;\n"
"void main()\n"
"{\n"
"vertexColor = vec3(1.0,0.0,0.0);\n"
//"vertexColor = aColor;\n"
"gl_Position = projection * view * model * vec4(aPos, 1);\n"
"}\0";

const char* vertexShaderSource3_2 = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"uniform float angle;"
"void main()\n"
"{"
"    mat3 A = mat3(cos(angle), -sin(angle), 0,"
"        sin(angle), cos(angle), 0,"
"        0, 0, 1);"
"    gl_Position = vec4(A * aPos, 1.0);"
"}\0";


const char* fragmentShaderSource3_3 = "#version 330 core\n"
"in vec3 vertexColor;\n"
"in vec3 vertexNormal;\n"
"in vec3 fragPos;\n"
//"uniform vec3 lightColor;\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"        FragColor = vec4(vertexColor, 1.0);\n"
//"        FragColor = vec4(vertexColor * lightColor, 1.0);\n"
"}\0";

const char* fragmentShaderSource3_2 = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);;\n"
"}\0";

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void hw_3_1(const std::vector<std::string> &params) {
    // HW 3.1: Open a window using GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);


    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }


    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);


        glClearColor(0.1f, 0.8f, 0.7f, 0.85f);
        glClear(GL_COLOR_BUFFER_BIT);


        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return;

}

void hw_3_2(const std::vector<std::string> &params) {
    // HW 3.2: Render a single 2D triangle
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);


    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }


    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);


        glClearColor(0.1f, 0.8f, 0.7f, 0.85f);
        glClear(GL_COLOR_BUFFER_BIT);

        float vertices[] = {
            -0.5f, -0.5f, 0.0f,
             0.5f, -0.5f, 0.0f,
             0.0f,  0.5f, 0.0f
        };

        unsigned int VBO;
        glGenBuffers(1, &VBO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        unsigned int vertexShader;
        vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSource3_2, NULL);
        glCompileShader(vertexShader);

        int  success;
        char infoLog[512];
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

        if (!success)
        {
            glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        }



        unsigned int fragmentShader;
        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource3_2, NULL);
        glCompileShader(fragmentShader);

        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

        if (!success)
        {
            glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

        unsigned int shaderProgram;
        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);

        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        }

        glUseProgram(shaderProgram);

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // 0. copy our vertices array in a buffer for OpenGL to use
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // 1. then set the vertex attributes pointers
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // 2. use our shader program when we want to render an object
        glUseProgram(shaderProgram);


        float timeValue = glfwGetTime();
        float angle = (timeValue);
        int angleUniformLocation = glGetUniformLocation(shaderProgram, "angle");
        glUseProgram(shaderProgram);
        glUniform1f(angleUniformLocation, angle);


        unsigned int VAO;
        glGenVertexArrays(1, &VAO);
        // 1. bind Vertex Array Object
        glBindVertexArray(VAO);
        // 2. copy our vertices array in a buffer for OpenGL to use
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // 3. then set our vertex attributes pointers
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);



        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return;
}
void hw_3_3(const std::vector<std::string>& params) {
    if (params.size() == 0) {
        return;
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);



    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }


    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);


        glClearColor(0.1f, 0.8f, 0.7f, 0.85f);
        glClear(GL_COLOR_BUFFER_BIT);

   

        unsigned int VBO;
        glGenBuffers(1, &VBO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3f) * scene.meshes.at(0).vertices.size(), scene.meshes.at(0).vertices.data(), GL_STATIC_DRAW);


        unsigned int EBO;
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Vector3i) * scene.meshes.at(0).faces.size(), scene.meshes.at(0).faces.data(), GL_STATIC_DRAW);


        unsigned int vertexShader;
        vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSource3_3, NULL);
        glCompileShader(vertexShader);

        int  success;
        char infoLog[512];
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

        if (!success)
        {
            glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        }



        unsigned int fragmentShader;
        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource3_2, NULL);
        glCompileShader(fragmentShader);

        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

        if (!success)
        {
            glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

        unsigned int shaderProgram;
        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);

        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        }

        glUseProgram(shaderProgram);

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // 0. copy our vertices array in a buffer for OpenGL to use
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3f) * scene.meshes.at(0).vertices.size(), scene.meshes.at(0).vertices.data(), GL_STATIC_DRAW);
        // 1. then set the vertex attributes pointers
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // 2. use our shader program when we want to render an object
        glUseProgram(shaderProgram);


        float timeValue = glfwGetTime();
        float angle = (timeValue);
        int angleUniformLocation = glGetUniformLocation(shaderProgram, "angle");
        glUseProgram(shaderProgram);
        glUniform1f(angleUniformLocation, angle);


        //model matrix
        int modelMatrixUnifLocation = glGetUniformLocation(shaderProgram, "model");
        glm::mat4 model = glm::make_mat4(&scene.meshes.at(0).model_matrix.data[0][0]);
        glUseProgram(shaderProgram);
        glUniformMatrix4fv(modelMatrixUnifLocation, 1, GL_FALSE, glm::value_ptr(model));

        //view matrix
        int viewMatrixUnifLocation = glGetUniformLocation(shaderProgram, "view");
        glm::mat4 view = glm::make_mat4(&scene.camera.cam_to_world.data[0][0]);
        glUseProgram(shaderProgram);
        glUniformMatrix4fv(viewMatrixUnifLocation, 1, GL_FALSE, glm::value_ptr(view));

        //projection matrix
        Real a = (Real)scene.camera.resolution.x / (Real)scene.camera.resolution.y;
        Real s = scene.camera.s;
        Real zfar = scene.camera.z_far;
        Real znear = scene.camera.z_near;
        Matrix4x4f projectionMat = {
            1.0 / (a * s), 0.0, 0.0, 0.0,
            0.0, 1 / s, 0.0, 0.0,
            0.0, 0.0, -zfar / (zfar - znear), -(zfar * znear) / (zfar - znear),
            0.0, 0.0, -1.0, 0.0
        };
        int projMatrixUnifLocation = glGetUniformLocation(shaderProgram, "projection");
        glm::mat4 proj = glm::make_mat4(&projectionMat.data[0][0]);
        glUseProgram(shaderProgram);
        glUniformMatrix4fv(projMatrixUnifLocation, 1, GL_FALSE, glm::value_ptr(proj));


        unsigned int VAO;
        glGenVertexArrays(1, &VAO);
        // 1. bind Vertex Array Object
        glBindVertexArray(VAO);
        // 2. copy our vertices array in a buffer for OpenGL to use
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3f) * scene.meshes.at(0).vertices.size(), scene.meshes.at(0).vertices.data(), GL_STATIC_DRAW);
        // 3. then set our vertex attributes pointers
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glDrawElements(GL_TRIANGLES, sizeof(Vector3i) * scene.meshes.at(0).faces.size(), GL_UNSIGNED_INT, 0);

        //glDrawArrays(GL_TRIANGLES, 0, 3);



        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return;
}


void hw_3_3_2(const std::vector<std::string> &params) {
    // HW 3.3: Render a scene
    if (params.size() == 0) {
        return;
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);


    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }


    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);


        glClearColor(0.1f, 0.8f, 0.7f, 0.85f);
        glClear(GL_COLOR_BUFFER_BIT);

        //glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3f) * scene.meshes.at(0).vertices.size(), scene.meshes.at(0).vertices.data(), GL_STATIC_DRAW);
        

        unsigned int vertexShader;
        vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSource3_3, NULL);
        glCompileShader(vertexShader);

        int  success;
        char infoLog[512];
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

        if (!success)
        {
            glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        }


        unsigned int fragmentShader;
        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource3_3, NULL);
        glCompileShader(fragmentShader);

        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

        if (!success)
        {
            glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

        unsigned int shaderProgram;
        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);

        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        }

        glUseProgram(shaderProgram);

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        

        // 2. use our shader program when we want to render an object
        glUseProgram(shaderProgram);


        //float timeValue = glfwGetTime();
        //float angle = (timeValue);
        //int angleUniformLocation = glGetUniformLocation(shaderProgram, "angle");
        //glUseProgram(shaderProgram);
        //glUniform1f(angleUniformLocation, angle);


        //model matrix
        int modelMatrixUnifLocation = glGetUniformLocation(shaderProgram, "model");
        glm::mat4 model = glm::make_mat4(&scene.meshes.at(0).model_matrix.data[0][0]);
        glUseProgram(shaderProgram);
        glUniformMatrix4fv(modelMatrixUnifLocation, 1, GL_FALSE, glm::value_ptr(model));

        //view matrix
        int viewMatrixUnifLocation = glGetUniformLocation(shaderProgram, "view");
        glm::mat4 view = glm::make_mat4(&scene.camera.cam_to_world.data[0][0]);
        glUseProgram(shaderProgram);
        glUniformMatrix4fv(viewMatrixUnifLocation, 1, GL_FALSE, glm::value_ptr(view));

        //projection matrix
        Real a = (Real)scene.camera.resolution.x / (Real)scene.camera.resolution.y;
        Real s = scene.camera.s;
        Real zfar = scene.camera.z_far;
        Real znear = scene.camera.z_near;
        Matrix4x4f projectionMat = {
            1.0 / (a * s), 0.0, 0.0, 0.0,
            0.0, 1 / s, 0.0, 0.0,
            0.0, 0.0, -zfar / (zfar - znear), -(zfar * znear) / (zfar - znear),
            0.0, 0.0, -1.0, 0.0
        };
        int projMatrixUnifLocation = glGetUniformLocation(shaderProgram, "projection");
        glm::mat4 proj = glm::make_mat4(&projectionMat.data[0][0]);
        glUseProgram(shaderProgram);
        glUniformMatrix4fv(projMatrixUnifLocation, 1, GL_FALSE, glm::value_ptr(proj));


        unsigned int VAO;
        glGenVertexArrays(1, &VAO);
        // 1. bind Vertex Array Object
        glBindVertexArray(VAO);

        unsigned int VBO_vertex;
        glGenBuffers(1, &VBO_vertex);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_vertex);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3f) * scene.meshes.at(0).vertices.size(), scene.meshes.at(0).vertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0 /* layout index */,
            3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        unsigned int VBO_color;
        glGenBuffers(1, &VBO_color);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_color);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3f) * scene.meshes.at(0).vertex_colors.size(), scene.meshes.at(0).vertex_colors.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(1 /* layout index */,
            3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

        unsigned int EBO;
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Vector3i) * scene.meshes.at(0).faces.size(), scene.meshes.at(0).faces.data(), GL_STATIC_DRAW);

        // 2. copy our vertices array in a buffer for OpenGL to use
        glBindBuffer(GL_ARRAY_BUFFER, VBO_vertex);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3f)* scene.meshes.at(0).vertices.size(), scene.meshes.at(0).vertices.data(), GL_STATIC_DRAW);
        // 3. then set our vertex attributes pointers
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glDrawElements(GL_TRIANGLES, scene.meshes.at(0).vertices.size(), GL_UNSIGNED_INT, 0);
        //glDrawArrays(GL_TRIANGLES, 0, 3); replaced by glDrawElements



        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return;


}

void hw_3_4(const std::vector<std::string> &params) {
    // HW 3.4: Render a scene with lighting
    if (params.size() == 0) {
        return;
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;
}
