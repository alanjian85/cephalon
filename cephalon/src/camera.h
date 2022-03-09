#ifndef CEPHALON_CAMERA_H_
#define CEPHALON_CAMERA_H_

#include <bgfx/bgfx.h>
#include <bx/math.h>

namespace cephalon {
    class PerspectiveCamera {
    public:
        PerspectiveCamera(bx::Vec3 position = bx::Vec3(0.0f), float yaw = 90.0f, float pitch = 0.0f, 
                          float fov = 45.0f, float aspect = 0.0f, float near = 0.1f, float far = 100.0f)
            : position_(position), direction_(0.0f), right_(0.0f)
        {
            yaw_ = yaw;
            pitch_ = pitch;

            fov_ = fov;
            aspect_ = aspect;
            near_ = near;
            far_ = far;
        }

        void setPosition(bx::Vec3 position) {
            position_ = position;
        }

        bx::Vec3 getPosition() const {
            return position_;
        }

        void setYaw(float yaw) {
            yaw_ = yaw;
        }

        float getYaw() const {
            return yaw_;
        }

        void setPitch(float pitch) {
            pitch_ = pitch;
        }

        float getPitch() const {
            return pitch_;
        }

        void setAspect(float aspect) {
            aspect_ = aspect;
        }

        void update() {
            direction_ = bx::Vec3(
                bx::cos(bx::toRad(yaw_)) * bx::cos(bx::toRad(pitch_)),
                bx::sin(bx::toRad(pitch_)),
                bx::sin(bx::toRad(yaw_)) * bx::cos(bx::toRad(pitch_))
            );
            right_ = bx::normalize(bx::cross(bx::Vec3(0.0f, 1.0f, 0.0f), direction_));
            bx::mtxLookAt(view_, position_, bx::add(position_, direction_));
            bx::mtxProj(proj_, fov_, aspect_, near_, far_, bgfx::getCaps()->homogeneousDepth);
        }

        auto getDirection() const {
            return direction_;
        }

        auto getRight() const {
            return right_;
        }

        auto getView() const {
            return view_;
        }

        auto getProj() const {
            return proj_;
        }
    private:
        bx::Vec3 position_;

        float yaw_;
        float pitch_;

        float fov_;
        float aspect_;
        float near_;
        float far_;

        bx::Vec3 direction_;
        bx::Vec3 right_;
        float view_[16];
        float proj_[16];
    };
}

#endif // CEPHALON_CAMERA_H_