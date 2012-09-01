<?php
$platforms = cl_get_platform_ids();
var_dump($platforms);

$devices = cl_get_device_ids($platforms[0], CL_DEVICE_TYPE_ALL);
var_dump($devices);

$context = cl_create_context($devices[0]);
var_dump($context);

$queue = cl_create_command_queue($context);
var_dump($queue);

$source = __DIR__ . '/mandelbrot.cl';
$program = cl_create_program_with_source($context, $source, $err);
var_dump($program, $err);
//cl_build_program($program, $devices[0]);
cl_build_program($program);