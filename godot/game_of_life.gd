class_name GoLGd extends Node2D

var cell_size: int = 10
var grid_width: int = 1
var grid_height: int = 1
var timeout: float = 0.5
var grid = []
var next_grid = []

var on_timer: bool = true

var timer: Timer

func _init():
	timer = Timer.new()
	timer.wait_time = timeout
	timer.autostart = true
	timer.connect("timeout", _on_timer_timeout)
	add_child(timer)


func _ready():
	print("Started Game of Life: GDScript Impl")
	reset()

func _process(_delta: float) -> void:
	if !on_timer:
		update_grid()
		queue_redraw()

func update_grid_size():
	var v_rec: Rect2 = get_viewport_rect()
	grid_width = int(v_rec.size.x / cell_size)
	grid_height = int(v_rec.size.y / cell_size)

func init_grids():
	grid = []
	next_grid = []
	for y in range(grid_height):
		var row = []
		var next_row = []
		for x in range(grid_width):
			row.append(false)
			next_row.append(false)
		grid.append(row)
		next_grid.append(next_row)

func randomize_grid():
	for y in range(grid_height):
		for x in range(grid_width):
			grid[y][x] = randf() < 0.2 # 20% de probabilidad de estar viva

func _on_timer_timeout():
	update_grid()
	queue_redraw()

func update_grid():
	for y in range(grid_height):
		for x in range(grid_width):
			var alive_neighbors = count_alive_neighbors(x, y)
			var alive = grid[y][x]
			if alive:
				next_grid[y][x] = alive_neighbors == 2 or alive_neighbors == 3
			else:
				next_grid[y][x] = alive_neighbors == 3

	# Swap las referencias
	var temp = grid
	grid = next_grid
	next_grid = temp

func count_alive_neighbors(x, y):
	var count = 0
	for dy in range(-1, 2):
		for dx in range(-1, 2):
			if dx == 0 and dy == 0:
				continue
			var nx = x + dx
			var ny = y + dy
			if nx >= 0 and nx < grid_width and ny >= 0 and ny < grid_height:
				if grid[ny][nx]:
					count += 1
	return count

func _draw():
	for y in range(grid_height):
		for x in range(grid_width):
			if grid[y][x]:
				draw_rect(Rect2(x * cell_size, y * cell_size, cell_size, cell_size), Color.SEASHELL)

func update_cell_size(size: int) -> void:
	if size != cell_size:
		cell_size = size
		reset()
		
func update_timeout(time: float) -> void:
	if time <= 0.005:
		on_timer = false
		timer.stop()
	else:
		on_timer = true
		if time != timeout:
			timeout = time
			timer.wait_time = timeout
			timer.start()

func reset() -> void:
	update_grid_size()
	init_grids()
	randomize_grid()
