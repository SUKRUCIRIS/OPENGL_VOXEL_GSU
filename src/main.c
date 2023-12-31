#include "./core/core.h"
#include "./game/chunk.h"

int main(void)
{
	GLFWwindow *window = create_window(0, 0, 1, 1, 4);
	if (window == 0)
	{
		return -1;
	}

	init_programs();

	int world_size = 2048;
	int chunk_size = 64;
	int chunk_range = 4;
	float render_distance = (float)chunk_size * chunk_range * 2;

	int window_w = 0, window_h = 0;
	glfwGetWindowSize(window, &window_w, &window_h);
	camera *cam = create_camera(window_w, window_h, (vec3){0.0f, 5, 60.0f}, 60, 0.1f, render_distance, 1, 100, -15, (vec3){1, 0, 0});

	lighting *light = create_lighting(window, cam, 4096, 4096, render_distance / 64, render_distance / 16, render_distance / 4, render_distance);

	int **hm = create_heightmap(world_size, world_size, 100, 1453, 1071, 175, 100);

	struct aiScene *gsu_model = load_model("./models/gsu.fbx", 1);
	set_gsu_model(gsu_model);

	player sukru;
	sukru.speed = 0.005f;
	sukru.jumping = 0;
	sukru.onland = 1;
	sukru.fp_camera = cam;
	sukru.width = 1;
	sukru.height = 3;
	sukru.hm = hm;
	sukru.dimensionx = world_size;
	sukru.dimensionz = world_size;
	sukru.jumpdurationms = 100;
	chunk_op *chunks = create_chunk_op(chunk_size, chunk_range, &sukru, hm, world_size, world_size);
	unsigned char freec = 0;
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	while (!glfwWindowShouldClose(window))
	{
		start_game_loop();

		glfwPollEvents();
		poll_events(window);

		update_chunk_op(chunks, light->lightDir);

		if (get_key_pressed(GLFW_KEY_K) == 1)
		{
			if (freec == 0)
			{
				freec = 1;
			}
			else
			{
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
				freec = 0;
			}
		}
		if (freec == 0)
		{
			run_input_player(&sukru, window, get_frame_timems());
		}
		else
		{
			run_input_free_camera(cam, window);
		}

		glUseProgram(get_def_shadowmap_br_program());
		use_lighting(light, get_def_shadowmap_br_program(), 1);
		use_chunk_op(chunks, get_def_shadowmap_br_program(), cam);

		glUseProgram(get_def_tex_light_br_program());
		use_lighting(light, get_def_tex_light_br_program(), 0);
		use_camera(cam, get_def_tex_light_br_program());
		use_chunk_op(chunks, get_def_tex_light_br_program(), cam);

		glfwSwapBuffers(window);

		end_game_loop();
	}

	delete_camera(cam);
	delete_lighting(light);

	delete_chunk_op(chunks);

	delete_all_physic();
	destroy_programs();
	delete_window(window);

	free(hm);
	free_model(gsu_model);

	return 0;
}