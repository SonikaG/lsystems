#include "string_axioms.h"

#include <ctype.h>

using namespace std;

void print_vec2(glm::vec3 vec, char* name)
{
	std::cout << name << ": " << vec.x << " " << vec.y << " " << vec.z << "\n";
}

void print_vec2(glm::vec4 vec, char* name)
{
	std::cout << name << ": " << vec.x << " " << vec.y << " " << vec.z << "\n";
}

void print_matrix2(glm::mat4 matrix)
{
	std::cout << "|" << matrix[0].x << " " << matrix[1].x << " " << matrix[2].x << " " << matrix[3].x << "|\n";
	std::cout << "|" << matrix[0].y << " " << matrix[1].y << " " << matrix[2].y << " " << matrix[3].y << "|\n";
	std::cout << "|" << matrix[0].z << " " << matrix[1].z << " " << matrix[2].z << " " << matrix[3].z << "|\n";
	std::cout << "|" << matrix[0].w << " " << matrix[1].w << " " << matrix[2].w << " " << matrix[3].w << "|\n";
}

void print_all_chars(const vector<char>& my_chars)
{
	for(int i = 0; i < my_chars.size(); ++i)
	{
		cout << my_chars[i];
	}

	cout << "\n";
}

void String_Axioms::create_next_depth()
{
	vector<char> next_depth;

	int pos = levels.size() - 1;

	for(int i = 0; i < levels[pos].size(); ++i)
	{
		// if it's 0-9
		if(isdigit(levels[pos][i]))
		{
			int digit = levels[pos][i] - '0';

			for(int j = 0; j < rules.getRule(digit).length(); ++j)
			{
				next_depth.push_back(rules.getRule(digit)[j]);
			}

		}
		else
		{
			next_depth.push_back(levels[pos][i]);
		}
	}

	levels.push_back(next_depth);	
}

void String_Axioms::create_depth_n(int n)
{
	int depth = levels.size();

	for(int i = depth; i <= n; ++i)
	{
		create_next_depth();
	}

}

void String_Axioms::add_axiom0(string axiom)
{
	int i = 0;

	vector<char> base_depth;

	for(i = 0; i < axiom.length(); ++i)
	{
		base_depth.push_back(axiom[i]);
	}

	levels.push_back(base_depth);
}

String_Axioms::String_Axioms(string axiom, const Rules& rules_start): rules(rules_start), levels()
{

	add_axiom0(axiom);

	print_all_chars(levels[0]);

	create_depth_n(4);

	print_all_chars(levels[1]);

	print_all_chars(levels[2]);

	print_all_chars(levels[3]);

	print_all_chars(levels[4]);

	glm::vec4 start_pos = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

	glm::vec4 start_pos1 = glm::vec4(30.0f, 0.0f, 0.0f, 1.0f);

	glm::vec4 start_pos2 = glm::vec4(0.0f, 0.0f, 30.0f, 1.0f);

	glm::vec4 start_pos3 = glm::vec4(-30.0f, 0.0f, 0.0f, 1.0f);

	//generateCylinderLines(start_pos, end_pos, glm::mat4(1.0f), 2.0f);

	int pos = 0;

	recur_tree(1, pos, start_pos1, glm::mat4(1.0f), 0.5f, 3.0f);

	pos = 0;

	recur_tree(2, pos, start_pos2, glm::mat4(1.0f), 0.5f, 3.0f);

	pos = 0;

	recur_tree(3, pos, start_pos3, glm::mat4(1.0f), 0.5f, 3.0f);
	
	pos = 0;

	recur_tree(4, pos, start_pos, glm::mat4(1.0f), 0.5f, 3.0f);
}

void String_Axioms::generateCylinderLines(glm::vec4 start_pos, glm::vec4 end_pos, glm::mat4 Axis, float radius)
{
	//Axis = glm::rotate(Axis, (float) (M_PI/3), glm::vec3(0.0f, 0.0f, 1.0f));

	//float radius = 0.1f;
	float angle;
	//float length = glm::length(end_pos - start_pos);
	//std::cout << "length: " << length << "\n";

	// Translation vector the start joint
	glm::mat4 T1(1.0f);
	T1[3] = start_pos;

	glm::mat4 T2(1.0f);
	T2[3] = end_pos;

	glm::mat4 R = glm::inverse(Axis);

	glm::vec4 prev_start_joint;
	glm::vec4 prev_end_joint;

	bool start = true;

	for(angle = 0.0f; angle < (2.0f*M_PI); angle += 2.0f*M_PI/TREE_LINE_COUNT)
	{
		glm::vec4 start_joint = T1 * R * glm::vec4(radius*glm::cos(angle), 0.0f, radius*glm::sin(angle), 1.0f);
		glm::vec4 end_joint = T2 * R * glm::vec4(radius*glm::cos(angle), 0.0f, radius*glm::sin(angle), 1.0f);

		tree_vertices.push_back(start_joint);
		tree_vertices.push_back(end_joint);

		if(start)
		{
			start = false;
		} 
		else
		{
			tree_vertices.push_back(prev_start_joint);
			tree_vertices.push_back(start_joint);
			tree_vertices.push_back(prev_end_joint);
			tree_vertices.push_back(end_joint);
		}

		prev_start_joint = start_joint;
		prev_end_joint = end_joint;
	}
}

void String_Axioms::recur_tree(int depth, int& pos, glm::vec4 start_pos, glm::mat4 Axis, float radius, float move)
{
	glm::vec4 end_pos = start_pos;

	print_vec2(start_pos, "start pos");
	print_matrix2(Axis);
	std::cout << "AFTER\n\n";

	while(pos < levels[depth].size())
	{
	
		switch(levels[depth][pos++])
		{
		    case 'F': 
		    {
		    	// 		   Y-Axis
		    	end_pos += Axis[1] * move;
         		break;
         	}
         	case '+': 
		    {	//										   Z-Axis
		    	Axis = glm::rotate(Axis, (float) (M_PI/6), glm::vec3(Axis[2].x, Axis[2].y, Axis[2].z));
		    	//Axis = glm::rotate(Axis, (float) (M_PI/6), glm::vec3(0.0f, 0.0f, 1.0f));
         		break;
         	}
         	case '-': 
		    {
		    	//										    Z-Axis
		    	Axis = glm::rotate(Axis, (float) (-M_PI/6), glm::vec3(Axis[2].x, Axis[2].y, Axis[2].z));
		    	std::cout << "matrix goes left?\n";
		    	print_matrix2(Axis);
		    	//Axis = glm::rotate(Axis, (float) (-M_PI/6), glm::vec3(Axis[2].x, Axis[2].y, Axis[2].z));
         		break;
         	}
         	case '*': 
		    {
		    	//										   X-Axis
		    	Axis = glm::rotate(Axis, (float) (M_PI/6), glm::vec3(Axis[0].x, Axis[0].y, Axis[0].z));
         		break;
         	}
         	case '/': 
		    {
		    	//										    X-Axis
		    	Axis = glm::rotate(Axis, (float) (-M_PI/6), glm::vec3(Axis[0].x, Axis[0].y, Axis[0].z));
         		break;
         	}
         	case '[': 
		    {
		    	recur_tree(depth, pos, end_pos, Axis, radius, move);
         		break;
         	}
         	case ']': 
		    {
		    	return;
         	}
         	case 'T': 
         	case '0':
         	case '1':
         	case '2':
         	case '3':
         	case '4':
         	case '5':
         	case '6':
         	case '7':
         	case '8':
         	case '9':
		    {
		    	// 		   Y-Axis
		    	print_vec2(start_pos, "\tstart");
		    	print_vec2(end_pos, "\tend");
		    	print_matrix2(Axis);
		    	generateCylinderLines(start_pos, end_pos, Axis, radius);
         		break;
         	}

         	default: std::cout << "Uh oh!\n";

		}

	}
}

Rules::Rules(): rules_container()
{
	rules_container.push_back("F[-F1]F[+F2]F0");
	rules_container.push_back("F[-F1]F[+FT]F1");
	rules_container.push_back("F[-FT]F[+F2]F2");
}

const vector<string>& Rules::getRules() const
{
	return rules_container;
}

const string& Rules::getRule(int index) const
{
	return rules_container[index];
}



