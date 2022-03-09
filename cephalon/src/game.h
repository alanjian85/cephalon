#ifndef CEPHALON_GAME_H_
#define CEPHALON_GAME_H_

#include "camera.h"
#include "input.h"
#include "utils.h"

namespace cephalon {
    class Game {
    public:
        void init(int width, int height) {
            camera_.setPosition(bx::Vec3(0.0f, 0.0f, -3.0f));
            camera_.setAspect(static_cast<float>(width) / height);

            float vertices[] = {
                -0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f, 1.0f,
                 0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f, 1.0f,
                 0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f, 1.0f
            };

            layout_.begin()
                .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
                .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Float)
            .end();
            vbh_ = bgfx::createVertexBuffer(bgfx::copy(vertices, sizeof(vertices)), layout_);

            program_ = LoadProgram("vs_shader", "fs_shader");
        }

        void update(float delta) {
            camera_.update();
            const auto camera_speed = 2.5f;
            if (Input::getKey(Key::kW))
                camera_.setPosition(bx::add(camera_.getPosition(), bx::mul(camera_.getDirection(), camera_speed * delta)));
            if (Input::getKey(Key::kS))
                camera_.setPosition(bx::sub(camera_.getPosition(), bx::mul(camera_.getDirection(), camera_speed * delta)));
            if (Input::getKey(Key::kA))
                camera_.setPosition(bx::sub(camera_.getPosition(), bx::mul(camera_.getRight(), camera_speed * delta)));
            if (Input::getKey(Key::kD))
                camera_.setPosition(bx::add(camera_.getPosition(), bx::mul(camera_.getRight(), camera_speed * delta)));
        }

        void render() {
            bgfx::setViewTransform(0, camera_.getView(), camera_.getProj());
            bgfx::setVertexBuffer(0, vbh_);
            bgfx::submit(0, program_);
        }
    private:
        bgfx::VertexLayout layout_;
        bgfx::VertexBufferHandle vbh_;

        bgfx::ProgramHandle program_;
    
        PerspectiveCamera camera_;
    };
}

#endif // CEPHALON_GAME_H_