#include <array>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <streambuf>
#include <vector>

#include <CL/cl2.hpp>

void check_err(cl_int err) {
    if (err != CL_SUCCESS) {
        std::cerr << err << '\n';
        throw std::runtime_error("");
    }
}

using arr_int_type = uint64_t;

int main([[maybe_unused]] int argc, char **argv) {

    constexpr auto arr_size = 1000;

    cl_int err;
    auto queue = cl::CommandQueue(CL_QUEUE_PROFILING_ENABLE, &err);
    check_err(err);

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

    std::filesystem::path exec_dir(argv[0]);
    std::string cl_loc = exec_dir.parent_path().append("main.cl").string();
    std::ifstream file(cl_loc);
    std::string source((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());

    cl::Program program(source, true, &err);
    check_err(err);

    cl::Kernel kernel_mult = cl::Kernel(program, "mult", &err);
    check_err(err);
    err = kernel_mult.setArg(0, inputBuff);
    check_err(err);
    err = kernel_mult.setArg(1, outputBuff);
    check_err(err);
    err = queue.enqueueNDRangeKernel(kernel_mult, cl::NullRange,
                                     cl::NDRange(arr_size), cl::NullRange);
    check_err(err);
    err = queue.finish();
    check_err(err);

    err = queue.enqueueReadBuffer(outputBuff, CL_TRUE, 0,
                                  output.size() * sizeof(arr_int_type),
                                  output.data());
    check_err(err);

    for (auto i : output) {
        std::cout << i << '\n';
    }

    return 0;
}
