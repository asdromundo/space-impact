use godot::classes::{INode2D, Node2D, Timer};
use godot::obj::NewAlloc;
use godot::prelude::*;

#[derive(GodotClass)]
#[class(base=Node2D)]
struct GoLRust {
    grid: Vec<Vec<bool>>,
    next_grid: Vec<Vec<bool>>,
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
impl INode2D for GoLRust {
    fn init(base: Base<Node2D>) -> Self {
        // Init Grids
        let grid_width = 1;
        let grid_height = 1;
        let grid = Self::get_random_grid(grid_width, grid_height);
        let next_grid = grid.clone();
        let timer = Timer::new_alloc();
        Self {
            grid,
            next_grid,
            base,
            cell_size: 10,
            grid_width: grid_width as i64,
            grid_height: grid_height as i64,
            on_timer: true,
            timeout: 0.5,
            timer,
        }
    }

    fn ready(&mut self) {
        godot_print!("Started Game of Life: Rust Impl");
        self.reset();
        self.timer.set_wait_time(self.timeout); // 1 segundo
        self.timer.set_autostart(true); // Inicia automáticamente
        self.timer.signals().timeout().connect_other(self, |this| {
            this.on_timer_timeout();
        });
        // add to scene
        {
            self.base
                .to_gd()
                .add_child(&self.timer.clone().upcast::<Node>());
        }
    }

    fn process(&mut self, _delta: f64) {
        if !self.on_timer {
            self.update_grid();
            self.base_mut().queue_redraw();
        }
    }

    fn draw(&mut self) {
        for y in 0..self.grid_height as usize {
            for x in 0..self.grid_width as usize {
                if self.grid[y][x] {
                    self.base.to_gd().draw_rect(
                        Rect2 {
                            position: Vector2 {
                                x: (x * self.cell_size as usize) as f32,
                                y: (y * self.cell_size as usize) as f32,
                            },
                            size: Vector2 {
                                x: self.cell_size as f32,
                                y: self.cell_size as f32,
                            },
                        },
                        Color::SEASHELL,
                    );
                }
            }
        }
    }
}

#[godot_api]
impl GoLRust {
    fn get_random_grid(width: usize, height: usize) -> Vec<Vec<bool>> {
        (0..height)
            .map(|_| (0..width).map(|_| rand::random::<f32>() < 0.2).collect())
            .collect()
    }

    fn on_timer_timeout(&mut self) {
        self.update_grid();
        self.base_mut().queue_redraw();
    }

    fn update_grid(&mut self) {
        for y in 0..self.grid_height as usize {
            for x in 0..self.grid_width as usize {
                let alive_neighbors = self.count_alive_neighbors(x, y);
                let alive = self.grid[y][x];
                self.next_grid[y][x] = if alive {
                    alive_neighbors == 2 || alive_neighbors == 3
                } else {
                    alive_neighbors == 3
                };
            }
        }

        // Intercambiar las referencias de `grid` y `next_grid`
        std::mem::swap(&mut self.grid, &mut self.next_grid);
    }

    #[func]
    fn update_grid_size(&mut self) {
        let v_rec = self.base.to_gd().get_viewport_rect();
        self.grid_width = (v_rec.size.x / self.cell_size as f32) as i64;
        self.grid_height = (v_rec.size.y / self.cell_size as f32) as i64;
    }

    fn count_alive_neighbors(&self, x: usize, y: usize) -> usize {
        let mut count = 0;

        for dy in -1..=1 {
            for dx in -1..=1 {
                if dx == 0 && dy == 0 {
                    continue;
                }

                let nx = x as isize + dx;
                let ny = y as isize + dy;

                if nx >= 0
                    && ny >= 0
                    && nx < self.grid_width as isize
                    && ny < self.grid_height as isize
                {
                    if self.grid[ny as usize][nx as usize] {
                        count += 1;
                    }
                }
            }
        }

        count
    }

    #[func]
    fn update_cell_size(&mut self, size: i64) {
        if size != self.cell_size {
            self.cell_size = size;
            self.reset();
        }
    }

    #[func]
    fn update_timeout(&mut self, time: f64) {
        if time <= 0.005 {
            self.on_timer = false;
            self.timer.stop();
        } else {
            self.on_timer = true;
            if time != self.timeout {
                self.timeout = time;
                self.timer.set_wait_time(time);
                self.timer.start();
            }
        }
    }

    #[func]
    fn reset(&mut self) {
        self.update_grid_size();
        self.grid = GoLRust::get_random_grid(self.grid_width as usize, self.grid_height as usize);
        self.next_grid = self.grid.clone();
        self.base_mut().queue_redraw();
    }
}
