
#include <iostream>
#include <glm/vec3.hpp>
#include "SpringApplication.h"


template <typename T, glm::precision P>
std::ostream& operator << (std::ostream& output, const glm::detail::tvec3<T,P>& v) {
	output << '[';
	for( glm::length_t i=0; i<v.length(); ++i ) {
		if(i > 0) output << ", ";
		output << v[i];
	}
	output << ']';
	return output;
}



int main(int argc, char** argv)
{
	SpringApplication app;
    return app.SystemExecute();
}
