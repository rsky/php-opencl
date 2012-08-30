/*
   +----------------------------------------------------------------------+
   | All rights reserved                                                  |
   |                                                                      |
   | Redistribution and use in source and binary forms, with or without   |
   | modification, are permitted provided that the following conditions   |
   | are met:                                                             |
   |                                                                      |
   | 1. Redistributions of source code must retain the above copyright    |
   |    notice, this list of conditions and the following disclaimer.     |
   | 2. Redistributions in binary form must reproduce the above copyright |
   |    notice, this list of conditions and the following disclaimer in   |
   |    the documentation and/or other materials provided with the        |
   |    distribution.                                                     |
   | 3. The names of the authors may not be used to endorse or promote    |
   |    products derived from this software without specific prior        |
   |    written permission.                                               |
   |                                                                      |
   | THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS  |
   | "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT    |
   | LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS    |
   | FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE       |
   | COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  |
   | INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, |
   | BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;     |
   | LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER     |
   | CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT   |
   | LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN    |
   | ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE      |
   | POSSIBILITY OF SUCH DAMAGE.                                          |
   +----------------------------------------------------------------------+
   | Authors: Ryusuke Sekiyama <rsky0711@gmail.com>                       |
   +----------------------------------------------------------------------+
*/

#include "php_opencl.h"
#include <OpenCL/opencl.h>

/* {{{ type definitions */

typedef struct {
	cl_context context;
} phpcl_context_t;

typedef struct {
	cl_command_queue command_queue;
} phpcl_command_queue_t;

typedef struct {
	cl_mem mem;
} phpcl_mem_t;

typedef struct {
	cl_program program;
} phpcl_program_t;

typedef struct {
	cl_kernel kernel;
} phpcl_kernel_t;

typedef struct {
	cl_event event;
} phpcl_event_t;

typedef struct {
	cl_sampler sampler;
} phpcl_sampler_t;

typedef enum {
	PARAM_TYPE_BITFIELD = 0,
	PARAM_TYPE_BOOL,
	PARAM_TYPE_SIZE,
	PARAM_TYPE_UINT,
	PARAM_TYPE_ULONG,
	PARAM_TYPE_STRING,
	PARAM_TYPE_PLATFORM,
	PARAM_TYPE_MAX_WORK_ITEM_SIZES,
} phpcl_param_type_t;

typedef struct {
	const char *key;
	cl_device_info name;
	phpcl_param_type_t type;
} phpcl_device_info_param_t;

typedef struct {
	const char *key;
	cl_platform_info name;
} phpcl_platform_info_param_t;

/* }}} */
/* {{{ globals */

static int le_platform;
static int le_device;
static int le_context;
static int le_command_queue;
static int le_mem;
static int le_event;
static int le_program;
static int le_kernel;
static int le_sampler;

static const phpcl_device_info_param_t device_info_list[] = {
	{ "type",                          CL_DEVICE_TYPE,                          PARAM_TYPE_BITFIELD  },
	{ "vendor_id",                     CL_DEVICE_VENDOR_ID,                     PARAM_TYPE_UINT      },
	{ "max_compute_units",             CL_DEVICE_MAX_COMPUTE_UNITS,             PARAM_TYPE_UINT      },
	{ "max_work_item_dimensions",      CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS,      PARAM_TYPE_UINT      },
	{ "max_work_group_size",           CL_DEVICE_MAX_WORK_GROUP_SIZE,           PARAM_TYPE_SIZE      },
	{ "max_work_item_sizes",           CL_DEVICE_MAX_WORK_ITEM_SIZES, PARAM_TYPE_MAX_WORK_ITEM_SIZES },
	{ "preferred_vector_width_char",   CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR,   PARAM_TYPE_SIZE      },
	{ "preferred_vector_width_short",  CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT,  PARAM_TYPE_SIZE      },
	{ "preferred_vector_width_int",    CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT,    PARAM_TYPE_SIZE      },
	{ "preferred_vector_width_long",   CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG,   PARAM_TYPE_SIZE      },
	{ "preferred_vector_width_float",  CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT,  PARAM_TYPE_SIZE      },
	{ "preferred_vector_width_double", CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE, PARAM_TYPE_SIZE      },
#ifdef CL_DEVICE_PREFERRED_VECTOR_WIDTH_HALF /* OpenCL 1.1 */
	{ "preferred_vector_width_half",   CL_DEVICE_PREFERRED_VECTOR_WIDTH_HALF,   PARAM_TYPE_UINT      },
#endif
#ifdef CL_DEVICE_NATIVE_VECTOR_WIDTH_CHAR /* OpenCL 1.1 */
	{ "native_vector_width_char",      CL_DEVICE_NATIVE_VECTOR_WIDTH_CHAR,      PARAM_TYPE_UINT      },
	{ "native_vector_width_short",     CL_DEVICE_NATIVE_VECTOR_WIDTH_SHORT,     PARAM_TYPE_UINT      },
	{ "native_vector_width_int",       CL_DEVICE_NATIVE_VECTOR_WIDTH_INT,       PARAM_TYPE_UINT      },
	{ "native_vector_width_long",      CL_DEVICE_NATIVE_VECTOR_WIDTH_LONG,      PARAM_TYPE_UINT      },
	{ "native_vector_width_float",     CL_DEVICE_NATIVE_VECTOR_WIDTH_FLOAT,     PARAM_TYPE_UINT      },
	{ "native_vector_width_double",    CL_DEVICE_NATIVE_VECTOR_WIDTH_DOUBLE,    PARAM_TYPE_UINT      },
	{ "native_vector_width_half",      CL_DEVICE_NATIVE_VECTOR_WIDTH_HALF,      PARAM_TYPE_UINT      },
#endif
	{ "max_clock_frequency",           CL_DEVICE_MAX_CLOCK_FREQUENCY,           PARAM_TYPE_UINT      },
	{ "address_bits",                  CL_DEVICE_ADDRESS_BITS,                  PARAM_TYPE_UINT      },
	{ "max_read_image_args",           CL_DEVICE_MAX_READ_IMAGE_ARGS,           PARAM_TYPE_UINT      },
	{ "max_write_image_args",          CL_DEVICE_MAX_WRITE_IMAGE_ARGS,          PARAM_TYPE_UINT      },
	{ "max_mem_alloc_size",            CL_DEVICE_MAX_MEM_ALLOC_SIZE,            PARAM_TYPE_ULONG     },
	{ "image2d_max_width",             CL_DEVICE_IMAGE2D_MAX_WIDTH,             PARAM_TYPE_SIZE      },
	{ "image2d_max_height",            CL_DEVICE_IMAGE2D_MAX_HEIGHT,            PARAM_TYPE_SIZE      },
	{ "image3d_max_width",             CL_DEVICE_IMAGE3D_MAX_WIDTH,             PARAM_TYPE_SIZE      },
	{ "image3d_max_height",            CL_DEVICE_IMAGE3D_MAX_HEIGHT,            PARAM_TYPE_SIZE      },
	{ "image3d_max_depth",             CL_DEVICE_IMAGE3D_MAX_DEPTH,             PARAM_TYPE_SIZE      },
	{ "image_support",                 CL_DEVICE_IMAGE_SUPPORT,                 PARAM_TYPE_BOOL      },
	{ "max_parameter_size",            CL_DEVICE_MAX_PARAMETER_SIZE,            PARAM_TYPE_SIZE      },
	{ "max_samplers",                  CL_DEVICE_MAX_SAMPLERS,                  PARAM_TYPE_UINT      },
	{ "mem_base_addr_align",           CL_DEVICE_MEM_BASE_ADDR_ALIGN,           PARAM_TYPE_UINT      },
	{ "min_data_type_align_size",      CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE,      PARAM_TYPE_UINT      },
	{ "single_fp_config",              CL_DEVICE_SINGLE_FP_CONFIG,              PARAM_TYPE_BITFIELD  },
	{ "global_mem_cache_type",         CL_DEVICE_GLOBAL_MEM_CACHE_TYPE,         PARAM_TYPE_UINT      },
	{ "global_mem_cacheline_size",     CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE,     PARAM_TYPE_UINT      },
	{ "global_mem_cache_size",         CL_DEVICE_GLOBAL_MEM_CACHE_SIZE,         PARAM_TYPE_ULONG     },
	{ "global_mem_size",               CL_DEVICE_GLOBAL_MEM_SIZE,               PARAM_TYPE_ULONG     },
	{ "max_constant_buffer_size",      CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE,      PARAM_TYPE_ULONG     },
	{ "max_constant_args",             CL_DEVICE_MAX_CONSTANT_ARGS,             PARAM_TYPE_UINT      },
	{ "local_mem_type",                CL_DEVICE_LOCAL_MEM_TYPE,                PARAM_TYPE_UINT      },
	{ "local_mem_size",                CL_DEVICE_LOCAL_MEM_SIZE,                PARAM_TYPE_ULONG     },
	{ "error_correction_support",      CL_DEVICE_ERROR_CORRECTION_SUPPORT,      PARAM_TYPE_BOOL      },
	{ "profiling_timer_resolution",    CL_DEVICE_PROFILING_TIMER_RESOLUTION,    PARAM_TYPE_SIZE      },
	{ "endian_little",                 CL_DEVICE_ENDIAN_LITTLE,                 PARAM_TYPE_BOOL      },
	{ "available",                     CL_DEVICE_AVAILABLE,                     PARAM_TYPE_BOOL      },
	{ "compiler_available",            CL_DEVICE_COMPILER_AVAILABLE,            PARAM_TYPE_BOOL      },
	{ "execution_capabilities",        CL_DEVICE_EXECUTION_CAPABILITIES,        PARAM_TYPE_BITFIELD  },
	{ "queue_properties",              CL_DEVICE_QUEUE_PROPERTIES,              PARAM_TYPE_BITFIELD  },
	{ "name",                          CL_DEVICE_NAME,                          PARAM_TYPE_STRING    },
	{ "vendor",                        CL_DEVICE_VENDOR,                        PARAM_TYPE_STRING    },
	{ "driver_version",                CL_DRIVER_VERSION,                       PARAM_TYPE_STRING    },
	{ "profile",                       CL_DEVICE_PROFILE,                       PARAM_TYPE_STRING    },
	{ "version",                       CL_DEVICE_VERSION,                       PARAM_TYPE_STRING    },
	{ "extensions",                    CL_DEVICE_EXTENSIONS,                    PARAM_TYPE_STRING    },
	{ "platform",                      CL_DEVICE_PLATFORM,                      PARAM_TYPE_PLATFORM  },
#ifdef CL_DEVICE_DOUBLE_FP_CONFIG
	{ "double_fp_config",              CL_DEVICE_DOUBLE_FP_CONFIG,              PARAM_TYPE_BITFIELD  },
#endif
#ifdef CL_DEVICE_HALF_FP_CONFIG
	{ "half_fp_config",                CL_DEVICE_HALF_FP_CONFIG,                PARAM_TYPE_BITFIELD  },
#endif
#ifdef CL_DEVICE_HOST_UNIFIED_MEMORY
	{ "host_unified_memory",           CL_DEVICE_HOST_UNIFIED_MEMORY,           PARAM_TYPE_BOOL      },
#endif
#ifdef CL_DEVICE_OPENCL_C_VERSION
	{ "opencl_c_version",              CL_DEVICE_OPENCL_C_VERSION,              PARAM_TYPE_STRING    },
#endif
	{ NULL, 0, 0 }
};

static const phpcl_platform_info_param_t platform_info_list[] = {
	{ "profile",    CL_PLATFORM_PROFILE    },
	{ "version",    CL_PLATFORM_VERSION    },
	{ "name",       CL_PLATFORM_NAME       },
	{ "vendor",     CL_PLATFORM_VENDOR     },
	{ "extensions", CL_PLATFORM_EXTENSIONS },
	{ NULL, 0 }
};

/* }}} */
/* {{{ function prototypes */
/* {{{ module functions */

static PHP_MINIT_FUNCTION(opencl);
static PHP_MINFO_FUNCTION(opencl);

/* }}} */
/* {{{ php functions */

static PHP_FUNCTION(cl_get_platform_ids);
static PHP_FUNCTION(cl_get_platform_info);
static PHP_FUNCTION(cl_get_device_ids);
static PHP_FUNCTION(cl_get_device_info);

/* }}} */
/* {{{ php function helpers */

static const char *phpcl_errstr(cl_int err);
static zval *phpcl_get_platform_info(cl_platform_id platform TSRMLS_DC);
static zval *phpcl_get_device_info(cl_device_id device TSRMLS_DC);

/* }}} */
/* {{{ module function helpers */

static void phpcl_register_constants(int module_number TSRMLS_DC);
static void phpcl_register_resources(int module_number TSRMLS_DC);

/* }}} */
/* {{{ resource destructors */

static void phpcl_free_context(zend_rsrc_list_entry *rsrc TSRMLS_DC);
static void phpcl_free_command_queue(zend_rsrc_list_entry *rsrc TSRMLS_DC);
static void phpcl_free_mem(zend_rsrc_list_entry *rsrc TSRMLS_DC);
static void phpcl_free_program(zend_rsrc_list_entry *rsrc TSRMLS_DC);
static void phpcl_free_kernel(zend_rsrc_list_entry *rsrc TSRMLS_DC);
static void phpcl_free_event(zend_rsrc_list_entry *rsrc TSRMLS_DC);
static void phpcl_free_sampler(zend_rsrc_list_entry *rsrc TSRMLS_DC);

/* }}} */
/* {{{ internal destructors */

static void phpcl_release_context(phpcl_context_t *ptr TSRMLS_DC);
static void phpcl_release_command_queue(phpcl_command_queue_t *ptr TSRMLS_DC);
static void phpcl_release_mem(phpcl_mem_t *ptr TSRMLS_DC);
static void phpcl_release_program(phpcl_program_t *ptr TSRMLS_DC);
static void phpcl_release_kernel(phpcl_kernel_t *ptr TSRMLS_DC);
static void phpcl_release_event(phpcl_event_t *ptr TSRMLS_DC);
static void phpcl_release_sampler(phpcl_sampler_t *ptr TSRMLS_DC);

/* }}} */
/* }}} function prototypes */
/* {{{ argument informations */

ZEND_BEGIN_ARG_INFO_EX(arg_info_cl_get_platform_info, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, platform)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arg_info_cl_get_device_ids, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 0)
	ZEND_ARG_INFO(0, platform)
	ZEND_ARG_INFO(0, device_type)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arg_info_cl_get_device_info, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, device)
ZEND_END_ARG_INFO()

/* }}} argument informations */
/* {{{ phpcl_functions */

static zend_function_entry phpcl_functions[] = {
	PHP_FE(cl_get_platform_ids,     NULL)
	PHP_FE(cl_get_platform_info,    arg_info_cl_get_platform_info)
	PHP_FE(cl_get_device_ids,       arg_info_cl_get_device_ids)
	PHP_FE(cl_get_device_info,      arg_info_cl_get_device_info)
	{ NULL, NULL, NULL }
};

/* }}} phpcl_functions */
/* {{{ cross-extension dependencies */

static zend_module_dep phpcl_deps[] = {
	/*ZEND_MOD_OPTIONAL("gd")*/
	/*ZEND_MOD_OPTIONAL("imagick")*/
	{NULL, NULL, NULL, 0}
};

/* }}} cross-extension dependencies */
/* {{{ opencl_module_entry */

zend_module_entry opencl_module_entry = {
	STANDARD_MODULE_HEADER_EX, NULL,
	phpcl_deps,
	"opencl",
	phpcl_functions,
	PHP_MINIT(opencl),
	NULL,
	NULL,
	NULL,
	PHP_MINFO(opencl),
	PHPCL_VERSION,
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_OPENCL
ZEND_GET_MODULE(opencl)
#endif

/* }}} */
/* {{{ module functions */

static PHP_MINIT_FUNCTION(opencl)
{
	phpcl_register_constants(module_number TSRMLS_CC);
	phpcl_register_resources(module_number TSRMLS_CC);
	return SUCCESS;
}

static PHP_MINFO_FUNCTION(opencl)
{
}

/* }}} module functions */
/* {{{ php functions */
/* {{{ php cl_platform_id functions */

/* {{{ array cl_get_platform_ids(void) */
static PHP_FUNCTION(cl_get_platform_ids)
{
	cl_int err = CL_SUCCESS;
	cl_uint num_entries = 0;
	cl_platform_id *platforms = NULL;
	cl_uint index = 0;

	RETVAL_FALSE;

	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

	err = clGetPlatformIDs(0, NULL, &num_entries);
	if (err != CL_SUCCESS) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING,
			"clGetPlatformIDs() failed [%s]", phpcl_errstr(err));
	}

	platforms = ecalloc(num_entries, sizeof(cl_platform_id));
	if (!platforms) {
		return;
	}

	err = clGetPlatformIDs(num_entries, platforms, NULL);
	if (err != CL_SUCCESS) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING,
			"clGetPlatformIDs() failed [%s]", phpcl_errstr(err));
		efree(platforms);
		return;
	}

	array_init_size(return_value, num_entries);

	for (index = 0; index < num_entries; index++) {
		cl_platform_id platform = platforms[index];
		zval *entry;
		MAKE_STD_ZVAL(entry);
		ZEND_REGISTER_RESOURCE(entry, platform, le_platform);
		add_next_index_zval(return_value, entry);
	}

	efree(platforms);
}
/* }}} */
/* {{{ array cl_get_platform_info(resource cl_platform_id platform) */
static PHP_FUNCTION(cl_get_platform_info)
{
	zval *zid = NULL;
	cl_platform_id platform = NULL;
	zval *platform_info = NULL;

	RETVAL_FALSE;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zid) == FAILURE) {
		return;
	}
	ZEND_FETCH_RESOURCE(platform, cl_platform_id, &zid, -1, "cl_platform_id", le_platform);

	platform_info = phpcl_get_platform_info(platform TSRMLS_CC);
	if (platform_info) {
		RETURN_ZVAL(platform_info, 0, 1);
	}
}
/* }}} */

/* }}} php cl_platform_id functions */
/* {{{ php cl_device_id functions */

/* {{{ array cl_get_device_ids([resource cl_platform_id platform[, int device_type]]) */
static PHP_FUNCTION(cl_get_device_ids)
{
	zval *zid = NULL;
	cl_platform_id platform = NULL;
	long ltype = 0L;
	cl_device_type device_type = CL_DEVICE_TYPE_DEFAULT;

	cl_int err = CL_SUCCESS;
	cl_uint num_entries = 0;
	cl_device_id *devices = NULL;
	cl_uint index = 0;

	RETVAL_FALSE;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|r!l", &zid, &ltype) == FAILURE) {
		return;
	}
	if (zid) {
		ZEND_FETCH_RESOURCE(platform, cl_platform_id, &zid, -1, "cl_platform_id", le_platform);
	}
	if (ZEND_NUM_ARGS() == 2) {
		device_type = (cl_device_type)ltype;
	}

	err = clGetDeviceIDs(platform, device_type, 0, NULL, &num_entries);
	if (err != CL_SUCCESS) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING,
			"clGetDeviceIDs() failed [%s]", phpcl_errstr(err));
		efree(devices);
		return;
	}

	devices = ecalloc(num_entries, sizeof(cl_device_id));
	if (!devices) {
		return;
	}

	err = clGetDeviceIDs(platform, device_type, num_entries, devices, NULL);
	if (err != CL_SUCCESS) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING,
			"clGetDeviceIDs() failed [%s]", phpcl_errstr(err));
		efree(devices);
		return;
	}

	array_init_size(return_value, num_entries);

	for (index = 0; index < num_entries; index++) {
		cl_device_id device = devices[index];
		zval *entry;
		MAKE_STD_ZVAL(entry);
		ZEND_REGISTER_RESOURCE(entry, device, le_device);
		add_next_index_zval(return_value, entry);
	}

	efree(devices);
}
/* }}} */
/* {{{ array cl_get_device_info(resource cl_device_id device) */
static PHP_FUNCTION(cl_get_device_info)
{
	zval *zid = NULL;
	cl_device_id device = NULL;
	zval *device_info = NULL;

	RETVAL_FALSE;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zid) == FAILURE) {
		return;
	}
	ZEND_FETCH_RESOURCE(device, cl_device_id, &zid, -1, "cl_device_id", le_device);

	device_info = phpcl_get_device_info(device TSRMLS_CC);
	if (device_info) {
		RETURN_ZVAL(device_info, 0, 1);
	}
}
/* }}} */

/* }}} php cl_device_id functions */
/* }}} php functions */
/* {{{ php function helpers */

/* {{{ phpcl_errstr() */
static const char *phpcl_errstr(cl_int err)
{
	switch(err) {
	case CL_SUCCESS:
		return "SUCCESS";
	case CL_DEVICE_NOT_FOUND:
		return "DEVICE_NOT_FOUND";
	case CL_DEVICE_NOT_AVAILABLE:
		return "DEVICE_NOT_AVAILABLE";
	case CL_COMPILER_NOT_AVAILABLE:
		return "COMPILER_NOT_AVAILABLE";
	case CL_MEM_OBJECT_ALLOCATION_FAILURE:
		return "MEM_OBJECT_ALLOCATION_FAILURE";
	case CL_OUT_OF_RESOURCES:
		return "OUT_OF_RESOURCES";
	case CL_OUT_OF_HOST_MEMORY:
		return "OUT_OF_HOST_MEMORY";
	case CL_PROFILING_INFO_NOT_AVAILABLE:
		return "PROFILING_INFO_NOT_AVAILABLE";
	case CL_MEM_COPY_OVERLAP:
		return "MEM_COPY_OVERLAP";
	case CL_IMAGE_FORMAT_MISMATCH:
		return "IMAGE_FORMAT_MISMATCH";
	case CL_IMAGE_FORMAT_NOT_SUPPORTED:
		return "IMAGE_FORMAT_NOT_SUPPORTED";
	case CL_BUILD_PROGRAM_FAILURE:
		return "BUILD_PROGRAM_FAILURE";
	case CL_MAP_FAILURE:
		return "MAP_FAILURE";
	case CL_MISALIGNED_SUB_BUFFER_OFFSET:
		return "MISALIGNED_SUB_BUFFER_OFFSET";
	case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST:
		return "EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";
	case CL_INVALID_VALUE:
		return "INVALID_VALUE";
	case CL_INVALID_DEVICE_TYPE:
		return "INVALID_DEVICE_TYPE";
	case CL_INVALID_PLATFORM:
		return "INVALID_PLATFORM";
	case CL_INVALID_DEVICE:
		return "INVALID_DEVICE";
	case CL_INVALID_CONTEXT:
		return "INVALID_CONTEXT";
	case CL_INVALID_QUEUE_PROPERTIES:
		return "INVALID_QUEUE_PROPERTIES";
	case CL_INVALID_COMMAND_QUEUE:
		return "INVALID_COMMAND_QUEUE";
	case CL_INVALID_HOST_PTR:
		return "INVALID_HOST_PTR";
	case CL_INVALID_MEM_OBJECT:
		return "INVALID_MEM_OBJECT";
	case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
		return "INVALID_IMAGE_FORMAT_DESCRIPTOR";
	case CL_INVALID_IMAGE_SIZE:
		return "INVALID_IMAGE_SIZE";
	case CL_INVALID_SAMPLER:
		return "INVALID_SAMPLER";
	case CL_INVALID_BINARY:
		return "INVALID_BINARY";
	case CL_INVALID_BUILD_OPTIONS:
		return "INVALID_BUILD_OPTIONS";
	case CL_INVALID_PROGRAM:
		return "INVALID_PROGRAM";
	case CL_INVALID_PROGRAM_EXECUTABLE:
		return "INVALID_PROGRAM_EXECUTABLE";
	case CL_INVALID_KERNEL_NAME:
		return "INVALID_KERNEL_NAME";
	case CL_INVALID_KERNEL_DEFINITION:
		return "INVALID_KERNEL_DEFINITION";
	case CL_INVALID_KERNEL:
		return "INVALID_KERNEL";
	case CL_INVALID_ARG_INDEX:
		return "INVALID_ARG_INDEX";
	case CL_INVALID_ARG_VALUE:
		return "INVALID_ARG_VALUE";
	case CL_INVALID_ARG_SIZE:
		return "INVALID_ARG_SIZE";
	case CL_INVALID_KERNEL_ARGS:
		return "INVALID_KERNEL_ARGS";
	case CL_INVALID_WORK_DIMENSION:
		return "INVALID_WORK_DIMENSION";
	case CL_INVALID_WORK_GROUP_SIZE:
		return "INVALID_WORK_GROUP_SIZE";
	case CL_INVALID_WORK_ITEM_SIZE:
		return "INVALID_WORK_ITEM_SIZE";
	case CL_INVALID_GLOBAL_OFFSET:
		return "INVALID_GLOBAL_OFFSET";
	case CL_INVALID_EVENT_WAIT_LIST:
		return "INVALID_EVENT_WAIT_LIST";
	case CL_INVALID_EVENT:
		return "INVALID_EVENT";
	case CL_INVALID_OPERATION:
		return "INVALID_OPERATION";
	case CL_INVALID_GL_OBJECT:
		return "INVALID_GL_OBJECT";
	case CL_INVALID_BUFFER_SIZE:
		return "INVALID_BUFFER_SIZE";
	case CL_INVALID_MIP_LEVEL:
		return "INVALID_MIP_LEVEL";
	case CL_INVALID_GLOBAL_WORK_SIZE:
		return "INVALID_GLOBAL_WORK_SIZE";
	case CL_INVALID_PROPERTY:
		return "INVALID_PROPERTY";
	}
	return "UNKNOWN";
}
/* }}} */
/* {{{ phpcl_get_platform_info() */
static zval *phpcl_get_platform_info(cl_platform_id platform TSRMLS_DC)
{
	const phpcl_platform_info_param_t *param = platform_info_list;
	cl_int err = CL_SUCCESS;
	char buf[1024] = { 0 };
	size_t len = 0;
	zval *zinfo;

	MAKE_STD_ZVAL(zinfo);
	array_init_size(zinfo, 8);

	snprintf(buf, sizeof(buf), "%p", platform);
	add_assoc_string(zinfo, "id", buf, 1);

	while (param->key != NULL) {
		err = clGetPlatformInfo(platform, param->name, sizeof(buf), buf, &len);
		if (err == CL_SUCCESS) {
			add_assoc_stringl(zinfo, param->key, buf, len, 1);
		} else {
			add_assoc_null(zinfo, param->key);
		}
		param++;
	}

	return zinfo;
}
/* }}} */
/* {{{ phpcl_get_device_info() */
static zval *phpcl_get_device_info(cl_device_id device TSRMLS_DC)
{
	const phpcl_device_info_param_t *param = device_info_list;
	cl_int err = CL_SUCCESS;
	char buf[1024] = { 0 };
	size_t len = 0;
	cl_uint max_work_item_dimensions = 0;
	zval *zinfo;

	err = clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS,
	                      sizeof(max_work_item_dimensions),
	                      &max_work_item_dimensions, NULL);
	if (err != CL_SUCCESS) {
		max_work_item_dimensions = 0;
	}

	MAKE_STD_ZVAL(zinfo);
	array_init_size(zinfo, 64);

	snprintf(buf, sizeof(buf), "%p", device);
	add_assoc_string(zinfo, "id", buf, 1);

	while (param->key != NULL) {
		switch (param->type) {
			case PARAM_TYPE_BITFIELD: {
				cl_bitfield val = 0;
				err = clGetDeviceInfo(device, param->name, sizeof(val), &val, NULL);
				if (err == CL_SUCCESS) {
					long lval = (long)val;
					add_assoc_long(zinfo, param->key, lval);
				} 
			}
			break;

			case PARAM_TYPE_BOOL: {
				cl_bool val = 0;
				err = clGetDeviceInfo(device, param->name, sizeof(val), &val, NULL);
				if (err == CL_SUCCESS) {
					zend_bool bval = (zend_bool)val;
					add_assoc_bool(zinfo, param->key, bval);
				}
			}
			break;

			case PARAM_TYPE_SIZE: {
				size_t val = 0;
				err = clGetDeviceInfo(device, param->name, sizeof(val), &val, NULL);
				if (err == CL_SUCCESS) {
					long lval = (long)val;
					add_assoc_long(zinfo, param->key, lval);
				}
			}
			break;

			case PARAM_TYPE_UINT: {
				cl_uint val = 0;
				err = clGetDeviceInfo(device, param->name, sizeof(val), &val, NULL);
				if (err == CL_SUCCESS) {
					long lval = (long)val;
					add_assoc_long(zinfo, param->key, lval);
				}
			}
			break;

			case PARAM_TYPE_ULONG: {
				cl_ulong val = 0;
				err = clGetDeviceInfo(device, param->name, sizeof(val), &val, NULL);
				if (err == CL_SUCCESS) {
					long lval = (long)val;
					add_assoc_long(zinfo, param->key, lval);
				}
			}
			break;

			case PARAM_TYPE_STRING: {
				err = clGetDeviceInfo(device, param->name, sizeof(buf), buf, &len);
				if (err == CL_SUCCESS) {
					add_assoc_stringl(zinfo, param->key, buf, len, 1);
				}
			}
			break;

			case PARAM_TYPE_PLATFORM: {
				cl_platform_id platform;
				err = clGetDeviceInfo(device, param->name, sizeof(cl_platform_id), &platform, NULL);
				if (err == CL_SUCCESS) {
					zval *entry;
					MAKE_STD_ZVAL(entry);
					ZEND_REGISTER_RESOURCE(entry, platform, le_platform);
					add_assoc_zval(zinfo, param->key, entry);
				}
			}
			break;

			case PARAM_TYPE_MAX_WORK_ITEM_SIZES: {
				size_t siz = sizeof(size_t) * max_work_item_dimensions;
				size_t *sizes = ecalloc(max_work_item_dimensions, sizeof(size_t));
				if (!sizes) {
					break;
				}

				err = clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_SIZES, siz, sizes, NULL);
				if (err == CL_SUCCESS) {
					cl_uint i;
					zval *zsizes;
					MAKE_STD_ZVAL(zsizes);
					array_init_size(zsizes, max_work_item_dimensions);
					for (i = 0; i < max_work_item_dimensions; i++) {
						long lval = (long)sizes[i];
						add_next_index_long(zsizes, lval);
					}
					add_assoc_zval(zinfo, param->key, zsizes);
				}

				efree(sizes);
			}
			break;

			default:
				err = CL_SUCCESS;
				add_assoc_null(zinfo, param->key);
		}

		if (err != CL_SUCCESS) {
			add_assoc_null(zinfo, param->key);
		}

		param++;
	}

	return zinfo;
}
/* }}} */

/* }}} php function helpers */
/* {{{ module function helpers */

/* {{{ phpcl_register_constants() */

#define PHP_CL_REGISTER_CONSTANT(name) \
	REGISTER_LONG_CONSTANT(#name, name, CONST_PERSISTENT | CONST_CS)

static void phpcl_register_constants(int module_number TSRMLS_DC)
{
	/* Error Codes */
	PHP_CL_REGISTER_CONSTANT(CL_SUCCESS);
	PHP_CL_REGISTER_CONSTANT(CL_DEVICE_NOT_FOUND);
	PHP_CL_REGISTER_CONSTANT(CL_DEVICE_NOT_AVAILABLE);
	PHP_CL_REGISTER_CONSTANT(CL_COMPILER_NOT_AVAILABLE);
	PHP_CL_REGISTER_CONSTANT(CL_MEM_OBJECT_ALLOCATION_FAILURE);
	PHP_CL_REGISTER_CONSTANT(CL_OUT_OF_RESOURCES);
	PHP_CL_REGISTER_CONSTANT(CL_OUT_OF_HOST_MEMORY);
	PHP_CL_REGISTER_CONSTANT(CL_PROFILING_INFO_NOT_AVAILABLE);
	PHP_CL_REGISTER_CONSTANT(CL_MEM_COPY_OVERLAP);
	PHP_CL_REGISTER_CONSTANT(CL_IMAGE_FORMAT_MISMATCH);
	PHP_CL_REGISTER_CONSTANT(CL_IMAGE_FORMAT_NOT_SUPPORTED);
	PHP_CL_REGISTER_CONSTANT(CL_BUILD_PROGRAM_FAILURE);
	PHP_CL_REGISTER_CONSTANT(CL_MAP_FAILURE);
	PHP_CL_REGISTER_CONSTANT(CL_MISALIGNED_SUB_BUFFER_OFFSET);
	PHP_CL_REGISTER_CONSTANT(CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST);

	PHP_CL_REGISTER_CONSTANT(CL_INVALID_VALUE);
	PHP_CL_REGISTER_CONSTANT(CL_INVALID_DEVICE_TYPE);
	PHP_CL_REGISTER_CONSTANT(CL_INVALID_PLATFORM);
	PHP_CL_REGISTER_CONSTANT(CL_INVALID_DEVICE);
	PHP_CL_REGISTER_CONSTANT(CL_INVALID_CONTEXT);
	PHP_CL_REGISTER_CONSTANT(CL_INVALID_QUEUE_PROPERTIES);
	PHP_CL_REGISTER_CONSTANT(CL_INVALID_COMMAND_QUEUE);
	PHP_CL_REGISTER_CONSTANT(CL_INVALID_HOST_PTR);
	PHP_CL_REGISTER_CONSTANT(CL_INVALID_MEM_OBJECT);
	PHP_CL_REGISTER_CONSTANT(CL_INVALID_IMAGE_FORMAT_DESCRIPTOR);
	PHP_CL_REGISTER_CONSTANT(CL_INVALID_IMAGE_SIZE);
	PHP_CL_REGISTER_CONSTANT(CL_INVALID_SAMPLER);
	PHP_CL_REGISTER_CONSTANT(CL_INVALID_BINARY);
	PHP_CL_REGISTER_CONSTANT(CL_INVALID_BUILD_OPTIONS);
	PHP_CL_REGISTER_CONSTANT(CL_INVALID_PROGRAM);
	PHP_CL_REGISTER_CONSTANT(CL_INVALID_PROGRAM_EXECUTABLE);
	PHP_CL_REGISTER_CONSTANT(CL_INVALID_KERNEL_NAME);
	PHP_CL_REGISTER_CONSTANT(CL_INVALID_KERNEL_DEFINITION);
	PHP_CL_REGISTER_CONSTANT(CL_INVALID_KERNEL);
	PHP_CL_REGISTER_CONSTANT(CL_INVALID_ARG_INDEX);
	PHP_CL_REGISTER_CONSTANT(CL_INVALID_ARG_VALUE);
	PHP_CL_REGISTER_CONSTANT(CL_INVALID_ARG_SIZE);
	PHP_CL_REGISTER_CONSTANT(CL_INVALID_KERNEL_ARGS);
	PHP_CL_REGISTER_CONSTANT(CL_INVALID_WORK_DIMENSION);
	PHP_CL_REGISTER_CONSTANT(CL_INVALID_WORK_GROUP_SIZE);
	PHP_CL_REGISTER_CONSTANT(CL_INVALID_WORK_ITEM_SIZE);
	PHP_CL_REGISTER_CONSTANT(CL_INVALID_GLOBAL_OFFSET);
	PHP_CL_REGISTER_CONSTANT(CL_INVALID_EVENT_WAIT_LIST);
	PHP_CL_REGISTER_CONSTANT(CL_INVALID_EVENT);
	PHP_CL_REGISTER_CONSTANT(CL_INVALID_OPERATION);
	PHP_CL_REGISTER_CONSTANT(CL_INVALID_GL_OBJECT);
	PHP_CL_REGISTER_CONSTANT(CL_INVALID_BUFFER_SIZE);
	PHP_CL_REGISTER_CONSTANT(CL_INVALID_MIP_LEVEL);
	PHP_CL_REGISTER_CONSTANT(CL_INVALID_GLOBAL_WORK_SIZE);
	PHP_CL_REGISTER_CONSTANT(CL_INVALID_PROPERTY);

	/* OpenCL Version */
	PHP_CL_REGISTER_CONSTANT(CL_VERSION_1_0);
#ifdef CL_VERSION_1_1
	PHP_CL_REGISTER_CONSTANT(CL_VERSION_1_1);
#endif

	/* cl_bool */
	PHP_CL_REGISTER_CONSTANT(CL_FALSE);
	PHP_CL_REGISTER_CONSTANT(CL_TRUE);
	PHP_CL_REGISTER_CONSTANT(CL_BLOCKING);
	PHP_CL_REGISTER_CONSTANT(CL_NON_BLOCKING);

	/* cl_platform_info */
	PHP_CL_REGISTER_CONSTANT(CL_PLATFORM_PROFILE);
	PHP_CL_REGISTER_CONSTANT(CL_PLATFORM_VERSION);
	PHP_CL_REGISTER_CONSTANT(CL_PLATFORM_NAME);
	PHP_CL_REGISTER_CONSTANT(CL_PLATFORM_VENDOR);
	PHP_CL_REGISTER_CONSTANT(CL_PLATFORM_EXTENSIONS);

	/* cl_device_type - bitfield */
	PHP_CL_REGISTER_CONSTANT(CL_DEVICE_TYPE_DEFAULT);
	PHP_CL_REGISTER_CONSTANT(CL_DEVICE_TYPE_CPU);
	PHP_CL_REGISTER_CONSTANT(CL_DEVICE_TYPE_GPU);
	PHP_CL_REGISTER_CONSTANT(CL_DEVICE_TYPE_ACCELERATOR);
	PHP_CL_REGISTER_CONSTANT(CL_DEVICE_TYPE_ALL);

	/* cl_device_info */
	PHP_CL_REGISTER_CONSTANT(CL_DEVICE_TYPE);
	PHP_CL_REGISTER_CONSTANT(CL_DEVICE_VENDOR_ID);
	PHP_CL_REGISTER_CONSTANT(CL_DEVICE_MAX_COMPUTE_UNITS);
	PHP_CL_REGISTER_CONSTANT(CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS);
	PHP_CL_REGISTER_CONSTANT(CL_DEVICE_MAX_WORK_GROUP_SIZE);
	PHP_CL_REGISTER_CONSTANT(CL_DEVICE_MAX_WORK_ITEM_SIZES);
	PHP_CL_REGISTER_CONSTANT(CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR);
	PHP_CL_REGISTER_CONSTANT(CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT);
	PHP_CL_REGISTER_CONSTANT(CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT);
	PHP_CL_REGISTER_CONSTANT(CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG);
	PHP_CL_REGISTER_CONSTANT(CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT);
	PHP_CL_REGISTER_CONSTANT(CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE);
	PHP_CL_REGISTER_CONSTANT(CL_DEVICE_MAX_CLOCK_FREQUENCY);
	PHP_CL_REGISTER_CONSTANT(CL_DEVICE_ADDRESS_BITS);
	PHP_CL_REGISTER_CONSTANT(CL_DEVICE_MAX_READ_IMAGE_ARGS);
	PHP_CL_REGISTER_CONSTANT(CL_DEVICE_MAX_WRITE_IMAGE_ARGS);
	PHP_CL_REGISTER_CONSTANT(CL_DEVICE_MAX_MEM_ALLOC_SIZE);
	PHP_CL_REGISTER_CONSTANT(CL_DEVICE_IMAGE2D_MAX_WIDTH);
	PHP_CL_REGISTER_CONSTANT(CL_DEVICE_IMAGE2D_MAX_HEIGHT);
	PHP_CL_REGISTER_CONSTANT(CL_DEVICE_IMAGE3D_MAX_WIDTH);
	PHP_CL_REGISTER_CONSTANT(CL_DEVICE_IMAGE3D_MAX_HEIGHT);
	PHP_CL_REGISTER_CONSTANT(CL_DEVICE_IMAGE3D_MAX_DEPTH);
	PHP_CL_REGISTER_CONSTANT(CL_DEVICE_IMAGE_SUPPORT);
	PHP_CL_REGISTER_CONSTANT(CL_DEVICE_MAX_PARAMETER_SIZE);
	PHP_CL_REGISTER_CONSTANT(CL_DEVICE_MAX_SAMPLERS);
	PHP_CL_REGISTER_CONSTANT(CL_DEVICE_MEM_BASE_ADDR_ALIGN);
	PHP_CL_REGISTER_CONSTANT(CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE);
	PHP_CL_REGISTER_CONSTANT(CL_DEVICE_SINGLE_FP_CONFIG);
	PHP_CL_REGISTER_CONSTANT(CL_DEVICE_GLOBAL_MEM_CACHE_TYPE);
	PHP_CL_REGISTER_CONSTANT(CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE);
	PHP_CL_REGISTER_CONSTANT(CL_DEVICE_GLOBAL_MEM_CACHE_SIZE);
	PHP_CL_REGISTER_CONSTANT(CL_DEVICE_GLOBAL_MEM_SIZE);
	PHP_CL_REGISTER_CONSTANT(CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE);
	PHP_CL_REGISTER_CONSTANT(CL_DEVICE_MAX_CONSTANT_ARGS);
	PHP_CL_REGISTER_CONSTANT(CL_DEVICE_LOCAL_MEM_TYPE);
	PHP_CL_REGISTER_CONSTANT(CL_DEVICE_LOCAL_MEM_SIZE);
	PHP_CL_REGISTER_CONSTANT(CL_DEVICE_ERROR_CORRECTION_SUPPORT);
	PHP_CL_REGISTER_CONSTANT(CL_DEVICE_PROFILING_TIMER_RESOLUTION);
	PHP_CL_REGISTER_CONSTANT(CL_DEVICE_ENDIAN_LITTLE);
	PHP_CL_REGISTER_CONSTANT(CL_DEVICE_AVAILABLE);
	PHP_CL_REGISTER_CONSTANT(CL_DEVICE_COMPILER_AVAILABLE);
	PHP_CL_REGISTER_CONSTANT(CL_DEVICE_EXECUTION_CAPABILITIES);
	PHP_CL_REGISTER_CONSTANT(CL_DEVICE_QUEUE_PROPERTIES);
	PHP_CL_REGISTER_CONSTANT(CL_DEVICE_NAME);
	PHP_CL_REGISTER_CONSTANT(CL_DEVICE_VENDOR);
	PHP_CL_REGISTER_CONSTANT(CL_DRIVER_VERSION);
	PHP_CL_REGISTER_CONSTANT(CL_DEVICE_PROFILE);
	PHP_CL_REGISTER_CONSTANT(CL_DEVICE_VERSION);
	PHP_CL_REGISTER_CONSTANT(CL_DEVICE_EXTENSIONS);
	PHP_CL_REGISTER_CONSTANT(CL_DEVICE_PLATFORM);
#ifdef CL_DEVICE_DOUBLE_FP_CONFIG
	PHP_CL_REGISTER_CONSTANT(CL_DEVICE_DOUBLE_FP_CONFIG);
#endif
#ifdef CL_DEVICE_HALF_FP_CONFIG
	PHP_CL_REGISTER_CONSTANT(CL_DEVICE_HALF_FP_CONFIG);
#endif
#ifdef CL_DEVICE_PREFERRED_VECTOR_WIDTH_HALF
	PHP_CL_REGISTER_CONSTANT(CL_DEVICE_PREFERRED_VECTOR_WIDTH_HALF);
#endif
#ifdef CL_DEVICE_HOST_UNIFIED_MEMORY
	PHP_CL_REGISTER_CONSTANT(CL_DEVICE_HOST_UNIFIED_MEMORY);
#endif
#ifdef CL_DEVICE_NATIVE_VECTOR_WIDTH_CHAR
	PHP_CL_REGISTER_CONSTANT(CL_DEVICE_NATIVE_VECTOR_WIDTH_CHAR);
	PHP_CL_REGISTER_CONSTANT(CL_DEVICE_NATIVE_VECTOR_WIDTH_SHORT);
	PHP_CL_REGISTER_CONSTANT(CL_DEVICE_NATIVE_VECTOR_WIDTH_INT);
	PHP_CL_REGISTER_CONSTANT(CL_DEVICE_NATIVE_VECTOR_WIDTH_LONG);
	PHP_CL_REGISTER_CONSTANT(CL_DEVICE_NATIVE_VECTOR_WIDTH_FLOAT);
	PHP_CL_REGISTER_CONSTANT(CL_DEVICE_NATIVE_VECTOR_WIDTH_DOUBLE);
	PHP_CL_REGISTER_CONSTANT(CL_DEVICE_NATIVE_VECTOR_WIDTH_HALF);
#endif
#ifdef CL_DEVICE_OPENCL_C_VERSION
	PHP_CL_REGISTER_CONSTANT(CL_DEVICE_OPENCL_C_VERSION);
#endif

	/* cl_device_fp_config - bitfield */
	PHP_CL_REGISTER_CONSTANT(CL_FP_DENORM);
	PHP_CL_REGISTER_CONSTANT(CL_FP_INF_NAN);
	PHP_CL_REGISTER_CONSTANT(CL_FP_ROUND_TO_NEAREST);
	PHP_CL_REGISTER_CONSTANT(CL_FP_ROUND_TO_ZERO);
	PHP_CL_REGISTER_CONSTANT(CL_FP_ROUND_TO_INF);
	PHP_CL_REGISTER_CONSTANT(CL_FP_FMA);
	PHP_CL_REGISTER_CONSTANT(CL_FP_SOFT_FLOAT);

	/* cl_device_mem_cache_type */
	PHP_CL_REGISTER_CONSTANT(CL_NONE);
	PHP_CL_REGISTER_CONSTANT(CL_READ_ONLY_CACHE);
	PHP_CL_REGISTER_CONSTANT(CL_READ_WRITE_CACHE);

	/* cl_device_local_mem_type */
	PHP_CL_REGISTER_CONSTANT(CL_LOCAL);
	PHP_CL_REGISTER_CONSTANT(CL_GLOBAL);

	/* cl_device_exec_capabilities - bitfield */
	PHP_CL_REGISTER_CONSTANT(CL_EXEC_KERNEL);
	PHP_CL_REGISTER_CONSTANT(CL_EXEC_NATIVE_KERNEL);

	/* cl_command_queue_properties - bitfield */
	PHP_CL_REGISTER_CONSTANT(CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE);
	PHP_CL_REGISTER_CONSTANT(CL_QUEUE_PROFILING_ENABLE);

	/* cl_context_info  */
	PHP_CL_REGISTER_CONSTANT(CL_CONTEXT_REFERENCE_COUNT);
	PHP_CL_REGISTER_CONSTANT(CL_CONTEXT_DEVICES);
	PHP_CL_REGISTER_CONSTANT(CL_CONTEXT_PROPERTIES);
	PHP_CL_REGISTER_CONSTANT(CL_CONTEXT_NUM_DEVICES);

	/* cl_context_info + cl_context_properties */
	PHP_CL_REGISTER_CONSTANT(CL_CONTEXT_PLATFORM);

	/* cl_command_queue_info */
	PHP_CL_REGISTER_CONSTANT(CL_QUEUE_CONTEXT);
	PHP_CL_REGISTER_CONSTANT(CL_QUEUE_DEVICE);
	PHP_CL_REGISTER_CONSTANT(CL_QUEUE_REFERENCE_COUNT);
	PHP_CL_REGISTER_CONSTANT(CL_QUEUE_PROPERTIES);

	/* cl_mem_flags - bitfield */
	PHP_CL_REGISTER_CONSTANT(CL_MEM_READ_WRITE);
	PHP_CL_REGISTER_CONSTANT(CL_MEM_WRITE_ONLY);
	PHP_CL_REGISTER_CONSTANT(CL_MEM_READ_ONLY);
	PHP_CL_REGISTER_CONSTANT(CL_MEM_USE_HOST_PTR);
	PHP_CL_REGISTER_CONSTANT(CL_MEM_ALLOC_HOST_PTR);
	PHP_CL_REGISTER_CONSTANT(CL_MEM_COPY_HOST_PTR);

	/* cl_channel_order */
	PHP_CL_REGISTER_CONSTANT(CL_R);
	PHP_CL_REGISTER_CONSTANT(CL_A);
	PHP_CL_REGISTER_CONSTANT(CL_RG);
	PHP_CL_REGISTER_CONSTANT(CL_RA);
	PHP_CL_REGISTER_CONSTANT(CL_RGB);
	PHP_CL_REGISTER_CONSTANT(CL_RGBA);
	PHP_CL_REGISTER_CONSTANT(CL_BGRA);
	PHP_CL_REGISTER_CONSTANT(CL_ARGB);
	PHP_CL_REGISTER_CONSTANT(CL_INTENSITY);
	PHP_CL_REGISTER_CONSTANT(CL_LUMINANCE);
	PHP_CL_REGISTER_CONSTANT(CL_Rx);
	PHP_CL_REGISTER_CONSTANT(CL_RGx);
	PHP_CL_REGISTER_CONSTANT(CL_RGBx);

	/* cl_channel_type */
	PHP_CL_REGISTER_CONSTANT(CL_SNORM_INT8);
	PHP_CL_REGISTER_CONSTANT(CL_SNORM_INT16);
	PHP_CL_REGISTER_CONSTANT(CL_UNORM_INT8);
	PHP_CL_REGISTER_CONSTANT(CL_UNORM_INT16);
	PHP_CL_REGISTER_CONSTANT(CL_UNORM_SHORT_565);
	PHP_CL_REGISTER_CONSTANT(CL_UNORM_SHORT_555);
	PHP_CL_REGISTER_CONSTANT(CL_UNORM_INT_101010);
	PHP_CL_REGISTER_CONSTANT(CL_SIGNED_INT8);
	PHP_CL_REGISTER_CONSTANT(CL_SIGNED_INT16);
	PHP_CL_REGISTER_CONSTANT(CL_SIGNED_INT32);
	PHP_CL_REGISTER_CONSTANT(CL_UNSIGNED_INT8);
	PHP_CL_REGISTER_CONSTANT(CL_UNSIGNED_INT16);
	PHP_CL_REGISTER_CONSTANT(CL_UNSIGNED_INT32);
	PHP_CL_REGISTER_CONSTANT(CL_HALF_FLOAT);
	PHP_CL_REGISTER_CONSTANT(CL_FLOAT);

	/* cl_mem_object_type */
	PHP_CL_REGISTER_CONSTANT(CL_MEM_OBJECT_BUFFER);
	PHP_CL_REGISTER_CONSTANT(CL_MEM_OBJECT_IMAGE2D);
	PHP_CL_REGISTER_CONSTANT(CL_MEM_OBJECT_IMAGE3D);

	/* cl_mem_info */
	PHP_CL_REGISTER_CONSTANT(CL_MEM_TYPE);
	PHP_CL_REGISTER_CONSTANT(CL_MEM_FLAGS);
	PHP_CL_REGISTER_CONSTANT(CL_MEM_SIZE);
	PHP_CL_REGISTER_CONSTANT(CL_MEM_HOST_PTR);
	PHP_CL_REGISTER_CONSTANT(CL_MEM_MAP_COUNT);
	PHP_CL_REGISTER_CONSTANT(CL_MEM_REFERENCE_COUNT);
	PHP_CL_REGISTER_CONSTANT(CL_MEM_CONTEXT);
	PHP_CL_REGISTER_CONSTANT(CL_MEM_ASSOCIATED_MEMOBJECT);
	PHP_CL_REGISTER_CONSTANT(CL_MEM_OFFSET);

	/* cl_image_info */
	PHP_CL_REGISTER_CONSTANT(CL_IMAGE_FORMAT);
	PHP_CL_REGISTER_CONSTANT(CL_IMAGE_ELEMENT_SIZE);
	PHP_CL_REGISTER_CONSTANT(CL_IMAGE_ROW_PITCH);
	PHP_CL_REGISTER_CONSTANT(CL_IMAGE_SLICE_PITCH);
	PHP_CL_REGISTER_CONSTANT(CL_IMAGE_WIDTH);
	PHP_CL_REGISTER_CONSTANT(CL_IMAGE_HEIGHT);
	PHP_CL_REGISTER_CONSTANT(CL_IMAGE_DEPTH);

	/* cl_addressing_mode */
	PHP_CL_REGISTER_CONSTANT(CL_ADDRESS_NONE);
	PHP_CL_REGISTER_CONSTANT(CL_ADDRESS_CLAMP_TO_EDGE);
	PHP_CL_REGISTER_CONSTANT(CL_ADDRESS_CLAMP);
	PHP_CL_REGISTER_CONSTANT(CL_ADDRESS_REPEAT);
	PHP_CL_REGISTER_CONSTANT(CL_ADDRESS_MIRRORED_REPEAT);

	/* cl_filter_mode */
	PHP_CL_REGISTER_CONSTANT(CL_FILTER_NEAREST);
	PHP_CL_REGISTER_CONSTANT(CL_FILTER_LINEAR);

	/* cl_sampler_info */
	PHP_CL_REGISTER_CONSTANT(CL_SAMPLER_REFERENCE_COUNT);
	PHP_CL_REGISTER_CONSTANT(CL_SAMPLER_CONTEXT);
	PHP_CL_REGISTER_CONSTANT(CL_SAMPLER_NORMALIZED_COORDS);
	PHP_CL_REGISTER_CONSTANT(CL_SAMPLER_ADDRESSING_MODE);
	PHP_CL_REGISTER_CONSTANT(CL_SAMPLER_FILTER_MODE);

	/* cl_map_flags - bitfield */
	PHP_CL_REGISTER_CONSTANT(CL_MAP_READ);
	PHP_CL_REGISTER_CONSTANT(CL_MAP_WRITE);

	/* cl_program_info */
	PHP_CL_REGISTER_CONSTANT(CL_PROGRAM_REFERENCE_COUNT);
	PHP_CL_REGISTER_CONSTANT(CL_PROGRAM_CONTEXT);
	PHP_CL_REGISTER_CONSTANT(CL_PROGRAM_NUM_DEVICES);
	PHP_CL_REGISTER_CONSTANT(CL_PROGRAM_DEVICES);
	PHP_CL_REGISTER_CONSTANT(CL_PROGRAM_SOURCE);
	PHP_CL_REGISTER_CONSTANT(CL_PROGRAM_BINARY_SIZES);
	PHP_CL_REGISTER_CONSTANT(CL_PROGRAM_BINARIES);

	/* cl_program_build_info */
	PHP_CL_REGISTER_CONSTANT(CL_PROGRAM_BUILD_STATUS);
	PHP_CL_REGISTER_CONSTANT(CL_PROGRAM_BUILD_OPTIONS);
	PHP_CL_REGISTER_CONSTANT(CL_PROGRAM_BUILD_LOG);

	/* cl_build_status */
	PHP_CL_REGISTER_CONSTANT(CL_BUILD_SUCCESS);
	PHP_CL_REGISTER_CONSTANT(CL_BUILD_NONE);
	PHP_CL_REGISTER_CONSTANT(CL_BUILD_ERROR);
	PHP_CL_REGISTER_CONSTANT(CL_BUILD_IN_PROGRESS);

	/* cl_kernel_info */
	PHP_CL_REGISTER_CONSTANT(CL_KERNEL_FUNCTION_NAME);
	PHP_CL_REGISTER_CONSTANT(CL_KERNEL_NUM_ARGS);
	PHP_CL_REGISTER_CONSTANT(CL_KERNEL_REFERENCE_COUNT);
	PHP_CL_REGISTER_CONSTANT(CL_KERNEL_CONTEXT);
	PHP_CL_REGISTER_CONSTANT(CL_KERNEL_PROGRAM);

	/* cl_kernel_work_group_info */
	PHP_CL_REGISTER_CONSTANT(CL_KERNEL_WORK_GROUP_SIZE);
	PHP_CL_REGISTER_CONSTANT(CL_KERNEL_COMPILE_WORK_GROUP_SIZE);
	PHP_CL_REGISTER_CONSTANT(CL_KERNEL_LOCAL_MEM_SIZE);
	PHP_CL_REGISTER_CONSTANT(CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE);
	PHP_CL_REGISTER_CONSTANT(CL_KERNEL_PRIVATE_MEM_SIZE);

	/* cl_event_info  */
	PHP_CL_REGISTER_CONSTANT(CL_EVENT_COMMAND_QUEUE);
	PHP_CL_REGISTER_CONSTANT(CL_EVENT_COMMAND_TYPE);
	PHP_CL_REGISTER_CONSTANT(CL_EVENT_REFERENCE_COUNT);
	PHP_CL_REGISTER_CONSTANT(CL_EVENT_COMMAND_EXECUTION_STATUS);
	PHP_CL_REGISTER_CONSTANT(CL_EVENT_CONTEXT);

	/* cl_command_type */
	PHP_CL_REGISTER_CONSTANT(CL_COMMAND_NDRANGE_KERNEL);
	PHP_CL_REGISTER_CONSTANT(CL_COMMAND_TASK);
	PHP_CL_REGISTER_CONSTANT(CL_COMMAND_NATIVE_KERNEL);
	PHP_CL_REGISTER_CONSTANT(CL_COMMAND_READ_BUFFER);
	PHP_CL_REGISTER_CONSTANT(CL_COMMAND_WRITE_BUFFER);
	PHP_CL_REGISTER_CONSTANT(CL_COMMAND_COPY_BUFFER);
	PHP_CL_REGISTER_CONSTANT(CL_COMMAND_READ_IMAGE);
	PHP_CL_REGISTER_CONSTANT(CL_COMMAND_WRITE_IMAGE);
	PHP_CL_REGISTER_CONSTANT(CL_COMMAND_COPY_IMAGE);
	PHP_CL_REGISTER_CONSTANT(CL_COMMAND_COPY_IMAGE_TO_BUFFER);
	PHP_CL_REGISTER_CONSTANT(CL_COMMAND_COPY_BUFFER_TO_IMAGE);
	PHP_CL_REGISTER_CONSTANT(CL_COMMAND_MAP_BUFFER);
	PHP_CL_REGISTER_CONSTANT(CL_COMMAND_MAP_IMAGE);
	PHP_CL_REGISTER_CONSTANT(CL_COMMAND_UNMAP_MEM_OBJECT);
	PHP_CL_REGISTER_CONSTANT(CL_COMMAND_MARKER);
	PHP_CL_REGISTER_CONSTANT(CL_COMMAND_ACQUIRE_GL_OBJECTS);
	PHP_CL_REGISTER_CONSTANT(CL_COMMAND_RELEASE_GL_OBJECTS);
	PHP_CL_REGISTER_CONSTANT(CL_COMMAND_READ_BUFFER_RECT);
	PHP_CL_REGISTER_CONSTANT(CL_COMMAND_WRITE_BUFFER_RECT);
	PHP_CL_REGISTER_CONSTANT(CL_COMMAND_COPY_BUFFER_RECT);
	PHP_CL_REGISTER_CONSTANT(CL_COMMAND_USER);

	/* command execution status */
	PHP_CL_REGISTER_CONSTANT(CL_COMPLETE);
	PHP_CL_REGISTER_CONSTANT(CL_RUNNING);
	PHP_CL_REGISTER_CONSTANT(CL_SUBMITTED);
	PHP_CL_REGISTER_CONSTANT(CL_QUEUED);

	/* cl_buffer_create_type  */
	PHP_CL_REGISTER_CONSTANT(CL_BUFFER_CREATE_TYPE_REGION);

	/* cl_profiling_info  */
	PHP_CL_REGISTER_CONSTANT(CL_PROFILING_COMMAND_QUEUED);
	PHP_CL_REGISTER_CONSTANT(CL_PROFILING_COMMAND_SUBMIT);
	PHP_CL_REGISTER_CONSTANT(CL_PROFILING_COMMAND_START);
	PHP_CL_REGISTER_CONSTANT(CL_PROFILING_COMMAND_END);
}

/* }}} */
/* {{{ phpcl_register_resources() */

#define PHP_CL_REGISTER_RESOURCE(name) \
	le_##name = zend_register_list_destructors_ex(\
		phpcl_free_##name, NULL, "cl_" #name, module_number)

static void phpcl_register_resources(int module_number TSRMLS_DC)
{
	le_platform = zend_register_list_destructors_ex(
		NULL, NULL, "cl_platform", module_number);
	le_device = zend_register_list_destructors_ex(
		NULL, NULL, "cl_device", module_number);
	PHP_CL_REGISTER_RESOURCE(context);
	PHP_CL_REGISTER_RESOURCE(command_queue);
	PHP_CL_REGISTER_RESOURCE(mem);
	PHP_CL_REGISTER_RESOURCE(program);
	PHP_CL_REGISTER_RESOURCE(kernel);
	PHP_CL_REGISTER_RESOURCE(event);
	PHP_CL_REGISTER_RESOURCE(sampler);
}

/* }}} module function helpers */
/* {{{ resource destructors */

static void phpcl_free_context(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	phpcl_release_context((phpcl_context_t *)rsrc->ptr TSRMLS_CC);
}

static void phpcl_free_command_queue(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	phpcl_release_command_queue((phpcl_command_queue_t *)rsrc->ptr TSRMLS_CC);
}

static void phpcl_free_mem(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	phpcl_release_mem((phpcl_mem_t *)rsrc->ptr TSRMLS_CC);
}

static void phpcl_free_program(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	phpcl_release_program((phpcl_program_t *)rsrc->ptr TSRMLS_CC);
}

static void phpcl_free_kernel(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	phpcl_release_kernel((phpcl_kernel_t *)rsrc->ptr TSRMLS_CC);
}

static void phpcl_free_event(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	phpcl_release_event((phpcl_event_t *)rsrc->ptr TSRMLS_CC);
}

static void phpcl_free_sampler(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	phpcl_release_sampler((phpcl_sampler_t *)rsrc->ptr TSRMLS_CC);
}

/* }}} */
/* {{{ internal destructors */

static void phpcl_release_context(phpcl_context_t *ptr TSRMLS_DC)
{
	clReleaseContext(ptr->context);
	efree(ptr);
}

static void phpcl_release_command_queue(phpcl_command_queue_t *ptr TSRMLS_DC)
{
	clReleaseCommandQueue(ptr->command_queue);
	efree(ptr);
}

static void phpcl_release_mem(phpcl_mem_t *ptr TSRMLS_DC)
{
	clReleaseMemObject(ptr->mem);
	efree(ptr);
}

static void phpcl_release_program(phpcl_program_t *ptr TSRMLS_DC)
{
	clReleaseProgram(ptr->program);
	efree(ptr);
}

static void phpcl_release_kernel(phpcl_kernel_t *ptr TSRMLS_DC)
{
	clReleaseKernel(ptr->kernel);
	efree(ptr);
}

static void phpcl_release_event(phpcl_event_t *ptr TSRMLS_DC)
{
	clReleaseEvent(ptr->event);
	efree(ptr);
}

static void phpcl_release_sampler(phpcl_sampler_t *ptr TSRMLS_DC)
{
	clReleaseSampler(ptr->sampler);
	efree(ptr);
}

// }}}

/* }}} module function helpers */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
