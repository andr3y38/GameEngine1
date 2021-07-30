#pragma once

namespace GE {
	struct Vertex {
		float x, y, z;
		float u, v;


		Vertex(float _x, float _y, float _z, float _u, float _v) {
			// Location
			x = _x;
			y = _y;
			z = _z;
			// Colour
			u = _u;
			v = _v;
			
		}

		Vertex() {
			x = y = z = 0.0f;
			u = v = 0.0f;
		}

	};

	class Model {
	public:
		Model() {
			vertices = nullptr;
			numVertices = 0;
		}
		
		
		~Model() {

			delete[] vertices;

		}

		bool loadFromFile(const char* filename);

		void* getVertices() {
			return (void*)vertices;
		}

		int getNumVertices() {
			return numVertices;
		}
	private:
		Vertex* vertices;
		int numVertices;






	};

}