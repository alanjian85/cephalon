#include "chunk.h"
using namespace cephalon;

#include <iterator>

#include "utils.h"

bgfx::VertexLayout Chunk::layout_;
bgfx::ProgramHandle Chunk::program_;

void Chunk::init() {
    layout_.begin()
        .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
        .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
    .end();
    program_ = LoadProgram("vs_chunks", "fs_chunks");
}

void Chunk::cleanup() {
    bgfx::destroy(program_);
}

Chunk::Chunk()
{
    dirty_ = false;
    blocks_.resize(kChunkSize.x * kChunkSize.y * kChunkSize.z, &blocks::kAir);
    vertex_buffer_ = bgfx::createDynamicVertexBuffer(0u, layout_, BGFX_BUFFER_ALLOW_RESIZE);
    index_buffer_ = bgfx::createDynamicIndexBuffer(0u, BGFX_BUFFER_ALLOW_RESIZE);
}

Chunk::~Chunk() noexcept {
    bgfx::destroy(vertex_buffer_);
    bgfx::destroy(index_buffer_);
}

void Chunk::setBlock(Vec3i pos, const Block& block) {
    assert(pos.x >= 0 && pos.x < kChunkSize.x);
    assert(pos.y >= 0 && pos.y < kChunkSize.y);
    assert(pos.z >= 0 && pos.z < kChunkSize.z);
    auto index = pos.z * kChunkSize.x * kChunkSize.y + pos.y * kChunkSize.x + pos.x;
    if (blocks_[index] != &block) {
        dirty_ = true;
        blocks_[index] = &block;
    }
}

const Block& Chunk::getBlock(Vec3i pos) const {
    assert(pos.x >= 0 && pos.x < kChunkSize.x);
    assert(pos.y >= 0 && pos.y < kChunkSize.y);
    assert(pos.z >= 0 && pos.z < kChunkSize.z);
    return *blocks_[pos.z * kChunkSize.x * kChunkSize.y + pos.y * kChunkSize.x + pos.x];
}

void Chunk::rebuild() {
    std::vector<Vertex> vertices;
    std::vector<std::uint16_t> indices;

    for (int x = 0; x < kChunkSize.x; ++x) {
        for (int y = 0; y < kChunkSize.y; ++y) {
            for (int z = 0; z < kChunkSize.z; ++z) {
                auto& block = getBlock(Vec3i(x, y, z));
                if (!block.isAir()) {
                    auto color = block.getColor();

                    // right
                    if (x == kChunkSize.x - 1 || getBlock(Vec3i(x + 1, y, z)).isAir()) {
                        Vertex block_vertices[] = {
                            { bx::Vec3(x + 0.5f, y - 0.5f, z - 0.5f), color },
                            { bx::Vec3(x + 0.5f, y - 0.5f, z + 0.5f), color },
                            { bx::Vec3(x + 0.5f, y + 0.5f, z - 0.5f), color },
                            { bx::Vec3(x + 0.5f, y + 0.5f, z + 0.5f), color }
                        };

                        auto index_base = static_cast<int>(vertices.size());
                        int block_indices[] = {
                            index_base + 1, index_base + 3, index_base + 2,
                            index_base + 1, index_base + 2, index_base + 0
                        };

                        vertices.insert(vertices.cend(), std::cbegin(block_vertices), std::cend(block_vertices));
                        indices.insert(indices.cend(), std::cbegin(block_indices), std::cend(block_indices));
                    }

                    // left
                    if (x == 0 || getBlock(Vec3i(x - 1, y, z)).isAir()) {
                        Vertex block_vertices[] = {
                            { bx::Vec3(x - 0.5f, y - 0.5f, z - 0.5f), color },
                            { bx::Vec3(x - 0.5f, y - 0.5f, z + 0.5f), color },
                            { bx::Vec3(x - 0.5f, y + 0.5f, z - 0.5f), color },
                            { bx::Vec3(x - 0.5f, y + 0.5f, z + 0.5f), color }
                        };

                        auto index_base = static_cast<int>(vertices.size());
                        int block_indices[] = {
                            index_base + 0, index_base + 2, index_base + 3,
                            index_base + 0, index_base + 3, index_base + 1,
                        };

                        vertices.insert(vertices.cend(), std::cbegin(block_vertices), std::cend(block_vertices));
                        indices.insert(indices.cend(), std::cbegin(block_indices), std::cend(block_indices));
                    }

                    // top
                    if (y == kChunkSize.y - 1 || getBlock(Vec3i(x, y + 1, z)).isAir()) {
                        Vertex block_vertices[] = {
                            { bx::Vec3(x - 0.5f, y + 0.5f, z - 0.5f), color },
                            { bx::Vec3(x - 0.5f, y + 0.5f, z + 0.5f), color },
                            { bx::Vec3(x + 0.5f, y + 0.5f, z - 0.5f), color },
                            { bx::Vec3(x + 0.5f, y + 0.5f, z + 0.5f), color }
                        };

                        auto index_base = static_cast<int>(vertices.size());
                        int block_indices[] = {
                            index_base + 2, index_base + 3, index_base + 1,
                            index_base + 2, index_base + 1, index_base + 0,
                        };

                        vertices.insert(vertices.cend(), std::cbegin(block_vertices), std::cend(block_vertices));
                        indices.insert(indices.cend(), std::cbegin(block_indices), std::cend(block_indices));
                    }

                    // bottom
                    if (y == 0 || getBlock(Vec3i(x, y - 1, z)).isAir()) {
                        Vertex block_vertices[] = {
                            { bx::Vec3(x - 0.5f, y - 0.5f, z - 0.5f), color },
                            { bx::Vec3(x - 0.5f, y - 0.5f, z + 0.5f), color },
                            { bx::Vec3(x + 0.5f, y - 0.5f, z - 0.5f), color },
                            { bx::Vec3(x + 0.5f, y - 0.5f, z + 0.5f), color }
                        };

                        auto index_base = static_cast<int>(vertices.size());
                        int block_indices[] = {
                            index_base + 3, index_base + 2, index_base + 0,
                            index_base + 3, index_base + 0, index_base + 1,
                        };

                        vertices.insert(vertices.cend(), std::cbegin(block_vertices), std::cend(block_vertices));
                        indices.insert(indices.cend(), std::cbegin(block_indices), std::cend(block_indices));
                    }

                    // back
                    if (z == kChunkSize.z - 1 || getBlock(Vec3i(x, y, z + 1)).isAir()) {
                        Vertex block_vertices[] = {
                            { bx::Vec3(x - 0.5f, y - 0.5f, z + 0.5f), color },
                            { bx::Vec3(x - 0.5f, y + 0.5f, z + 0.5f), color },
                            { bx::Vec3(x + 0.5f, y - 0.5f, z + 0.5f), color },
                            { bx::Vec3(x + 0.5f, y + 0.5f, z + 0.5f), color }
                        };

                        auto index_base = static_cast<int>(vertices.size());
                        int block_indices[] = {
                            index_base + 1, index_base + 3, index_base + 2,
                            index_base + 0, index_base + 1, index_base + 2,
                        };

                        vertices.insert(vertices.cend(), std::cbegin(block_vertices), std::cend(block_vertices));
                        indices.insert(indices.cend(), std::cbegin(block_indices), std::cend(block_indices));
                    }

                    // front
                    if (z == 0 || getBlock(Vec3i(x, y, z - 1)).isAir()) {
                        Vertex block_vertices[] = {
                            { bx::Vec3(x - 0.5f, y - 0.5f, z - 0.5f), color },
                            { bx::Vec3(x - 0.5f, y + 0.5f, z - 0.5f), color },
                            { bx::Vec3(x + 0.5f, y - 0.5f, z - 0.5f), color },
                            { bx::Vec3(x + 0.5f, y + 0.5f, z - 0.5f), color }
                        };

                        auto index_base = static_cast<int>(vertices.size());
                        int block_indices[] = {
                            index_base + 2, index_base + 3, index_base + 1,
                            index_base + 2, index_base + 1, index_base + 0
                        };

                        vertices.insert(vertices.cend(), std::cbegin(block_vertices), std::cend(block_vertices));
                        indices.insert(indices.cend(), std::cbegin(block_indices), std::cend(block_indices));
                    }
                }
            }
        }
    }

    if (!vertices.empty()) {
        bgfx::update(vertex_buffer_, 0, bgfx::copy(vertices.data(), vertices.size() * sizeof(Vertex)));
        bgfx::update(index_buffer_, 0, bgfx::copy(indices.data(), indices.size() * sizeof(std::uint16_t)));
    }
    dirty_ = false;
}

void Chunk::render() {
    bgfx::setVertexBuffer(0, vertex_buffer_);
    bgfx::setIndexBuffer(index_buffer_);
    bgfx::submit(0, program_);
}