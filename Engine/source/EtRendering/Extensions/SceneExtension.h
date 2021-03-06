#pragma once


namespace et {
namespace render {


//----------------------
// I_SceneExtension
//
// Interface for a data structure that extends the render scene
//
class I_SceneExtension
{
public:
	virtual ~I_SceneExtension() = default;

	virtual core::HashString GetId() const = 0;
};


} // namespace render
} // namespace et
