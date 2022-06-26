#include <array>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <streambuf>
#include <vector>

#include <CL/cl2.hpp>

#include <spdlog/spdlog.h>

#include "opengl_test.h"

using arr_int_type = uint64_t;

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

    cl_double3 center{{0, 0, 0}};
    cl_double3 lowLeft{{-1, -1, -1}};
    constexpr auto temp_width = 512U;
    constexpr auto temp_height = 512U;
    constexpr auto two = 2.0;
    std::vector<cl_double3> temp_arr(temp_width * temp_height);
    std::vector<cl_double3> temp_res(temp_width * temp_height);

    for (auto i = 0U; i < temp_width; i++) {
        for (auto j = 0U; j < temp_height; j++) {
            auto idx = i + (temp_height - 1 - j) * temp_height;
            temp_arr[idx].x = lowLeft.x + two / (temp_width - 1) * i;
            temp_arr[idx].y = lowLeft.y + two / (temp_height - 1) * j;
            temp_arr[idx].z = -1;
        }
    }
    spdlog::info("created data");

    auto tempInputBuff =
        cl::Buffer(queue, temp_arr.begin(), temp_arr.end(), true, false);
    auto tempOutputBuff =
        cl::Buffer(queue, temp_res.begin(), temp_res.end(), false, false);
    spdlog::info("created cl::Buffer(s)");

    cl::Kernel kernel_raycast = cl::Kernel(program, "raycast");
    kernel_raycast.setArg(0, center);
    kernel_raycast.setArg(1, tempInputBuff);
    kernel_raycast.setArg(2, tempOutputBuff);
    spdlog::info("created cl::Kernel");

    queue.enqueueNDRangeKernel(kernel_raycast, cl::NullRange,
                               cl::NDRange(temp_width * temp_height),
                               cl::NullRange);
    queue.finish();

    queue.enqueueReadBuffer(tempOutputBuff, CL_TRUE, 0,
                            temp_res.size() * sizeof(cl_double3),
                            temp_res.data());
    spdlog::info("success");

    constexpr auto packSize = 3;
    constexpr auto textureInitialColor = 0;
    constexpr auto maxColor = 255.999;
    std::vector<uint8_t> temp(temp_width * temp_height * packSize,
                              textureInitialColor);
    for (uint64_t i = 0; i < temp_width * temp_height; ++i) {
        auto curr = temp_res[i];
        temp[i * packSize] = static_cast<uint8_t>(curr.x * maxColor);
        temp[i * packSize + 1] = static_cast<uint8_t>(curr.y * maxColor);
        temp[i * packSize + 2] = static_cast<uint8_t>(curr.z * maxColor);
    }

    opengl_test(temp);
}
