#include "opengl-framework/opengl-framework.hpp" // Inclue la librairie qui va nous servir à faire du rendu
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "../lib/tinyobjloader/tiny_obj_loader.h"
#include <iostream>

auto load_mesh(std::filesystem::path const& path) -> gl::Mesh
{
    // On lit le fichier avec tinyobj
    auto reader = tinyobj::ObjReader{};
    reader.ParseFromFile(gl::make_absolute_path(path).string(), {});

    if (!reader.Error().empty()) { throw std::runtime_error("Failed to read 3D model:\n" + reader.Error()); }

    if (!reader.Warning().empty()) { std::cout << "Warning while reading 3D model:\n" + reader.Warning(); }
    // On met tous les attributs dans un tableau
    auto vertices = std::vector<float>{};
    for (auto const& shape : reader.GetShapes())
    {
        for (auto const& idx : shape.mesh.indices)
        {
            // Position
            vertices.push_back(reader.GetAttrib().vertices[3 * idx.vertex_index + 0]);
            vertices.push_back(reader.GetAttrib().vertices[3 * idx.vertex_index + 1]);
            vertices.push_back(reader.GetAttrib().vertices[3 * idx.vertex_index + 2]);

            // UV
            vertices.push_back(reader.GetAttrib().texcoords[2 * idx.texcoord_index + 0]);
            vertices.push_back(reader.GetAttrib().texcoords[2 * idx.texcoord_index + 1]);

            // Normale
            vertices.push_back(reader.GetAttrib().normals[3 * idx.normal_index + 0]);
            vertices.push_back(reader.GetAttrib().normals[3 * idx.normal_index + 1]);
            vertices.push_back(reader.GetAttrib().normals[3 * idx.normal_index + 2]);
        };
    };
    auto result = gl::Mesh{{
        .vertex_buffers = {{
            .layout = {gl::VertexAttribute::Position3D{0},gl::VertexAttribute::UV{1},gl::VertexAttribute::Normal3D{2}},
            .data = vertices,
        }},
    } };
    return result;
    // TODO créer et return un gl::mesh, qui utilisera le tableau `vertices` en tant que `data` pour son vertex buffer.
    // Attention, il faudra bien spécifier le layout pour qu'il corresponde à l'ordre des attributs dans le tableau `vertices`.
};

auto load_texture(std::filesystem::path const& path) -> gl::Texture{
    auto result = gl::Texture{
        gl::TextureSource::File{ // Peut être un fichier, ou directement un tableau de pixels
            .path = gl::make_absolute_path(path).string(),
            .flip_y = true, // Il n'y a pas de convention universelle sur la direction de l'axe Y. Les fichiers (.png, .jpeg) utilisent souvent une direction différente de celle attendue par OpenGL. Ce booléen flip_y est là pour inverser la texture si jamais elle n'apparaît pas dans le bon sens.
            .texture_format = gl::InternalFormat::RGBA8, // Format dans lequel la texture sera stockée. On pourrait par exemple utiliser RGBA16 si on voulait 16 bits par canal de couleur au lieu de 8. (Mais ça ne sert à rien dans notre cas car notre fichier ne contient que 8 bits par canal, donc on ne gagnerait pas de précision). On pourrait aussi stocker en RGB8 si on ne voulait pas de canal alpha. On utilise aussi parfois des textures avec un seul canal (R8) pour des usages spécifiques.
        },
        gl::TextureOptions{
            .minification_filter = gl::Filter::Linear, // Comment on va moyenner les pixels quand on voit l'image de loin ?
            .magnification_filter = gl::Filter::Linear, // Comment on va interpoler entre les pixels quand on zoom dans l'image ?
            .wrap_x = gl::Wrap::Repeat,   // Quelle couleur va-t-on lire si jamais on essaye de lire en dehors de la texture ?
            .wrap_y = gl::Wrap::Repeat,   // Idem, mais sur l'axe Y. En général on met le même wrap mode sur les deux axes.
        }
    };
    return result;
};

int main()
{
    // Initialisation
    gl::init("TPs de Rendering"); // On crée une fenêtre et on choisit son nom
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE_MINUS_DST_ALPHA, GL_ONE);
    gl::maximize_window(); // On peut la maximiser si on veut
    auto camera = gl::Camera{};
    gl::set_events_callbacks({camera.events_callbacks()});
    gl::set_events_callbacks({camera.events_callbacks()});

    glm::mat4 const view_matrix = camera.view_matrix();

    glm::vec3 const uniform_light = glm::vec3(0.2f, 0.3f, -1.f);

    auto const shader = gl::Shader{{
        .vertex   = gl::ShaderSource::File{"res/vertex.vert"},
        .fragment = gl::ShaderSource::File{"res/fragment.frag"},
    }};

    auto const shaderMain = gl::Shader{ {
		.vertex   = gl::ShaderSource::File{"res/vertexMain.vert"},
		.fragment = gl::ShaderSource::File{"res/fragmentMain.frag"},
	}};

    auto const triangle_mesh = gl::Mesh{{
        .vertex_buffers = {{
            .layout = {gl::VertexAttribute::Position2D{0 /*Index de l'attribut dans le shader, on en reparle juste après*/}},
            .data   = {
                -0.5f, 0.5f, // Position2D du 1er sommet
                0.5f, -0.5f, // Position2D du 2ème sommet
                -0.5f, -0.5f  // Position2D du 3ème sommet
            },
        }},
    }};

    auto const triangle_mesh2 = gl::Mesh{{
        .vertex_buffers = {{
            .layout = {gl::VertexAttribute::Position2D{0 /*Index de l'attribut dans le shader, on en reparle juste après*/}},
            .data   = {
                -0.5f, 0.5f, // Position2D du 1er sommet
                0.5f, -0.5f, // Position2D du 2ème sommet
                0.5f, 0.5f  // Position2D du 3ème sommet
            },
        }},
    }};

    auto const rectangle_mesh = gl::Mesh{{
        .vertex_buffers = {{
            .layout = {gl::VertexAttribute::Position2D{0}},
            .data   = {
                -0.3f, -0.3f, // Position2D du 1er sommet
                +0.3f, -0.3f, // Position2D du 2ème sommet
                +0.3f, +0.3f, // Position2D du 3ème sommet
                -0.3f, +0.3f  // Position2D du 4ème sommet
            },
        }},
        .index_buffer   = {
            0, 1, 2, // Indices du premier triangle : on utilise le 1er, 2ème et 3ème sommet
            0, 2, 3  // Indices du deuxième triangle : on utilise le 1er, 3ème et 4ème sommet
        },
    }};

    auto const background_mesh = gl::Mesh{{
        .vertex_buffers = {{
            .layout = {gl::VertexAttribute::Position2D{0},gl::VertexAttribute::UV{1}},
            .data = {
                -2.f, -2.f,0,0 // Position2D du 1er sommet
                +2.f, -2.f,1,0 // Position2D du 2ème sommet
                +2.f, +2.f,1,1 // Position2D du 3ème sommet
                -2.f, +2.f,0,1  // Position2D du 4ème sommet
            },
        }},
        .index_buffer = {
            0, 1, 2, // Indices du premier triangle : on utilise le 1er, 2ème et 3ème sommet
            0, 2, 3  // Indices du deuxième triangle : on utilise le 1er, 3ème et 4ème sommet
        },
    }};

    auto const cube_mesh = gl::Mesh{ {
        .vertex_buffers = {{
            .layout = {gl::VertexAttribute::Position3D{0},gl::VertexAttribute::UV{1}},
            .data = {
				-0.5f, -0.5f, -0.5f, 0, 0, // Position3D du 1er sommet
				+0.5f, -0.5f, -0.5f, 1, 0,// Position3D du 2ème sommet
				+0.5f, +0.5f, -0.5f, 1, 1,// Position3D du 3ème sommet
				-0.5f, +0.5f, -0.5f, 0, 1,// Position3D du 4ème sommet

				-0.5f, -0.5f, +0.5f, 0, 0,// Position3D du 5ème sommet
				+0.5f, -0.5f, +0.5f, 1, 0,// Position3D du 6ème sommet
				+0.5f, +0.5f, +0.5f, 1, 1,// Position3D du 7ème sommet
				-0.5f, +0.5f, +0.5f, 0, 1// Position3D du 8ème sommet
			},
		}},
        .index_buffer = {
			0, 1, 2, 0, 2, 3, // Face 1
			1, 5, 6, 1, 6, 2, // Face 2
			5, 4, 7, 5, 7, 6, // Face 3
			4, 0, 3, 4, 3, 7, // Face 4
			3, 2, 6, 3, 6, 7, // Face 5
			4, 5, 1, 4, 1, 0  // Face 6
		},
	}};

	auto const care_mesh = gl::Mesh{{
        .vertex_buffers = {{
            .layout = {gl::VertexAttribute::Position3D{0},gl::VertexAttribute::UV{1}},
            .data = {
                +2.0f, -2.0f, 0, 1, 0, // Position3D du 1er sommet
                +2.0f, +2.0f, 0, 1, 1, // Position3D du 2ème sommet
                -2.0f, -2.0f, 0, 0, 0, // Position3D du 3ème sommet
                -2.0f, +2.0f, 0, 0, 1  // Position3D du 4ème sommet
            },
        }},
        .index_buffer = {
            0, 1, 3, // Indices du premier triangle : on utilise le 1er, 2ème et 3ème sommet
            0, 2, 3  // Indices du deuxième triangle : on utilise le 1er, 3ème et 4ème sommet
        },
    }};

    auto const builded_texture = gl::Texture{
        gl::TextureSource::File{ // Peut être un fichier, ou directement un tableau de pixels
            .path = "res/texture.png",
            .flip_y = true, // Il n'y a pas de convention universelle sur la direction de l'axe Y. Les fichiers (.png, .jpeg) utilisent souvent une direction différente de celle attendue par OpenGL. Ce booléen flip_y est là pour inverser la texture si jamais elle n'apparaît pas dans le bon sens.
            .texture_format = gl::InternalFormat::RGBA8, // Format dans lequel la texture sera stockée. On pourrait par exemple utiliser RGBA16 si on voulait 16 bits par canal de couleur au lieu de 8. (Mais ça ne sert à rien dans notre cas car notre fichier ne contient que 8 bits par canal, donc on ne gagnerait pas de précision). On pourrait aussi stocker en RGB8 si on ne voulait pas de canal alpha. On utilise aussi parfois des textures avec un seul canal (R8) pour des usages spécifiques.
        },
        gl::TextureOptions{
            .minification_filter = gl::Filter::Linear, // Comment on va moyenner les pixels quand on voit l'image de loin ?
            .magnification_filter = gl::Filter::Linear, // Comment on va interpoler entre les pixels quand on zoom dans l'image ?
            .wrap_x = gl::Wrap::Repeat,   // Quelle couleur va-t-on lire si jamais on essaye de lire en dehors de la texture ?
            .wrap_y = gl::Wrap::Repeat,   // Idem, mais sur l'axe Y. En général on met le même wrap mode sur les deux axes.
        }
    };

    auto const builded_texture2 = gl::Texture{
        gl::TextureSource::File{ // Peut être un fichier, ou directement un tableau de pixels
            .path = "res/fourareen/fourareen2K_albedo.jpg",
            .flip_y = true, // Il n'y a pas de convention universelle sur la direction de l'axe Y. Les fichiers (.png, .jpeg) utilisent souvent une direction différente de celle attendue par OpenGL. Ce booléen flip_y est là pour inverser la texture si jamais elle n'apparaît pas dans le bon sens.
            .texture_format = gl::InternalFormat::RGBA8, // Format dans lequel la texture sera stockée. On pourrait par exemple utiliser RGBA16 si on voulait 16 bits par canal de couleur au lieu de 8. (Mais ça ne sert à rien dans notre cas car notre fichier ne contient que 8 bits par canal, donc on ne gagnerait pas de précision). On pourrait aussi stocker en RGB8 si on ne voulait pas de canal alpha. On utilise aussi parfois des textures avec un seul canal (R8) pour des usages spécifiques.
        },
        gl::TextureOptions{
            .minification_filter = gl::Filter::Linear, // Comment on va moyenner les pixels quand on voit l'image de loin ?
            .magnification_filter = gl::Filter::Linear, // Comment on va interpoler entre les pixels quand on zoom dans l'image ?
            .wrap_x = gl::Wrap::Repeat,   // Quelle couleur va-t-on lire si jamais on essaye de lire en dehors de la texture ?
            .wrap_y = gl::Wrap::Repeat,   // Idem, mais sur l'axe Y. En général on met le même wrap mode sur les deux axes.
        }
    };

	auto render_target = gl::RenderTarget{ gl::RenderTarget_Descriptor{
	    .width = gl::framebuffer_width_in_pixels(),
	    .height = gl::framebuffer_height_in_pixels(),
	    .color_textures = {
		    gl::ColorAttachment_Descriptor{
			    .format = gl::InternalFormat_Color::RGBA8,
			    .options = {
				    .minification_filter = gl::Filter::NearestNeighbour, // On va toujours afficher la texture à la taille de l'écran,
				    .magnification_filter = gl::Filter::NearestNeighbour, // donc les filtres n'auront pas d'effet. Tant qu'à faire on choisit le moins coûteux.
				    .wrap_x = gl::Wrap::ClampToEdge,
				    .wrap_y = gl::Wrap::ClampToEdge,
			    },
		    },
	    },
	    .depth_stencil_texture = gl::DepthStencilAttachment_Descriptor{
		    .format = gl::InternalFormat_DepthStencil::Depth32F,
		    .options = {
			    .minification_filter = gl::Filter::NearestNeighbour,
			    .magnification_filter = gl::Filter::NearestNeighbour,
			    .wrap_x = gl::Wrap::ClampToEdge,
			    .wrap_y = gl::Wrap::ClampToEdge,
		    },
	    },
    }};

    gl::set_events_callbacks({
        camera.events_callbacks(),
        {.on_framebuffer_resized = [&](gl::FramebufferResizedEvent const& e) {
            if (e.width_in_pixels != 0 && e.height_in_pixels != 0) // OpenGL crash si on tente de faire une render target avec une taille de 0
            render_target.resize(e.width_in_pixels, e.height_in_pixels);
        }},
    });

    //glClearColor(0.f, 0.f, 1.f, 1.f); // Choisis la couleur à utiliser. Les paramètres sont R, G, B, A avec des valeurs qui vont de 0 à 1
    //glClear(GL_COLOR_BUFFER_BIT); // Exécute concrètement l'action d'appliquer sur tout l'écran la couleur choisie au-dessus

    while (gl::window_is_open())
    {
		render_target.render([&]() {
			glClearColor(0.2f, 0.2f, 0.2f, 1.f); // Choisis la couleur à utiliser. Les paramètres sont R, G, B, A avec des valeurs qui vont de 0 à 1
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Exécute concrètement l'action d'appliquer sur tout l'écran la couleur choisie au-dessus
			glm::mat4 const view_matrix = camera.view_matrix(); // On récupère la matrice de vue de la caméra
			glm::mat4 const projection_matrix = glm::infinitePerspective(1.f /*field of view in radians*/, gl::framebuffer_aspect_ratio() /*aspect ratio*/, 0.001f /*near plane*/);
			glm::mat4 const ortho_projection = glm::ortho(-(800.0f / 100.0f), 800.0f / 100.0f, 600.0f / 100.0f, -(600.0f / 100.0f), -1000.0f, 1000.0f);
			glm::mat4 const rotation = glm::rotate(glm::mat4{1.f}, gl::time_in_seconds() /*angle de la rotation*/, glm::vec3{0.f, 0.f, 1.f} /* axe autour duquel on tourne */);
			glm::mat4 const translation = glm::translate(glm::mat4{1.f}, glm::vec3{0.f, 1.f, 0.f});
			glm::mat4 const modele = translation * rotation;
			shader.bind(); // On a besoin qu'un shader soit bind (i.e. "actif") avant de draw(). On en reparle dans la section d'après.
			float _baseTime = gl::time_in_seconds();
			shader.set_uniform("viewMatrix", projection_matrix * view_matrix);
			shader.set_uniform("time", _baseTime);
			shader.set_uniform("alpha", 1.f);
			shader.set_uniform("color", glm::vec4(0.9, 0.9, 0, 1));
			shader.set_uniform("the_texture", builded_texture2);
            shader.set_uniform("uniform_light", uniform_light);
            shader.set_uniform("base_illumination", 0.3f);
            load_mesh("..\\..\\..\\res\\fourareen\\fourareen.obj").draw(); // C'est ce qu'on appelle un "draw call" : on envoie l'instruction à la carte graphique de dessiner notre mesh.
		});
        glClearColor(0.1f, 0.1f, 0.1f, 1.f); // Choisis la couleur à utiliser. Les paramètres sont R, G, B, A avec des valeurs qui vont de 0 à 1
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Exécute concrètement l'action d'appliquer sur tout l'écran la couleur choisie au-dessus
        shaderMain.bind(); // On a besoin qu'un shader soit bind (i.e. "actif") avant de draw(). On en reparle dans la section d'après.
        float _baseTime = gl::time_in_seconds();
        shaderMain.set_uniform("aspectRatio", 1.0f);
        shaderMain.set_uniform("viewMatrix", glm::mat4(1.0f));
        shaderMain.set_uniform("time", _baseTime);
        shaderMain.set_uniform("alpha", 1.f);
        shaderMain.set_uniform("color", glm::vec4(0.9, 0.9, 0, 1));
        shaderMain.set_uniform("the_texture", render_target.color_texture(0));
        care_mesh.draw(); // C'est ce qu'on appelle un "draw call" : on envoie l'instruction à la carte graphique de dessiner notre mesh.
    }
}