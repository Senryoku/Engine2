#include <Transformation.hpp>

Transformation::Transformation(const glm::mat4& m)
{
	setModelMatrix(m);
}

Transformation::Transformation(const glm::vec3& p, const glm::quat& r, const glm::vec3& s) :
	_position(p),
	_rotation(r),
	_scale(s)
{
	computeMatrix();
}

Transformation::~Transformation()
{
	if(_parent != invalid_component_idx)
	{
		auto& parent = get_component<Transformation>(_parent);
		parent.remChild(*this);
	
		for(auto& c : _children)
			parent.addChild(c);
	} else {
		for(auto& c : _children)
			get_component<Transformation>(c).setParent(invalid_component_idx);
	}
}

void Transformation::setModelMatrix(const glm::mat4& m)
{ 
	_modelMatrix = m;
	
	glm::vec3 skew;
	glm::vec4 perspective;
	glm::decompose(_modelMatrix, _scale, _rotation, _position, skew, perspective);
	updateGlobalModelMatrix();
}
	
void Transformation::addChild(Transformation& t)
{
	if(t._parent != invalid_component_idx)
		get_component<Transformation>(t._parent).remChild(t);
	_children.push_back(getID<Transformation>(t));
	t.setParent(*this);
}

void Transformation::addChild(ComponentID t)
{
	addChild(get_component<Transformation>(t));
}

void Transformation::remChild(const Transformation& t)
{
	remChild(getID<Transformation>(t));
}

void Transformation::remChild(ComponentID t)
{
	_children.erase(std::find(_children.begin(), _children.end(), t));
}

void Transformation::setParent(Transformation& t)
{
	setParent(getID(t));
}

void Transformation::setParent(ComponentID t)
{
	_parent = t;
	updateGlobalModelMatrix();
}

inline void Transformation::computeMatrix()
{
	_modelMatrix = glm::translate(glm::mat4(1.0f), _position) * 
		glm::mat4_cast(_rotation) * 
		glm::scale(glm::mat4(1.0f), _scale);
	updateGlobalModelMatrix();
}

inline void Transformation::updateGlobalModelMatrix()
{
	_globalModelMatrix = _parent == invalid_component_idx ? _modelMatrix :
		get_component<Transformation>(_parent).getGlobalModelMatrix() * _modelMatrix;
	for(ComponentID c : _children)
		get_component<Transformation>(c).updateGlobalModelMatrix();
}
