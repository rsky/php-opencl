<?php

class Cl
{
    const SUCCESS = 0;
    const DEVICE_NOT_FOUND = -1;
    const DEVICE_NOT_AVAILABLE = -2;
    const COMPILER_NOT_AVAILABLE = -3;
    const MEM_OBJECT_ALLOCATION_FAILURE = -4;
    const OUT_OF_RESOURCES = -5;
    const OUT_OF_HOST_MEMORY = -6;
    const PROFILING_INFO_NOT_AVAILABLE = -7;
    const MEM_COPY_OVERLAP = -8;
    const IMAGE_FORMAT_MISMATCH = -9;
    const IMAGE_FORMAT_NOT_SUPPORTED = -10;
    const BUILD_PROGRAM_FAILURE = -11;
    const MAP_FAILURE = -12;
    const MISALIGNED_SUB_BUFFER_OFFSET = -13;
    const EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST = -14;
    const INVALID_VALUE = -30;
    const INVALID_DEVICE_TYPE = -31;
    const INVALID_PLATFORM = -32;
    const INVALID_DEVICE = -33;
    const INVALID_CONTEXT = -34;
    const INVALID_QUEUE_PROPERTIES = -35;
    const INVALID_COMMAND_QUEUE = -36;
    const INVALID_HOST_PTR = -37;
    const INVALID_MEM_OBJECT = -38;
    const INVALID_IMAGE_FORMAT_DESCRIPTOR = -39;
    const INVALID_IMAGE_SIZE = -40;
    const INVALID_SAMPLER = -41;
    const INVALID_BINARY = -42;
    const INVALID_BUILD_OPTIONS = -43;
    const INVALID_PROGRAM = -44;
    const INVALID_PROGRAM_EXECUTABLE = -45;
    const INVALID_KERNEL_NAME = -46;
    const INVALID_KERNEL_DEFINITION = -47;
    const INVALID_KERNEL = -48;
    const INVALID_ARG_INDEX = -49;
    const INVALID_ARG_VALUE = -50;
    const INVALID_ARG_SIZE = -51;
    const INVALID_KERNEL_ARGS = -52;
    const INVALID_WORK_DIMENSION = -53;
    const INVALID_WORK_GROUP_SIZE = -54;
    const INVALID_WORK_ITEM_SIZE = -55;
    const INVALID_GLOBAL_OFFSET = -56;
    const INVALID_EVENT_WAIT_LIST = -57;
    const INVALID_EVENT = -58;
    const INVALID_OPERATION = -59;
    const INVALID_GL_OBJECT = -60;
    const INVALID_BUFFER_SIZE = -61;
    const INVALID_MIP_LEVEL = -62;
    const INVALID_GLOBAL_WORK_SIZE = -63;
    const INVALID_PROPERTY = -64;
    const BLOCKING = 1;
    const NON_BLOCKING = 0;
    const PLATFORM_PROFILE = 2304;
    const PLATFORM_VERSION = 2305;
    const PLATFORM_NAME = 2306;
    const PLATFORM_VENDOR = 2307;
    const PLATFORM_EXTENSIONS = 2308;
    const DEVICE_TYPE_DEFAULT = 1;
    const DEVICE_TYPE_CPU = 2;
    const DEVICE_TYPE_GPU = 4;
    const DEVICE_TYPE_ACCELERATOR = 8;
    const DEVICE_TYPE_ALL = 4294967295;
    const DEVICE_TYPE = 4096;
    const DEVICE_VENDOR_ID = 4097;
    const DEVICE_MAX_COMPUTE_UNITS = 4098;
    const DEVICE_MAX_WORK_ITEM_DIMENSIONS = 4099;
    const DEVICE_MAX_WORK_GROUP_SIZE = 4100;
    const DEVICE_MAX_WORK_ITEM_SIZES = 4101;
    const DEVICE_PREFERRED_VECTOR_WIDTH_CHAR = 4102;
    const DEVICE_PREFERRED_VECTOR_WIDTH_SHORT = 4103;
    const DEVICE_PREFERRED_VECTOR_WIDTH_INT = 4104;
    const DEVICE_PREFERRED_VECTOR_WIDTH_LONG = 4105;
    const DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT = 4106;
    const DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE = 4107;
    const DEVICE_MAX_CLOCK_FREQUENCY = 4108;
    const DEVICE_ADDRESS_BITS = 4109;
    const DEVICE_MAX_READ_IMAGE_ARGS = 4110;
    const DEVICE_MAX_WRITE_IMAGE_ARGS = 4111;
    const DEVICE_MAX_MEM_ALLOC_SIZE = 4112;
    const DEVICE_IMAGE2D_MAX_WIDTH = 4113;
    const DEVICE_IMAGE2D_MAX_HEIGHT = 4114;
    const DEVICE_IMAGE3D_MAX_WIDTH = 4115;
    const DEVICE_IMAGE3D_MAX_HEIGHT = 4116;
    const DEVICE_IMAGE3D_MAX_DEPTH = 4117;
    const DEVICE_IMAGE_SUPPORT = 4118;
    const DEVICE_MAX_PARAMETER_SIZE = 4119;
    const DEVICE_MAX_SAMPLERS = 4120;
    const DEVICE_MEM_BASE_ADDR_ALIGN = 4121;
    const DEVICE_MIN_DATA_TYPE_ALIGN_SIZE = 4122;
    const DEVICE_SINGLE_FP_CONFIG = 4123;
    const DEVICE_GLOBAL_MEM_CACHE_TYPE = 4124;
    const DEVICE_GLOBAL_MEM_CACHELINE_SIZE = 4125;
    const DEVICE_GLOBAL_MEM_CACHE_SIZE = 4126;
    const DEVICE_GLOBAL_MEM_SIZE = 4127;
    const DEVICE_MAX_CONSTANT_BUFFER_SIZE = 4128;
    const DEVICE_MAX_CONSTANT_ARGS = 4129;
    const DEVICE_LOCAL_MEM_TYPE = 4130;
    const DEVICE_LOCAL_MEM_SIZE = 4131;
    const DEVICE_ERROR_CORRECTION_SUPPORT = 4132;
    const DEVICE_PROFILING_TIMER_RESOLUTION = 4133;
    const DEVICE_ENDIAN_LITTLE = 4134;
    const DEVICE_AVAILABLE = 4135;
    const DEVICE_COMPILER_AVAILABLE = 4136;
    const DEVICE_EXECUTION_CAPABILITIES = 4137;
    const DEVICE_QUEUE_PROPERTIES = 4138;
    const DEVICE_NAME = 4139;
    const DEVICE_VENDOR = 4140;
    const DRIVER_VERSION = 4141;
    const DEVICE_PROFILE = 4142;
    const DEVICE_VERSION = 4143;
    const DEVICE_EXTENSIONS = 4144;
    const DEVICE_PLATFORM = 4145;
    const DEVICE_DOUBLE_FP_CONFIG = 4146;
    const DEVICE_HALF_FP_CONFIG = 4147;
    const DEVICE_PREFERRED_VECTOR_WIDTH_HALF = 4148;
    const DEVICE_HOST_UNIFIED_MEMORY = 4149;
    const DEVICE_NATIVE_VECTOR_WIDTH_CHAR = 4150;
    const DEVICE_NATIVE_VECTOR_WIDTH_SHORT = 4151;
    const DEVICE_NATIVE_VECTOR_WIDTH_INT = 4152;
    const DEVICE_NATIVE_VECTOR_WIDTH_LONG = 4153;
    const DEVICE_NATIVE_VECTOR_WIDTH_FLOAT = 4154;
    const DEVICE_NATIVE_VECTOR_WIDTH_DOUBLE = 4155;
    const DEVICE_NATIVE_VECTOR_WIDTH_HALF = 4156;
    const DEVICE_OPENCL_C_VERSION = 4157;
    const FP_DENORM = 1;
    const FP_INF_NAN = 2;
    const FP_ROUND_TO_NEAREST = 4;
    const FP_ROUND_TO_ZERO = 8;
    const FP_ROUND_TO_INF = 16;
    const FP_FMA = 32;
    const FP_SOFT_FLOAT = 64;
    const NONE = 0;
    const READ_ONLY_CACHE = 1;
    const READ_WRITE_CACHE = 2;
    const MEM_TYPE_LOCAL = 1;
    const MEM_TYPE_GLOBAL = 2;
    const EXEC_KERNEL = 1;
    const EXEC_NATIVE_KERNEL = 2;
    const QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE = 1;
    const QUEUE_PROFILING_ENABLE = 2;
    const CONTEXT_REFERENCE_COUNT = 4224;
    const CONTEXT_DEVICES = 4225;
    const CONTEXT_PROPERTIES = 4226;
    const CONTEXT_NUM_DEVICES = 4227;
    const CONTEXT_PLATFORM = 4228;
    const QUEUE_CONTEXT = 4240;
    const QUEUE_DEVICE = 4241;
    const QUEUE_REFERENCE_COUNT = 4242;
    const QUEUE_PROPERTIES = 4243;
    const MEM_READ_WRITE = 1;
    const MEM_WRITE_ONLY = 2;
    const MEM_READ_ONLY = 4;
    const MEM_USE_HOST_PTR = 8;
    const MEM_ALLOC_HOST_PTR = 16;
    const MEM_COPY_HOST_PTR = 32;
    const R = 4272;
    const A = 4273;
    const RG = 4274;
    const RA = 4275;
    const RGB = 4276;
    const RGBA = 4277;
    const BGRA = 4278;
    const ARGB = 4279;
    const INTENSITY = 4280;
    const LUMINANCE = 4281;
    const Rx = 4282;
    const RGx = 4283;
    const RGBx = 4284;
    const SNORM_INT8 = 4304;
    const SNORM_INT16 = 4305;
    const UNORM_INT8 = 4306;
    const UNORM_INT16 = 4307;
    const UNORM_SHORT_565 = 4308;
    const UNORM_SHORT_555 = 4309;
    const UNORM_INT_101010 = 4310;
    const SIGNED_INT8 = 4311;
    const SIGNED_INT16 = 4312;
    const SIGNED_INT32 = 4313;
    const UNSIGNED_INT8 = 4314;
    const UNSIGNED_INT16 = 4315;
    const UNSIGNED_INT32 = 4316;
    const HALF_FLOAT = 4317;
    const FLOAT = 4318;
    const MEM_OBJECT_BUFFER = 4336;
    const MEM_OBJECT_IMAGE2D = 4337;
    const MEM_OBJECT_IMAGE3D = 4338;
    const MEM_TYPE = 4352;
    const MEM_FLAGS = 4353;
    const MEM_SIZE = 4354;
    const MEM_HOST_PTR = 4355;
    const MEM_MAP_COUNT = 4356;
    const MEM_REFERENCE_COUNT = 4357;
    const MEM_CONTEXT = 4358;
    const MEM_ASSOCIATED_MEMOBJECT = 4359;
    const MEM_OFFSET = 4360;
    const IMAGE_FORMAT = 4368;
    const IMAGE_ELEMENT_SIZE = 4369;
    const IMAGE_ROW_PITCH = 4370;
    const IMAGE_SLICE_PITCH = 4371;
    const IMAGE_WIDTH = 4372;
    const IMAGE_HEIGHT = 4373;
    const IMAGE_DEPTH = 4374;
    const ADDRESS_NONE = 4400;
    const ADDRESS_CLAMP_TO_EDGE = 4401;
    const ADDRESS_CLAMP = 4402;
    const ADDRESS_REPEAT = 4403;
    const ADDRESS_MIRRORED_REPEAT = 4404;
    const FILTER_NEAREST = 4416;
    const FILTER_LINEAR = 4417;
    const SAMPLER_REFERENCE_COUNT = 4432;
    const SAMPLER_CONTEXT = 4433;
    const SAMPLER_NORMALIZED_COORDS = 4434;
    const SAMPLER_ADDRESSING_MODE = 4435;
    const SAMPLER_FILTER_MODE = 4436;
    const MAP_READ = 1;
    const MAP_WRITE = 2;
    const PROGRAM_REFERENCE_COUNT = 4448;
    const PROGRAM_CONTEXT = 4449;
    const PROGRAM_NUM_DEVICES = 4450;
    const PROGRAM_DEVICES = 4451;
    const PROGRAM_SOURCE = 4452;
    const PROGRAM_BINARY_SIZES = 4453;
    const PROGRAM_BINARIES = 4454;
    const PROGRAM_BUILD_STATUS = 4481;
    const PROGRAM_BUILD_OPTIONS = 4482;
    const PROGRAM_BUILD_LOG = 4483;
    const BUILD_SUCCESS = 0;
    const BUILD_NONE = -1;
    const BUILD_ERROR = -2;
    const BUILD_IN_PROGRESS = -3;
    const KERNEL_FUNCTION_NAME = 4496;
    const KERNEL_NUM_ARGS = 4497;
    const KERNEL_REFERENCE_COUNT = 4498;
    const KERNEL_CONTEXT = 4499;
    const KERNEL_PROGRAM = 4500;
    const KERNEL_WORK_GROUP_SIZE = 4528;
    const KERNEL_COMPILE_WORK_GROUP_SIZE = 4529;
    const KERNEL_LOCAL_MEM_SIZE = 4530;
    const KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE = 4531;
    const KERNEL_PRIVATE_MEM_SIZE = 4532;
    const EVENT_COMMAND_QUEUE = 4560;
    const EVENT_COMMAND_TYPE = 4561;
    const EVENT_REFERENCE_COUNT = 4562;
    const EVENT_COMMAND_EXECUTION_STATUS = 4563;
    const EVENT_CONTEXT = 4564;
    const COMMAND_NDRANGE_KERNEL = 4592;
    const COMMAND_TASK = 4593;
    const COMMAND_NATIVE_KERNEL = 4594;
    const COMMAND_READ_BUFFER = 4595;
    const COMMAND_WRITE_BUFFER = 4596;
    const COMMAND_COPY_BUFFER = 4597;
    const COMMAND_READ_IMAGE = 4598;
    const COMMAND_WRITE_IMAGE = 4599;
    const COMMAND_COPY_IMAGE = 4600;
    const COMMAND_COPY_IMAGE_TO_BUFFER = 4601;
    const COMMAND_COPY_BUFFER_TO_IMAGE = 4602;
    const COMMAND_MAP_BUFFER = 4603;
    const COMMAND_MAP_IMAGE = 4604;
    const COMMAND_UNMAP_MEM_OBJECT = 4605;
    const COMMAND_MARKER = 4606;
    const COMMAND_ACQUIRE_GL_OBJECTS = 4607;
    const COMMAND_RELEASE_GL_OBJECTS = 4608;
    const COMMAND_READ_BUFFER_RECT = 4609;
    const COMMAND_WRITE_BUFFER_RECT = 4610;
    const COMMAND_COPY_BUFFER_RECT = 4611;
    const COMMAND_USER = 4612;
    const COMPLETE = 0;
    const RUNNING = 1;
    const SUBMITTED = 2;
    const QUEUED = 3;
    const BUFFER_CREATE_TYPE_REGION = 4640;
    const PROFILING_COMMAND_QUEUED = 4736;
    const PROFILING_COMMAND_SUBMIT = 4737;
    const PROFILING_COMMAND_START = 4738;
    const PROFILING_COMMAND_END = 4739;

    public static function createSomeContext()
    {
        $platforms = Platform::getPlatforms();
        foreach ($platforms as $platform) {
            $devices = $platform->getDevices(self::DEVICE_TYPE_ALL);
            foreach ($devices as $device) {
                return new Context($device);
            }
        }
        return null;
    }

    public static function loadClass($name)
    {
        if (strncmp($name, 'Cl\\', 3) === 0) {
            include __DIR__ . '/' . str_replace('\\', '/', $name) . '.php';
        }
    }

    public static function registerAutoload()
    {
        spl_autoload_register(array(__CLASS__, 'loadClass'));
    }

    public static function createException($code, $message = null)
    {
        if ($code < 0) {
            if ($code > self::CL_INVALID_VALUE) {
                return new \RuntimeException($message, $code);
            }
            return new \InvalidArgumentException($message, $code);
        }
        return new \Exception($message, $code);
    }
}
