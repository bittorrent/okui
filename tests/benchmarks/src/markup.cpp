/**
* Copyright 2017 BitTorrent Inc.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*    http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
#include <benchmark/benchmark.h>

#include <okui/ml/Context.h>

#include <string>
#include <fstream>
#include <streambuf>

static void MarkupContextLoadString(benchmark::State& state) {
    std::ifstream t(OKUI_BENCHMARK_RESOURCES_PATH "/markup.xml");
    std::string markup((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
    while (state.KeepRunning()) {
        okui::ml::Environment environment;
        okui::ml::Context context{&environment};
        context.define("state", "standby");
        benchmark::DoNotOptimize(context.load(markup));
    }
}

BENCHMARK(MarkupContextLoadString);

static void MarkupContextLoadXML(benchmark::State& state) {
    std::ifstream t(OKUI_BENCHMARK_RESOURCES_PATH "/markup.xml");
    std::string markup((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
    pugi::xml_document document;
    document.load_buffer(markup.data(), markup.size());
    while (state.KeepRunning()) {
        okui::ml::Environment environment;
        okui::ml::Context context{&environment};
        context.define("state", "standby");
        benchmark::DoNotOptimize(context.load(document.first_child()));
    }
}

BENCHMARK(MarkupContextLoadXML);
