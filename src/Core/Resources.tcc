#pragma once

namespace Resources
{

template<typename ShaderType>
inline ShaderType& getShader(const std::string& name)
{
	auto it = _shaders.find(name);
	if(it != _shaders.end())
	{
		return *static_cast<ShaderType*>(it->second.get());
	} else {
		auto newShader = new ShaderType();
		_shaders[name].reset(newShader);
		return *newShader;
	}
} 

template<typename T>
inline T& getTexture(const std::string& name)
{
	auto it = _textures.find(name);
	if(it != _textures.end())
	{
		return *static_cast<T*>(it->second.get());
	} else {
		auto newTexture = new T();
		_textures[name].reset(newTexture);
		return *newTexture;
	}
} 

template<typename ShaderType>
ShaderType& load(const std::string& path)
{
	return load<ShaderType>(path, path);
}

template<typename ShaderType>
ShaderType& load(const std::string& name, const std::string& path)
{
	auto& s = Resources::getShader<ShaderType>(name);
	if(s) return s; // Already loaded.
	s.loadFromFile(path);
	s.compile();
	if(!s)
	{
		std::cerr << "Error compiling '" << path << "'. Exiting..." << std::endl;
		exit(1);
	}
	return s;
}

template<typename ... ShaderTypes>
Program& loadProgram(const std::string& name, ShaderTypes& ... shaders)
{
	auto& p = Resources::getProgram(name);
	if(p) return p; // Already loaded.
	p.attach_chain(shaders...);
	p.link();
	if(!p)
	{
		std::cerr << "Error linking GLSL program. Exiting..." << std::endl;
		exit(1);
	}
	return p;
}

} // Namespace Resources
