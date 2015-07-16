#include <GUIElement.hpp>

#include <ResourcesManager.hpp>

bool GUIElement::handleKey(int key, int scancode, int action, int mods)
{
	return false;
}

bool GUIElement::handleTextInput(unsigned int unicode)
{
	return false;
}

bool GUIElement::handleClick(const glm::vec2& coords, int button)
{
	auto c = p2c(coords);
	if(_aabb.contains(c))
		return onClick(c, button);
	return false;
}

bool GUIElement::onClick(const glm::vec2& coords, int button)
{
	return false;
};

void GUIElement::drawAABB(const glm::vec2& resolution, const glm::vec2& position, const glm::vec4& color) const
{
	auto& P = ResourcesManager::getInstance().getProgram("GUIRectangle");
	if(!P)
	{
		P = loadProgram("GUIRectangle",
				load<VertexShader>("src/GLSL/GUI/rect_vs.glsl"),
				load<GeometryShader>("src/GLSL/GUI/rect_gs.glsl"),
				load<FragmentShader>("src/GLSL/GUI/rect_fs.glsl")
			);
	}

	P.use();
	P.setUniform("Resolution", resolution);
	P.setUniform("Position", position);
	P.setUniform("Min", _aabb.min);
	P.setUniform("Max", _aabb.max);
	P.setUniform("Color", color);
	glDrawArrays(GL_POINTS, 0, 1); // Dummy draw call
	P.useNone();
}
