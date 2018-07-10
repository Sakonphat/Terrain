#ifndef TERRAIN
#define TERRAIN

#define _USE_MATH_DEFINES
#include <cmath>

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#define sgn(x)  __max(-1, __min(1, x))

glm::vec3 computeNormal(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3);


void GenerateTerrain(int size, float jagged, std::vector<glm::vec3> & out_vertices, std::vector<glm::vec3> & out_normals
							, std::vector<unsigned int> &out_indices);



#endif