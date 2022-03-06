#include "include/background.hpp"
#include "include/camera.hpp"
#include "include/box2d_utils.hpp"
#include <ctime>
#include <random>

/* GRAPHICS */

ktp::BackgroundGraphicsComponent::BackgroundGraphicsComponent() {
  constexpr auto size {2.5f};
  vertices_data_ = {
    // shape
     0.0f * size,  0.5f * size, 0.f,  // 0 top
     0.5f * size,  0.0f * size, 0.f,  // 1 right
     0.0f * size, -0.5f * size, 0.f,  // 2 bottom
    -0.5f * size,  0.0f * size, 0.f,  // 3 left
  };
  vertices_.setup(vertices_data_);
  // vertices
  vao_.linkAttrib(vertices_, 0, 3, GL_FLOAT, 3 * sizeof(GLfloat), nullptr);
  // EBO
  indices_.setup(indices_data_);
}

void ktp::BackgroundGraphicsComponent::update(const GameEntity& background) {
  shader_.use();
  vao_.bind();
  glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(indices_data_.size()), GL_UNSIGNED_INT, 0, stars_count_);
}

/* PHYSICS */

ktp::BackgroundPhysicsComponent::BackgroundPhysicsComponent(GameEntity* owner, BackgroundGraphicsComponent* graphics): graphics_(graphics) {
  owner_ = owner;
  generateStars();
  graphics_->stars_count_ = stars_.size();
  // subdata
  graphics_->vao_.bind();
  graphics_->subdata_.setup(nullptr, subdata_.size() * sizeof(GLfloat), GL_STREAM_DRAW);
  // subdata translations
  graphics_->vao_.linkAttrib(graphics_->subdata_, 1, 2, GL_FLOAT, kComponents_ * sizeof(GLfloat), nullptr);
  glVertexAttribDivisor(1, 1);
  // subdata colors
  graphics_->vao_.linkAttrib(graphics_->subdata_, 2, 4, GL_FLOAT, kComponents_ * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));
  glVertexAttribDivisor(2, 1);
}

ktp::BackgroundPhysicsComponent& ktp::BackgroundPhysicsComponent::operator=(BackgroundPhysicsComponent&& other) {
  if (this != &other) {
    // inherited members
    body_     = other.body_;
    collided_ = other.collided_;
    delta_    = std::move(other.delta_);
    owner_    = std::exchange(other.owner_, nullptr);
    size_     = other.size_;
    // own members
    graphics_ = std::exchange(other.graphics_, nullptr);
    stars_    = std::move(other.stars_);
    subdata_  = std::move(other.subdata_);
  }
  return *this;
}

void ktp::BackgroundPhysicsComponent::generateStars() {
  #if __GNUC__ || __MINGW32__
    std::mt19937 generator(time(nullptr));
  #else
    std::random_device seed; // this gives the same output in GCC 8.1.0
    std::mt19937 generator(seed());
  #endif
  std::uniform_int_distribution<unsigned int> distribution_stars(0u, 6000u);
  std::uniform_int_distribution<unsigned int> distribution_colors(0u, graphics_->star_colors_.size() - 1u);
  std::uniform_real_distribution<float> distribution_delta(0.001f, 0.1f);
  Star star {};

  for (auto i = 0; i < (int)(b2_screen_size_.x * kMetersToPixels); ++i) {
    for (auto j = 0; j < (int)(b2_screen_size_.y * kMetersToPixels); ++j) {
      if (distribution_stars(generator) == 1) {
        star.position_.x = (float)i;
        star.position_.y = (float)j;
        star.position_.z = 0.f;
        subdata_.push_back(star.position_.x);
        subdata_.push_back(star.position_.y);
        star.delta_ = {0.f, -distribution_delta(generator)};
        if (stars_.size() % 2 == 0 && stars_.size() % 3 == 0) {
          star.color_ = Palette::colorToGlmVec4(graphics_->star_colors_.data()[distribution_colors(generator)]);
        } else {
          star.color_ = Palette::colorToGlmVec4(Palette::white);
        }
        subdata_.push_back(star.color_.r);
        subdata_.push_back(star.color_.g);
        subdata_.push_back(star.color_.b);
        subdata_.push_back(star.color_.a);
        stars_.push_back(star);
      }
    }
  }
}

void ktp::BackgroundPhysicsComponent::update(const GameEntity& background, float delta_time) {
  for (std::size_t i = 0; i < stars_.size(); ++i) {
    if (stars_[i].position_.y < 0.f) {
      stars_[i].position_.y = b2_screen_size_.y * kMetersToPixels;
      subdata_[i * kComponents_ + 1] = stars_[i].position_.y;
    } else {
      stars_[i].position_.y += stars_[i].delta_.y * delta_time;
      subdata_[i * kComponents_ + 1] = stars_[i].position_.y;
    }
  }
  graphics_->subdata_.setupSubData(subdata_.data(), subdata_.size() * sizeof(GLfloat));
  updateMVP();
}

void ktp::BackgroundPhysicsComponent::updateMVP() {
  glm::mat4 model {1.f};
  const glm::mat4 mvp {camera_.projectionMatrix() * camera_.viewMatrix() * model};
  graphics_->shader_.use();
  graphics_->shader_.setMat4f("mvp", glm::value_ptr(mvp));
}
