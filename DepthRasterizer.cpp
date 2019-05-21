#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdint.h>
#include <float.h>
#include <math.h>
#include <string.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

int32_t MinInt32(int32_t a, int32_t b)
{
    return a < b ? a : b;
}
int32_t MaxInt32(int32_t a, int32_t b)
{
    return a > b ? a : b;
}

template<class T>
T EdgeFunction(T x0, T y0, T x1, T y1, T x2, T y2)
{
    return (x2 - x0) * (y1 - y0) - (y2 - y0) * (x1 - x0);
}
bool IsTopLeftEdge(int32_t x0, int32_t y0, int32_t x1, int32_t y1)
{
    return (y0 == y1 && x0 > x1) || y0 < y1;
}

int main(int argc, char* argv[])
{
    const uint32_t width = 640;
    const uint32_t height = 480;
    const uint32_t pixel_count = width * height;

    float* depth = new float[pixel_count];
    // Clear depth buffer
    for (uint32_t i = 0; i < pixel_count; ++i)
    {
        depth[i] = 1.0f;
    }

    const glm::vec3 vertices_in_world_space[] =
    {
        glm::vec3(-1.0f, -1.0f, -1.0f),
        glm::vec3(-1.0f, -1.0f,  1.0f),
        glm::vec3(-1.0f,  1.0f, -1.0f),
        glm::vec3(-1.0f,  1.0f,  1.0f),
        glm::vec3( 1.0f, -1.0f, -1.0f),
        glm::vec3( 1.0f, -1.0f,  1.0f),
        glm::vec3( 1.0f,  1.0f, -1.0f),
        glm::vec3( 1.0f,  1.0f,  1.0f),
    };
    const uint32_t indices[] =
    {
        0, 1, 2, 1, 3, 2,
        4, 6, 5, 5, 6, 7,
        0, 5, 1, 0, 4, 5,
        2, 7, 6, 2, 3, 7,
        0, 6, 4, 0, 2, 6,
        1, 7, 3, 1, 5, 7,
    };
    const uint32_t vertex_count = sizeof(vertices_in_world_space) / sizeof(glm::vec3);
    const uint32_t triangle_count = sizeof(indices) / (sizeof(uint32_t) * 3);

    glm::mat4 view = glm::lookAt(glm::vec3(4.0f, 4.0f, -4.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), static_cast<float>(width) / static_cast<float>(height), 1.0f, 100.0f);
    glm::mat4 view_proj = proj * view;

    glm::vec4 vertices_in_clip_space[vertex_count];
    for (uint32_t i = 0; i < vertex_count; ++i)
    {
        vertices_in_clip_space[i] = view_proj * glm::vec4(vertices_in_world_space[i], 1.0f);
    }

    // Iterate over all triangles
    for (uint32_t tri = 0; tri < triangle_count; ++tri)
    {
        float x0f = vertices_in_clip_space[indices[tri * 3 + 0]].x;
        float y0f = vertices_in_clip_space[indices[tri * 3 + 0]].y;
        float z0f = vertices_in_clip_space[indices[tri * 3 + 0]].z;
        float w0f = vertices_in_clip_space[indices[tri * 3 + 0]].w;

        float x1f = vertices_in_clip_space[indices[tri * 3 + 1]].x;
        float y1f = vertices_in_clip_space[indices[tri * 3 + 1]].y;
        float z1f = vertices_in_clip_space[indices[tri * 3 + 1]].z;
        float w1f = vertices_in_clip_space[indices[tri * 3 + 1]].w;

        float x2f = vertices_in_clip_space[indices[tri * 3 + 2]].x;
        float y2f = vertices_in_clip_space[indices[tri * 3 + 2]].y;
        float z2f = vertices_in_clip_space[indices[tri * 3 + 2]].z;
        float w2f = vertices_in_clip_space[indices[tri * 3 + 2]].w;

        // TODO: Clipping

        // Perspective divide
        x0f /= w0f; y0f /= w0f; z0f /= w0f;
        x1f /= w1f; y1f /= w1f; z1f /= w1f;
        x2f /= w2f; y2f /= w2f; z2f /= w2f;

        // Convert XY coordinates to NDC space
        x0f = (0.5f + 0.5f * x0f); y0f = (0.5f - 0.5f * y0f);
        x1f = (0.5f + 0.5f * x1f); y1f = (0.5f - 0.5f * y1f);
        x2f = (0.5f + 0.5f * x2f); y2f = (0.5f - 0.5f * y2f);

        // Convert XY coordinates to screen space
        x0f = x0f * width; y0f = y0f * height;
        x1f = x1f * width; y1f = y1f * height;
        x2f = x2f * width; y2f = y2f * height;

        // Convert XY coordinates to integers (round down)
        int32_t x0i = static_cast<int32_t>(x0f - 0.5f);
        int32_t y0i = static_cast<int32_t>(y0f - 0.5f);
        int32_t x1i = static_cast<int32_t>(x1f - 0.5f);
        int32_t y1i = static_cast<int32_t>(y1f - 0.5f);
        int32_t x2i = static_cast<int32_t>(x2f - 0.5f);
        int32_t y2i = static_cast<int32_t>(y2f - 0.5f);

        // Compute screen space bounds
        int32_t x_min = MinInt32(x0i, MinInt32(x1i, x2i));
        int32_t y_min = MinInt32(y0i, MinInt32(y1i, y2i));
        int32_t x_max = MaxInt32(x0i, MaxInt32(x1i, x2i));
        int32_t y_max = MaxInt32(y0i, MaxInt32(y1i, y2i));

        // Early out if triangle is out of screen
        if (x_min > width - 1 || y_min > height - 1 || x_max < 0 || y_max < 0)
            continue;

        // Clamp to screen bounds
        x_min = x_min < 0 ? 0 : x_min;
        y_min = y_min < 0 ? 0 : y_min;
        x_max = x_max > width - 1 ? width - 1 : x_max;
        y_max = y_max > height - 1 ? height - 1 : y_max;

        // Compute double triangle area using edge function
        float double_area = EdgeFunction<float>(x0f, y0f, x1f, y1f, x2f, y2f);

        // Backface culling
        if (double_area <= 0.0f)
            continue;

        // Precompute Z vectors
        float z10f = (z1f - z0f) / double_area;
        float z20f = (z2f - z0f) / double_area;

        // Calculate bias for fill rule
        int32_t bias0 = IsTopLeftEdge(x1i, y1i, x2i, y2i) ? 0 : 1;
        int32_t bias1 = IsTopLeftEdge(x2i, y2i, x0i, y0i) ? 0 : 1;
        int32_t bias2 = IsTopLeftEdge(x0i, y0i, x1i, y1i) ? 0 : 1;

        // Iterate over all pixels within the bounds
        for (int32_t y = y_min; y <= y_max; ++y)
        {
            for (int32_t x = x_min; x <= x_max; ++x)
            {
                // Compute barycentric coordinates
                // Can be optimized by precomputing starting point + increments
                int32_t w0 = EdgeFunction<int32_t>(x1i, y1i, x2i, y2i, x, y);
                int32_t w1 = EdgeFunction<int32_t>(x2i, y2i, x0i, y0i, x, y);
                int32_t w2 = EdgeFunction<int32_t>(x0i, y0i, x1i, y1i, x, y);

                // Test if pixel is inside triangle
                if (w0 < bias0 || w1 < bias1 || w2 < bias2)
                    continue;

                // Calculate depth value and compare against depth buffer
                float z = z0f + z10f * w1 + z20f * w2;
                if (z < depth[y * width + x])
                {
                    depth[y * width + x] = z;
                }
            }
        }
    }

    // Find minimum and maximum Z values
    float z_min = 1.0f;
    float z_max = 0.0f;
    for (uint32_t y = 0; y < height; ++y)
    {
        for (uint32_t x = 0; x < width; ++x)
        {
            float z = depth[y * width + x];
            if (z_min > z && z >= 0.0f)
                z_min = z;
            if (z_max < z && z < 1.0f)
                z_max = z;
        }
    }

    // Calculate Z range
    float z_range = fmaxf(z_max - z_min, FLT_EPSILON);
    float z_range_rcp = 1.0f / z_range;

    // Open file
    const char* filepath = "depth.ppm";
    FILE* file = fopen(filepath, "w");
    if (file == NULL)
    {
        return 1;
    }

    // Write PPM header
    fprintf(file, "P3\n%lu %lu\n255\n", width, height);

    // Write pixel data
    for (uint32_t y = 0; y < height; ++y)
    {
        for (uint32_t x = 0; x < width; ++x)
        {
            float z = fmaxf(0.0f, fminf(1.0f, (depth[y * width + x] - z_min) * z_range_rcp));
            uint8_t r = static_cast<uint8_t>(z * 255.0f + 0.5f);
            uint8_t g = static_cast<uint8_t>(z * 255.0f + 0.5f);
            uint8_t b = static_cast<uint8_t>(z * 255.0f + 0.5f);
            fprintf(file, "%lu %lu %lu\n", r, g, b);
        }
    }

    // Close file
    fclose(file);

    delete[] depth;

    return 0;
}