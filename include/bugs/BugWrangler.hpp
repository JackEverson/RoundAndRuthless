#include "IndexBuffer.hpp"
#include "VertexBuffer.hpp"
#include "VertexArray.hpp"
#include "Shader.hpp"
#include "Texture.hpp"

// virtual class for bug renders
class BugWrangler {

public:
	virtual ~BugWrangler() = default;

	virtual void Render() = 0;
	virtual void AddBug(int index) = 0;



private:



};