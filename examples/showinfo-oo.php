<?php
require_once __DIR__ . '/../classes/Cl.php';
Cl::registerAutoload();

$platforms = Cl\Platform::getPlatforms();
foreach ($platforms as $platform) {
    print_r($platform->getInfo());
    $devices = $platform->getDevices(Cl::DEVICE_TYPE_ALL);
    foreach ($devices as $device) {
        print_r($device->getInfo());
    }
}
