#include "game.h"
using namespace cephalon;

#include <iostream>

#include "input.h"

Game::Game(int width, int height) {
    camera_.position = { 0.0f, 3.0f, 0.0f };
    camera_.pitch = -15.0f;
    camera_.aspect = static_cast<float>(width) / height;

    chunks_program_ = LoadProgram("vs_chunks", "fs_chunks");

    world_.setGenerator([](Vec3i pos, Chunk& chunk) {
        for (int x = 0; x < Chunk::kChunkSize.x; ++x) {
            for (int z = 0; z < Chunk::kChunkSize.x; ++z) {
                chunk.setBlock(Vec3i(x, 1, z), blocks::kGrass);
                chunk.setBlock(Vec3i(x, 0, z), blocks::kDirt);
            }
        }
        chunk.setBlock(Vec3i(0, 2, 0), blocks::kStone);
    });
}

void Game::update(float delta) {
    camera_.update();
    const auto camera_speed = 15.0f;
    if (Input::getKey(Key::kW))
        camera_.position = bx::add(camera_.position, bx::mul(camera_.direction, camera_speed * delta));
    if (Input::getKey(Key::kS))
        camera_.position = bx::sub(camera_.position, bx::mul(camera_.direction, camera_speed * delta));
    if (Input::getKey(Key::kA))
        camera_.position = bx::sub(camera_.position, bx::mul(camera_.right, camera_speed * delta));
    if (Input::getKey(Key::kD))
        camera_.position = bx::add(camera_.position, bx::mul(camera_.right, camera_speed * delta));

    const auto mouseSensitivity = 0.3f;

    auto xoffset = Input::getRelativeMouseX() * mouseSensitivity;
    auto yoffset = Input::getRelativeMouseY() * mouseSensitivity;

    camera_.yaw -= xoffset;
    camera_.pitch -= yoffset;

    if (camera_.pitch > 89.0f)
        camera_.pitch = 89.0f;
    if (camera_.pitch < -89.0f)
        camera_.pitch = -89.0f;

    world_.update(camera_.position);

    auto bounding_boxes = world_.getBoundingBoxes(AABB(camera_.position - Vec3i(Config::kDestroyDistance), camera_.position + Vec3i(Config::kDestroyDistance)));
    for (auto bounding_box : bounding_boxes) {
        if (bounding_box.intersect(camera_.position, camera_.direction, 0.0f, Config::kDestroyDistance)) {
            if (Input::getMouseLeft()) {
                world_.setBlock(bounding_box.min, blocks::kAir);
            }
        }
    }
}

void Game::render() {
    bgfx::setViewTransform(0, camera_.view, camera_.proj);
    world_.render(chunks_program_);
}

Game::~Game() {
    bgfx::destroy(chunks_program_);
}