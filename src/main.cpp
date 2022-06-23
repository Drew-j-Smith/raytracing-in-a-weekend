#include <array>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <streambuf>
#include <vector>

#include <CL/cl2.hpp>

#include <spdlog/spdlog.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <gl/glew.h>

#include <GLFW/glfw3.h>

using arr_int_type = uint64_t;
constexpr int windowStartWidth = 1280;
constexpr int windowStartHeight = 720;

int main([[maybe_unused]] int argc, char **argv) {

    constexpr auto arr_size = 1000;

    spdlog::set_level(spdlog::level::info);

    auto ctx = cl::Context(
        CL_DEVICE_TYPE_DEFAULT, nullptr,
        [](const char *errinfo, [[maybe_unused]] const void *private_info,
           [[maybe_unused]] size_t cb, [[maybe_unused]] void *user_data) {
            spdlog::error("opencl error: {}", errinfo);
        });
    spdlog::info("created cl::Context");

    auto queue = cl::CommandQueue(ctx, CL_QUEUE_PROFILING_ENABLE);
    spdlog::info("created cl::CommandQueue");

    std::array<arr_int_type, arr_size> input;
    for (std::size_t i = 0; i < input.size(); ++i) {
        input[i] = static_cast<arr_int_type>(i);
    }
    auto inputBuff = cl::Buffer(queue, input.begin(), input.end(), true, false);

    std::array<arr_int_type, arr_size> output;
    auto outputBuff =
        cl::Buffer(queue, output.begin(), output.end(), false, false);
    spdlog::info("created cl::Buffer(s)");

    std::filesystem::path exec_dir(argv[0]);
    std::string cl_loc = exec_dir.parent_path().append("main.cl").string();
    std::ifstream file(cl_loc);
    std::string source((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());

    cl_int err;
    cl::Program program(ctx, source, true, &err);
    if (err != CL_SUCCESS) {
        cl::BuildLogType log = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>();
        for (const auto &pair : log) {
            spdlog::error("Build error on Device {} \nBuild info:{}",
                          pair.first.getInfo<CL_DEVICE_VENDOR>(), pair.second);
        }
    }
    spdlog::info("created and compliled cl::Program");

    cl::Kernel kernel_mult = cl::Kernel(program, "mult");
    kernel_mult.setArg(0, inputBuff);
    kernel_mult.setArg(1, outputBuff);
    spdlog::info("created cl::Kernel");

    queue.enqueueNDRangeKernel(kernel_mult, cl::NullRange,
                               cl::NDRange(arr_size), cl::NullRange);
    queue.finish();

    queue.enqueueReadBuffer(outputBuff, CL_TRUE, 0,
                            output.size() * sizeof(arr_int_type),
                            output.data());

    for (auto i : output) {
        spdlog::debug("{}", i);
    }

    spdlog::info("success");

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
    constexpr int textureInitialColor = 255;
    constexpr int packSize = 3;
    std::vector<uint8_t> temp(textureWidth * textureHeight * packSize,
                              textureInitialColor);
    for (uint64_t i = 0; i < textureWidth; ++i) {
        for (uint64_t j = 0; j < textureHeight; ++j) {
            temp[i * textureWidth * packSize + j * packSize] =
                static_cast<uint8_t>(i / 2);
            temp[i * textureWidth * packSize + j * packSize + 1] =
                static_cast<uint8_t>(j / 2);
            temp[i * textureWidth * packSize + j * packSize + 2] =
                static_cast<uint8_t>((i + j) / 4);
        }
    }

    glDebugMessageCallback(
        [](GLenum source, GLenum type, GLuint id, GLenum severity,
           [[maybe_unused]] GLsizei length, const GLchar *message,
           [[maybe_unused]] const void *userParam) {
            auto formated = fmt::format("opengl debug: source: {} type: {} id: "
                                        "{} severity: {}\n message: {}",
                                        source, type, id, severity, message);
            switch (severity) {
            case GL_DEBUG_SEVERITY_NOTIFICATION:
                spdlog::info(formated);
                break;
            case GL_DEBUG_SEVERITY_LOW:
                spdlog::warn(formated);
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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, textureWidth, textureHeight, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, temp.data());
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
                            GL_RGB, GL_UNSIGNED_BYTE, temp.data());
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
