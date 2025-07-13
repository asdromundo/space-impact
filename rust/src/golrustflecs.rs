use flecs_ecs::core::World;
use flecs_ecs::macros::Component;
use flecs_ecs::prelude::*;
use godot::classes::{INode2D, Node2D, RenderingServer, Timer};
use godot::obj::NewAlloc;
use godot::prelude::*;
use rand::Rng;

#[derive(GodotClass)]
#[class(base=Node2D)]
struct GoLRustFlecs {
    world: World,
    base: Base<Node2D>,
    #[var]
    cell_size: i64,
    #[var]
    grid_width: i64,
    #[var]
    grid_height: i64,
    #[var]
    timeout: f64,
    #[var]
    on_timer: bool,
    timer: Gd<Timer>,
}

#[godot_api]
impl INode2D for GoLRustFlecs {
    fn init(base: Base<Node2D>) -> Self {
        let world = World::new();
        let timer = Timer::new_alloc();

        Self {
            world,
            base,
            cell_size: 10,
            grid_width: 1,
            grid_height: 1,
            timeout: 0.5,
            on_timer: true,
            timer,
        }
    }

    fn ready(&mut self) {
        godot_print!("Started Game of Life: Rust Flecs Impl");

        // Configurar el timer
        self.timer.set_wait_time(self.timeout);
        self.timer.set_autostart(true);
        self.timer.signals().timeout().connect_other(self, |this| {
            this.on_timer_timeout();
        });
        self.base
            .to_gd()
            .add_child(&self.timer.clone().upcast::<Node>());

        // Inicializar Flecs
        self.world.component::<Cell>();
        self.world.component::<Position>();
        self.world.component::<Neighbours>();

        // Crear celdas y relaciones de vecinos
        self.reset();

        // Registrar sistemas
        self.register_systems();
    }

    fn process(&mut self, delta: f64) {
        self.world.progress_time(delta as f32);
    }
}

#[godot_api]
impl GoLRustFlecs {
    #[func]
    fn reset(&mut self) {
        self.update_grid_size();

        // Limpiar el mundo
        self.world.delete_entities_with::<Cell>();

        godot_print!("{}, {}", self.grid_width, self.grid_height);

        let mut cells = Vec::new();

        // Crear entidades para cada celda
        for y in 0..self.grid_height {
            for x in 0..self.grid_width {
                let alive = rand::random_bool(0.2); // 20% probabilidad de estar viva
                let entity = self
                    .world
                    .entity()
                    .set(Position {
                        x: x as u16,
                        y: y as u16,
                    })
                    .set(Cell { alive });

                cells.push(entity);
            }
        }

        godot_print!("{}", cells.len());

        // Crear relaciones de vecinos
        for y in 0..self.grid_height {
            for x in 0..self.grid_width {
                let idx = (y * self.grid_width + x) as usize;
                let cell = cells[idx];

                for dy in -1..=1 {
                    for dx in -1..=1 {
                        if dx == 0 && dy == 0 {
                            continue; // Ignorar la celda misma
                        }

                        let nx = x + dx;
                        let ny = y + dy;

                        if nx >= 0 && ny >= 0 && nx < self.grid_width && ny < self.grid_height {
                            let neighbor_idx = (ny * self.grid_width + nx) as usize;
                            let neighbor = cells[neighbor_idx];
                            cell.add_first::<Neighbours>(neighbor);
                        }
                    }
                }
            }
        }

        self.base_mut().queue_redraw();
    }

    fn register_systems(&mut self) {
        // Sistema para actualizar las celdas basado en las reglas del Juego de la Vida
        let update = self
            .world
            .system_named::<&Cell>("update_system")
            // .with_id((
            //     Neighbours::id(&self.world),
            //     flecs::Wildcard::id(&self.world),
            // ))
            .with_first::<Neighbours>(flecs::Wildcard::ID)
            // .with_first::<Neighbours>(flecs::Wildcard::ID)
            .kind_id(0)
            .run(|mut it| {
                let mut total = 0;
                godot_print!("Contando celdas con Neighbours");

                while it.next() {
                    total += it.count(); // <- esto es lo correcto
                }

                godot_print!("Total de celdas: {}", total);
            });

        update.run();

        // // Sistema para renderizar las celdas
        // self.world
        //     .system_named::<(&Position, &Cell)>("draw_system")
        //     .run_iter(|it, (positions, cells)| {
        //         let canvas = self.base.to_gd();

        //         for i in it.iter() {
        //             let pos = &positions[i];
        //             let cell = &cells[i];

        //             if cell.alive {
        //                 canvas.draw_rect(
        //                     Rect2 {
        //                         position: Vector2 {
        //                             x: (pos.x as i64 * self.cell_size) as f32,
        //                             y: (pos.y as i64 * self.cell_size) as f32,
        //                         },
        //                         size: Vector2 {
        //                             x: self.cell_size as f32,
        //                             y: self.cell_size as f32,
        //                         },
        //                     },
        //                     Color::SEASHELL,
        //                 );
        //             }
        //         }
        //     });
    }

    fn on_timer_timeout(&mut self) {
        self.world.progress();
        self.base_mut().queue_redraw();
    }

    #[func]
    fn update_grid_size(&mut self) {
        let viewport_rect = self.base.to_gd().get_viewport_rect();
        self.grid_width = (viewport_rect.size.x / self.cell_size as f32) as i64;
        self.grid_height = (viewport_rect.size.y / self.cell_size as f32) as i64;
    }
}

#[derive(Component)]
struct Position {
    x: u16,
    y: u16,
}

#[derive(Component)]
struct Cell {
    alive: bool,
}

#[derive(Component)]
struct Neighbours;
