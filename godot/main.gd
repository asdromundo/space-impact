extends Node

enum GoLImpl {None, GDScriptImpl, GodotRustImpl, FlecsRustImpl}

@onready var size_label : Label =$UI/Background/VBoxContainer/CellSizeLabel
@onready var update_time_label : Label = $UI/Background/VBoxContainer/UpdateTimeLabel
@onready var fps_counter : Label = $UI/Background/VBoxContainer/FPSCounter

var cell_size = 10
var timeout = 0.5

var slider_val : int = 4
var current_impl : GoLImpl = GoLImpl.None
var gdscript_impl : GoLGd
var flecs_rust_impl: GoLRustFlecs
var rust_impl: GoLRust
var empty_impl : Node2D
var current_scene : Node2D

func _process(_delta: float) -> void:
	fps_counter.text = "FPS %.2f" % Engine.get_frames_per_second()
	

func _ready():
	gdscript_impl = GoLGd.new()
	flecs_rust_impl = GoLRustFlecs.new()
	rust_impl = GoLRust.new()
	empty_impl = Node2D.new()
	current_scene = empty_impl
	add_child(current_scene)
	_update_labels()


func _on_option_button_item_selected(index: int) -> void:
	var new_impl: GoLImpl = index as GoLImpl
	if new_impl == current_impl:
		return
	current_impl = new_impl
	remove_child(current_scene)
	match current_impl:
		GoLImpl.None:
			current_scene = empty_impl
		GoLImpl.GDScriptImpl:
			current_scene = gdscript_impl
		GoLImpl.FlecsRustImpl:
			current_scene = flecs_rust_impl
		GoLImpl.GodotRustImpl:
			current_scene = rust_impl
			
	if current_impl != GoLImpl.None:
		current_scene.cell_size = cell_size
		current_scene.timeout = timeout

	add_child(current_scene)


func _on_cell_size_slider_value_changed(value: float) -> void:
	cell_size = int(value)
	_update_labels()
	if current_scene.has_method("update_cell_size"):
		current_scene.update_cell_size(int(value))
	


func _on_update_time_slider_value_changed(value: float) -> void:
	timeout = value
	_update_labels()
	if current_scene.has_method("update_timeout"):
		current_scene.update_timeout(value)


func _on_reset_button_pressed() -> void:
	if current_scene.has_method("reset"):
		current_scene.reset()

func _update_labels() -> void:
	size_label.text = "Tamaño: %d" % cell_size
	update_time_label.text = "Update delay: %.2f" % timeout
