#include <Scene.hpp>

Scene::~Scene()
{
	for(auto p : _lights)
		delete p;
}

void Scene::init()
{
	_pointLightBuffer.init();
	_pointLightBuffer.bind(1);
}
	
void Scene::updateLights()
{
	// Updates GPU Buffer
	for(size_t i = 0; i < _lights.size(); ++i)
	{
		if(_dirtyLights)
		{
			_lights[i]->getGPUBuffer().bind(i + 2);
			_lights[i]->updateMatrices();
		}
	}
	for(size_t i = 0; i < _omniLights.size(); ++i)
	{
		if(_dirtyLights)
		{
			_omniLights[i].getGPUBuffer().bind(i + 12);
			_omniLights[i].updateMatrices();
		}
	}
	_dirtyLights = false;
	
	// Update Shadow maps
	for(auto l : getLights())
		if(l->dynamic) // Only if asked to
		{
			l->updateMatrices();
			l->drawShadowMap(getObjects());
		}
	
	for(auto& l : getOmniLights())
		if(l.dynamic)
		{
			l.updateMatrices();
			l.drawShadowMap(getObjects());
		}
}

void Scene::update()
{
	if(_dirtyLights)
		updateLights();
	if(_dirtyPointLights)
		updatePointLightBuffer();
}

void Scene::draw(const glm::mat4& p, const glm::mat4& v) const
{
	if(_skybox)
		_skybox.draw(p, v);

	for(const auto& o : _objects)
	{
		if(o.isVisible(p, v))
			o.draw();
	}
}
