#ifndef STRING_AXIOMS_H
#define STRING_AXIOMS_H

#include <vector>
#include <string>
#include <iostream>

#include <glm/gtx/component_wise.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/glm.hpp>

#define TREE_LINE_COUNT 24.0f

using namespace std;

class Rules
{
private:
	vector<string> rules_container;

public:
	Rules();
	const vector<string>& getRules() const;
	const string& getRule(int index) const; 
};

class String_Axioms
{
private:
	vector<vector<char>> levels;
	const Rules& rules;
	
	glm::mat4 axis_system;
	float initial_radius; 
	float branch_length;

	void add_axiom0(string axiom);
	void create_depth_n(int n);
	void create_next_depth();
public:
	glm::vec4 initial_pos;

	std::vector<glm::vec4> tree_vertices;
	String_Axioms(string axiom, const Rules& rules_start, glm::vec4 initial_pos_);

	void generateCylinderLines(glm::vec4 start_pos, glm::vec4 end_pos, glm::mat4 Axis, float radius);
	void recur_tree(int depth, int& pos, glm::vec4 start_pos, glm::mat4 Axis, float radius, float move);
	void reset_tree();
	glm::vec3 getCenter();

};



#endif