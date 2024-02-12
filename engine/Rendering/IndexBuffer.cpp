#include "IndexBuffer.h"

IndexBuffer::IndexBuffer(GLuint* indices, GLsizei count) : Count(count) {	
	glGenBuffers(1, &IndexBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(GLuint), indices, GL_STATIC_DRAW);
}
IndexBuffer::~IndexBuffer() {
	glDeleteBuffers(1, &IndexBufferID);
}

// Bind the Index buffer
void IndexBuffer::Bind() const {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferID);
}

// Unbind the Index buffer
void IndexBuffer::Unbind() const {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}