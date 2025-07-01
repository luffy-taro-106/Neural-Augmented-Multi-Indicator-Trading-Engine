#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "macd_strategy.h"
#include "rsi_strategy.h"
#include "supertrend_strategy.h"

namespace py = pybind11;

PYBIND11_MODULE(bindings, m) {
    py::class_<Candle>(m, "Candle")
    .def(py::init<>())  // default constructor
    .def(py::init<double, double, double, double, double>(),  // custom constructor
         py::arg("open"), py::arg("high"), py::arg("low"), py::arg("close"), py::arg("volume"))
    .def_readwrite("open", &Candle::open)
    .def_readwrite("high", &Candle::high)
    .def_readwrite("low", &Candle::low)
    .def_readwrite("close", &Candle::close)
    .def_readwrite("volume", &Candle::volume);

    py::class_<TradeResult>(m, "TradeResult")
        .def(py::init<>())
        .def_readwrite("success_rate", &TradeResult::success_rate)
        .def_readwrite("avg_return", &TradeResult::avg_return)
        .def_readwrite("total_trades", &TradeResult::total_trades)
        .def_readwrite("positions", &TradeResult::positions);

    m.def("run_macd_strategy", &run_macd_strategy, py::arg("candles"), py::arg("profit_threshold") = 0.01);
    m.def("run_rsi_strategy", &run_rsi_strategy, py::arg("candles"), py::arg("profit_threshold") = 0.01);
    m.def("run_supertrend_strategy", &run_supertrend_strategy, py::arg("candles"), py::arg("atr_period") = 10, py::arg("multiplier") = 3.0, py::arg("profit_threshold") = 0.01);
}
