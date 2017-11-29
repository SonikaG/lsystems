#ifndef BONE_GEOMETRY_H
#define BONE_GEOMETRY_H

#include <ostream>
#include <vector>
#include <map>
#include <limits>
#include <glm/glm.hpp>
#include <mmdadapter.h>

#define BONE_LINE_COUNT 24.0f
#define BONE_RADIUS 0.1f
#define HIGHLIGHT_RADIUS 0.2f
#define AXIS_LENGTH 2.0f
#define TEST_BONE 54

class Ray;

void print_vec(glm::vec3 vec, char* name);
void print_vec(glm::vec4 vec, char* name);
void print_matrix(glm::mat4 matrix);

struct BoundingBox {
	BoundingBox()
		: min(glm::vec3(-std::numeric_limits<float>::max())),
		max(glm::vec3(std::numeric_limits<float>::max())) {}
	glm::vec3 min;
	glm::vec3 max;
};

struct Bone {
private:
	int jid;
	float Length;
	int bid = -1;
	
	glm::vec3 end_joint;
	glm::mat4 Axis;
	glm::mat4 T;
	glm::mat4 R;
	glm::mat4 S;
	glm::mat4 U;
	glm::mat4 D;
	std::vector<Bone*> bone_children;
	Bone* parent_bone;

	int smallest_mag(glm::vec3 my_vector);

public:
	Bone(int _jid, glm::vec3& offset, Bone* parent);
	~Bone();

	// Project 5
	void generateLBSMatrices(std::vector<glm::mat4>& Ds, std::vector<glm::mat4>& Us, glm::mat4 TRs, glm::mat4 TSs);

	Bone* findBoneTS(int _jid, glm::mat4& TSs);
	Bone* findBoneTR(int _jid, glm::mat4& TRs);

	glm::mat4 getTransformed();
	glm::mat4 getDeformed();

	void setTransformed(glm::mat4 TRs);
	void setDeformed(glm::mat4 TSs);

	const std::vector<Bone*>& getBoneChildren() const {return bone_children; } 

	int getBid() { return bid; }
	void setBid(int id) { bid = id; }

	// End of Project 5

	void addBone(int _jid, glm::vec3 offset);
	int getJid() {return jid;}
	bool hasParent() {return (parent_bone != NULL);}
	
	void addJointVertices(std::vector<glm::vec4>& skeleton_vertices, glm::mat4 TSs, std::string depth);
	
	void generateLine(std::vector<glm::vec4>& skeleton_vertices, glm::mat4 TSs);
	void generateCylinderLines(std::vector<glm::vec4>& skeleton_vertices, glm::mat4 TSs, float radius);
	void generateCylinderLinesRaw(std::vector<glm::vec4>& skeleton_vertices, float radius);
	void generateAxis(std::vector<glm::vec4>& skeleton_vertices, glm::mat4 TSs, float length);
	void generateNormalAxis(std::vector<glm::vec4>& skeleton_vertices, glm::mat4 TSs, float length);
	void generateBinormalAxis(std::vector<glm::vec4>& skeleton_vertices, glm::mat4 TSs, float length);

	void findBoneIntersect(const Ray& ray, Bone*& bone, float& t, glm::mat4 TSS, glm::mat4& TSs);
	void intersectRay(const Ray& ray, Bone*& bone, float& t, glm::mat4 TSS, glm::mat4& TSs);

	void generateBoneTSs(glm::mat4& TSs);

	void boneScan();

	void rotateBone(glm::vec3 rotation_axis, float angle_magnitude);
	void rollBone(float angle_magnitude);
	void setChildrenRotation();
};

struct Joint {
	// FIXME: Implement your Joint data structure.
	// Note: PMD represents weights on joints, but you need weights on
	//       bones to calculate the actual animation.
};


struct Skeleton {
	// FIXME: create skeleton and bone data structures
private:
	Bone* bone_root;
	
	std::vector<glm::vec4> skeleton_vertices;
	std::vector<glm::vec4> cylinder_vertices;
	std::vector<glm::vec4> normal_vertices;
	std::vector<glm::vec4> binormal_vertices;

	std::vector<std::vector<float>> weights;

public:
	Skeleton();
	~Skeleton();
	void addBone(int _jid, glm::vec3 offset, int parent);
	void addRootBone(glm::vec3 offset);
	Bone* getBoneRoot() { return bone_root; }

	void generateVertices();

	// Project 5
	
	void generateLBSMatrices(std::vector<glm::mat4>& Ds, std::vector<glm::mat4>& Us);
	int initializeWeightsMatrix(std::vector<SparseTuple>& weights_data, int bone_count, int vertex_count);
	void setJointWeights(std::vector<SparseTuple>& weights_data);

	glm::mat4 findBoneMatrixU(int _jid);
	glm::mat4 findBoneMatrixD(int _jid);

	const std::vector<std::vector<float>>& getWeights() const;

	// End Project 5

	void initVertices(std::vector<glm::vec4>& vertices, int size);
	void initCylinderVertices();
	void initNormalVertices();
	void initBinormalVertices();

	const std::vector<glm::vec4>& getVertices() const;
	const std::vector<glm::vec4>& getCylinderVertices() const;
	const std::vector<glm::vec4>& getNormalVertices() const;
	const std::vector<glm::vec4>& getBinormalVertices() const;

	std::vector<glm::vec4>& getVerticesVector();
	std::vector<glm::vec4>& getCylinderVerticesVector();


	//void findBoneIntersect(const Ray* ray, Bone* bone, float& t);
	void highlightBones(const Ray& ray, Bone*& highlight_bone);

	void regenerateHighlightBone(Bone* highlight_bone);
	void regenerateNormalVertices(Bone* highlight_bone);
	void regenerateBinormalVertices(Bone* highlight_bone);
};

struct Mesh {
	Mesh();
	~Mesh();
	std::vector<glm::vec4> vertices;
	std::vector<glm::vec4> animated_vertices;
	std::vector<glm::uvec3> faces;
	std::vector<glm::vec4> vertex_normals;
	std::vector<glm::vec4> face_normals;
	std::vector<glm::vec2> uv_coordinates;
	std::vector<Material> materials;

	std::vector<glm::mat4> Ds;
	std::vector<glm::mat4> Us;
	std::vector<float> weights_array;
	std::vector<int> bone_ids;

	BoundingBox bounds;
	Skeleton skeleton;
	bool isDirty;
	bool show;

	int bone_count;
	int vertex_count;

	void loadpmd(const std::string& fn);
	void updateAnimation();
	int getNumberOfBones() const 
	{ 
		return bone_count;
		// FIXME: return number of bones in skeleton
	}
	glm::vec3 getCenter() const { return 0.5f * glm::vec3(bounds.min + bounds.max); }
private:
	void computeBounds();
	void computeNormals();
};

class Ray
{
public:
	glm::vec3 p;
	glm::vec3 v;
};

#endif
