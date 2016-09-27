#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <memory>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <Graphics/UniformGLM.hpp>
#include <Shaders.hpp>
#include <Uniform.hpp>
#include <Texture2D.hpp>
#include <Texture3D.hpp>

#include <Log.hpp>

/**
 * Material
 * Association of a Shader program and a set of Uniforms.
 * @see Program
 * @see Uniform
**/
class Material
{
public:
	//	Constructors
	Material() =default;
	Material(const Program& P);
	Material(Material&&) =default;
	
	/**
	 * Copy constructor
	**/
	Material(const Material&);
	
	/**
	 * Default destructor
	**/
	~Material() =default;
	
	Material& operator=(const Material&);
	Material& operator=(Material&&) =default;

	//	Getters/Setters
	const Program& getShadingProgram() const;
	void setShadingProgram(const Program& P);
	
	const Program* getShadingProgramPtr() const;
	void setShadingProgramPtr(const Program* P);
	
	////////////////////////////////////////////////////////////////
	// Special cases for textures
	
	void setUniform(const std::string& name, const Texture3D& value);
	void setUniform(const std::string& name, const Texture2D& value);
	void setUniform(const std::string& name, const CubeMap& value);
	void setUniform(const std::string& name, const Texture& value);
	
	////////////////////////////////////////////////////////////////
	// Generic Uniform setting
	
	template<typename T>
	inline T getUniform(const std::string&);
	
	template<typename T>
	inline Uniform<T>* searchUniform(const std::string&);
	
	template<typename T>
	inline void setUniform(const std::string& name, const T& value);
	
	inline void setSubroutine(ShaderType stage, const std::string& uniformName, const std::string& functionName);
	
	inline void use() const;
	
	inline void useNone() const;
	
	void bind() const;
	
	void unbind() const;
	
	void updateLocations();

private:
	const Program*	_shadingProgram = nullptr;
	
	std::vector<std::unique_ptr<GenericUniform>>	_uniforms;
	GLuint 											_textureCount = 0;
	
	class SubroutineState
	{
	public:
		ShaderType							shadertype;
		std::map<std::string, std::string>	activeSubroutines;
		std::vector<GLuint>					activeIndices;

		inline void use() const
		{
			glUniformSubroutinesuiv(to_underlying(shadertype), activeIndices.size(), activeIndices.data());
		}

		void update(const Program& p);
	};
	
	std::map<ShaderType, SubroutineState>	_subroutines;
	
	GLint getLocation(const std::string& name) const;
};

inline const Program& Material::getShadingProgram() const
{
	assert(_shadingProgram != nullptr);
	return *_shadingProgram;
}

inline const Program* Material::getShadingProgramPtr() const
{
	assert(_shadingProgram != nullptr);
	return _shadingProgram;
}


inline void Material::setShadingProgram(const Program& P)
{
	_shadingProgram = &P;
	updateLocations();
}

inline void Material::setShadingProgramPtr(const Program* P)
{
	_shadingProgram = P;
	updateLocations();
}

inline void Material::setUniform(const std::string& name, const Texture3D& value)
{
	setUniform(name, static_cast<const Texture&>(value));
}

inline void Material::setUniform(const std::string& name, const Texture2D& value)
{
	setUniform(name, static_cast<const Texture&>(value));
}

inline void Material::setUniform(const std::string& name, const CubeMap& value)
{
	setUniform(name, static_cast<const Texture&>(value));
}

inline void Material::setUniform(const std::string& name, const Texture& value)
{
	GLint Location = getLocation(name);
	
	if(Location >= 0)
	{
		for(auto& U : _uniforms)
		{
			if(U.get()->getName() == name)
			{
				static_cast<Uniform<Texture>*>(U.get())->setValue(value);
				return;
			}
		}
		_uniforms.push_back(std::unique_ptr<GenericUniform>(new Uniform<Texture>(name, Location, _textureCount, value)));
		++_textureCount;
	} else {
		Log::error("Material: Uniform ", name, " not found.");
	}
}

////////////////////////////////////////////////////////////////
// Generic Uniform setting

template<typename T>
T Material::getUniform(const std::string& name)
{
	for(const auto& u : _uniforms)
	{
		if(u.get()->getName() == name)
			return dynamic_cast<Uniform<T>*>(u.get())->getValue();
	}
	Log::error("Uniform ", name, " not found.");
	return T{};
}

template<typename T>
Uniform<T>* Material::searchUniform(const std::string& name)
{
	for(const auto& u : _uniforms)
	{
		if(u.get()->getName() == name)
			return dynamic_cast<Uniform<T>*>(u.get());
	}
	return nullptr;
}

template<typename T>
void Material::setUniform(const std::string& name, const T& value)
{
	GLint Location = getLocation(name);
	
	if(Location >= 0)
	{
		for(auto& U : _uniforms)
		{
			if(U.get()->getName() == name)
			{
				//static_cast<Uniform<T>*>(U.get())->setValue(value);
				// In case the types differs... We better create a shinny new one.
				U.reset(new Uniform<T>(name, Location, value));
				
				return;
			}
		}
		_uniforms.push_back(std::unique_ptr<GenericUniform>(new Uniform<T>(name, Location, value)));
	} else {
		Log::error("Warning: Uniform '", name, "' not found (", __PRETTY_FUNCTION__, ").");
	}
}

inline void Material::setSubroutine(ShaderType stage, const std::string& uniformName, const std::string& functionName)
{
	auto& tmp = _subroutines[stage];
	tmp.shadertype = stage;
	tmp.activeSubroutines[uniformName] = functionName;
	tmp.update(*_shadingProgram);
}
	
inline void Material::use() const
{
	if(_shadingProgram != nullptr)
		_shadingProgram->use();
	
	for(const auto& s : _subroutines)
		s.second.use();
		
	bind();
}

inline void Material::useNone() const
{
	unbind();
	Program::useNone();
}