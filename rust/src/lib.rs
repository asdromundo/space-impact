use godot::prelude::*;

struct RustExtension;

#[gdextension]
unsafe impl ExtensionLibrary for RustExtension {}

mod game_of_life;
mod golrust;
mod golrustflecs;
