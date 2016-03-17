#include "../../stdafx.hpp"

#include "DiffuseMaterial.hpp"

#include "../../Graphics/MeshFilter.hpp"
#include "../../Graphics/TextureData.hpp"
#include "../../Graphics/ShaderData.hpp"

DiffuseMaterial::DiffuseMaterial() :
	Material("Resources/PosNormTex3D.glsl"),
	m_UseDifTex(false),
	m_AmbientColor(glm::vec3(0.f, 0.f, 0.f)),
	m_DiffuseColor(glm::vec3(0.65f, 0.65f, 0.65f)),
	m_SpecularColor(glm::vec3(1.0f, 1.0f, 1.0f))
{
	m_LayoutFlags = VertexFlags::POSITION | VertexFlags::NORMAL | VertexFlags::TEXCOORD;
}
DiffuseMaterial::DiffuseMaterial(std::string texDiffusePath):
	Material("Resources/PosNormTex3D.glsl"),
	m_TexDiffusePath(texDiffusePath),
	m_UseDifTex(true),
	m_AmbientColor(glm::vec3(0.f, 0.f, 0.f)),
	m_DiffuseColor(glm::vec3(0.65f, 0.65f, 0.65f)),
	m_SpecularColor(glm::vec3(1.0f, 1.0f, 1.0f))
{
	m_LayoutFlags = VertexFlags::POSITION | VertexFlags::NORMAL | VertexFlags::TEXCOORD;
}
DiffuseMaterial::~DiffuseMaterial()
{
}

void DiffuseMaterial::LoadTextures()
{
	glUseProgram(m_Shader->GetProgram());
	m_uUseDifTex = glGetUniformLocation(m_Shader->GetProgram(), "useDifTex");
	glUniform1i(m_uUseDifTex, m_UseDifTex);
	if (m_UseDifTex)
	{
		//Load Textures
		m_TexDiffuse = ContentManager::Load<TextureData>(m_TexDiffusePath);
		//Bind Textures to shader
		glUniform1i(glGetUniformLocation(m_Shader->GetProgram(), "texDiffuse"), 0);
	}
}

void DiffuseMaterial::AccessShaderAttributes()
{
	m_uLightDir = glGetUniformLocation(m_Shader->GetProgram(), "lightDir");
	m_uCamPos = glGetUniformLocation(m_Shader->GetProgram(), "camPos");

	m_uAmbCol = glGetUniformLocation(m_Shader->GetProgram(), "ambientColor");
	m_uDifCol = glGetUniformLocation(m_Shader->GetProgram(), "diffuseColor");
	m_uSpecCol = glGetUniformLocation(m_Shader->GetProgram(), "specularColor");
	m_uSpecPow = glGetUniformLocation(m_Shader->GetProgram(), "specularPower");
}

void DiffuseMaterial::UploadDerivedVariables()
{
	//Bind active textures
	if (m_UseDifTex)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_TexDiffuse->GetHandle());
	}
	//Upload uniforms
	glUniform3f(m_uLightDir, m_LightDir.x, m_LightDir.y, m_LightDir.z);
	glm::vec3 cPos = CAMERA->GetTransform()->GetPosition();
	glUniform3f(m_uCamPos, cPos.x, cPos.y, cPos.z);

	glUniform3f(m_uAmbCol, m_AmbientColor.x, m_AmbientColor.y, m_AmbientColor.z);
	glUniform3f(m_uDifCol, m_DiffuseColor.x, m_DiffuseColor.y, m_DiffuseColor.z);
	glUniform3f(m_uSpecCol, m_SpecularColor.x, m_SpecularColor.y, m_SpecularColor.z);
	glUniform1f(m_uSpecPow, m_SpecularPower);
}