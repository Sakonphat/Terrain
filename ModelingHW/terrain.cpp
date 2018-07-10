
#include "terrain.h"
#include <time.h>

// ---------------------------------------------------------------
// Helper function to compute normal vector
// We need normal vector to shade the model (you will learn shading in API class)
// ---------------------------------------------------------------

glm::vec3 computeNormal(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3){
	glm::vec3 v21 = v2 - v1;
	glm::vec3 v31 = v3 - v1;
	return glm::normalize(glm::cross(v21, v31));
}




// ---------------------------------------------------------------
// Terrain topology is a 2d array of square grid, size x size
//	- size: size x size of the square terrain, size should be in [8, 1024]
//	- jagged: to control the roughness of the terrain or H reduce scale, value between [0..1]
// ---------------------------------------------------------------

void GenerateTerrain(int size, float jagged, std::vector<glm::vec3> & out_vertices, std::vector<glm::vec3> & out_normals
						, std::vector<unsigned int> &out_indices)
{
	// Clear the array first
	out_vertices.clear();
	out_normals.clear();
	out_indices.clear();

	// allocate memory 
	out_vertices.reserve((size+1)*(size+1));
	out_normals.reserve((size + 1)*(size + 1));
	out_indices.reserve(size*size*6);

	// construct the terrain grid mesh with vertex.y = 0
	//	- the boundary of the mesh is in [-40,40] for x, z coordinate
	glm::vec3 v1;
	for (int z = (size/2); z >= (-size/2); z--){
		for (int x = (-size/2); x <= (size/2); x++){
			v1 = glm::vec3(40.0 * x / size, 0, 40.0 * z / size - 40);
			out_vertices.push_back(v1);
			out_normals.push_back(glm::vec3(0,1,0));
		}
	}
	unsigned int i1, i2, i3;
	for (int row = 0; row < size; row++) {
		for (int col = 0; col < size; col++) {

			i1 = (size + 1)*row + col;
			i2 = (size + 1)*row + (col+1);
			i3 = (size + 1)*(row + 1) + (col + 1);
			out_indices.push_back(i1);
			out_indices.push_back(i2);
			out_indices.push_back(i3);

			i1 = (size + 1)*row + col;
			i2 = (size + 1)*(row + 1) + (col + 1);
			i3 = (size + 1)*(row + 1) + col;
			out_indices.push_back(i1);
			out_indices.push_back(i2);
			out_indices.push_back(i3);
		}
	}

	

	//+ set the y component of each vertex using Diamond-Square algorithm
	//random 4 coner duay
	unsigned int diamondIndex_topLeft, diamondIndex_topRight, diamondIndex_botLeft, diamondIndex_botRight;
	unsigned int sqIndex_top, sqIndex_left, sqIndex_right, sqIndex_bot;
	//size = 4;

	int currentSize = size;
	int midP = -1;

	float range = 5.0f;
	float random = 0.0f;

	bool first = true;

	float firstRandom = 0.0f;
	srand(time(NULL));
	firstRandom = (((rand() % 200) - 100) / 100.0f)*range;
	while (true) {
		
		//Diamond
		for (int x = 0; x < size; x += currentSize) {
			for (int y = 0; y < size; y += currentSize) {

				if (first == true) {
					//First find 4 conners
					
					diamondIndex_topRight = firstRandom;
					diamondIndex_topLeft = firstRandom;
					diamondIndex_botLeft = firstRandom;
					diamondIndex_botRight =  firstRandom;
					first = false;
				}
				if (first == false) {
					//find 4 conners
					diamondIndex_topRight = (x + currentSize) + (size + 1)*(y + currentSize);
					diamondIndex_topLeft = x + (size + 1)*(y + currentSize);
					diamondIndex_botLeft = x + (size + 1)*y;
					diamondIndex_botRight = (x + currentSize) + (size + 1)*y;
				}


				//calculate
				midP = (diamondIndex_topRight + diamondIndex_topLeft + diamondIndex_botLeft + diamondIndex_botRight) / 4;
				random = (((rand() % 200) - 100) / 100.0f)*range;
				printf("random diamond= %f", &random);
				//random = random * (1/pow(2, jagged));

				std::cout << "TopLeft = " << diamondIndex_topLeft << "  TopRight" << diamondIndex_topRight << "  BotLeft" << diamondIndex_botLeft << "  BotRight"
					<< diamondIndex_botRight << "  MidPoint = " << midP << "  Random = " << random << std::endl;

				out_vertices[midP].y = (out_vertices[diamondIndex_botLeft].y + out_vertices[diamondIndex_botRight].y +
										out_vertices[diamondIndex_topLeft].y + out_vertices[diamondIndex_topRight].y) / 4 + random;

			}
		}
		

		float avgSqure = 0.0f;
		
		//Square
		for (int y = 0; y <= size; y += currentSize/2) {
			for (int x = (y+ currentSize/2) % currentSize; x <= size; x += currentSize) {

				midP = x + (size + 1)*y;
				//find 4 square
				sqIndex_right = midP + currentSize / 2;
				sqIndex_left = midP - currentSize / 2;
				sqIndex_top = midP + (size + 1)*(currentSize / 2);
				sqIndex_bot = midP - (size + 1)*(currentSize / 2);
				
				random = (((rand() % 200) - 100) / 100.0f)*range;
				printf("random square= %f", &random);
				//random = random * (1/pow(2,jagged));
				if (midP < size) {
					avgSqure = ((out_vertices[sqIndex_right].y + out_vertices[sqIndex_left].y + out_vertices[sqIndex_top].y) / 3)+ random;
				}
				else if (midP % (size + 1) == 0) {
					avgSqure = ((out_vertices[sqIndex_right].y + out_vertices[sqIndex_top].y + out_vertices[sqIndex_bot].y) / 3)+ random;
				}
				else if (midP % (size+1) == size) {
					avgSqure = ((out_vertices[sqIndex_bot].y + out_vertices[sqIndex_left].y + out_vertices[sqIndex_top].y) / 3)+ random;
				}
				else if (midP > size) {
					avgSqure = ((out_vertices[sqIndex_right].y + out_vertices[sqIndex_left].y + out_vertices[sqIndex_bot].y) / 3)+ random;
				}


				std::cout << "MidPoint = " << midP << std::endl;
				out_vertices[midP].y = avgSqure;

			}
		}
		range = range*(1 / pow(2, jagged));
		currentSize = currentSize / 2;
		
		
		if (currentSize <= 1)
			break;
	}
	




}


