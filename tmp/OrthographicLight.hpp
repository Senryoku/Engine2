#pragma once

#include <Camera.hpp>
#include <DirectionalLight.hpp>

/**
 * OrthographicLight
 *
 * Describes an OrthographicLight which can cast shadows (Variance Shadow Mapping).
**/
class OrthographicLight : public DirectionalLight
{
public:
	struct GPUData
	{
		glm::vec4	direction_info;
		glm::vec4	color_info;
		glm::mat4	depthMVP;
	};
	
	/**
	 * TEMP!
	 * Should be automatically computed.
	 * @todo Remove
	**/
	glm::vec3	_position= glm::vec3{0.0};
	float		_size = 50.0f;
	float		_near = 1.0f;
	float		_far = 500.0f;
	/// TEMP!
	/// @todo Remove
	OrthographicLight(unsigned int shadowMapResolution = 2048) :
		DirectionalLight(shadowMapResolution)
	{}
	
	/**
	 * Constructor
	 *
	 * @param target Camera used to compute the view and projection matrices
	 *					(so the light affected all visible objects).
	 * @param shadowMapResolution Resolution of the shadow map depth texture.
	**/
	OrthographicLight(const Camera& target, unsigned int shadowMapResolution = 2048);
	
	/**
	 * Destructor
	**/
	virtual ~OrthographicLight() =default;
	
	/**
	 * @return OrthographicLight's data structured for GPU use.
	**/
	inline GPUData getGPUData() const { return GPUData{glm::vec4(getDirection(), 0.0), 
															glm::vec4(glm::vec3(getColor()), 0.0), 
															getBiasedMatrix()}; }

	/**
	 * Updates OrthographicLight's internal transformation matrices according to
	 * its current direction.
	**/
	virtual void updateMatrices() override;
	
protected:
	const Camera*	_camera = nullptr;
};
