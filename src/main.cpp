#include <pybind11/pybind11.h>
#include <pybind11/chrono.h>

#include <atomic>
#include <chrono>
#include <thread>

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

std::shared_ptr<std::thread> th_ = nullptr;

class Event
{
    std::atomic<bool> flag_ = false;

public:
    void Set() { flag_ = true; }
    bool IsSet() { return flag_; }
};

void BusyWait(std::chrono::duration<long long> duration, Event &event)
{
    auto current_time = std::chrono::system_clock::now();
    while (std::chrono::system_clock::now() < current_time + duration && !event.IsSet())
        ;
}

void start(std::chrono::duration<long long> duration, Event &event)
{
    if (th_.get() != nullptr)
    {
        throw std::runtime_error("only one thread can be created at a time (derived from C++)");
    }
    th_ = std::make_shared<std::thread>(BusyWait, duration, std::ref(event));
}

void join()
{
    if (th_.get() == nullptr)
    {
        throw std::runtime_error("cannot join thread before it is started (derived from C++)");
    }
    th_->join();
    th_.reset();
}

namespace py = pybind11;

PYBIND11_MODULE(sleep_thread, m)
{
    py::class_<Event>(m, "Event")
        .def(py::init())
        .def("set", &Event::Set)
        .def("is_set", &Event::IsSet);

    m.def(
        "start", [](std::chrono::duration<long long> duration, Event &event)
        {
        py::gil_scoped_release _;
        start(duration, event); });

    m.def(
        "join", []()
        {
        py::gil_scoped_acquire _;
        join(); });

#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}
