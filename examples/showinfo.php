<?php

$platforms = cl_get_platform_ids();
foreach ($platforms as $platform) {
    print_r(cl_get_platform_info($platform));
    $devices = cl_get_device_ids($platform, CL_DEVICE_TYPE_ALL);
    foreach ($devices as $device) {
        print_r(cl_get_device_info($device));
    }
}
