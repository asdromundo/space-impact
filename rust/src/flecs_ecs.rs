use flecs_ecs::core::World;
use flecs_ecs::macros::Component;
use flecs_ecs::prelude::*;
use godot::classes::{INode2D, Node2D, RenderingServer, Texture2D};
use godot::prelude::*;

#[derive(GodotClass)]
#[class(base=Node2D)]
struct FlecsEcs {
    world: World,
    base: Base<Node2D>,
}

#[godot_api]
impl INode2D for FlecsEcs {
    fn init(base: Base<Node2D>) -> Self {
        godot_print!("Iniciando Flecs");
        let world = World::new();
        // Singletons
        world.set(RenderContext {
            rs: RenderingServer::singleton(),
        });
        // Systems
        world
            .system::<(&mut Position, &Velocity)>()
            .each_iter(|it, _i, (p, v)| {
                p.x += v.x * it.delta_time();
                p.y += v.y * it.delta_time();
            });

        world
            .system::<(&Texture, &Position, &mut RenderContext)>()
            .term_at(2)
            .singleton()
            .each(|(texture, pos, ctx)| {
                let xform = Transform2D::IDENTITY.translated(Vector2::new(pos.x, pos.y));

                // Aplicar transformación
                ctx.rs.canvas_item_set_transform(texture.ci_rid, xform);
            });

        world
            .system::<(&Texture, &mut RenderContext)>()
            .term_at(1)
            .singleton()
            .kind::<flecs::pipeline::OnStart>()
            .each(|(texture, ctx)| {
                let size = texture.t.get_size();
                // Centrar y dibujar
                ctx.rs.canvas_item_add_texture_rect(
                    texture.ci_rid,
                    Rect2::new(-size / 2.0, size),
                    texture.t.get_rid(),
                );
            });

        // Systems

        Self { world, base }
    }

    fn ready(&mut self) {
        let e = self.world.entity_named("Player1");
        let mut rs = RenderingServer::singleton();

        // Crear canvas item RID
        let canvas_item = rs.canvas_item_create();

        // Asociarlo al nodo
        let canvas = self.base().get_canvas_item();
        rs.canvas_item_set_parent(canvas_item, canvas);

        // Cargar textura
        let tex = load::<Texture2D>("res://godot-rust.png");
        e.set(Position { x: 10.0, y: 20.0 })
            .set(Velocity { x: 1.0, y: 2.0 })
            .set(Texture {
                t: tex,
                ci_rid: canvas_item,
            });
    }

    fn process(&mut self, delta: f64) {
        self.world.progress_time(delta as f32);
    }
}

#[derive(Component)]
struct Velocity {
    x: f32,
    y: f32,
}

#[derive(Component)]
struct Position {
    x: f32,
    y: f32,
}

#[derive(Component)]
pub struct Texture {
    pub t: Gd<Texture2D>, // Godot texture reference
    pub ci_rid: Rid,      // Canvas Item RID
}

#[derive(Component)]
struct Player;

#[derive(Component)]
struct RenderContext {
    rs: Gd<RenderingServer>,
}
