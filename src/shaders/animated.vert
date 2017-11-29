R"zzz(
#version 330 core
uniform vec4 light_position;
uniform vec3 camera_position;
uniform mat4[127] Ds;
uniform mat4[127] Us;
in vec4 vertex_position;
in vec4 normal;
in vec2 uv;

in vec4 weights1;
in vec4 weights2;
in vec4 weights3;

in vec4 bone_ids1;
in vec4 bone_ids2;
in vec4 bone_ids3;

out vec4 vs_light_direction;
out vec4 vs_normal;
out vec2 vs_uv;
out vec4 vs_camera_direction;

// Inspired from https://twistedpairdevelopment.wordpress.com/2013/02/11/rotating-a-vector-by-a-quaternion-in-glsl/
vec4 multQuat(vec4 q1, vec4 q2)
{
	return vec4(
		q1.w * q2.x + q1.x * q2.w + q1.z * q2.y - q1.y * q2.z,
		q1.w * q2.y + q1.y * q2.w + q1.x * q2.z - q1.z * q2.x,
		q1.w * q2.z + q1.z * q2.w + q1.y * q2.x - q1.x * q2.y,
		q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z
);
}


// Mat4ToQuat inspired from http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/
vec4 mat4toQuat0(mat4 a){
  	vec4 q = vec4(1.0, 1.0, 1.0, 1.0);

	float trace = a[0][0] + a[1][1] + a[2][2];

	if( trace > 0 ) {
    	float s = 0.5f / sqrt(trace + 1.0f);
    	q.w = 0.25f / s;
    	q.x = ( a[2][1] - a[1][2] ) * s;
    	q.y = ( a[0][2] - a[2][0] ) * s;
    	q.z = ( a[1][0] - a[0][1] ) * s;
    } else {
    	if ( a[0][0] > a[1][1] && a[0][0] > a[2][2] ) {
      		float s = 2.0f * sqrt( 1.0f + a[0][0] - a[1][1] - a[2][2]);
      		q.w = (a[2][1] - a[1][2] ) / s;
      		q.x = 0.25f * s;
      		q.y = (a[0][1] + a[1][0] ) / s;
    		q.z = (a[0][2] + a[2][0] ) / s;
    	} else if (a[1][1] > a[2][2]) {
    		float s = 2.0f * sqrt( 1.0f + a[1][1] - a[0][0] - a[2][2]);
    		q.w = (a[0][2] - a[2][0] ) / s;
    		q.x = (a[0][1] + a[1][0] ) / s;
    		q.y = 0.25f * s;
    		q.z = (a[1][2] + a[2][1] ) / s;
    	} else {
      		float s = 2.0f * sqrt( 1.0f + a[2][2] - a[0][0] - a[1][1] );
      		q.w = (a[1][0] - a[0][1] ) / s;
      		q.x = (a[0][2] + a[2][0] ) / s;
      		q.y = (a[1][2] + a[2][1] ) / s;
      		q.z = 0.25f * s;
    	}
	}

	float mag = sqrt(q.w*q.w + q.x*q.x + q.y*q.y + q.z*q.z);

	return q / (mag);
}

vec4 mat4toQuat1(vec4 q0, mat4 T){
  	vec4 t = vec4(T[3].x, T[3].y, T[3].z, 0.0f);
  	return 0.5f * multQuat(t, q0);
}

// Rotation by quaternion inspired by https://twistedpairdevelopment.wordpress.com/2013/02/11/rotating-a-vector-by-a-quaternion-in-glsl/
vec3 rotateQuad( vec4 quat, vec3 vec )
{
	vec4 qv = multQuat( quat, vec4(vec, 0.0) );
	return multQuat( qv, vec4(-quat.x, -quat.y, -quat.z, quat.w) ).xyz;
}

// Transformation of dual quads inspired from https://github.com/niubijiejie/DynamicFusion/blob/master/include/dualquaternion/dual_quat_cu.hpp
vec3 dualQuadTrans(vec3 v_pos, vec4 dq0, vec4 dq1)
{
	float norma = sqrt(dq0.w*dq0.w + dq0.x*dq0.x + dq0.y*dq0.y + dq0.z*dq0.z);

	dq0 = dq0 / norma;
	dq1 = dq1 / norma;

	vec3 v0 = vec3(dq0.x, dq0.y, dq0.z);
	vec3 v1 = vec3(dq1.x, dq1.y, dq1.z);

	vec3 trans = (v1*dq0.w - v0*dq1.w + cross(v1,v0)) * 2.0f;

	return rotateQuad(dq0, v_pos) + trans;
}

// Dual Blend Skinning inspired from http://rodolphe-vaillant.fr/?e=29
void main() {
	gl_Position = vec4(0.0, 0.0, 0.0, 0.0);
	int n = 0;

	vec4 dq0 = vec4(0.0, 0.0, 0.0, 0.0);
	vec4 dq1 = vec4(0.0, 0.0, 0.0, 0.0);

	int pivot = int(bone_ids1[0]);
	mat4 T0 = Ds[pivot] * inverse(Us[pivot]);

	vec4 q0 = mat4toQuat0(T0);

	for (n = 0; n < 4; n++) {
		if(weights1[n] <= 0.0)
			continue;	

		mat4 T = Ds[int(bone_ids1[n])] * inverse(Us[int(bone_ids1[n])]);

		float w = weights1[n];

		// Convert matrix to quaternion
		vec4 qi0 = mat4toQuat0(T);
		vec4 qi1 = mat4toQuat1(qi0, T);

		if(dot(q0, qi0) < 0.0f)
			w *= -1.0f;

		dq0 += qi0 * w;
		dq1 += qi1 * w;

		//gl_Position += weights1[n] * T * vertex_position; 
	}

	for (n = 0; n < 4; n++) {
		if(weights2[n] <= 0.0)
			continue;

		mat4 T = Ds[int(bone_ids2[n])] * inverse(Us[int(bone_ids2[n])]);

		float w = weights2[n];

		// Convert matrix to quaternion
		vec4 qi0 = mat4toQuat0(T);
		vec4 qi1 = mat4toQuat1(qi0, T);

		if(dot(q0, qi0) < 0.0f)
			w *= -1.0f;

		dq0 += qi0 * w;
		dq1 += qi1 * w;

		//gl_Position += weights2[n] * T * vertex_position; 
	}

	for (n = 0; n < 4; n++) {
		if(weights3[n] <= 0.0)
			continue;

		mat4 T = Ds[int(bone_ids3[n])] * inverse(Us[int(bone_ids3[n])]);

		float w = weights3[n];

		// Convert matrix to quaternion
		vec4 qi0 = mat4toQuat0(T);
		vec4 qi1 = mat4toQuat1(qi0, T);

		if(dot(q0, qi0) < 0.0f)
			w *= -1.0f;

		dq0 += qi0 * w;
		dq1 += qi1 * w;

		//gl_Position += weights3[n] * T * vertex_position; 
	}

	if(dq0.x == 0.0f && dq0.y == 0.0f && dq0.z == 0.0f && dq0.w == 0.0f && dq1.x == 0.0f && dq1.y == 0.0f && dq1.z == 0.0f && dq1.w == 0.0f)
	{
		dq0.w = 1.0f;
	}

	vec3 v_pos = vec3(vertex_position.x, vertex_position.y, vertex_position.z);

	vec3 end_pos = dualQuadTrans(v_pos, dq0, dq1);

	gl_Position = vec4(end_pos.x, end_pos.y, end_pos.z, 1.0);

	vs_light_direction = light_position - gl_Position;
	vs_camera_direction = vec4(camera_position, 1.0) - gl_Position;
	vs_normal = normal;
	vs_uv = uv;
}
)zzz"
