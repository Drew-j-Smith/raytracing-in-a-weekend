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

void check_err(cl_int err) {
    if (err != CL_SUCCESS) {
        spdlog::dump_backtrace();
        spdlog::error("failed with error code: {}", err);
        throw std::runtime_error("");
    }
}

using arr_int_type = uint64_t;

int main([[maybe_unused]] int argc, char **argv) {

    constexpr auto arr_size = 1000;

    spdlog::enable_backtrace(32);
    spdlog::set_level(spdlog::level::info);

    cl_int err;
    auto queue = cl::CommandQueue(CL_QUEUE_PROFILING_ENABLE, &err);
    check_err(err);
    spdlog::debug("created CommandQueue");

    std::array<arr_int_type, arr_size> input;
    for (std::size_t i = 0; i < input.size(); ++i) {
        input[i] = static_cast<arr_int_type>(i);
    }
    auto inputBuff =
        cl::Buffer(queue, input.begin(), input.end(), true, false, &err);
    check_err(err);

    std::array<arr_int_type, arr_size> output;
    auto outputBuff =
        cl::Buffer(queue, output.begin(), output.end(), false, false, &err);
    check_err(err);
    spdlog::debug("created Buffers");

    std::filesystem::path exec_dir(argv[0]);
    std::string cl_loc = exec_dir.parent_path().append("main.cl").string();
    std::ifstream file(cl_loc);
    std::string source((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());

    cl::Program program(source, true, &err);
    if (err != CL_SUCCESS) {
        cl::BuildLogType log = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(&err);
        check_err(err);
        for (const auto &pair : log) {
            spdlog::error("{}", pair.second);
        }
        return 0;
    }
    spdlog::debug("created and compliled Program");

    cl::Kernel kernel_mult = cl::Kernel(program, "mult", &err);
    check_err(err);
    err = kernel_mult.setArg(0, inputBuff);
    check_err(err);
    err = kernel_mult.setArg(1, outputBuff);
    check_err(err);
    spdlog::debug("created Kernel");

    err = queue.enqueueNDRangeKernel(kernel_mult, cl::NullRange,
                                     cl::NDRange(arr_size), cl::NullRange);
    check_err(err);

    err = queue.enqueueReadBuffer(outputBuff, CL_TRUE, 0,
                                  output.size() * sizeof(arr_int_type),
                                  output.data());
    check_err(err);

    for (auto i : output) {
        spdlog::debug("{}", i);
    }

    spdlog::info("success");

    const char *glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    if (!glfwInit())
        return 1;

    // Create window with graphics context
    GLFWwindow *window = glfwCreateWindow(1280, 720, "test", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);

    bool err2 = glewInit() != GLEW_OK;

    if (err2) {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
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

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);

        // feed inputs to dear imgui, start new frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // render your GUI
        ImGui::Begin("Demo window");
        ImGui::ShowDemoWindow();
        ImGui::End();

        // Render dear imgui into screen
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        int display_w;
        int display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
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
