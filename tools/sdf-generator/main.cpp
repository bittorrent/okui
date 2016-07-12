#include <cmath>
#include <functional>
#include <set>
#include <vector>
#include <thread>
#include <mutex>

#define NANOSVG_IMPLEMENTATION
#include "nanosvg.h"

#include "png.h"

void IterateCubicBeziers(NSVGshape* shape, const std::function<void(const float*)>& f) {
    for (auto path = shape->paths; path; path = path->next) {
        for (size_t i = 0; i + 3 < path->npts; i += 3) {
            auto pts = &path->pts[i << 1];
            f(pts);
        }
    }
}

// forcibly inlined for performance
#define CUBIC_BEZIER(t, p0, p1, p2, p3) \
    ((1.0 - t) * (1.0 - t) * (1.0 - t) * p0 + 3.0 * (1.0 - t) * (1.0 - t) * t * p1 + 3.0 * (1.0 - t) * t * t * p2 + t * t * t * p3)

std::set<double> ActiveEdges(NSVGshape* shape, double y) {
    std::set<double> ret;

    IterateCubicBeziers(shape, [&](const float* pts) {
        constexpr auto step = 0.0000003;
        constexpr auto threshold = 0.0003;
        constexpr auto gapThreshold = 0.52;
        for (auto t = 0.0; t <= 1.0; t += step) {
            auto by = CUBIC_BEZIER(t, pts[1], pts[3], pts[5], pts[7]);
            if (std::fabs(y - by) < threshold) {
                ret.insert(std::round(CUBIC_BEZIER(t, pts[0], pts[2], pts[4], pts[6]) / gapThreshold) * gapThreshold);
            }
        }
    });

    if (ret.size() == 1) {
        // this is most likely a sharp corner. just ignore it
        ret.clear();
    } else if (ret.size() % 2) {
        // this is bad. tweak the parameters until we detect an even number of edges for each line
        printf("\n");
        for (auto d : ret) { printf("%f ", d); }
        printf("\n              ");
    }

    return ret;
}

inline double Distance(NSVGimage* svg, double x, double y) {
    double ret2 = svg->width * svg->width + svg->height * svg->height;

    for (auto shape = svg->shapes; shape; shape = shape->next) {
        IterateCubicBeziers(shape, [&](const float* pts) {
            constexpr auto step = 0.00005;
            for (auto t = 0.0; t <= 1.0; t += step) {
                auto yd = std::fabs(y - CUBIC_BEZIER(t, pts[1], pts[3], pts[5], pts[7]));
                auto yd2 = yd * yd;
                if (yd2 > ret2) { continue; }

                auto xd = std::fabs(x - CUBIC_BEZIER(t, pts[0], pts[2], pts[4], pts[6]));
                auto xd2 = xd * xd;
                if (xd2 > ret2) { continue; }

                auto d2 = yd2 + xd2;
                if (d2 < ret2) {
                    ret2 = d2;
                }
            }
        });
    }

    return sqrt(ret2);
}

inline bool IsInsideActiveEdges(const std::set<double>& activeEdges, double x) {
    auto ret = false;
    for (auto& edge : activeEdges) {
        if (edge > x) {
            break;
        }
        ret = !ret;
    }
    return ret;
}

struct Pixel {
    uint16_t g = htons(std::numeric_limits<uint16_t>::max());
    uint16_t a;
};

void GetRowPixels(std::vector<Pixel>& pixels, NSVGimage* svg, double svgY, double range) {
    std::vector<std::set<double>> activeEdges;
    for (auto shape = svg->shapes; shape; shape = shape->next) {
        activeEdges.emplace_back(ActiveEdges(shape, svgY));
    }

    const auto scale = (double)pixels.size() / svg->width;

    for (size_t x = 0; x < pixels.size(); ++x) {
        double svgX = 0.5 + x / scale;
        auto d = -Distance(svg, svgX, svgY);

        for (auto& ae : activeEdges) {
            if (IsInsideActiveEdges(ae, svgX)) {
                d = -d;
                break;
            }
        }

        pixels[x].a = htons(std::numeric_limits<uint16_t>::max() * (0.5 * std::min(std::max((scale * d) / range, -1.0), 1.0) + 0.5));
    }
}

bool GeneratePNG(NSVGimage* svg, FILE* f, size_t outputWidth, size_t outputHeight, double range) {
    std::vector<std::vector<Pixel>> pixels;

    auto pngWrite = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!pngWrite) {
        fprintf(stderr, "unable to allocate png write struct\n");
        return false;
    }

    auto pngInfo = png_create_info_struct(pngWrite);
    if (!pngInfo) {
        fprintf(stderr, "unable to allocate png info struct\n");
        png_destroy_write_struct(&pngWrite, nullptr);
        return false;
    }

    if (setjmp(png_jmpbuf(pngWrite))) {
        fprintf(stderr, "error generating png\n");
        png_free_data(pngWrite, pngInfo, PNG_FREE_ALL, -1);
        png_destroy_write_struct(&pngWrite, nullptr);
        return false;
    }

    png_init_io(pngWrite, f);
    png_set_IHDR(pngWrite, pngInfo, outputWidth, outputHeight, 16, PNG_COLOR_TYPE_GRAY_ALPHA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
    png_write_info(pngWrite, pngInfo);

    pixels.resize(outputHeight);

    for (auto& v : pixels) {
        v.resize(outputWidth);
    }

    std::vector<std::thread> threads;

    std::mutex stdoutMutex;

    std::atomic_uintmax_t numComplete{0};

    auto work = [&](auto y1, auto y2) {
        for (size_t y = y1; y < y2; ++y) {
            double svgY = 0.5 + (double)y * svg->height / outputHeight;
            GetRowPixels(pixels[y], svg, svgY, range);

            std::lock_guard<std::mutex> lock(stdoutMutex);
            auto num = numComplete++;
            printf("\b\b\b\b\b\b\b\b\b\b\b\b\b%zu / %zu", num, outputHeight);
            fflush(stdout);
        }
    };

    const auto numThreads = std::thread::hardware_concurrency();

    for (int i = 0; i < numThreads; ++i) {
        threads.push_back(std::thread([=]() { work(i * (outputWidth / numThreads), i == numThreads - 1 ? outputWidth : (i + 1) * (outputWidth / numThreads)); }));
    }

    for (auto& t : threads) {
        t.join();
    }

    printf("\b\b\b\b\b\b\b\b\b\b\b\b\b");

    for (auto& r : pixels) {
        png_write_row(pngWrite, reinterpret_cast<png_const_bytep>(r.data()));
    }

    png_write_end(pngWrite, nullptr);

    png_free_data(pngWrite, pngInfo, PNG_FREE_ALL, -1);
    png_destroy_write_struct(&pngWrite, nullptr);
    return true;
}

int main(int argc, const char* argv[]) {
    if (argc < 6) {
        printf("usage: %s input output width height range\n", argv[0]);
        return 1;
    }

    const auto input = argv[1];
    const auto output = argv[2];
    const auto outputWidth = atoi(argv[3]);
    const auto outputHeight = atoi(argv[4]);
    const auto range = atoi(argv[5]);

    auto svg = nsvgParseFromFile(input, "px", 96.0);
    if (!svg) {
        fprintf(stderr, "unable to open svg\n");
        return 1;
    }

    auto f = fopen(output, "wb");
    if (!f) {
        fprintf(stderr, "unable to open output file for writing\n");
        nsvgDelete(svg);
        return 1;
    }

    auto success = GeneratePNG(svg, f, outputWidth, outputHeight, range);

    fclose(f);
    nsvgDelete(svg);

    return success ? 0 : 1;
}
