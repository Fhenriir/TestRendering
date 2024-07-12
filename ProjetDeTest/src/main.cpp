#include "opengl-framework/opengl-framework.hpp" // Inclue la librairie qui va nous servir à faire du rendu
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"

int main()
{
    // Initialisation
    gl::init("TPs de Rendering"); // On crée une fenêtre et on choisit son nom
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE_MINUS_DST_ALPHA, GL_ONE);
    gl::maximize_window(); // On peut la maximiser si on veut
    auto camera = gl::Camera{};
    gl::set_events_callbacks({ camera.events_callbacks() });
    gl::set_events_callbacks({camera.events_callbacks()});

    glm::mat4 const view_matrix = camera.view_matrix();

    auto const shader = gl::Shader{{
        .vertex   = gl::ShaderSource::File{"res/vertex.vert"},
        .fragment = gl::ShaderSource::File{"res/fragment.frag"},
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
                -1, +1, 0, 0, 0, // Position3D du 1er sommet
                +1, +1, 0, 0, 1, // Position3D du 2ème sommet
                -1, -1, 0, 1, 0, // Position3D du 3ème sommet
                +1, -1, 0, 1, 1  // Position3D du 4ème sommet
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
			shader.set_uniform("the_texture", builded_texture);
			cube_mesh.draw(); // C'est ce qu'on appelle un "draw call" : on envoie l'instruction à la carte graphique de dessiner notre mesh.
		});
        glClearColor(0.1f, 0.1f, 0.1f, 1.f); // Choisis la couleur à utiliser. Les paramètres sont R, G, B, A avec des valeurs qui vont de 0 à 1
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Exécute concrètement l'action d'appliquer sur tout l'écran la couleur choisie au-dessus
        glm::mat4 const view_matrix = camera.view_matrix();
        glm::mat4 const projection_matrix = glm::infinitePerspective(1.f /*field of view in radians*/, gl::framebuffer_aspect_ratio() /*aspect ratio*/, 0.001f /*near plane*/);
        shader.bind(); // On a besoin qu'un shader soit bind (i.e. "actif") avant de draw(). On en reparle dans la section d'après.
        float _baseTime = gl::time_in_seconds();
        shader.set_uniform("viewMatrix", projection_matrix* view_matrix);
        shader.set_uniform("time", _baseTime);
        shader.set_uniform("alpha", 1.f);
        shader.set_uniform("color", glm::vec4(0.9, 0.9, 0, 1));
        shader.set_uniform("the_texture", render_target.color_texture(0));
        care_mesh.draw(); // C'est ce qu'on appelle un "draw call" : on envoie l'instruction à la carte graphique de dessiner notre mesh.
    }
}