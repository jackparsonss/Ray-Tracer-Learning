#pragma once
#include <memory>
#include <glm/glm.hpp>
#include "Camera.h"

#include "Ray.h"

#include "Walnut/Image.h"

class Renderer
{
public:
	Renderer() = default;

	void on_resize(uint32_t width, uint32_t height);
	void render(const Camera& camera);
	std::shared_ptr<Walnut::Image> get_final_image() const { return m_FinalImage; }
private:
	glm::vec4 trace_ray(const Ray& ray);

	std::shared_ptr<Walnut::Image> m_FinalImage;
	uint32_t* m_ImageData = nullptr;
};
