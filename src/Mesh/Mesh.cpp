#include "Mesh.h"
#include "../VertexBufferLayout.h"
#include "GeometryFactory.h"



Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
	: m_Vertices(vertices), m_Indices(indices)
{
	SetupMesh();
}


Mesh::~Mesh()
{
	m_VAO->unBind();
	m_VBO->Unbind();
	m_EBO->Unbind();
}

void Mesh::SetupMesh()
{
	if (m_Vertices.empty() || m_Indices.empty())
	{
		std::cerr << "Mesh::SetupMesh() was called with an empty vertex/index array " << std::endl;
		return;
	}

	//set up the buffers

	m_VAO = std::make_unique<VertexArray>();
	m_VBO = std::make_unique<VertexBuffer>(m_Vertices.data(), m_Vertices.size() * sizeof(Vertex));
	m_EBO = std::make_unique<IndexBuffer>(m_Indices.data(), m_Indices.size());


	VertexBufferLayout layout;
	layout.Push<float>(3); //position x,y,z
	layout.Push<float>(3); //normals nx, ny, nz
	layout.Push<float>(3); //colour r, g, b
	layout.Push<float>(2); //texture coordinates u, v
	layout.Push<float>(3); //tangent tx, ty, tz

	m_VAO->AddBuffer(*m_VBO, layout);

	m_VAO->unBind();
	m_VBO->Unbind();
	m_EBO->Unbind();

}

void Mesh::Draw() //but ultimately this is terrible and we will be making a better one in the future.
{
	if (!m_VAO || !m_EBO)
	{
		std::cerr << "Mesh::Draw() called before SetupMesh()!" << std::endl;
		return;
	}

	Renderer renderer;

	renderer.Draw(*m_VAO, *m_EBO);
}

glm::mat4 Mesh::getTransformMatrix() const
{
	glm::mat4 transform = glm::mat4(1.0f);


	transform = glm::translate(transform, m_Position); //translate our position

	transform = glm::rotate(transform, glm::radians(m_Rotation.x), glm::vec3(1, 0, 0));// X axis rotation (PITCH)
	transform = glm::rotate(transform, glm::radians(m_Rotation.y), glm::vec3(0, 1, 0));// Y axis rotation (YAW)
	transform = glm::rotate(transform, glm::radians(m_Rotation.z), glm::vec3(0, 0, 1));// Z axis rotation (ROLL)

	transform = glm::scale(transform, m_Scale); //translate our scale

	return transform;
}
