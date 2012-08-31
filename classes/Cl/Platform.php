<?php
namespace Cl;
use Cl;

class Platform
{
    private $platformId;

    public static function getPlatforms()
    {
        $platforms = array();
        foreach (cl_get_platform_ids() as $platformId) {
            $platforms[] = new Platform($platformId);
        }
        return $platforms;
    }

    public function __construct($platformId)
    {
        if (is_resource($platformId) &&
            get_resource_type($platformId) === 'cl_platform') {
            $this->platformId = $platformId;
        } else {
            throw new \InvalidArgumentException(__METHOD__
                . 'expects parameter 1 to be a valid cl_platform resource');
        }
    }

    public function getId()
    {
        return $this->platformId;
    }

    public function getInfo($name = null)
    {
        if (is_null($name)) {
            return cl_get_platform_info($this->platformId);
        }
        return cl_get_platform_info($this->platformId, $name);
    }

    public function getDevices($type = Cl::DEVICE_TYPE_DEFAULT)
    {
        return Device::getDevices($this, $type);
    }
}
