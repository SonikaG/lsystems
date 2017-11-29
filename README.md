# GraphicsProject5
- Nelson Wapon Safo, Sonika Garg and Taylor Kline

- Completed all features

- Running this project works like the given instructions:
	mkdir build
	cd build
	cmake ..
	make
	./bin/skinning ../assets/pmd/[model of your choice]

- Saved images go into your current working directory (The name of the file is saved_file.jpg)

- Completed extra feature of z-fighting:
	- set kNear in config.h to 0.00001f and the mesh renders properly
	- in gui.cc, at line 37 and at line 192, the projection matrix is changed to handle depth linearly

- Completed extra feature of Linear Blend Skinning on GPU
	- Created uniform matrices for Deformed and Undeformed Matrices per bone, only handle bones that have a weight assigned to them to save space. Maximum size of uniform matrix array for all bones in 127 for each matrix to use GPU registers
	- Created arrays of weights per vertex sent as "in" to handle all NON-ZERO weights for each vertex for bones
	- Created arrays of bone_ids per vertex sent as "in" to index into the uniform matrices for the weights that are non-zero per vertex

	- There is a maximum for all the models of 12 weights of 12 bones per vertex, so to not overfill the GPU, I optimized my code to send an array of 12 weights for each vertex to avoid all weights as a uniform which is size (vertex_count*bone_count)

	- Created a new RenderPass constructor in the RenderPass files to handle an array of 12 floats for 12 weights and 12 bone_ids for each vertex
	- GPU code is in animated.vert inside the shaders folder

- Completed extra feature of dual quaternion skinning
	- Full implementation is in the GPU in animated.vert, deformed and underformed matrices were transformed into quaternions on the GPU
