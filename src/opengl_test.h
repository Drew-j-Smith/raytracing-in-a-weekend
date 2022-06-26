
#include <array>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <streambuf>
#include <vector>

#include <spdlog/spdlog.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <gl/glew.h>

#include <GLFW/glfw3.h>

constexpr int windowStartWidth = 1280;
constexpr int windowStartHeight = 720;

template <typename T> int opengl_test(std::vector<float> &data, T &&callable) {
    const char *glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    if (glfwInit() != GLFW_TRUE) {
        spdlog::error("glfwInit failed");
        return 1;
    }

    glfwSetErrorCallback([](int errCode, const char *msg) {
        spdlog::error("glfw errored with code {}: {}", errCode, msg);
    });

    // Create window with graphics context
    GLFWwindow *window = glfwCreateWindow(windowStartWidth, windowStartHeight,
                                          "test", nullptr, nullptr);
    if (window == nullptr) {
        spdlog::error("glfwCreateWindow failed");
        return 1;
    }
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        spdlog::error("Failed to initialize OpenGL loader!");
        return 1;
    }

    int screen_width;
    int screen_height;
    glfwGetFramebufferSize(window, &screen_width, &screen_height);
    glViewport(0, 0, screen_width, screen_height);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    constexpr int textureWidth = 512;
    constexpr int textureHeight = 512;

    glDebugMessageCallback(
        [](GLenum source, GLenum type, GLuint id, GLenum severity,
           [[maybe_unused]] GLsizei length, const GLchar *message,
           [[maybe_unused]] const void *userParam) {
            auto formated = fmt::format("opengl debug: source: {} type: {} id: "
                                        "{} severity: {}\n message: {}",
                                        source, type, id, severity, message);
            switch (severity) {
            case GL_DEBUG_SEVERITY_NOTIFICATION:
                spdlog::debug(formated);
                break;
            case GL_DEBUG_SEVERITY_LOW:
                spdlog::info(formated);
                break;
            case GL_DEBUG_SEVERITY_MEDIUM:
                spdlog::warn(formated);
                break;
            case GL_DEBUG_SEVERITY_HIGH:
                spdlog::error(formated);
                break;
            }
        },
        nullptr);
    glEnable(GL_DEBUG_OUTPUT);

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, textureWidth, textureHeight, 0,
                 GL_RGB, GL_FLOAT, data.data());
    glBindTexture(GL_TEXTURE_2D, 0);

    GLuint readFboId = 0;
    glGenFramebuffers(1, &readFboId);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, readFboId);
    glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, texture, 0);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

    while (glfwWindowShouldClose(window) != GLFW_TRUE) {
        glfwPollEvents();

        int display_w;
        int display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);

        // if Changed
        glTextureSubImage2D(texture, 0, 0, 0, textureWidth, textureHeight,
                            GL_RGB, GL_FLOAT, data.data());
        glBindFramebuffer(GL_READ_FRAMEBUFFER, readFboId);
        glBlitFramebuffer(0, 0, textureWidth, textureHeight, 0, 0, display_w,
                          display_h, GL_COLOR_BUFFER_BIT, GL_LINEAR);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

        // feed inputs to dear imgui, start new frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // render your GUI
        ImGui::Begin("Demo window");
        ImGui::Text("framerate %.2f", io.Framerate);
        callable();
        ImGui::End();

        ImGui::ShowDemoWindow();

        // Render dear imgui into screen
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glViewport(0, 0, display_w, display_h);
        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}