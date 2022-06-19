#include <array>
#include <cstdint>
#include <iostream>
#include <vector>

#include <CL/cl2.hpp>

void check_err(cl_int err) {
    if (err != CL_SUCCESS) {
        std::cerr << err << '\n';
        throw std::runtime_error("");
    }
}

int main() {

    constexpr auto arr_size = 30000;

    cl_int err;
    cl::Context ctx = cl::Context::getDefault(&err);
    check_err(err);

    cl::Device device = cl::Device::getDefault(&err);
    check_err(err);

    cl::CommandQueue queue(
        clCreateCommandQueueWithProperties(ctx(), device(), nullptr, &err));
    check_err(err);

    std::array<uint64_t, arr_size> input;
    for (std::size_t i = 0; i < input.size(); ++i) {
        input[i] = static_cast<uint64_t>(i);
    }
    cl::Buffer inputBuff(queue, input.begin(), input.end(), false, false, &err);
    check_err(err);

    std::array<uint64_t, arr_size> output;
    cl::Buffer outputBuff(queue, output.begin(), output.end(), true, false,
                          &err);
    check_err(err);

    err = queue.enqueueWriteBuffer(
        inputBuff, CL_TRUE, 0, input.size() * sizeof(uint64_t), input.data());
    check_err(err);

    cl::string source{
        R"(
void kernel mult(global const ulong* a, global ulong* b){
    int id = get_global_id(0);
    for (int i = 0; i < 30000; ++i) {
        b[id] = a[id] * a[id];
    }
}
        )"};

    cl::Program program(ctx, source, true, &err);
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
                                  output.size() * sizeof(uint64_t),
                                  output.data());
    check_err(err);

    // for (auto i : output) {
    //     std::cout << i << '\n';
    // }

    return 0;
}
