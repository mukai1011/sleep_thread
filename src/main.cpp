#include <pybind11/pybind11.h>
#include <pybind11/chrono.h>

#include <atomic>
#include <chrono>
#include <thread>

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

std::thread th_;

class Event
{
    std::atomic<bool> flag_ = false;

public:
    void Set() { flag_ = true; }
    bool IsSet() { return flag_; }
};

void start(std::chrono::duration<long long> duration, Event &event) {
    auto busy_wait = [](std::chrono::duration<long long> duration, Event &event)
    {
        auto current_time = std::chrono::system_clock::now();
        while (std::chrono::system_clock::now() < current_time + duration && !event.IsSet())
            ;
    };
    th_ = std::thread(busy_wait, duration, std::ref(event));
}

void join() {
    th_.join();
}


namespace py = pybind11;

PYBIND11_MODULE(python_example, m)
{
    py::class_<Event>(m, "Event")
        .def(py::init())
        .def("set", &Event::Set)
        .def("is_set", &Event::IsSet);

    m.def("start", [](std::chrono::duration<long long> duration, Event &cancel) {
        py::gil_scoped_release _;
        start(duration, cancel);
    }, R"pbdoc(
        Subtract two numbers

        Some other explanation about the subtract function.
    )pbdoc");

    m.def("join", []() {
        py::gil_scoped_acquire _;
        join();
    }, R"pbdoc(
        Subtract two numbers

        Some other explanation about the subtract function.
    )pbdoc");

#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}
