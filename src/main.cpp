#include <array>
#include <cstdint>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <streambuf>
#include <vector>

#include <CL/cl2.hpp>

#include <spdlog/spdlog.h>
#include <spdlog/stopwatch.h>

#include "../opencl-include/structs.h"
#include "opengl_test.h"

int main([[maybe_unused]] int argc, char **argv) {

    try {
        spdlog::set_level(spdlog::level::info);

        auto ctx = cl::Context(
            CL_DEVICE_TYPE_DEFAULT, nullptr, // NOLINT
            [](const char *errinfo, [[maybe_unused]] const void *private_info,
               [[maybe_unused]] size_t cb, [[maybe_unused]] void *user_data) {
                spdlog::error("opencl error: {}", errinfo);
            });
        spdlog::info("created cl::Context");

        auto queue = cl::CommandQueue(ctx, CL_QUEUE_PROFILING_ENABLE); // NOLINT
        spdlog::info("created cl::CommandQueue");

        // have to reload to invalidate cache on change
        std::filesystem::path exec_dir(argv[0]); // NOLINT
        auto binary_dir = exec_dir.parent_path().string();
        std::ifstream file(binary_dir + "/opencl-include/main.cl");
        std::string source((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());

        cl_int err{};
        cl::Program program(ctx, source, false, &err);
        std::string include_str = std::format("-I{}", binary_dir);
        spdlog::info("include flag:{}", include_str);
        err = program.build(include_str.c_str());
        if (err != CL_SUCCESS) {
            cl::BuildLogType log = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>();
            for (const auto &pair : log) {
                spdlog::error("Build error on Device {} \nBuild info:{}",
                              pair.first.getInfo<CL_DEVICE_VENDOR>(),
                              pair.second);
            }
        }
        spdlog::info("created and compliled cl::Program");

        constexpr auto temp_width = 512ULL;
        constexpr auto temp_height = 512ULL;
        std::vector<cl_float4> temp_res(temp_width * temp_height,
                                        {{1, 1, 1, 1}});
        Camera cam{{{0, 0, 0}}, {{-1, -1, -1}}, {{2, 0, 0}}, {{0, 2, 0}}};

        auto temp_output_buff =
            cl::Buffer(queue, temp_res.begin(), temp_res.end(), false, false);
        spdlog::info("created cl::Buffer(s)");

        cl::Kernel kernel_raycast = cl::Kernel(program, "raycast");
        kernel_raycast.setArg(0, temp_output_buff);
        kernel_raycast.setArg(1, static_cast<cl_uint>(temp_width));
        kernel_raycast.setArg(2, cam);
        spdlog::info("created cl::Kernel");

        cl_float4 color1{{1, 1, 1, 1}};
        constexpr auto color2_r = 0.5F;
        constexpr auto color2_g = 0.7F;
        cl_float4 color2{{color2_r, color2_g, 1, 1}};
        cl_float3 origin{{0, 0, 0}};

        opengl_test(temp_res, [&]() {
            ImGui::ColorEdit3("Color1##1", &color1.x);
            ImGui::ColorEdit3("Color2##2", &color2.x);
            constexpr auto one_hundred = 100;
            ImGui::SliderFloat3("XYZ##3", &origin.x, -one_hundred, one_hundred,
                                "%.3f", ImGuiSliderFlags_Logarithmic);
            cam.m_origin = {{origin.x, origin.y, origin.z}};
            cam.m_lower_left_corner = {
                {origin.x - 1, origin.y - 1, origin.z - 1}};
            kernel_raycast.setArg(2, cam);
            kernel_raycast.setArg(3, color1);
            kernel_raycast.setArg(4, color2);
            queue.enqueueNDRangeKernel(kernel_raycast, cl::NullRange,
                                       cl::NDRange(temp_width * temp_height),
                                       cl::NullRange);
            queue.enqueueReadBuffer(temp_output_buff, CL_TRUE, 0,
                                    temp_res.size() * sizeof(cl_float4),
                                    temp_res.data());
        });
    } catch (std::exception &e) {
        std::cerr << e.what() << '\n' << std::flush;
    }
}
