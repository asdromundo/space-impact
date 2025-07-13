use godot::{classes::Timer, prelude::*};

#[derive(GodotClass)]
#[class(base = Node2D, init)]
pub struct GoL {
    #[export]
    timer: Option<Gd<Timer>>,
    #[base]
    base: Base<Node2D>,
}

#[godot_api]
impl GoL {
    #[func]
    fn reset(&mut self) {
        unimplemented!();
    }
    #[func]
    fn set_timeout(&mut self, timeout: f64) {
        if let Some(timer) = &mut self.timer {
            timer.set_wait_time(timeout);
        }
    }
}
