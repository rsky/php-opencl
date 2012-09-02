<?php
$width = 500;
$height = 500;

$platforms = cl_get_platform_ids();
var_dump($platforms);
print_r(cl_get_platform_info($platforms[0]));

$devices = cl_get_device_ids($platforms[0], CL_DEVICE_TYPE_ALL);
var_dump($devices);
print_r(cl_get_device_info($devices[0]));

$context = cl_create_context($devices[0]);
var_dump($context);
print_r(cl_get_context_info($context));

$queue = cl_create_command_queue($context);
var_dump($queue);
print_r(cl_get_command_queue_info($queue));

$source = __DIR__ . '/mandelbrot.cl';
$program = cl_create_program_with_source($context, $source, $err);
var_dump($program, $err);
//cl_build_program($program, $devices[0]);
cl_build_program($program);
print_r(cl_get_program_info($program));

$kernel = cl_create_kernel($program, 'Mandelbrot');
var_dump($kernel);
print_r(cl_get_kernel_info($kernel));

$memroy = cl_create_buffer($context, CL_MEM_WRITE_ONLY, $width * $height);
var_dump($memroy);
print_r(cl_get_mem_object_info($memroy));
