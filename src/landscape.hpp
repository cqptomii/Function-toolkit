#ifndef TERRAIN_HPP
#define TERRAIN_HPP

#include "noise/noise.hpp"
#include "noise/perlin.hpp"
#include "utils/Math_toolkit.hpp"
#include <algorithm>
#include <numeric>
#include <vector>

struct Vec2 {
    double x;
    double y;
};

class Landscape {

    int octaves = 6;
    double amplitude = 1.0;
    double frequency = 1.0;
    double persistence = 0.5;
    double lacunarity = 2.00;

    Noise* continent_noise = nullptr;
    Noise* terrain_noise = nullptr;
    Noise* erosion_noise = nullptr;
    Noise* peaks_noise = nullptr;
    Noise* detail_noise = nullptr;
    Noise* river_noise = nullptr;

    double scale = 0.01;
    double max_height = 255.0;
    double sea_level = 0.30;
    
    double spline(double x){
        if (x < 0.3) return 0.0;         // océan
        if (x < 0.5) return (x - 0.3) * 50; // plage
        if (x < 0.7) return 10 + (x - 0.5) * 100; // collines
        return 30 + (x - 0.7) * 300;     // montagnes
    }

public:
    Landscape(NoiseType type, unsigned int seed){
        switch (type) {
            case NoiseType::Perlin:
                continent_noise = new PerlinNoise(seed);
                terrain_noise = new PerlinNoise(seed + 1);
                erosion_noise = new PerlinNoise(seed + 2);
                peaks_noise = new PerlinNoise(seed + 3, 7.0, 1.0, 1.0, 0.52, 2.05);
                detail_noise = new PerlinNoise(seed + 4);
                river_noise = new PerlinNoise(seed + 5);
                break;
            // Add cases for other noise types as needed
            default:
                continent_noise = new PerlinNoise(seed);
                terrain_noise = new PerlinNoise(seed + 1);
                erosion_noise = new PerlinNoise(seed + 2);
                peaks_noise = new PerlinNoise(seed + 3, 7.0, 1.0, 1.0, 0.52, 2.05);
                detail_noise = new PerlinNoise(seed + 4);
                river_noise = new PerlinNoise(seed + 5);
                break;
        }
    }
    ~Landscape(){
        delete continent_noise;
        delete terrain_noise;
        delete erosion_noise;
        delete peaks_noise;
        delete detail_noise;
        delete river_noise;
    };
    double **generate_landscape(int width, int height){
        const int size = width * height;
        std::vector<double> base(size, 0.0);
        std::vector<double> flow_acc(size, 1.0);
        std::vector<int> downslope(size, -1);
        std::vector<int> order(size);

        auto index_of = [width](int x, int y) {
            return y * width + x;
        };

        for (int i = 0; i < width; i++) {
            for (int j = 0; j < height; j++) {
                double x = i * scale;
                double y = j * scale;

                // Coarse continental shape keeps oceans and large land masses.
                double continent = continent_noise->fbm(x * 0.11, y * 0.11, 22.0);
                continent = (continent + 1.0) * 0.5;
                continent = std::pow(continent, 1.15);
                double land_mask = Math_toolkit::smooth_step(0.20, 0.70, continent);

                double macro = terrain_noise->warping(x * 0.19, y * 0.19, 12.0);
                macro = (macro + 1.0) * 0.5;

                double mountains = peaks_noise->warping2(x * 0.05, y * 0.05, 71.0);
                mountains = (mountains + 1.0) * 0.5;
                mountains = std::pow(mountains, 3.0);

                double detail = detail_noise->warping(x * 0.5, y * 0.5, 130.0);
                detail = (detail + 1.0) * 0.5;

                double erosion = erosion_noise->warping(x * 0.08, y * 0.08, 40.0);
                erosion = (erosion + 1.0) * 0.5;

                double h = (0.4 + 0.58 * macro + 0.72 * mountains + 0.18 * detail) * Math_toolkit::lerp(0.10, 1.0, land_mask);
                h *= Math_toolkit::lerp(0.70, 1.0, erosion);
                h = std::clamp(h, 0.0, 1.0);

                base[index_of(i, j)] = h;
            }
        }

        // Build a D8 flow graph by routing each cell to its steepest downhill neighbor.
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                const int idx = index_of(x, y);
                const double h = base[idx];
                double lowest_h = h;
                int target = -1;

                for (int dy = -1; dy <= 1; dy++) {
                    for (int dx = -1; dx <= 1; dx++) {
                        if (dx == 0 && dy == 0) {
                            continue;
                        }
                        const int nx = x + dx;
                        const int ny = y + dy;
                        if (nx < 0 || ny < 0 || nx >= width || ny >= height) {
                            continue;
                        }

                        const int nidx = index_of(nx, ny);
                        const double nh = base[nidx];
                        if (nh < lowest_h) {
                            lowest_h = nh;
                            target = nidx;
                        }
                    }
                }

                downslope[idx] = target;
            }
        }

        std::iota(order.begin(), order.end(), 0);
        std::sort(order.begin(), order.end(), [&](int a, int b) {
            return base[a] > base[b];
        });

        for (int idx : order) {
            const int dst = downslope[idx];
            if (dst >= 0) {
                flow_acc[dst] += flow_acc[idx];
            }
        }

        double max_flow = 1.0;
        for (int idx = 0; idx < size; idx++) {
            if (base[idx] > sea_level) {
                max_flow = std::max(max_flow, flow_acc[idx]);
            }
        }

        double **map = new double*[width];
        for (int i = 0; i < width; i++) {
            map[i] = new double[height];
            for (int j = 0; j < height; j++) {
                const int idx = index_of(i, j);
                double h = base[idx];

                double x = i * scale;
                double y = j * scale;

                const double flow_norm = std::log1p(flow_acc[idx]) / std::log1p(max_flow);
                const double trunk = Math_toolkit::smooth_step(0.48, 0.90, flow_norm);
                const double inland = Math_toolkit::smooth_step(sea_level + 0.02, 0.72, h);
                const double mouth = 1.0 - Math_toolkit::smooth_step(sea_level + 0.03, sea_level + 0.18, h);

                // Thin tributaries from warped lines, reinforced by real flow accumulation.
                double river_line = 1.0 - std::abs(river_noise->warping(x * 0.12, y * 0.12, 33.0));
                river_line = std::pow(std::clamp(river_line, 0.0, 1.0), 7.0);

                double carve = (0.012 * river_line + 0.085 * trunk) * inland;
                carve *= Math_toolkit::lerp(1.0, 1.65, mouth);

                if (h > sea_level + 0.004) {
                    h -= carve;
                }

                // Subtle floodplains around major rivers in lowlands.
                const double lowland = 1.0 - Math_toolkit::smooth_step(0.52, 0.78, h);
                h -= trunk * lowland * 0.02;

                map[i][j] = std::clamp(h, 0.0, 1.0) * max_height;
            }
        }

        return map;
    }

    int **generate_biome_map(int width, int height, double **height_map) {
        int **map = new int*[width];

        const int size = width * height;
        std::vector<double> temp_raw(size, 0.0);
        std::vector<double> moist_raw(size, 0.0);
        std::vector<double> temp_smooth(size, 0.0);
        std::vector<double> moist_smooth(size, 0.0);

        auto index_of = [width](int x, int y) {
            return y * width + x;
        };

        // Pass 1: low-frequency climate fields.
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                const int idx = index_of(x, y);
                const double xf = x * scale;
                const double yf = y * scale;
                const double h = std::clamp(height_map[x][y] / max_height, 0.0, 1.0);
                const double latitude = std::abs((static_cast<double>(y) / static_cast<double>(height - 1)) * 2.0 - 1.0);

                // Warp climate sampling to avoid long latitude stripes.
                double warp_x = erosion_noise->fbm(xf * 0.008, yf * 0.008, 301.0);
                double warp_y = terrain_noise->fbm(xf * 0.008, yf * 0.008, 509.0);
                const double wx = xf + warp_x * 1.8;
                const double wy = yf + warp_y * 1.8;

                double jet = continent_noise->fbm(xf * 0.012, yf * 0.012, 777.0);
                jet = (jet + 1.0) * 0.5;
                const double latitude_warped = std::clamp(latitude + (jet - 0.5) * 0.24, 0.0, 1.0);

                double continental = continent_noise->fbm(wx * 0.016, wy * 0.016, 411.0);
                continental = (continental + 1.0) * 0.5;

                double temp_noise = terrain_noise->fbm(wx * 0.022, wy * 0.022, 91.0);
                temp_noise = (temp_noise + 1.0) * 0.5;

                double moisture_noise = detail_noise->fbm(wx * 0.035, wy * 0.035, 17.0);
                moisture_noise = (moisture_noise + 1.0) * 0.5;

                temp_raw[idx] = std::clamp((1.0 - latitude_warped) * 0.56 + temp_noise * 0.32 + continental * 0.12 - h * 0.34, 0.0, 1.0);
                moist_raw[idx] = std::clamp((1.0 - h) * 0.22 + moisture_noise * 0.58 + (1.0 - continental) * 0.20, 0.0, 1.0);
            }
        }

        // Pass 2: box blur 3x3 to avoid biome "salt and pepper".
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                double t_sum = 0.0;
                double m_sum = 0.0;
                int count = 0;

                for (int dy = -1; dy <= 1; dy++) {
                    for (int dx = -1; dx <= 1; dx++) {
                        const int nx = x + dx;
                        const int ny = y + dy;
                        if (nx < 0 || ny < 0 || nx >= width || ny >= height) {
                            continue;
                        }

                        const int nidx = index_of(nx, ny);
                        t_sum += temp_raw[nidx];
                        m_sum += moist_raw[nidx];
                        count++;
                    }
                }

                const int idx = index_of(x, y);
                temp_smooth[idx] = (count > 0) ? (t_sum / static_cast<double>(count)) : temp_raw[idx];
                moist_smooth[idx] = (count > 0) ? (m_sum / static_cast<double>(count)) : moist_raw[idx];
            }
        }

        // Pass 3: biome classification from smoothed climate.
        for (int x = 0; x < width; x++) {
            map[x] = new int[height];
            for (int y = 0; y < height; y++) {
                const int idx = index_of(x, y);
                const double xf = x * scale;
                const double yf = y * scale;
                const double h = std::clamp(height_map[x][y] / max_height, 0.0, 1.0);

                double biome_jitter = detail_noise->fbm(xf * 0.018, yf * 0.018, 999.0);
                biome_jitter = (biome_jitter + 1.0) * 0.5;

                const double temperature = std::clamp(temp_smooth[idx] + (biome_jitter - 0.5) * 0.08, 0.0, 1.0);
                const double moisture = std::clamp(moist_smooth[idx] + (0.5 - biome_jitter) * 0.08, 0.0, 1.0);

                int biome = 0;
                if (h < 0.24) biome = 0; // deep ocean
                else if (h < sea_level) biome = 1; // shallow ocean
                else if (h < sea_level + 0.03) biome = 2; // beach
                else if (h > 0.82) biome = (temperature < 0.35) ? 10 : 9; // snow / alpine
                else if (temperature < 0.22) biome = 8; // tundra
                else if (temperature < 0.40) biome = (moisture < 0.44) ? 11 : 7; // steppe / taiga
                else if (temperature < 0.64) biome = (moisture < 0.28) ? 3 : ((moisture < 0.62) ? 4 : 5); // desert / grassland / forest
                else biome = (moisture < 0.26) ? 3 : ((moisture < 0.50) ? 6 : 5); // hot desert / savanna / rainforest

                map[x][y] = biome;
            }
        }

        return map;
    }

    double **noise_map(int width, int height){
        double **map = new double*[width];
        for (int i = 0; i < width; i++) {
            map[i] = new double[height];
            for (int j = 0; j < height; j++) {
                
                double x = i * scale;
                double y = j * scale;

                double h = this->continent_noise->fbm(x, y,0.0);
                h = (h + 1.0) * 0.5;
                map[i][j] = h * max_height;
            }
        }
        return map;
    }
    double **height_map(int width, int height){
        double **map = new double*[width];
        for (int i = 0; i < width; i++) {
            map[i] = new double[height];
           
            for (int j = 0; j < height; j++) {
                
                double x = i * scale;
                double y = j * scale;

                // --- Continents ---
                double continent = this->continent_noise->warping(x * 0.1, y * 0.1, 3.45);
                continent = (continent + 1.0) * 0.5;
                continent = pow(continent, 1.5);

                // --- Mask terre ---
                double mask = Math_toolkit::smooth_step(0.3, 0.7, continent);

                // --- Relief ---
                double terrain = this->erosion_noise->fbm(x * 0.3, y * 0.3, 1.2456);
                terrain = (terrain + 1.0) * 0.5;

                // --- Montagnes ---
                double mountains = this->peaks_noise->fbm(x * 0.05, y * 0.05, 0.0);
                mountains = (mountains + 1.0) * 0.5;
                mountains = pow(mountains, 3.0);

                // Full-map style: preserve landform everywhere and use continent as modulation.
                double height = (terrain * 0.75 + mountains * 0.9) * Math_toolkit::lerp(0.45, 1.0, mask);
                height = std::clamp(height, 0.0, 1.0);

                map[i][j] = height * max_height;
            }
        }
        return map;
    }
};

#endif //TERRAIN_HPP