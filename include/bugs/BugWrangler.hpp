#ifndef BUG_WRANGLER_HPP
#define BUG_WRANGLER_HPP

#include "IndexBuffer.hpp"
#include "VertexBuffer.hpp"
#include "VertexArray.hpp"
#include "Shader.hpp"
#include "Texture.hpp"

// virtual class for bug renders
class BugWrangler {

public:
	
	virtual ~BugWrangler() = default;

	virtual void Render(const glm::mat4& view, const glm::mat4& projection) = 0;
	virtual void AddBug(int index, float x, float y) = 0;



private:



};

#endif