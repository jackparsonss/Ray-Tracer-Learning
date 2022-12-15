#include"Renderer.h"

#include"Walnut/Random.h"

namespace utils
{
	static uint32_t convert_to_RGBA(glm::vec4 color)
	{
		uint8_t r = (uint8_t)(color.r * 255.0f);
		uint8_t g = (uint8_t)(color.g * 255.0f);
		uint8_t b = (uint8_t)(color.b * 255.0f);
		uint8_t a = (uint8_t)(color.a * 255.0f);

		uint32_t result = (a << 24) | (b << 16) | (g << 8) | r;
		return result;
	}
}

void Renderer::render(const Camera& camera)
{
	Ray ray;
	ray.Origin = camera.GetPosition();

	// render every pixel
	// y on outside to take advantage of spatial locality
	for(uint32_t y = 0; y < m_FinalImage->GetHeight(); y++)
	{
		for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++)
		{
			// glm::vec2 coord = { (float)x / (float)m_FinalImage->GetWidth(), (float)y / (float)m_FinalImage->GetHeight() };
			// coord = coord * 2.0f - 1.0f; // -1 -> 1
			//
			// float aspect_ratio = m_FinalImage->GetWidth() / (float)m_FinalImage->GetHeight();
			// coord.x *= aspect_ratio; // prevent stretching
			ray.Direction = camera.GetRayDirections()[x + y * m_FinalImage->GetWidth()];

			glm::vec4 color = trace_ray(ray);
			color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));

			m_ImageData[x + y * m_FinalImage->GetWidth()] = utils::convert_to_RGBA(color);
		}
	}

	m_FinalImage->SetData(m_ImageData);
}

void Renderer::on_resize(uint32_t width, uint32_t height)
{
	if(m_FinalImage)
	{
		if (m_FinalImage->GetWidth() == width && m_FinalImage->GetHeight() == height)
			return;

		m_FinalImage->Resize(width, height);
	}
	else
	{
		m_FinalImage = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
	}

	delete[] m_ImageData;
	m_ImageData = new uint32_t[width * height];
}

glm::vec4 Renderer::trace_ray(const Ray& ray)
{
	glm::vec3 sphere_origin(0.0f, 0.0f, 0.0f);
	float radius = 0.5f;
	// ray_direction = glm::normalize(ray_direction); // causes a=1 always but more expensive computation than dot product
 
	// a = ray origin
	// b = ray direction
	// r = radius
	// t = hit distance
	
	// setup for quadratic equation
	float a = glm::dot(ray.Direction, ray.Direction);
	float b = 2.0f * glm::dot(ray.Origin, ray.Direction);
	float c = glm::dot(ray.Origin, ray.Origin) - radius * radius;

	// use quadratic equation to check disc
	float discriminant = b * b - 4.0f * a * c;

	if(discriminant < 0.0f)
	{
		return glm::vec4(0, 0, 0, 1);
	}

	// solve quadratic equation
	float t0 = (-b + glm::sqrt(discriminant)) / (2.0f * a);
	float t1 = (-b - glm::sqrt(discriminant)) / (2.0f * a);

	// glm::vec3 hit_point0 = ray_origin + ray_direction * t0;
	glm::vec3 hit_point1 = ray.Origin + ray.Direction * t1; // closest hit(smallest value, closer to camera)
	glm::vec3 normal = glm::normalize(hit_point1 - sphere_origin);

	glm::vec3 light_direction = glm::normalize(glm::vec3(-1, -1, -1));

	float d = glm::max(glm::dot(normal, -light_direction), 0.0f); // same as cos(angle)j

	glm::vec3 sphereColor(0, 1, 1);
	sphereColor *= d;
	return glm::vec4(sphereColor, 1.0f);
}


