<?php
namespace Cl;
use Cl;

class Device
{
    private $deviceId;

    public static function getDevices(Platform $platform = null,
                                      $type = Cl::DEVICE_TYPE_DEFAULT)
    {
        $platformId = $platform ? $platform->getId() : null;
        $devices = array();
        foreach (cl_get_device_ids($platformId, $type) as $deviceId) {
            $devices[] = new Device($deviceId);
        }
        return $devices;
    }

    public function __construct($deviceId)
    {
        if (is_resource($deviceId) &&
            get_resource_type($deviceId) === 'cl_device') {
            $this->deviceId = $deviceId;
        } else {
            throw new \InvalidArgumentException(__METHOD__
                . 'expects parameter 1 to be a valid cl_device resource');
        }
    }

    public function getId()
    {
        return $this->deviceId;
    }

    public function getInfo($name = null)
    {
        if (is_null($name)) {
            return cl_get_device_info($this->deviceId);
        }
        return cl_get_device_info($this->deviceId, $name);
    }
}
